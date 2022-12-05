#include "reader.h"
#include <algorithm>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

using T = long;

auto nums = al::read_file<T>("9.txt");
constexpr auto preamble = 25;

auto check_one(int i) {
  for (auto x = i - preamble; x < i; ++x)
    for (auto y = x + 1; y < i; ++y)
      if (nums[x] + nums[y] == nums[i])
        return true;
  return false;
}

auto puzzle_one() {
  for (auto i = preamble; i < nums.size(); ++i)
    if (!check_one(i))
      return nums[i];
  throw std::runtime_error{"no solution"};
}

auto puzzle_two(T p1) {
  for (auto i = 0; i < nums.size(); ++i) {
    auto total = T{};
    for (auto j = i; j < nums.size(); ++j) {
      total += nums[j];
      if (total < p1)
        continue;
      if (total > p1)
        break;
      auto min = *std::min_element(nums.begin() + i, nums.begin() + j);
      auto max = *std::max_element(nums.begin() + i, nums.begin() + j);
      return min + max;
    }
  }
  throw std::runtime_error{"no solution"};
}

int main() {
  auto p1 = puzzle_one();
  std::cout << "Total nums: " << nums.size() << '\n';         // 1000
  std::cout << "Puzzle one: " << p1 << '\n';                  // 22406676
  std::cout << "Puzzle two: " << puzzle_two(p1) << std::endl; // 2942387
}
