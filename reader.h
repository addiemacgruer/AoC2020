#pragma once

#include <algorithm>
#include <fstream>
#include <iostream>
#include <memory>
#include <numeric>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

namespace al {

template <class T = std::string>
auto read_file(const std::string &fname) {
  auto in = std::fstream{fname};
  auto rval = std::vector<T>{};
  auto tmp = std::string{};
  while (std::getline(in, tmp))
    rval.emplace_back(tmp);
  return rval;
}

template <typename F>
auto read_file(F f, const std::string &fname) {
  using T = decltype(f(std::string{}));
  auto in = std::fstream{fname};
  auto rval = std::vector<T>{};
  auto tmp = std::string{};
  while (std::getline(in, tmp))
    rval.emplace_back(f(tmp));
  return rval;
}

template <>
auto read_file<long>(const std::string &fname) {
  return read_file([](const std::string &t) { return std::stol(t); }, fname);
}

template <class T>
auto starts_with(const T &s, const T &e) {
  return e.size() <= s.size() && std::equal(e.begin(), e.end(), s.begin());
}

template <class T>
auto ends_with(const T &s, const T &e) {
  return e.size() <= s.size() && std::equal(e.rbegin(), e.rend(), s.rbegin());
}

inline auto split(const std::string_view &input, char delim) {
  auto rval = std::vector<std::string_view>{};
  auto start = size_t{};
  for (auto end = size_t{}; end < input.size(); ++end) {
    if (input[end] == delim) {
      rval.emplace_back(input.begin() + start, end - start);
      start = end + 1;
      continue;
    }
  }
  rval.emplace_back(input.begin() + start, input.size() - start);
  return rval;
}

inline auto stol(const std::string_view &s) {
  return std::stol(std::string{s});
}

inline auto stol(const std::string &s) {
  return std::stol(s);
}

struct PipeDeleter {
  auto operator()(FILE *f) {
    return pclose(f);
  }
};

inline std::string exec(const char *cmd) {
  auto buffer = std::array<char, 128>{};
  auto result = std::string{};
  auto pipe = std::unique_ptr<FILE, PipeDeleter>(popen(cmd, "r"));
  if (!pipe)
    throw std::runtime_error("popen() failed!");
  while (fgets(buffer.data(), buffer.size(), pipe.get()))
    result += buffer.data();
  return result;
}

} // namespace al
