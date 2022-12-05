#include <algorithm>
#include <deque>
#include <fstream>
#include <future>
#include <iostream>
#include <map>
#include <set>
#include <string>

using Deck = std::deque<int>;

auto deck1in = Deck{};
auto deck2in = Deck{};

auto read() {
  auto in = std::ifstream{"22.txt"};
  auto tmp = std::string{};
  std::getline(in, tmp); // Player 1:
  while (true) {
    std::getline(in, tmp);
    if (tmp.empty())
      break;
    deck1in.push_back(std::stoi(tmp));
  }
  std::getline(in, tmp); // Player 2:
  while (std::getline(in, tmp))
    deck2in.push_back(std::stoi(tmp));
}

auto totalise(const Deck &deck) {
  auto rval = size_t();
  auto card = deck.size();
  for (auto it = deck.cbegin(); it != deck.cend(); ++it) {
    rval += *it * card;
    --card;
  }
  return rval;
}

auto print_cards(Deck &deck) {
  for (auto card : deck)
    std::cout << ' ' << card;
}

auto puzzle_one(Deck deck1, Deck deck2) {
  while (!deck1.empty() && !deck2.empty()) {
    auto d1 = deck1.front();
    auto d2 = deck2.front();
    auto d1win = d1 > d2;
    if (d1win) {
      deck1.push_back(std::max(d1, d2));
      deck1.push_back(std::min(d1, d2));
    } else {
      deck2.push_back(std::max(d1, d2));
      deck2.push_back(std::min(d1, d2));
    }
    deck1.pop_front();
    deck2.pop_front();
  }
  return deck1.empty() ? totalise(deck2) : totalise(deck1);
}

auto subdeck(const Deck &deck, int num) {
  auto rval = Deck{};
  for (auto it = deck.begin(); num--; ++it)
    rval.push_back(*it);
  return rval;
}

auto checksums = std::vector<std::set<size_t>>{};
auto cached = std::unordered_map<size_t, std::pair<size_t, size_t>>{};

auto duplicate(const Deck &deck1, const Deck &deck2) {
  auto checksum = totalise(deck1) << 32 | totalise(deck2);
  if (std::find(checksums.back().begin(), checksums.back().end(), checksum) !=
      checksums.back().end()) {
    return true;
  }
  checksums.back().insert(checksum);
  return false;
}

auto recurse(Deck deck1, Deck deck2) {
  checksums.push_back({});
  while (!deck1.empty() && !deck2.empty()) {
    auto dupe = duplicate(deck1, deck2);
    if (dupe)
      return std::make_pair(totalise(deck1), size_t{});
    auto d1 = deck1.front();
    auto d2 = deck2.front();
    deck1.pop_front();
    deck2.pop_front();
    auto d1win = d1 > d2;
    if (d1 <= deck1.size() && d2 <= deck2.size()) {
      auto s1d = subdeck(deck1, d1);
      auto s2d = subdeck(deck2, d2);
      auto r = recurse(s1d, s2d);
      d1win = r.first > r.second;
      checksums.pop_back();
    }
    if (d1win) {
      deck1.push_back(d1);
      deck1.push_back(d2);
    } else {
      deck2.push_back(d2);
      deck2.push_back(d1);
    }
  }
  return std::make_pair(totalise(deck1), totalise(deck2));
}

auto puzzle_two(Deck deck1, Deck deck2) {
  auto r = recurse(deck1, deck2);
  return std::max(r.first, r.second);
}

int main() {
  read();
  auto p1 = std::async(std::launch::deferred, puzzle_one, deck1in, deck2in);
  auto p2 = std::async(std::launch::deferred, puzzle_two, deck1in, deck2in);
  std::cout << deck1in.size() << '/' << deck2in.size() << std::endl; // 25/25
  std::cout << p1.get() << std::endl;                                // 32401
  std::cout << p2.get() << std::endl;                                // 31436
}
