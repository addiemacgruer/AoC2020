#include "reader.h"
#include <iostream>
#include <string>
#include <vector>

auto trees = al::read_file<std::string>("3.txt");

auto puzz(int dx, int dy) {
  auto x = 0;
  auto y = 0;
  auto t = long{};
  while (y < trees.size()) {
    if (trees[y][x] == '#')
      ++t;
    x = (x + dx) % trees[0].size();
    y += dy;
  }
  return t;
}

int main(int, char **) {
  auto x = puzz(3, 1);
  std::cout << "Puzzle one: " << x << '\n';
  auto y = puzz(1, 1) * x * puzz(5, 1) * puzz(7, 1) * puzz(1, 2);
  std::cout << "Puzzle two: " << y << std::endl;
}
