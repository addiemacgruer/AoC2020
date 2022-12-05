#include "reader.h"
#include <future>

constexpr auto launch = std::launch::deferred;
using T = size_t;

auto startTime = T{0};
auto busses = std::vector<T>{};

auto read() {
  auto in = std::fstream{"13.txt"};
  auto tmp = std::string{};
  std::getline(in, tmp);
  startTime = al::stol(tmp);
  std::getline(in, tmp);
  for (auto &bus : al::split(tmp, ','))
    busses.push_back(bus == "x" ? -1 : al::stol(bus));
}

auto puzzle_one(T startTime) {
  for (auto t = startTime;; ++t)
    for (auto l : busses) {
      if (l == -1 || t % l != 0)
        continue;
      return (t - startTime) * l;
    }
}

auto puzzle_two() {
  auto rval = T{};
  auto increase = busses[0];
  for (auto i = 1; i < busses.size(); ++i) {
    if (busses[i] == -1)
      continue;
    while ((rval + i) % busses[i] != 0)
      rval += increase;
    increase *= busses[i];
  }
  return rval;
}

auto main() -> int {
  read();
  auto p1 = std::async(launch, puzzle_one, startTime);
  auto p2 = std::async(launch, puzzle_two);
  std::cout << "Busses: " << busses.size() << std::endl; // 62
  std::cout << "P1: " << p1.get() << std::endl;          // 2382
  std::cout << "P2: " << p2.get() << std::endl;          // 906332393333683
}
