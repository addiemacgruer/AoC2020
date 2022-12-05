#include "reader.h"
#include <set>

using T = int;
auto fourd = false;

struct Pos {
  T x, y, z, w;
  Pos(T x_, T y_, T z_, T w_) : x{x_}, y{y_}, z{z_}, w{w_} {}
};

bool operator<(const Pos &a, const Pos &b) {
  if (a.x != b.x)
    return a.x < b.x;
  if (a.y != b.y)
    return a.y < b.y;
  if (a.z != b.z)
    return a.z < b.z;
  return a.w < b.w;
}

bool operator==(const Pos &a, const Pos &b) {
  return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
}

using Grid = std::set<Pos>;

auto read() {
  auto rval = Grid{};
  auto input = al::read_file("17.txt");
  auto y = T{};
  for (auto &line : input) {
    for (T x = 0; x < T(line.size()); ++x)
      if (line[x] == '#')
        rval.emplace(x, y, 0, 0);
    ++y;
  }
  return rval;
}

auto count_next(Grid &g, T x, T y, T z, T w) {
  auto rval = 0;
  for (T dw = -1; dw <= 1; ++dw) {
    if (w != 0 && !fourd)
      continue;
    for (T dx = -1; dx <= 1; ++dx)
      for (T dy = -1; dy <= 1; ++dy)
        for (T dz = -1; dz <= 1; ++dz) {
          if (dx == 0 && dy == 0 && dz == 0 && dw == 0)
            continue;
          if (g.find(Pos{x + dx, y + dy, z + dz, w + dw}) != g.end())
            ++rval;
        }
  }
  return rval;
}

auto calc_next(Grid &in) {
  auto out = Grid{};
  T sx = 0, sy = 0, sz = 0, sw = 0, ex = 0, ey = 0, ez = 0, ew = 0;
  for (auto &p : in) {
    sx = std::min(sx, p.x);
    sy = std::min(sy, p.y);
    sz = std::min(sz, p.z);
    sw = std::min(sw, p.w);
    ex = std::max(ex, p.x);
    ey = std::max(ey, p.y);
    ez = std::max(ez, p.z);
    ew = std::max(ew, p.w);
  }
  for (auto w = sw - 1; w <= ew + 1; ++w) {
    if (w != 0 && !fourd)
      continue;
    for (auto z = sz - 1; z <= ez + 1; ++z) {
      for (auto y = sy - 1; y <= ey + 1; ++y) {
        for (auto x = sx - 1; x <= ex + 1; ++x) {
          auto exists = in.find(Pos{x, y, z, w}) != in.end();
          auto c = count_next(in, x, y, z, w);
          if (exists && (c == 2 || c == 3))
            out.emplace(x, y, z, w);
          if (!exists && c == 3)
            out.emplace(x, y, z, w);
        }
      }
    }
  }
  return out;
}

auto puzzle_one(Grid next) {
  for (auto i = 0; i < 6; ++i)
    next = calc_next(next);
  return next.size();
}

int main() {
  auto grid = read();
  std::cout << grid.size() << std::endl;
  std::cout << puzzle_one(grid) << std::endl; // 304
  fourd = true;
  std::cout << puzzle_one(grid) << std::endl; // 1868
}
