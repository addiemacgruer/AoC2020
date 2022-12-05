#include "reader.h"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

struct Card {
  int row{};
  int column{};

  Card(const std::string &input) {
    for (auto x = 0; x < 7; ++x) {
      row <<= 1;
      if (input[x] == 'B')
        row += 1;
    }
    for (auto x = 0; x < 3; ++x) {
      column <<= 1;
      if (input[x + 7] == 'R')
        column += 1;
    }
  }

  int id() const {
    return row * 8 + column;
  }
};

auto cards = al::read_file<Card>("5.txt");

template <class T>
auto kernighan(T v) {
  auto c = T{};
  for (; v; ++c)
    v &= v - 1;
  return c;
}

template <class T>
auto bit(T v) {
  auto c = T{};
  while ((v & 0x1) == 0) {
    ++c;
    v >>= 1;
  }
  return c;
}

int main() {
  std::cout << "Total boarding cards: " << cards.size() << '\n'; // 846
  auto p1 = 0;
  for (auto &card : cards)
    p1 = std::max(p1, card.id());
  std::cout << "Puzzle one: " << p1 << '\n'; // 858

  auto all = std::vector<uint8_t>{};

  for (auto &card : cards) {
    while (all.size() <= card.row)
      all.push_back(0);
    all[card.row] |= (1 << card.column);
  }

  for (auto x = 0; x < all.size(); ++x) {
    auto &row = all[x];
    if (kernighan(row) == 7) {
      auto empty = bit(~row);
      std::cout << "Puzzle two: " << (x * 8 + empty) << std::endl; // 557
      break;
    }
  }
}
