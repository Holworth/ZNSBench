#include "zbd_fs.h"
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <ratio>
#include <unistd.h>
#include <cstdio>

int RunAsync(ZonedBlockDevice *zbd, int id);
int RunSync(ZonedBlockDevice *zbd, int id);

void PrepareWrite(ZonedBlockDevice *zbd, int id);

// int write_fd;
// int read_fd;

const size_t kBufferSize = 2 * 1024ULL * 1024ULL;

int main(int argc, char *argv[]) {
  auto zbd = new ZonedBlockDevice("/dev/nvme0n1");
  zbd->Open(false, true);


  // write_fd = open("./test_file", O_DIRECT | O_WRONLY | O_CREAT);
  PrepareWrite(zbd, 0);

  if (std::strcmp(argv[1], "async") == 0) {
    RunAsync(zbd, 0);
  } else {
    RunSync(zbd, 0);
  }

  remove("./test_file");
}

void PrepareWrite(ZonedBlockDevice *zbd, int id) {
  auto zone = zbd->io_zones_[id];
  auto sz = 512 * 1024ULL * 1024ULL;
  char* buf;
  posix_memalign((void**)&buf, sysconf(_SC_PAGE_SIZE), sz);
  std::memset(buf, '1', sz);
  zone->Append(buf, sz);
  // write(write_fd, buf, sz);

  free(buf);
}

int SyncRead(int fd, size_t sz, size_t off, char *buf) {
  auto start = std::chrono::steady_clock::now();
  int readsz = 0;
  while (readsz < sz) {
    auto s = pread(fd, buf + readsz, sz - readsz, off + readsz);
    readsz += s;
  }
  auto end = std::chrono::steady_clock::now();
  auto dura = std::chrono::duration_cast<std::chrono::microseconds>(end-start);
  printf("Read Pass Time: %lu us\n", dura.count());
  return 0;
}

auto do_check = [](char *buf, size_t sz) -> bool {
  auto start = std::chrono::steady_clock::now();
  for (uint64_t i = 0; i < sz; ++i) {
    if (buf[i] != '1')
      return false;
  }
  auto end = std::chrono::steady_clock::now();
  auto dura = std::chrono::duration_cast<std::chrono::microseconds>(end-start);
  printf("Check Pass Time: %lu us\n", dura.count());
  return true;
};

int RunAsync(ZonedBlockDevice *zbd, int id) {
  auto start = std::chrono::steady_clock::now();
  auto limit = 512 * 1024ULL * 1024ULL;

  auto zone = zbd->io_zones_[id];
  auto buf_sz = kBufferSize;
  // Needs two buffers
  char *buf[2];
  posix_memalign((void**)(&buf[0]), sysconf(_SC_PAGE_SIZE), buf_sz);
  posix_memalign((void**)(&buf[1]), sysconf(_SC_PAGE_SIZE), buf_sz);
  int curr_buf_id = 0;

  auto curr_off = zone->start_;
  auto done_sz = 0;


  AsyncIORequest async;
  while (done_sz < limit) {
    // Init
    if (done_sz == 0) {
      SyncRead(zbd->GetReadDirectFD(), buf_sz, curr_off, buf[curr_buf_id]);
    } else {
      while (!async.CheckFinish())
        ;
    }

    // Submit the async command before processing the data
    auto s = async.Init();
    assert(s);
    async.PrepareRead(zbd->GetReadDirectFD(), buf_sz, curr_off + buf_sz,
                      buf[(curr_buf_id + 1) % 2]);
    s = async.Submit();
    assert(s);
    if (!do_check(buf[curr_buf_id], buf_sz)) {
      abort();
    }
    curr_buf_id = (curr_buf_id + 1) % 2;
    curr_off += buf_sz;
    done_sz += buf_sz;
  }

  free(buf[0]);
  free(buf[1]);

  auto end = std::chrono::steady_clock::now();
  auto dura =
      std::chrono::duration_cast<std::chrono::microseconds>(end - start);
  std::cout << "[Async Pass Time]: " << dura.count() << std::endl;

  return 0;
}

int RunSync(ZonedBlockDevice *zbd, int id) {
  auto start = std::chrono::steady_clock::now();
  auto limit = 512 * 1024ULL * 1024ULL;

  auto zone = zbd->io_zones_[id];
  auto buf_sz = kBufferSize;
  // Needs two buffers
  char *buf[1];

  posix_memalign((void**)(&buf[0]), sysconf(_SC_PAGE_SIZE), buf_sz);

  auto curr_off = zone->start_;
  auto done_sz = 0;

  while (done_sz < limit) {
    SyncRead(zbd->GetReadDirectFD(), buf_sz, curr_off, buf[0]);
    if (!do_check(buf[0], buf_sz)) {
      abort();
    }
    curr_off += buf_sz;
    done_sz += buf_sz;
  }

  free(buf[0]);

  auto end = std::chrono::steady_clock::now();
  auto dura =
      std::chrono::duration_cast<std::chrono::microseconds>(end - start);
  std::cout << "[Sync Pass Time]: " << dura.count() << std::endl;

  return 0;
}
