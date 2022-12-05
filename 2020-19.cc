#include "reader.h"
#include <fstream>
#include <future>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

using T = int;

struct Rule {
  virtual ~Rule() = default;
  virtual std::set<T> match(const char *m) = 0;
};

auto rules = std::map<int, std::unique_ptr<Rule>>{};
auto mess = std::vector<std::string>{};

struct Literal : public Rule {
  char c;
  Literal(char c_) : c{c_} {}
  std::set<T> match(const char *s) override {
    if (c == s[0])
      return {1};
    return {};
  }
};

struct Sequence : public Rule {
  std::vector<int> seq;
  Sequence(const std::string &s) {
    for (auto &t : al::split(s, ' ')) {
      seq.push_back(std::stoi(std::string{t}));
    }
  }

  std::set<T> match(const char *s) override {
    auto rval = std::set<T>{};
    rval.insert(0);
    for (auto rule : seq) {
      auto next = std::set<T>{};
      for (auto offset : rval)
        for (auto match : rules[rule]->match(s + offset))
          next.insert(offset + match);
      //      rval = next;
      std::swap(rval, next);
    }
    return rval;
  }
};

struct Or : public Rule {
  std::vector<std::unique_ptr<Sequence>> sequences{};
  Or(const std::string &s) {
    auto split = s.find('|');
    sequences.push_back(std::make_unique<Sequence>(s.substr(0, split - 1)));
    sequences.push_back(std::make_unique<Sequence>(s.substr(split + 2)));
  }
  std::set<T> match(const char *s) override {
    auto rval = std::set<T>{};
    for (auto &seq : sequences)
      for (auto &m : seq->match(s))
        rval.insert(m);
    return rval;
  }
};

auto make_rule(const std::string &input) -> std::unique_ptr<Rule> {
  if (input[0] == '"')
    return std::make_unique<Literal>(input[1]);
  if (input.find('|') != std::string::npos)
    return std::make_unique<Or>(input);
  return std::make_unique<Sequence>(input);
}

auto read() {
  auto in = std::fstream{"19.txt"};
  auto tmp = std::string{};
  while (true) {
    std::getline(in, tmp);
    if (tmp.empty())
      break;
    auto colon = tmp.find(':');
    int num = std::stoi(tmp.substr(0, colon));
    auto rule = make_rule(tmp.substr(colon + 2));
    rules.insert(std::make_pair(num, std::move(rule)));
  }
  while (std::getline(in, tmp)) {
    mess.push_back(tmp);
  }
};

auto match_message(const std::string &message) {
  auto matches = rules[0]->match(message.c_str());
  auto match_count = std::count_if(
      matches.begin(), matches.end(), [&](size_t l) { return l == message.size(); });
  return match_count >= 1;
}

template <typename It, typename Pred>
auto par_count_if(It first, It last, Pred pred, size_t chunk_sz = 100) {
  auto n = static_cast<size_t>(std::distance(first, last));
  if (n <= chunk_sz)
    return std::count_if(first, last, pred);
  auto middle = std::next(first, n / 2);
  auto future =
      std::async([=, &pred] { return par_count_if(first, middle, pred, chunk_sz); });
  auto num = par_count_if(middle, last, pred, chunk_sz);
  return num + future.get();
}

int main() {
  read();
  std::cout << rules.size() << '/' << mess.size() << std::endl;
  std::cout << par_count_if(mess.begin(), mess.end(), match_message) << std::endl; // 195
  rules[8] = make_rule("42 | 42 8");
  rules[11] = make_rule("42 31 | 42 11 31");
  std::cout << par_count_if(mess.begin(), mess.end(), match_message) << std::endl; //  309
}
