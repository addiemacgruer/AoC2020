#include "reader.h"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

struct Direction {
  char c;
  int d;
  Direction(const std::string &tmp) : c{tmp[0]}, d{std::stoi(tmp.substr(1))} {}
  Direction(char c_, int d_) : c{c_}, d{d_} {}
};

struct Pos {
  int x;
  int y;
  int h;
};

auto dirs = al::read_file<Direction>("12.txt");

auto in_r(int d) {
  auto h = d % 360;
  return h < 0 ? h + 360 : h;
}

auto eval_one(Pos &p, const Direction &d) -> Pos {
  switch (d.c) {
  case 'N':
    return {p.x, p.y - d.d, p.h};
  case 'S':
    return {p.x, p.y + d.d, p.h};
  case 'E':
    return {p.x + d.d, p.y, p.h};
  case 'W':
    return {p.x - d.d, p.y, p.h};
  case 'L':
    return {p.x, p.y, in_r(p.h + d.d)};
  case 'R':
    return {p.x, p.y, in_r(p.h - d.d)};
  case 'F':
    switch (p.h) {
    case 0:
      return eval_one(p, {'E', d.d});
    case 90:
      return eval_one(p, {'N', d.d});
    case 180:
      return eval_one(p, {'W', d.d});
    case 270:
      return eval_one(p, {'S', d.d});
    }
  }
  throw std::runtime_error{"bad_dir"};
}

auto wp = Pos{10, -1, 0};

auto eval_two(Pos &p2, const Direction &d) {
  switch (d.c) {
  case 'N':
    wp = Pos{wp.x, wp.y - d.d, wp.h};
    return p2;
  case 'S':
    wp = Pos{wp.x, wp.y + d.d, wp.h};
    return p2;
  case 'E':
    wp = Pos{wp.x + d.d, wp.y, wp.h};
    return p2;
  case 'W':
    wp = Pos{wp.x - d.d, wp.y, wp.h};
    return p2;
  case 'L':
    return eval_two(p2, {'R', in_r(-d.d)});
  case 'F':
    return Pos{p2.x + wp.x * d.d, p2.y + wp.y * d.d, p2.h};
  case 'R':
    switch (d.d) {
    case 0:
      return p2;
    case 90:
      wp = Pos{-wp.y, wp.x, 0};
      return p2;
    case 180:
      wp = Pos{-wp.x, -wp.y, 0};
      return p2;
    case 270:
      wp = Pos{wp.y, -wp.x, 0};
      return p2;
    }
  }
  throw std::runtime_error{"bad_dir"};
}

template <typename F>
auto puzzle(F f) {
  auto cur = Pos{};
  for (auto &d : dirs)
    cur = f(cur, d);
  return std::abs(cur.x) + std::abs(cur.y);
}

int main() {
  auto p1 = puzzle(eval_one);
  auto p2 = puzzle(eval_two);
  std::cout << "Dirs size: " << dirs.size() << std::endl; // 768
  std::cout << "Part one:  " << p1 << std::endl;          // 441
  std::cout << "Part two:  " << p2 << std::endl;          // 40014
}
