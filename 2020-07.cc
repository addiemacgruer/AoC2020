#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using BagCount = std::pair<int, std::string>;

auto split(const std::string &input, char delim) {
  auto rval = std::vector<std::string>{};
  auto wip = std::string{};
  for (auto c : input) {
    if (c == delim) {
      rval.push_back(wip);
      wip.clear();
      continue;
    }
    wip += c;
  }
  if (!wip.empty())
    rval.push_back(wip);
  return rval;
}

auto ends_with(std::string const &value, std::string const &ending) {
  if (ending.size() > value.size())
    return false;
  return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

auto read_file(const std::string &fname) {
  auto in = std::fstream{fname};
  auto rval = std::unordered_map<std::string, std::vector<BagCount>>{};
  auto tmp = std::string{};
  while (std::getline(in, tmp)) {
    auto words = std::stringstream{tmp};
    auto word = std::string{};
    auto outercolour = std::string{};
    while (words >> word) {
      if (word == "bags")
        break;
      outercolour += (outercolour.empty() ? "" : " ") + word;
    }
    words >> word; // contain
    auto containees = std::vector<BagCount>{};
    auto innercolour = std::string{};
    while (words >> word) {
      if (word == "no") // no other bags
        break;
      auto count = std::stoi(word);
      while (words >> word) {
        if (word.rfind("bag", 0) == 0) // bag bag. bags, bags.
          break;
        innercolour += (innercolour.empty() ? "" : " ") + word;
      }
      containees.emplace_back(count, innercolour);
      innercolour.clear();
    }
    rval[outercolour] = containees;
  }
  return rval;
}

auto rules = read_file("7.txt");

using SSet = std::unordered_set<std::string>;

void contains(SSet &set, const std::string &kind) {
  if (set.find(kind) != set.end()) // already done
    return;
  for (auto &rule : rules)
    for (auto &c : rule.second) {
      if (c.second == kind) {
        contains(set, rule.first);
        set.insert(rule.first);
      }
    }
}

size_t inside(std::string &which) {
  auto rval = size_t{1};
  auto &cs = rules[which];
  for (auto &c : cs) {
    rval += c.first * inside(c.second);
  }
  return rval;
}

int main() {
  std::cout << "Rules: " << rules.size() << std::endl;
  auto mine = std::string{"shiny gold"};
  auto p1 = SSet{};
  contains(p1, mine);
  std::cout << "Part one: " << p1.size() << std::endl; // 246
  auto p2 = inside(mine);
  std::cout << "Part two: " << (p2 - 1) << std::endl; // 2976
}
