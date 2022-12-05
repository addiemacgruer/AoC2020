#include <fstream>
#include <iostream>
#include <string>
#include <vector>

enum class Op { Nop, Jmp, Acc };

struct Ps {
  int pc;
  int acc;
};

auto read_file(const std::string &fname) {
  auto rval = std::vector<std::pair<Op, int>>{};
  auto in = std::fstream{fname};
  auto tmp = std::string{};
  auto val = int{};
  while (in >> tmp >> val) {
    auto op = [](std::string &s) {
      if (s == "nop")
        return Op::Nop;
      if (s == "jmp")
        return Op::Jmp;
      if (s == "acc")
        return Op::Acc;
      throw std::runtime_error(s);
    }(tmp);
    rval.emplace_back(op, val);
  }
  return rval;
}

auto ops = read_file("8.txt");

auto toggled = -1;

auto evaluate(int pc, int acc) -> Ps {
  auto op = ops[pc].first;
  if (pc == toggled)
    op = (op == Op::Nop) ? Op::Jmp : Op::Nop;
  switch (op) {
  case Op::Nop:
    return {pc + 1, acc};
  case Op::Jmp:
    return {pc + ops[pc].second, acc};
  case Op::Acc:
    return {pc + 1, acc + ops[pc].second};
  }
}

auto run() {
  auto touched = std::vector<bool>(ops.size());
  auto ps = Ps{0, 0};
  while (!touched[ps.pc]) {
    touched[ps.pc] = true;
    ps = evaluate(ps.pc, ps.acc);
    if (ps.pc >= ops.size() || ps.pc < 0)
      break;
  }
  return ps;
}

auto part_two() {
  for (auto i = 0; i < ops.size(); ++i) {
    if (ops[i].first == Op::Acc)
      continue;
    toggled = i;
    auto ps = run();
    if (ps.pc == ops.size())
      return ps.acc;
  }
  throw std::runtime_error{"no solution"};
}

int main() {
  std::cout << "Total opcodes: " << ops.size() << '\n'; // 638
  std::cout << "Puzzle one: " << run().acc << '\n';     // 2014
  std::cout << "Puzzle two: " << part_two() << '\n';    // 2251
}
