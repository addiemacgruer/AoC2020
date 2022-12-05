#include "reader.h"
#include <regex>

constexpr auto BITS = 35;

auto mask_r = std::regex{"^mask = ([X10]+)$"};
auto set_r = std::regex{"^mem\\[([0-9]+)] = ([0-9]+)$"};

struct Instruction {
  virtual ~Instruction() = default;
  virtual void apply() = 0;
  virtual void apply2() = 0;
};

auto mask = std::string{"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"};

struct Mask : public Instruction {
  std::string value;
  Mask(const std::string &v) : value{v} {}
  void apply() override {
    mask = value;
  }
  void apply2() override {
    apply();
  }
};

auto memory = std::unordered_map<long, std::string>{};

auto s2l(const std::string &v) {
  auto rval = uint64_t{};
  for (auto c : v)
    rval = (rval << 1) + (c == '1');
  return rval;
}

auto l2s(long l) {
  auto rval = std::string{};
  for (auto i = 0; i <= BITS; ++i) {
    rval = ((l & 0x1) ? "1" : "0") + rval;
    l >>= 1;
  }
  return rval;
}

auto every(std::vector<std::string> &es, std::string i) {
  auto pos = i.find('X');
  if (pos == -1) {
    es.push_back(std::move(i));
    return;
  }
  i[pos] = '0';
  every(es, i);
  i[pos] = '1';
  every(es, std::move(i));
}

auto every(const std::string &i) {
  auto rval = std::vector<std::string>{};
  every(rval, i);
  return rval;
}

struct Set : public Instruction {
  long location;
  std::string value;

  Set(long l, long v) : location{l}, value{l2s(v)} {}

  void apply() override {
    auto nv = std::string{"zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz"};
    for (auto i = 0; i < mask.size(); ++i)
      switch (mask[i]) {
      case 'X':
        nv[i] = value[i];
        break;
      default:
        nv[i] = mask[i];
        break;
      }
    memory[location] = std::move(nv);
  }

  void apply2() override {
    auto mem = l2s(location);
    auto nv = std::string{"zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz"};
    for (auto i = 0; i < mask.size(); ++i)
      switch (mask[i]) {
      case '0':
        nv[i] = mem[i];
        break;
      case '1':
        nv[i] = '1';
        break;
      case 'X':
        nv[i] = 'X';
        break;
      }
    auto es = every(nv);
    for (auto &e : es)
      memory[s2l(e)] = value;
  }
};

auto read() {
  auto rval = std::vector<std::unique_ptr<Instruction>>{};
  auto in = al::read_file("14.txt");
  auto sm = std::smatch{};
  for (auto &i : in)
    if (std::regex_match(i, sm, mask_r))
      rval.push_back(std::make_unique<Mask>(sm[1]));
    else if (std::regex_match(i, sm, set_r))
      rval.push_back(std::make_unique<Set>(std::stol(sm[1]), std::stol(sm[2])));
    else
      throw std::runtime_error{i};
  return rval;
}

auto ins = decltype(read()){};

template <typename F>
auto puzzle(F f) {
  for (auto &i : ins)
    f(i);
  auto rval = decltype(s2l(std::string{})){};
  for (auto &e : memory)
    rval += s2l(e.second);
  return rval;
}

auto puzzle_one() {
  return puzzle([](auto &i) { i->apply(); });
}

auto puzzle_two() {
  return puzzle([](auto &i) { i->apply2(); });
}

int main() {
  ins = read();
  memory.clear();
  auto p1 = puzzle_one();
  memory.clear();
  auto p2 = puzzle_two();
  std::cout << ins.size() << std::endl;   // 577
  std::cout << "p1: " << p1 << std::endl; // 10035335144067
  std::cout << "p2: " << p2 << std::endl; // 3817372618036
}
