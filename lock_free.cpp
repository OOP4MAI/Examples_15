#include <future>
#include <iostream>

static const size_t sleep_micro_sec = 5;
std::atomic<bool> locked_flag{false};

void foo() {
  bool exp = false;
  while (!locked_flag.compare_exchange_strong(exp, true)) {
    exp = false;
    if (sleep_micro_sec == 0) {
      std::this_thread::yield();
    } else if (sleep_micro_sec != 0) {
      std::this_thread::sleep_for(std::chrono::microseconds(sleep_micro_sec));
    }
  }
}

auto main() -> int{
    auto a = std::async(std::launch::async,foo);
    auto b = std::async(std::launch::async,foo);
    std::cin.get();
    locked_flag.store(false);
    a.get();
    b.get();
    return 1;
}