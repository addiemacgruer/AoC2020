#include "reader.h"
#include <algorithm>
#include <future> // alea iacta est
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

using T = std::vector<std::string>;

auto seats = al::read_file<std::string>("11.txt");
auto width = seats[0].size();
auto height = seats.size();

auto next_to(int x, int y, const T &ss) {
  auto rval = 0;
  for (int dx = -1; dx <= 1; ++dx)
    for (int dy = -1; dy <= 1; ++dy) {
      if (dx == 0 && dy == 0)
        continue;
      if (x + dx < 0 || x + dx >= height)
        continue;
      if (y + dy < 0 || y + dy >= width)
        continue;
      if (ss[y + dy][x + dx] == '#')
        ++rval;
    }
  return rval;
}

auto find_in_dir(int x, int y, int dx, int dy, const T &ss) -> std::pair<int, int> {
  while (true) {
    x += dx;
    y += dy;
    if (x < 0 || x >= width)
      return std::make_pair(-1, -1);
    if (y < 0 || y >= height)
      return std::make_pair(-1, -1);
    if (ss[y][x] == 'L')
      return std::make_pair(x, y);
  }
}

auto generate_visible() {
  auto rval = std::unordered_map<int, std::vector<std::pair<int, int>>>{};
  for (auto y = 0; y < height; ++y)
    for (auto x = 0; x < width; ++x) {
      auto here = std::vector<std::pair<int, int>>{};
      if (seats[y][x] == '.')
        continue;
      for (int dx = -1; dx <= 1; ++dx)
        for (int dy = -1; dy <= 1; ++dy) {
          if (dx == 0 && dy == 0)
            continue;
          auto found = find_in_dir(x, y, dx, dy, seats);
          if (found.first != -1)
            here.push_back(found);
        }
      rval[y * width + x] = here;
    }
  return rval;
}

auto visible_map = generate_visible();

auto visible_from(int x, int y, const T &ss) {
  auto rval = 0;
  auto &pos = visible_map[y * width + x];
  for (auto &p : pos) {
    if (ss[p.second][p.first] == '#')
      ++rval;
  }
  return rval;
}

auto iterate_one(T ss) {
  auto next = T(ss.size());
  for (auto y = 0; y < height; ++y) {
    auto row = std::string{};
    for (auto x = 0; x < width; ++x) {
      if (ss[y][x] == '.') {
        row.push_back('.');
        continue;
      }
      auto count = next_to(x, y, ss);
      if (ss[y][x] == 'L' && count == 0) {
        row.push_back('#');
        continue;
      }
      if (ss[y][x] == '#' && count >= 4) {
        row.push_back('L');
        continue;
      }
      row.push_back(ss[y][x]);
    }
    next[y] = row;
  }
  auto same = true;
  for (auto y = 0; y < height; ++y)
    if (ss[y] != next[y]) {
      same = false;
      break;
    }
  if (same)
    return next;
  return iterate_one(std::move(next));
}

auto iterate_two(T ss) {
  auto next = T(ss.size());
  for (auto y = 0; y < height; ++y) {
    auto row = std::string{};
    for (auto x = 0; x < width; ++x) {
      if (ss[y][x] == '.') {
        row.push_back('.');
        continue;
      }
      auto count = visible_from(x, y, ss);
      if (ss[y][x] == 'L' && count == 0) {
        row.push_back('#');
        continue;
      }
      if (ss[y][x] == '#' && count >= 5) {
        row.push_back('L');
        continue;
      }
      row.push_back(ss[y][x]);
    }
    next[y] = row;
  }
  auto same = true;
  for (auto y = 0; y < height; ++y)
    if (ss[y] != next[y]) {
      same = false;
      break;
    }
  if (same)
    return next;
  return iterate_two(std::move(next));
}

template <typename F>
auto puzzle(F f, const T ss) {
  auto answer = f(std::move(ss));
  auto rval = 0;
  for (auto &row : answer)
    rval += std::count(row.begin(), row.end(), '#');
  return rval;
}

auto puzzle_one(const T ss) {
  return puzzle(iterate_one, std::move(ss));
}

auto puzzle_two(const T ss) {
  return puzzle(iterate_two, std::move(ss));
}

int main() {
  std::cout << "Total rows: " << seats.size() << '\n'; // 91
  auto p1 = std::async(std::launch::async, puzzle_one, seats);
  auto p2 = std::async(std::launch::async, puzzle_two, seats);
  std::cout << "Puzzle one: " << p1.get() << '\n';      // 2093
  std::cout << "Puzzle two: " << p2.get() << std::endl; // 1862
}
