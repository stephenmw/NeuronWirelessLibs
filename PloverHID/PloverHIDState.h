// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include <stdint.h>
#include <stddef.h>
#include <string.h>

namespace ploverhid {
// A physical-key state machine; the sender accepts a complete eight-byte report.
// The fixed queue bounds RAM and never coalesces press/release transitions.
template <size_t Keys, size_t Capacity = 64>
class State {
 public:
  State() { memset(owners_, 0xff, sizeof(owners_)); }
  bool owns(size_t address) const { return address < Keys && owners_[address] != 0xff; }
  uint32_t faults() const { return faults_; }
  void deliveryFault() { ++faults_; recover(); }
  void connection(uint32_t generation) {
    if (generation == generation_) return;
    generation_ = generation;
    recover();
  }
  void press(size_t address, uint8_t key) {
    if (address >= Keys || key >= 64 || owns(address)) return;
    owners_[address] = key;
    if (!generation_ || barrier_) { barrier_ = true; return; }
    if (counts_[key]++ == 0) {
      bitmap_[key / 8] |= uint8_t(1u << (7 - key % 8));
      enqueue();
    }
  }
  void release(size_t address) {
    if (!owns(address)) return;
    const uint8_t key = owners_[address];
    owners_[address] = 0xff;
    if (barrier_) return;
    if (counts_[key] && --counts_[key] == 0) {
      bitmap_[key / 8] &= uint8_t(~(1u << (7 - key % 8)));
      enqueue();
    }
  }
  template <typename Sender> void flush(Sender send) {
    if (!generation_) return;
    if (baseline_) {
      const uint8_t zero[8] = {};
      if (!send(zero)) return;
      baseline_ = false;
    }
    if (barrier_) {
      for (size_t i = 0; i < Keys; ++i) if (owns(i)) return;
      barrier_ = false;
    }
    // One report per scan, so a slow host cannot monopolize the scan loop.
    if (size_ && send(queue_[head_])) {
      head_ = (head_ + 1) % Capacity;
      --size_;
    }
  }
 private:
  void recover() {
    head_ = size_ = 0;
    memset(counts_, 0, sizeof(counts_));
    memset(bitmap_, 0, sizeof(bitmap_));
    baseline_ = barrier_ = true;
  }
  void enqueue() {
    if (size_ == Capacity) { deliveryFault(); return; }
    memcpy(queue_[(head_ + size_) % Capacity], bitmap_, 8);
    ++size_;
  }
  uint8_t owners_[Keys];
  uint8_t counts_[64] = {};
  uint8_t bitmap_[8] = {};
  uint8_t queue_[Capacity][8] = {};
  size_t head_ = 0, size_ = 0;
  uint32_t generation_ = 0, faults_ = 0;
  bool baseline_ = true, barrier_ = true;
};
}
