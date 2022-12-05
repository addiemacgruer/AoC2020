#include "reader.h"
#include <iostream>
#include <regex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

auto field_r = std::regex{"^([a-z ]+): ([0-9]+)-([0-9]+) or ([0-9]+)-([0-9]+)$"};

using Ticket = std::vector<int>;
using Valid = std::vector<bool>;
using Field = std::unordered_map<std::string, Valid>;

auto fields = Field{};
auto me = Ticket{};
auto nearby = std::vector<Ticket>{};

auto make_valid(const std::smatch &sm) {
  auto rval = Valid(1000);
  auto a = std::stoi(sm[2]);
  auto b = std::stoi(sm[3]);
  auto c = std::stoi(sm[4]);
  auto d = std::stoi(sm[5]);
  for (auto i = a; i <= b; ++i)
    rval[i] = true;
  for (auto i = c; i <= d; ++i)
    rval[i] = true;
  return rval;
}

auto make_ticket(const std::string &s) {
  auto rval = Ticket{};
  for (auto &t : al::split(s, ','))
    rval.push_back(std::stoi(std::string{t}));
  return rval;
}

auto read() {
  auto in = std::fstream{"16.txt"};
  auto tmp = std::string{};
  auto sm = std::smatch{};
  while (true) {
    std::getline(in, tmp);
    if (tmp.empty())
      break;
    if (!std::regex_match(tmp, sm, field_r))
      throw std::runtime_error{tmp};
    fields[sm[1]] = make_valid(sm);
  }
  std::getline(in, tmp); // your ticket
  std::getline(in, tmp);
  me = make_ticket(tmp);
  std::getline(in, tmp); // blank
  std::getline(in, tmp); // nearby tickets
  while (std::getline(in, tmp))
    nearby.push_back(make_ticket(tmp));
}

auto valids = std::vector<Ticket>{};

auto accumulate_validation() {
  auto valid = Valid(1000);
  for (auto &f : fields)
    for (auto i = 0; i < 1000; ++i)
      if (f.second[i])
        valid[i] = true;
  return valid;
}

auto puzzle_one() {
  auto rval = long{};
  auto valid = accumulate_validation();
  for (auto &n : nearby) {
    auto valid_ticket = true;
    for (auto &v : n)
      if (!valid[v]) {
        rval += v;
        valid_ticket = false;
      }
    if (valid_ticket)
      valids.push_back(n);
  }
  return rval;
}

using PossibilityMap = std::vector<std::unordered_set<std::string>>;

auto prepare_all_possibilities() {
  auto poss = PossibilityMap(fields.size());
  for (auto &p : poss)
    for (auto &f : fields)
      p.insert(f.first);
  return poss;
}

auto eliminate_invalid_possibilities(PossibilityMap &poss) {
  for (auto &valid : valids)
    for (auto i = size_t{}; i < valid.size(); ++i) {
      auto no = std::vector<std::string>{};
      for (auto &p : poss[i])
        if (!fields[p][valid[i]])
          no.push_back(p);
      for (auto &p : no)
        poss[i].erase(p);
    }
}

auto sieve_multiple_possibilities(PossibilityMap &poss) {
  auto unique = true;
  do {
    unique = true;
    for (auto i = size_t{}; i < poss.size(); ++i) {
      if (poss[i].size() > 1)
        if (poss[i].size() != 1) {
          unique = false;
          continue;
        }
      auto &val = *poss[i].begin();
      for (auto j = size_t{}; j < poss.size(); ++j) {
        if (i == j || poss[j].size() == 1)
          continue;
        poss[j].erase(val);
      }
    }
  } while (!unique);
}

auto count_beginners(PossibilityMap &poss, const std::string &what) {
  auto rval = size_t{1};
  for (auto i = size_t{}; i < poss.size(); ++i)
    if (al::starts_with(*poss[i].begin(), std::string{what}))
      rval *= me[i];
  return rval;
}

auto puzzle_two() {
  auto poss = prepare_all_possibilities();
  eliminate_invalid_possibilities(poss);
  sieve_multiple_possibilities(poss);
  return count_beginners(poss, "departure");
}

int main() {
  read();
  std::cout << "Fields: " << fields.size() << " Nearby: " << nearby.size() << std::endl;
  std::cout << puzzle_one() << " (of " << valids.size() << ')' << std::endl; // 26869
  std::cout << puzzle_two() << std::endl; //  855275529001
}
