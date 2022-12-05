#include "reader.h"
#include <fstream>
#include <iostream>
#include <unordered_set>
#include <vector>

/*
 *   |  0,1  |  1, 1 |
 *    \     / \     / 
 *     \   /   \   /   
 *      \ /     \ /     
 *       |       |
 * -1, 0 |  0, 0 |  1, 0 
 *       |       |
 *      / \     / \
 *     /   \   /   \
 *    /     \ /     \
 *   | -1,-1 |  0,-1 |
 */
auto input = al::read_file("24.txt");

using Coord = std::pair<int, int>;
using Floor = std::unordered_set<Coord>;
enum Dir { E, SE, SW, W, NW, NE };

namespace std {
template <>
struct hash<Coord> {
  size_t operator()(const Coord &c) const {
    return *reinterpret_cast<const size_t *>(&c.first) << 32 |
           *reinterpret_cast<const size_t *>(&c.second);
  }
};
} // namespace std

auto tokenise(const std::string &input) {
  auto rval = std::vector<Dir>{};
  for (auto i = 0; i < input.size(); ++i)
    if (input[i] == 'e')
      rval.push_back(Dir::E);
    else if (input[i] == 'w')
      rval.push_back(Dir::W);
    else if (input[i] == 's') {
      ++i;
      if (input[i] == 'e')
        rval.push_back(Dir::SE);
      else
        rval.push_back(Dir::SW);
    } else {
      ++i;
      if (input[i] == 'e')
        rval.push_back(Dir::NE);
      else
        rval.push_back(Dir::NW);
    }
  return rval;
}

auto parse_line(const std::string &input) {
  auto x = 0, y = 0;
  for (auto &dir : tokenise(input)) {
    switch (dir) {
    case Dir::E:
      ++x;
      break;
    case Dir::SE:
      --y;
      break;
    case Dir::SW:
      --x;
      --y;
      break;
    case Dir::W:
      --x;
      break;
    case Dir::NW:
      ++y;
      break;
    case Dir::NE:
      ++x;
      ++y;
      break;
    }
  }
  return std::make_pair(x, y);
}

auto puzzle_one() {
  auto floor = Floor{};
  for (auto &line : input) {
    auto parse = parse_line(line);
    auto it = floor.find(parse);
    if (it == floor.end())
      floor.insert(parse);
    else
      floor.erase(it);
  }
  return floor;
}

auto is_set(const Floor &floor, const Coord &coord) {
  return floor.find(coord) != floor.end();
}

auto operator+(const Coord &a, const Coord &b) {
  return Coord{a.first + b.first, a.second + b.second};
}

auto adjacents = std::vector<Coord>{{1, 0}, {0, -1}, {-1, -1}, {-1, 0}, {0, 1}, {1, 1}};

auto count_adjacent(const Floor &floor, const Coord &coord) {
  auto rval = 0;
  for (auto &adjacent : adjacents)
    if (is_set(floor, coord + adjacent))
      ++rval;
  return rval;
}

auto next(const Floor &floor) {
  auto rval = Floor{};
  auto minx = 0, miny = 0, maxx = 0, maxy = 0;
  for (auto &tile : floor) {
    minx = std::min(tile.first, minx);
    maxx = std::max(tile.first, maxx);
    miny = std::min(tile.second, miny);
    maxy = std::max(tile.second, maxy);
  }
  --minx;
  ++maxx;
  --miny;
  ++maxy;
  for (auto x = minx; x <= maxx; ++x)
    for (auto y = miny; y <= maxy; ++y) {
      auto pos = Coord{x, y};
      auto adjacent = count_adjacent(floor, pos);
      if ((is_set(floor, pos) && adjacent == 1) || adjacent == 2)
        rval.insert(pos);
    }
  return rval;
}

auto puzzle_two(Floor &floor) {
  for (auto i = 0; i < 100; ++i)
    floor = next(floor);
  return floor.size();
}

int main() {
  std::cout << "Instruction list: " << input.size() << std::endl;
  auto floor = puzzle_one();
  std::cout << "P1: " << floor.size() << std::endl;
  auto p2 = puzzle_two(floor);
  std::cout << "P2: " << p2 << std::endl;
}
