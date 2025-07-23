#include "lock_free_queue.hpp"
#include <atomic>
#include <thread>

namespace detail {
lock_free_queue::lock_free_queue(const lock_free_queue &other)
    : buffer_(other.buffer_.capacity()) {}

// move all the state of lock_free_queue. Other queue becomes unusable
lock_free_queue::lock_free_queue(lock_free_queue &&other)
    : buffer_(std::move(other.buffer_)),
      read_index_(other.read_index_.load(std::memory_order_acquire)),
      write_index_(other.write_index_.load(std::memory_order_acquire)) {
  // Reset the other queue's indices to make it unusable
  other.read_index_.store(0, std::memory_order_release);
  other.write_index_.store(0, std::memory_order_release);
}

void lock_free_queue::push(int32_t value) {
  uint32_t index;
  while (true) {
    index = write_index_.load(std::memory_order_relaxed);
    uint32_t read = read_index_.load(std::memory_order_acquire);
    if ((index - read) < buffer_.size())
      break;
    std::this_thread::yield();
  }

  index = write_index_.fetch_add(1, std::memory_order_acq_rel);
  slot_t &s = buffer_[index % buffer_.size()];

  while (s.ready_to_consume.load(std::memory_order_acquire)) {
    std::this_thread::yield();
  }

  s.value = value;
  s.ready_to_consume.store(true, std::memory_order_release);
}

bool lock_free_queue::pop(int32_t &val) {
  while (true) {
    uint32_t index = read_index_.load(std::memory_order_relaxed);
    slot_t &s = buffer_[index % buffer_.size()];
    if (s.ready_to_consume.load(std::memory_order_acquire)) {
      if (read_index_.compare_exchange_weak(index, index + 1,
                                            std::memory_order_acq_rel)) {
        val = s.value;
        s.ready_to_consume.store(false, std::memory_order_release);
        return true;
      }
    } else {
      return false;
    }
  }
}

} // namespace detail