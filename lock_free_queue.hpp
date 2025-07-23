#ifndef LOCK_FREE_QUEUE_HPP
#define LOCK_FREE_QUEUE_HPP

#include <atomic>
#include <cstdint>
#include <vector>

namespace detail {

struct slot_t {
  std::atomic<bool> ready_to_consume;
  int32_t value;
};

class lock_free_queue {
public:
  lock_free_queue(uint32_t capacity)
      : buffer_(capacity), read_index_(0), write_index_(0) {}

  // doesn't cope the state of buffer, only allocates same amount of memory.
  lock_free_queue(const lock_free_queue &other);
  // move all the state of lock_free_queue. Current queue becomes unusable
  lock_free_queue(lock_free_queue &&other);

  lock_free_queue &operator=(const lock_free_queue &other) = delete;
  lock_free_queue &operator=(lock_free_queue &&other) = delete;

  void push(int32_t value);
  [[nodiscard]] bool pop(int &val);

  [[nodiscard]] uint32_t get_read_index() const noexcept {
    return read_index_.load(std::memory_order_acquire);
  }
  [[nodiscard]] uint32_t get_write_index() const noexcept {
    return write_index_.load(std::memory_order_acquire);
  }

  ~lock_free_queue() = default;

private:
  std::vector<slot_t> buffer_;
  std::atomic<uint32_t> read_index_;
  std::atomic<uint32_t> write_index_;
};

}; // namespace detail

#endif