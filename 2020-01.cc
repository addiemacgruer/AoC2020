#include "reader.h"
#include <iostream>
#include <vector>

auto r = al::read_file<long>("1.txt");
auto size = r.size();

auto puzzle_one() {
  for (auto x = 0; x < size; ++x)
    for (auto y = x + 1; y < size; ++y)
      if (r[x] + r[y] == 2020) {
        std::cout << r[x] << 'x' << r[y] << '=' << (r[x] * r[y]) << '\n';
        return;
      }
}

auto puzzle_two() {
  for (auto x = 0; x < size; ++x)
    for (auto y = x + 1; y < size; ++y)
      for (auto z = y + 1; z < size; ++z)
        if (r[x] + r[y] + r[z] == 2020) {
          std::cout << r[x] << 'x' << r[y] << 'x' << r[z] << '=' << (r[x] * r[y] * r[z])
                    << '\n';
          return;
        }
}

int main() {
  puzzle_one(); // 889779
  puzzle_two(); // 76110336
}
