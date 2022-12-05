#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>

const auto INPUT = std::string{"20.txt"};

const auto SM1 = std::string{"                  # "};
const auto SM2 = std::string{"#    ##    ##    ###"};
const auto SM3 = std::string{" #  #  #  #  #  #   "};

using Image = std::vector<std::string>;

// a 'unique id' for a string - fortunately there's no dupes in the input
// used to compare our jigsaw pieces
auto edgeval(const std::string &edge) {
  auto rval = int{};
  for (auto c : edge)
    rval = (rval << 1) + (c == '#');
  auto rval2 = int{};
  for (auto i = edge.rbegin(); i != edge.rend(); ++i)
    rval2 = (rval2 << 1) + (*i == '#');
  return std::max(rval, rval2);
}

auto rotate_right(std::vector<std::string> &input) {
  auto w = input.size();
  auto output = std::vector<std::string>(w);
  for (auto i = size_t{}; i < w; ++i)
    for (auto j = size_t{}; j < w; ++j)
      output[i].push_back(input[w - j - 1][i]);
  return output;
}

auto vertical_flip(const std::vector<std::string> &input) {
  auto output = std::vector<std::string>{};
  for (auto i = ssize_t(input.size()) - 1; i >= 0; --i)
    output.push_back(input[i]);
  return output;
}

struct Tile {
  Image image;
  std::vector<decltype(edgeval(std::string("")))> edges{};

  auto calc_edges() {
    edges.clear();
    auto right = std::string{};
    auto left = std::string{};
    for (auto &line : image) {
      left += line.front();
      right += line.back();
    }
    edges.push_back(edgeval(image.front()));
    edges.push_back(edgeval(right));
    edges.push_back(edgeval(image.back()));
    edges.push_back(edgeval(left));
  }

  Tile(Image image_) : image{image_} {
    calc_edges();
  }

  auto rotate() {
    image = rotate_right(image);
    calc_edges();
  }

  auto flip() {
    image = vertical_flip(image);
    calc_edges();
  }
};

auto read() {
  auto rval = std::map<long, Tile>{};
  auto in = std::fstream{INPUT};
  auto tmp = std::string{};
  while (std::getline(in, tmp)) {
    auto num = std::stoi(tmp.substr(5, tmp.size() - 6)); // Tile XXXX:
    auto image = Image{};
    for (auto i = 0; i < 10; ++i) {
      std::getline(in, tmp);
      image.push_back(tmp);
    }
    std::getline(in, tmp); // blank
    rval.insert(std::make_pair(num, Tile(std::move(image))));
  }
  return rval;
}

auto tiles = read();
auto width = int(std::sqrt(tiles.size()));

// find the four pieces that have two edge ids that don't match any other piece.
// sort out the rest of things for part two while we're at it...

auto edgeCount = std::map<int, int>{};
auto corners = std::vector<int>{};
auto edges = std::vector<int>{};
auto middles = std::vector<int>{};

auto puzzle_one() {
  for (auto &tile : tiles)
    for (auto edge : tile.second.edges)
      ++edgeCount[edge];
  for (auto &tile : tiles) {
    auto edgecount = 0;
    for (auto edge : tile.second.edges) {
      if (edgeCount[edge] == 1)
        ++edgecount;
    }
    switch (edgecount) {
    case 0:
      middles.push_back(tile.first);
      break;
    case 1:
      edges.push_back(tile.first);
      break;
    case 2:
      corners.push_back(tile.first);
      break;
    default:
      throw std::runtime_error{"oh dear"};
      break;
    }
  }
  auto rval = size_t{1};
  for (auto corner : corners)
    rval *= corner;
  return rval;
}

// reconstruct the image for part two

auto find_top_left() {
  for (auto corner : corners) {
    auto cornerTile = tiles.at(corner);
    if (edgeCount[cornerTile.edges[0]] == 1 && edgeCount[cornerTile.edges[3]] == 1)
      return corner;
  }
  throw std::runtime_error{"oh no"};
}

auto image = std::vector<long>{};

auto get_tile_with_id(std::vector<int> &group, int id) {
  for (auto &tile : group)
    for (auto edge : tiles.at(tile).edges)
      if (edge == id) {
        auto rval = tile;
        group.erase(std::find(group.begin(), group.end(), tile));
        return rval;
      }
  throw std::runtime_error{"didn't find tile"};
}

auto reconstruct() {
  image.resize(tiles.size());
  auto tl = find_top_left();
  auto tlloc = std::find(corners.begin(), corners.end(), tl);
  corners.erase(tlloc);
  image[0] = tl;
  for (auto y = 0; y < width; ++y)
    for (auto x = 0; x < width; ++x) {
      auto id = y * width + x;
      if (image[id] != 0) {
        continue;
      }
      // prep
      auto xedge = x == 0 || x == width - 1;
      auto yedge = y == 0 || y == width - 1;
      auto corner = xedge && yedge;
      auto leftid = x == 0 ? -1 : tiles.at(image[id - 1]).edges[1];
      auto topid = y == 0 ? -1 : tiles.at(image[id - width]).edges[2];
      // get the next tile
      auto tile = int{};
      if (corner)
        tile = get_tile_with_id(corners, std::max(leftid, topid));
      else if (xedge || yedge)
        tile = get_tile_with_id(edges, std::max(leftid, topid));
      else
        tile = get_tile_with_id(middles, std::max(leftid, topid));
      // make matching left
      if (leftid == -1)
        while (edgeCount[tiles.at(tile).edges[3]] != 1) {
          tiles.at(tile).rotate();
        }
      else {
        while (leftid != tiles.at(tile).edges[3])
          tiles.at(tile).rotate();
      }
      // make matching top
      if (topid == -1)
        while (edgeCount[tiles.at(tile).edges[0]] != 1) {
          tiles.at(tile).flip();
        }
      else {
        while (topid != tiles.at(tile).edges[0])
          tiles.at(tile).flip();
      }
      image[id] = tile;
    }
  auto rval = std::vector<std::string>{};
  for (auto y = 0; y < width; ++y)
    for (auto line = 1; line < 9; ++line) {
      auto wip = std::string{};
      for (auto x = 0; x < width; ++x) {
        wip += tiles.at(image[y * width + x]).image[line].substr(1, 8);
      }
      rval.push_back(wip);
    }
  return rval;
}

// see if we have any monsters at all...

auto all_in(const std::string &in, const std::string &test, int offset) {
  for (auto i = 0; i < test.size(); ++i)
    if (test[i] == '#' && in[offset + i] != '#')
      return false;
  return true;
}

auto monster_match(const std::vector<std::string> &image, int x, int y) {
  return all_in(image[y], SM2, x) && all_in(image[y + 1], SM3, x) &&
         all_in(image[y - 1], SM1, x);
}

auto has_monster(const std::vector<std::string> &image) {
  for (auto y = 1; y < image.size() - 1; ++y)
    for (auto x = 0; x < image.size() - SM2.size(); ++x) {
      if (monster_match(image, x, y))
        return true;
    }
  return false;
}

// flip the image about until we have one with monsters in

auto normalise(std::vector<std::string> recon) {
  for (auto j = 0; j < 2; ++j) {
    for (auto i = 0; i <= 4; ++i) {
      if (has_monster(recon))
        return recon;
      recon = rotate_right(recon);
    }
    recon = vertical_flip(recon);
  }
  throw std::runtime_error{"cannot reconstruct"};
}

// finally, remove any matched monsters from the image, and count the waves.

auto erase_in(std::string &in, const std::string &test, int offset) {
  for (auto i = 0; i < test.size(); ++i)
    if (test[i] == '#')
      in[offset + i] = ' ';
}

auto erase_monster(std::vector<std::string> &image, int x, int y) {
  erase_in(image[y - 1], SM1, x);
  erase_in(image[y + 0], SM2, x);
  erase_in(image[y + 1], SM3, x);
}

auto puzzle_two(std::vector<std::string> copy) {
  for (auto y = 1; y < copy.size() - 1; ++y)
    for (auto x = 0; x < copy.size() - SM2.size(); ++x)
      if (monster_match(copy, x, y))
        erase_monster(copy, x, y);
  auto rval = size_t{};
  for (auto &line : copy)
    rval += std::count(line.begin(), line.end(), '#');
  return rval;
}

int main() {
  std::cout << tiles.size() << std::endl;
  std::cout << puzzle_one() << std::endl; // 29584525501199
  auto recon = reconstruct();
  recon = normalise(recon);
  std::cout << puzzle_two(recon) << std::endl; // 1665
}
