#include <iostream>
#include <vector>

using T = uint32_t;

auto m(const std::vector<T> &start, size_t end) {
  auto turns = std::vector<T>(end);
  for (auto i = T{}; i < start.size(); ++i)
    turns[start[i]] = i + 1;
  auto last = start.back();
  auto prev = T{};
  for (T i = start.size() + 1; i <= end; ++i) {
    last = prev;
    prev = (turns[prev] == 0) ? 0 : i - turns[prev];
    turns[last] = i;
  }
  return last;
}

int main() {
  std::cout << m({0, 13, 1, 16, 6, 17}, 2020) << std::endl;     // 234
  std::cout << m({0, 13, 1, 16, 6, 17}, 30000000) << std::endl; // 8984
}
