#include "lock_free_queue.hpp"
#include <atomic>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <stdint.h>
#include <thread>
#include <vector>

constexpr uint32_t CONSUMERS_COUNT = 2;
constexpr uint32_t QUEUE_CAPACITY = 256;
constexpr uint32_t AMOUNT_OF_PRODUCED_VALUES = 10000;

int main() {
  detail::lock_free_queue shared_queue(QUEUE_CAPACITY);
  std::atomic<bool> shutdown(false);

  std::thread producer;
  producer = std::thread([&shared_queue, &shutdown]() {
    for (uint32_t i = 0; i < AMOUNT_OF_PRODUCED_VALUES; ++i) {
      shared_queue.push(rand() % 1000);
    }
    shutdown.store(true, std::memory_order_release);
  });

  std::vector<std::thread> consumers;
  consumers.reserve(CONSUMERS_COUNT);

  for (uint32_t i = 0; i < CONSUMERS_COUNT; ++i) {
    consumers.emplace_back([&shared_queue, &shutdown, i]() {
      int32_t value;
      while (true) {
        if (shared_queue.pop(value)) {
          std::cout << "Consumer #" << i << " popped value: " << value
                    << std::endl;
        } else if (shutdown.load(std::memory_order_acquire) &&
                   shared_queue.get_read_index() >=
                       shared_queue.get_write_index()) {
          break; // done
        } else {
          std::this_thread::yield(); // wait for more work
        }
      }
    });
  }

  if (producer.joinable()) {
    producer.join();
  }

  for (auto &consumer : consumers) {
    if (consumer.joinable()) {
      consumer.join();
    }
  }

  return 0;
}