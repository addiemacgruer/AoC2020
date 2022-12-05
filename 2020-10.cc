#include "reader.h"
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

using T = long;

auto as = al::read_file<T>("10.txt");

auto puzzle_one() {
  auto one = 0;
  auto three = 0;
  for (auto i = 1; i < as.size(); ++i) {
    auto diff = as[i] - as[i - 1];
    if (diff == 1)
      ++one;
    if (diff == 3)
      ++three;
  }
  return one * three;
}

auto two_memo = std::vector<T>{};

auto puzzle_two(int offset) -> T {
  if (offset + 1 == as.size())
    return 1;
  if (two_memo[offset])
    return two_memo[offset];
  auto rval = T{};
  for (auto i = 1; i <= 3; ++i)
    if ((offset + i < as.size()) && (as[offset + i] - as[offset] <= 3))
      rval += puzzle_two(offset + i);
  two_memo[offset] = rval;
  return rval;
}

int main() {
  std::cout << "Total adapaters: " << as.size() << '\n'; // 97
  as.push_back(*std::max_element(as.begin(), as.end()) + 3);
  as.push_back(0);
  std::sort(as.begin(), as.end());
  two_memo.resize(as.size());
  std::cout << "Puzzle one: " << puzzle_one() << '\n';       // 2040
  std::cout << "Puzzle two: " << puzzle_two(0) << std::endl; // 28346956187648
}
