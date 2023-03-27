#pragma once

#include <atomic>
#include <string>
#include <vector>
#include <memory>
#include <cstring>
#include <cassert>

#include <libaio.h>

class Zone;
class ZonedBlockDevice;

class Zone {
  ZonedBlockDevice *zbd_;
  std::atomic_bool busy_;

public:
  explicit Zone(ZonedBlockDevice *zbd, struct zbd_zone *z);

  uint64_t start_;
  uint64_t capacity_; /* remaining capacity */
  uint64_t max_capacity_;
  uint64_t wp_;
  std::atomic<uint64_t> used_capacity_;

  bool Reset();
  bool Finish();
  bool Close();

  bool Append(char *data, uint32_t size);

  bool IsUsed();
  bool IsFull();
  bool IsEmpty();
  uint64_t GetZoneNr();
  uint64_t GetCapacityLeft();
  bool IsBusy() const { return this->busy_.load(std::memory_order_relaxed); }
  bool Acquire() {
    bool expected = false;
    return this->busy_.compare_exchange_strong(expected, true,
                                               std::memory_order_acq_rel);
  }
  bool Release() {
    bool expected = true;
    return this->busy_.compare_exchange_strong(expected, false,
                                               std::memory_order_acq_rel);
  }

  void LoopForAcquire() {
    while (!Acquire())
      ;
  }

  bool CheckRelease();
};

class ZonedBlockDevice {
public:
  std::string filename_;
  uint32_t block_sz_;
  uint64_t zone_sz_;
  uint32_t nr_zones_;
  std::vector<std::shared_ptr<Zone>> io_zones_;
  int read_f_;
  int read_direct_f_;
  int write_f_;
  time_t start_time_;
  uint32_t finish_threshold_ = 0;

  std::atomic<long> active_io_zones_;
  std::atomic<long> open_io_zones_;

  unsigned int max_nr_active_io_zones_;
  unsigned int max_nr_open_io_zones_;


public:
  ZonedBlockDevice(const std::string &bdevname) : filename_(bdevname) {}
  ~ZonedBlockDevice() = default;

  bool Open(bool readonly, bool exclusive);
  bool CheckScheduler();

  int GetReadFD() { return read_f_; }
  int GetReadDirectFD() { return read_direct_f_; }
  int GetWriteFD() { return write_f_; }

  uint64_t GetZoneSize() { return zone_sz_; }
  uint32_t GetNrZones() { return nr_zones_; }

  std::string GetFilename() { return filename_; }
  uint32_t GetBlockSize() { return block_sz_; }
};

// A wrapper for Linux AsyncIO, note that this struct only supports one
// async io request. 
struct AsyncIORequest {
  iocb cb;
  iocb* cb_ptr[1] = {&cb};
  io_context_t ctx;
  // If there is a pending async call
  bool pending_async = false;
  io_event event;

  // Init context of this Async request
  bool Init() {
    pending_async = false;
    std::memset(&ctx, 0, sizeof(io_context_t));
    auto ret = io_setup(1, &ctx);
    return ret == 0;
  }

  // Initialize an async read command
  void PrepareRead(int fd, size_t sz, uint64_t off, char* buf) {
    io_prep_pread(&cb, fd, buf, sz, off);
  }

  // Initialize an async write command
  void PrepareWrite(int fd, size_t sz, uint64_t off, char* buf) {
    io_prep_pwrite(&cb, fd, buf, sz, off);
  }

  // Submit this async command. Return false if any error happens
  // Set the pending flag.
  bool Submit() {
    auto ret = io_submit(ctx, 1, cb_ptr);
    if (ret < 0) {
      return false;
    }
    pending_async = true;
    return true;
  }

  bool CheckFinish() {
    assert(IsPending());
    timespec timeout;
    timeout.tv_sec = 0;
    // Check for 1ms
    timeout.tv_nsec = 1000000;
    if (io_getevents(ctx, 0, 1,  &event, &timeout) == 1) {
      return true;
    }
    return false;
  }

  bool IsPending() const { return pending_async; }
};
