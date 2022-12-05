#include <fstream>
#include <future>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

using Answer = std::vector<int>;

auto reset(Answer &vec) {
  std::fill(vec.begin(), vec.end(), 0);
}

template <typename F>
auto test(F f, Answer &qs, size_t count, const Answer &group) {
  for (auto i = 0; i < qs.size(); ++i)
    if (f(group[i], count))
      qs[i] += 1;
};

template <typename F>
auto read_file(F f, const std::string &fname) {
  auto qs = Answer(26);
  auto in = std::fstream{fname};
  auto tmp = std::string{};
  auto group = Answer(26);
  auto count = size_t{0};
  while (std::getline(in, tmp)) {
    if (tmp.empty()) {
      test(f, qs, count, group);
      reset(group);
      count = 0;
      continue;
    }
    ++count;
    for (auto c : tmp)
      group[c - 'a'] += 1;
  }
  test(f, qs, count, group);
  return qs;
}

template <typename F>
auto summate(F f) {
  auto qs = read_file(f, "6.txt");
  return std::accumulate(qs.begin(), qs.end(), 0);
}

auto puzzle_one() {
  return summate([](auto a, auto b) { return a > 0; });
}

auto puzzle_two() {
  return summate([](auto a, auto b) { return a == b; });
}

int main() { // 6534, 3402
  auto p1 = std::async(std::launch::async, puzzle_one);
  auto p2 = std::async(std::launch::async, puzzle_two);
  std::cout << "Part one: " << p1.get() << "\nPart two: " << p2.get() << '\n';
}
