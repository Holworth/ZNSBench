#include "gflags/gflags.h"
#include "histogram.h"
#include "zbd_fs.h"

#include <chrono>
#include <cstdlib>
#include <thread>

DEFINE_string(bench, "writeseq", "Write-Read patterns for this benchmark");
DEFINE_uint64(bs, 4096, "request size for each read-write operation");
DEFINE_uint64(threads, 1, "Number of threads to issue request");
DEFINE_uint64(duration, 60, "Seconds to run this bench");
DEFINE_string(dev, "", "The ZNS device to read and write");

class Benchmark {
  static constexpr int kMaxThreadNum = 32;

public:
  // Option to configure this benchmark
  struct Option {
    std::string bench;
    std::string dev;
    uint64_t bs;
    uint64_t threads;
    uint64_t duration;
  };

  // Some thread-local states
  struct ThreadState {
    // The method to be executed
    void (*method)(ThreadState *) = nullptr;
    // The device to be accessed
    ZonedBlockDevice *zbd;
    // configuration
    Option option;
    Statistics *statistic;

    // Id of this running thread
    uint64_t id;
  };

  struct Duration {
    using TimePoint = decltype(std::chrono::steady_clock::now());
    TimePoint start;
    uint64_t limit;

    Duration(const uint64_t seconds) : start(NowTime()), limit(seconds) {}

    bool Ending() { return ElapseTimeMicro(start) >= limit * 1000000; }

    static TimePoint NowTime() { return std::chrono::steady_clock::now(); }

    static uint64_t ElapseTimeMicro(TimePoint start) {
      auto dura = std::chrono::duration_cast<std::chrono::microseconds>(
          NowTime() - start);
      return dura.count();
    }
  };

  struct MetricsGuard {
    using TimePoint = decltype(std::chrono::steady_clock::now());
    TimePoint start;
    uint64_t sz;
    MetricsType type;
    Statistics *statistic;

    MetricsGuard(uint64_t _sz, Statistics *_statistic, MetricsType _type)
        : start(Duration::NowTime()), sz(_sz), statistic(_statistic),
          type(_type) {}

    ~MetricsGuard() {
      auto dura = Duration::ElapseTimeMicro(start);
      auto thpt = (double)sz * 1e6 / dura;
      statistic->AddThroughput(type, thpt);
      statistic->AddLatency(type, dura);
    }
  };

public:
  Benchmark(const Option &option)
      : option_(option), zbd_(std::make_shared<ZonedBlockDevice>(option.dev)) {
    if (!zbd_->Open(false, true)) {
      assert(false);
    }
    statistic_ = new Statistics();
  }

  ~Benchmark() { delete statistic_; }

  void Run() {
    // Do not support threads number great than 14
    if (option_.threads > 14) {
      abort();
    }

    for (uint64_t i = 0; i < option_.threads; ++i) {
      auto thread_stat = &thread_stats_[i];

      thread_stat->option = option_;
      thread_stat->id = i;
      thread_stat->statistic = statistic_;
      thread_stat->zbd = zbd_.get();

      if (option_.bench == "write") {
        thread_stat->method = &Benchmark::WriteSeq;
      } else if (option_.bench == "readseq") {
        thread_stat->method = &Benchmark::ReadSeq;
      } else if (option_.bench == "readrandom") {
        thread_stat->method = &Benchmark::ReadRandom;
      }

      running_threads_.emplace_back(YieldThread(thread_stat));
    }
  }

private:
  static void WriteSeq(ThreadState *state) {
    auto zbd = state->zbd;
    // Prepare some data to write
    char *buf = new char[state->option.bs];
    for (size_t i = 0; i < state->option.bs; ++i) {
      buf[i] = '1';
    }
    auto dura = Duration(state->option.duration);

    while (!dura.Ending()) {
      auto zone_id = rand() % zbd->GetNrZones();
      auto zone = zbd->io_zones_[zone_id];
      if (!zone->Acquire()) {
        continue;
      }
      {
        MetricsGuard guard(state->option.bs, state->statistic, kWrite);
        zone->Append(buf, state->option.bs);
        zone->CheckRelease();
      }
    }

    delete[] buf;
  }

  static void ReadRandom(ThreadState *state) {}
  static void ReadSeq(ThreadState *state) {}

  using RunningThread = std::shared_ptr<std::thread>;
  RunningThread YieldThread(ThreadState *t_state) {
    auto t = new std::thread([=]() { t_state->method(t_state); });
    return std::shared_ptr<std::thread>(t);
  }

private:
  Option option_;
  std::shared_ptr<ZonedBlockDevice> zbd_;

  ThreadState thread_stats_[kMaxThreadNum];
  std::vector<RunningThread> running_threads_;
  Statistics *statistic_;
};

int zns_bench(int argc, char *argv[]) { return 0; }

int main(int argc, char *argv[]) { return zns_bench(argc, argv); }