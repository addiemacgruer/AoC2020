#include "reader.h"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

using PairMap = std::unordered_map<std::string, std::string>;

auto read_file(const std::string &fname) {
  auto in = std::fstream{fname};
  auto rval = std::vector<PairMap>{};
  auto wip = PairMap{};
  auto tmp = std::string{};
  while (std::getline(in, tmp)) {
    if (tmp.empty()) {
      rval.push_back(wip);
      wip = PairMap{};
      continue;
    }
    auto kvs = al::split(tmp, ' ');
    for (auto &kv : kvs) {
      auto colon = kv.find(':');
      wip[std::string{kv.substr(0, colon)}] = kv.substr(colon + 1);
    }
  }
  if (!wip.empty())
    rval.push_back(wip);
  return rval;
}

auto passports = read_file("4.txt");

auto valid_one(const PairMap &passport) {
  if (passport.size() == 8)
    return true;
  if (passport.size() == 7 && passport.find("cid") == passport.end())
    return true;
  return false;
}

auto cm = std::string{"cm"};
auto in = std::string{"in"};

auto is_valid_height(const std::string &height) {
  if (al::ends_with(height, cm)) {
    auto cm = std::stoi(height.substr(0, height.size() - 2));
    return cm >= 150 && cm <= 193;
  }
  if (al::ends_with(height, in)) {
    auto in = std::stoi(height.substr(0, height.size() - 2));
    return in >= 59 && in <= 76;
  }
  return false;
}

auto is_valid_hcl(std::string &hcl) {
  if (hcl.size() != 7)
    return false;
  if (hcl[0] != '#')
    return false;
  for (auto i = 1; i < 7; ++i) {
    auto c = hcl[i];
    auto hx = (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f');
    if (!hx)
      return false;
  }
  return true;
}

auto ecl = std::vector<std::string>{"amb", "blu", "brn", "gry", "grn", "hzl", "oth"};

auto is_valid_pid(std::string &pid) {
  if (pid.size() != 9)
    return false;
  for (auto i = 0; i < 9; ++i) {
    auto c = pid[i];
    auto num = (c >= '0' && c <= '9');
    if (!num)
      return false;
  }
  return true;
}

auto valid_two(PairMap &p) {
  if (!valid_one(p))
    return false;
  auto byrv = p["byr"];
  auto byr = std::stoi(p["byr"]);
  if (byr < 1920 || byr > 2002)
    return false;
  auto iyr = std::stoi(p["iyr"]);
  if (iyr < 2010 || iyr > 2020)
    return false;
  auto eyr = std::stoi(p["eyr"]);
  if (eyr < 2020 || eyr > 2030)
    return false;
  if (!is_valid_height(p["hgt"]))
    return false;
  if (!is_valid_hcl(p["hcl"]))
    return false;
  if (std::find(ecl.begin(), ecl.end(), p["ecl"]) == ecl.end())
    return false;
  if (!is_valid_pid(p["pid"]))
    return false;
  return true;
}

int main() {
  std::cout << passports.size() << std::endl; // 295
  std::cout << "Part one: "
            << std::count_if(passports.begin(), passports.end(),
                   [](PairMap &p) { return valid_one(p); })
            << std::endl; // 264
  std::cout << "Part two: "
            << std::count_if(passports.begin(), passports.end(),
                   [](PairMap &p) { return valid_two(p); })
            << std::endl; //224
}
