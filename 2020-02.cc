#import <algorithm>
#import <fstream>
#import <future>
#import <iostream>
#import <vector>

class Password {
  int min;
  int max;
  char c;
  std::string passwd;

public:
  Password(int min_, int max_, char c_, std::string &passwd_)
      : min{min_}, max{max_}, c{c_}, passwd{std::move(passwd_)} {}

  auto is_valid_one() const {
    auto match = std::count(passwd.begin(), passwd.end(), c);
    return match >= min && match <= max;
  }

  auto is_valid_two() const {
    auto pos1 = passwd[min - 1] == c;
    auto pos2 = passwd[max - 1] == c;
    return pos1 != pos2;
  }
};

auto readfile(const std::string &fname) {
  auto in = std::fstream{fname};
  auto rval = std::vector<Password>();
  int min, max;
  char c, discard;
  auto passwd = std::string{};
  while (in >> min >> discard >> max >> c >> discard >> passwd)
    rval.emplace_back(min, max, c, passwd);
  return rval;
}

int main(int argc, char **argv) {
  auto r = readfile(argc == 2 ? argv[1] : "2.txt");
  auto p1 = [&]() {
    return std::count_if(r.begin(), r.end(),
                         [](auto &p) { return p.is_valid_one(); });
  };
  auto p2 = [&]() {
    return std::count_if(r.begin(), r.end(),
                         [](auto &p) { return p.is_valid_two(); });
  };
  std::cout << "Total: " << r.size() << '\n';     // 1000
  std::cout << "Part one: " << p1() << '\n';      // 600
  std::cout << "Part two: " << p2() << std::endl; // 245
}
