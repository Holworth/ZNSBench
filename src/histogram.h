//  Copyright (c) 2011-present, Facebook, Inc.  All rights reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).
//
// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#pragma once
#include <atomic>
#include <cassert>
#include <cstdint>
#include <map>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>

inline double ToMiB(uint64_t value) {
  return value / (1024.0 * 1024.0);
}

struct HistogramData {
  double median;
  double percentile95;
  double percentile99;
  double percentile999;
  double average;
  double standard_deviation;
  // zero-initialize new members since old Statistics::histogramData()
  // implementations won't write them.
  double max = 0.0;
  uint64_t count = 0;
  uint64_t sum = 0;
};

class HistogramBucketMapper {
public:
  HistogramBucketMapper();

  // converts a value to the bucket index.
  size_t IndexForValue(uint64_t value) const;
  // number of buckets required.

  size_t BucketCount() const { return bucketValues_.size(); }

  uint64_t LastValue() const { return maxBucketValue_; }

  uint64_t FirstValue() const { return minBucketValue_; }

  uint64_t BucketLimit(const size_t bucketNumber) const {
    assert(bucketNumber < BucketCount());
    return bucketValues_[bucketNumber];
  }

private:
  std::vector<uint64_t> bucketValues_;
  uint64_t maxBucketValue_;
  uint64_t minBucketValue_;
  std::map<uint64_t, uint64_t> valueIndexMap_;
};

struct HistogramStat {
  HistogramStat();
  ~HistogramStat() {}

  HistogramStat(const HistogramStat &) = delete;
  HistogramStat &operator=(const HistogramStat &) = delete;

  void Clear();
  bool Empty() const;
  void Add(uint64_t value);
  void Merge(const HistogramStat &other);

  inline uint64_t min() const { return min_.load(std::memory_order_relaxed); }
  inline uint64_t max() const { return max_.load(std::memory_order_relaxed); }
  inline uint64_t num() const { return num_.load(std::memory_order_relaxed); }
  inline uint64_t sum() const { return sum_.load(std::memory_order_relaxed); }
  inline uint64_t sum_squares() const {
    return sum_squares_.load(std::memory_order_relaxed);
  }
  inline uint64_t bucket_at(size_t b) const {
    return buckets_[b].load(std::memory_order_relaxed);
  }

  double Median() const;
  double Percentile(double p) const;
  double Average() const;
  double StandardDeviation() const;
  void Data(HistogramData *const data) const;
  std::string ToString() const;

  // To be able to use HistogramStat as thread local variable, it
  // cannot have dynamic allocated member. That's why we're
  // using manually values from BucketMapper
  std::atomic_uint_fast64_t min_;
  std::atomic_uint_fast64_t max_;
  std::atomic_uint_fast64_t num_;
  std::atomic_uint_fast64_t sum_;
  std::atomic_uint_fast64_t sum_squares_;
  std::atomic_uint_fast64_t buckets_[109]; // 109==BucketMapper::BucketCount()
  const uint64_t num_buckets_;
};

enum MetricsType {
  kWrite,
  kRead,
};

class Statistics {
public:
  Statistics() {
    thpt_.insert_or_assign(kWrite, new HistogramStat);
    thpt_.insert_or_assign(kRead, new HistogramStat);
    latency_.insert_or_assign(kWrite, new HistogramStat);
    latency_.insert_or_assign(kRead, new HistogramStat);
  }

  ~Statistics() {
    for (auto &[type, hist] : thpt_) {
      delete hist;
    }
    for (auto &[type, hist] : latency_) {
      delete hist;
    }
  }

  void AddThroughput(MetricsType type, uint64_t value) {
    thpt_[type]->Add(value);
  }

  void AddLatency(MetricsType type, uint64_t value) {
    latency_[type]->Add(value);
  }

  void Report() {
    ReportThroughput(kRead);
    ReportLatency(kRead);
    ReportThroughput(kWrite);
    ReportLatency(kWrite);
  }

  void ReportThroughput(MetricsType type) {
    HistogramData data;
    thpt_[type]->Data(&data);
    std::cout << "[Throughput]" 
              << "[Average: " << ToMiB(data.average) << "MiB/s]"
              << "[Max: " << ToMiB(data.max) << "MiB/s]" 
              << "[Median: " << ToMiB(data.median) << "MiB/s]\n";
  }

  void ReportLatency(MetricsType type) {
    HistogramData data;
    latency_[type]->Data(&data);
    std::cout << "[Latency]" 
              << "[Average: " << data.average << "us]"
              << "[Median: " << data.median << "us]"
              << "[P99: "  << data.percentile99 << "us]"
              << "[P999: " << data.percentile999 << "us]"
              << "[Max: " << data.max << "us]\n";
  }

private:
  std::unordered_map<MetricsType, HistogramStat *> thpt_;
  std::unordered_map<MetricsType, HistogramStat *> latency_;
};
