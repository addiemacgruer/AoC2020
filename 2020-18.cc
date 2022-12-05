#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

using T = int64_t;

auto ops = std::vector<char>{};
auto vals = std::vector<T>{};
auto val2 = std::vector<std::vector<T>>{};

struct Exp {
  virtual ~Exp() = default;
  virtual void apply() = 0;
  virtual void apply2() = 0;
};

struct Paren : public Exp {
  char c;
  Paren(char c_) : c{c_} {}
  void apply() override {
    if (c == '(') {
      ops.push_back('+');
      vals.push_back(0);
    } else {
      ops.pop_back();
      T val = vals.back();
      vals.pop_back();
      if (ops.back() == '+')
        vals.back() += val;
      else
        vals.back() *= val;
    }
  }

  void apply2() override {
    if (c == '(') {
      ops.push_back('+');
      val2.push_back(std::vector<T>{0});
    } else {
      ops.pop_back();
      T val = 1;
      for (auto v : val2.back()) {
        val *= v;
      }
      val2.pop_back();
      if (ops.back() == '+')
        val2.back().back() += val;
      else
        val2.back().push_back(val);
    }
  }
};

struct Op : public Exp {
  char c;
  Op(char c_) : c{c_} {}
  void apply() override {
    ops.back() = c;
  }
  void apply2() override {
    ops.back() = c;
  }
};

struct Num : public Exp {
  T num;
  Num(T num_) : num{num_} {}
  void apply() override {
    if (ops.back() == '+')
      vals.back() += num;
    else
      vals.back() *= num;
  };

  void apply2() override {
    if (ops.back() == '+')
      val2.back().back() += num;
    else
      val2.back().push_back(num);
  }
};

using ExpList = std::vector<std::unique_ptr<Exp>>;

auto isnum(char c) {
  return c >= '0' && c <= '9';
}

auto parse_exp(const std::string &input) {
  auto rval = ExpList{};
  for (auto i = size_t{}; i < input.size(); ++i) {
    if (input[i] == ' ')
      continue;
    if (input[i] == '(' || input[i] == ')') {
      rval.push_back(std::make_unique<Paren>(input[i]));
      continue;
    }
    if (input[i] == '+' || input[i] == '*') {
      rval.push_back(std::make_unique<Op>(input[i]));
      continue;
    }
    if (isnum(input[i])) {
      T num = input[i] - '0';
      while (isnum(input[i + 1])) {
        ++i;
        num *= 10 + (input[i] - '0');
      }
      rval.push_back(std::make_unique<Num>(num));
      continue;
    }
    throw std::runtime_error{"bad parse!"};
  }
  return rval;
}

auto read() {
  auto rval = std::vector<ExpList>{};
  auto in = std::fstream{"18.txt"};
  auto tmp = std::string{};
  while (std::getline(in, tmp)) {
    rval.push_back(parse_exp(tmp));
  }
  return rval;
}

auto eval(ExpList &exp) {
  ops.clear();
  ops.push_back('+');
  vals.clear();
  vals.push_back(0);
  for (auto &e : exp)
    e->apply();
  return vals[0];
}

auto puzzle_one(std::vector<ExpList> &exps) {
  auto rval = T{};
  for (auto &exp : exps)
    rval += eval(exp);
  return rval;
}

auto eval2(ExpList &exp) {
  ops.clear();
  ops.push_back('+');
  val2.clear();
  val2.push_back(std::vector<T>{0});
  for (auto &e : exp)
    e->apply2();
  auto f = T{1};
  for (auto v : val2[0])
    f *= v;
  return f;
}

auto puzzle_two(std::vector<ExpList> &exps) {
  auto rval = T{};
  for (auto &exp : exps)
    rval += eval2(exp);
  return rval;
}

int main() {
  auto exps = read();
  std::cout << exps.size() << std::endl;
  std::cout << puzzle_one(exps) << std::endl; // 202553439706
  std::cout << puzzle_two(exps) << std::endl; // 88534268715686
}
