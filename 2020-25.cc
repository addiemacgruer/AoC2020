#include <iostream>

constexpr auto p1 = size_t{17773298};
constexpr auto p2 = size_t{15530095};

constexpr auto next(size_t input, size_t subject = 7) {
  return (input * subject) % 20201227;
}

constexpr auto puzzle_one(size_t input, size_t target) {
  auto rval = size_t{};
  while (input != target) {
    ++rval;
    input = next(input);
  }
  return rval;
}

constexpr auto puzzle_two(size_t start, size_t subject, size_t loops) {
  for (auto i = 1; i < loops; ++i)
    start = next(start, subject);
  return start;
}

int main() {
  auto puzz1 = puzzle_one(1, p1);
  auto puzz2 = puzzle_one(1, p2);
  std::cout << puzz1 << std::endl;
  std::cout << puzz2 << std::endl;
  auto encryption1 = puzzle_two(p1, p1, puzz2);
  auto encryption2 = puzzle_two(p2, p2, puzz1);
  std::cout << encryption1 << " = " << encryption2 << std::endl; // 17980581
}
