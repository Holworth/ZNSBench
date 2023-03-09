#include "zbd_fs.h"

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <libzbd/zbd.h>
#include <linux/blkzoned.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

bool Zone::Reset() {
  size_t zone_sz = zbd_->GetZoneSize();
  unsigned int report = 1;
  struct zbd_zone z;
  int ret;

  assert(!IsUsed());
  assert(IsBusy());

  ret = zbd_reset_zones(zbd_->GetWriteFD(), start_, zone_sz);
  if (ret) {
    return false;
  }

  ret = zbd_report_zones(zbd_->GetReadFD(), start_, zone_sz, ZBD_RO_ALL, &z,
                         &report);

  if (ret || (report != 1)) {
    return false;
  }

  if (zbd_zone_offline(&z))
    capacity_ = 0;
  else
    max_capacity_ = capacity_ = zbd_zone_capacity(&z);

  wp_ = start_;

  return true;
}

bool Zone::Finish() {
  size_t zone_sz = zbd_->GetZoneSize();
  int fd = zbd_->GetWriteFD();
  int ret;

  assert(IsBusy());

  ret = zbd_finish_zones(fd, start_, zone_sz);
  if (ret) {
    return false;
  }

  capacity_ = 0;
  wp_ = start_ + zone_sz;

  return true;
}

bool Zone::Close() {
  size_t zone_sz = zbd_->GetZoneSize();
  int fd = zbd_->GetWriteFD();
  int ret;

  assert(IsBusy());

  if (!(IsEmpty() || IsFull())) {
    ret = zbd_close_zones(fd, start_, zone_sz);
    if (ret) {
      return false;
    }
  }

  return true;
}

bool Zone::Append(char *data, uint32_t size, bool is_gc) {

  char *ptr = data;
  uint32_t left = size;
  int fd = zbd_->GetWriteFD();
  int ret;

  if (capacity_ < size) {
    return false;
  }

  assert((size % zbd_->GetBlockSize()) == 0);

  // errno = 75, "Value too large for defined data type"
  while (left) {
    ret = pwrite(fd, ptr, left, wp_);
    // printf("[kqh] Zone write %d bytes\n", ret);
    if (ret < 0) {
      printf("[kqh] ZoneAppend Error: %s\n", strerror(errno));
      assert(false);
      return false;
    }

    ptr += ret;
    wp_ += ret;
    capacity_ -= ret;
    left -= ret;

    assert(wp_ <= start_ + max_capacity_);
  }

  return true;
}

bool Zone::CheckRelease() {
  if (!Release()) {
    assert(false);
    return false;
  }

  return true;
}

bool ZonedBlockDevice::Open(bool readonly, bool exclusive) {
  struct zbd_zone *zone_rep;
  unsigned int reported_zones;
  uint64_t addr_space_sz;
  zbd_info info;
  uint64_t i = 0;
  uint64_t m = 0;
  // Reserve one zone for metadata and another one for extent migration
  int reserved_zones = 2;
  int ret;

  if (!readonly && !exclusive)
    return false;

  /* The non-direct file descriptor acts as an exclusive-use semaphore */
  if (exclusive) {
    read_f_ = zbd_open(filename_.c_str(), O_RDONLY | O_EXCL, &info);
  } else {
    read_f_ = zbd_open(filename_.c_str(), O_RDONLY, &info);
  }

  if (read_f_ < 0) {
    printf("Failed to open zoned block device for read:\n");
    return false;
  }

  read_direct_f_ = zbd_open(filename_.c_str(), O_RDONLY | O_DIRECT, &info);
  if (read_direct_f_ < 0) {
    printf("Failed to open zoned block device for direct read:\n");
    return false;
  }

  if (readonly) {
    write_f_ = -1;
  } else {
    write_f_ = zbd_open(filename_.c_str(), O_WRONLY | O_DIRECT, &info);
    if (write_f_ < 0) {
      printf("Failed to open zoned block device for write:\n");
      return false;
    }
  }

  if (info.model != ZBD_DM_HOST_MANAGED) {
    printf("Not a host managed block device");
    return false;
  }

  if (!CheckScheduler()) {
    return false;
  }

  // xzw: we limit the total zones here to 500
  // info.nr_zones = 500;
  block_sz_ = info.pblock_size;
  zone_sz_ = info.zone_size;
  nr_zones_ = info.nr_zones;

  // Set the max open and active zone count to be 14, which is a real
  // hardware parameter. Note that the simulated ZNS SSD does not limit
  // this parameter
  info.max_nr_active_zones = info.max_nr_open_zones = 14;

  if (info.max_nr_active_zones == 0)
    max_nr_active_io_zones_ = info.nr_zones;
  else
    max_nr_active_io_zones_ = info.max_nr_active_zones - reserved_zones;

  if (info.max_nr_open_zones == 0)
    max_nr_open_io_zones_ = info.nr_zones;
  else
    max_nr_open_io_zones_ = info.max_nr_open_zones - reserved_zones;

  addr_space_sz = (uint64_t)nr_zones_ * zone_sz_;

  ret = zbd_list_zones(read_f_, 0, addr_space_sz, ZBD_RO_ALL, &zone_rep,
                       &reported_zones);

  if (ret || reported_zones != nr_zones_) {
    printf("Failed to list zones\n");
    return false;
  }

  active_io_zones_ = 0;
  open_io_zones_ = 0;

  for (; i < reported_zones; i++) {
    struct zbd_zone *z = &zone_rep[i];
    /* Only use sequential write required zones */
    if (zbd_zone_type(z) == ZBD_ZONE_TYPE_SWR) {
      if (!zbd_zone_offline(z)) {
        Zone *newZone = new Zone(this, z);
        if (!newZone->Acquire()) {
          assert(false);
          return false;
        }
        io_zones_.emplace_back(newZone);
        if (zbd_zone_imp_open(z) || zbd_zone_exp_open(z) ||
            zbd_zone_closed(z)) {
          active_io_zones_++;
          if (zbd_zone_imp_open(z) || zbd_zone_exp_open(z)) {
            if (!readonly) {
              newZone->Close();
            }
          }
        }
        auto status = newZone->CheckRelease();
        if (!status)
          return status;
      }
    }
  }

  free(zone_rep);
  start_time_ = time(NULL);

  return true;
}

bool ZonedBlockDevice::CheckScheduler() {
  std::ostringstream path;
  std::string s = filename_;
  std::fstream f;

  s.erase(0, 5);  // Remove "/dev/" from /dev/nvmeXnY
  path << "/sys/block/" << s << "/queue/scheduler";
  f.open(path.str(), std::fstream::in);
  if (!f.is_open()) {
    return false;
  }

  std::string buf;
  getline(f, buf);
  if (buf.find("[mq-deadline]") == std::string::npos) {
    f.close();
    return false;
  }

  f.close();
  return true;
}
