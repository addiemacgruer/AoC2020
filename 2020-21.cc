#include "reader.h"
#include <algorithm>
#include <iostream>
#include <map>
#include <set>
#include <vector>

const auto FILENAME = std::string{"21.txt"};

using List = std::vector<std::string>;

struct Food {
  List ingredients;
  List allergens;
  Food(const List i_, const List a_) : ingredients{i_}, allergens{a_} {
    std::sort(ingredients.begin(), ingredients.end());
    std::sort(allergens.begin(), allergens.end());
  }
};

using FoodList = std::vector<Food>;

auto read() {
  auto rval = FoodList{};
  auto in = al::read_file(FILENAME);
  for (auto &line : in) {
    auto ing = List{};
    auto all = List{};
    auto split = al::split(line, ' ');
    while (split.front() != "(contains") {
      ing.emplace_back(split.front());
      split.erase(split.begin());
    }
    split.erase(split.begin()); // (contains
    while (!split.empty()) {
      auto &first = split.front();
      if (first.back() == ')' || first.back() == ',')
        all.emplace_back(first.substr(0, first.size() - 1));
      else
        all.emplace_back(first);
      split.erase(split.begin());
    }
    rval.emplace_back(std::move(ing), std::move(all));
  }
  return rval;
}

auto all_allergens(const FoodList &foods) {
  auto rval = std::set<std::string>{};
  for (auto &food : foods)
    for (auto &a : food.allergens)
      rval.insert(a);
  return rval;
}

auto all_ingredients(const FoodList &foods) {
  auto rval = std::set<std::string>{};
  for (auto &food : foods)
    for (auto &i : food.ingredients)
      rval.insert(i);
  return rval;
}

auto find_always(const FoodList &foods, const std::string &all) {
  auto possible = List{};
  for (auto &food : foods) {
    if (std::find(food.allergens.begin(), food.allergens.end(), all) ==
        food.allergens.end())
      continue;
    if (possible.empty()) {
      for (auto &ing : food.ingredients)
        possible.push_back(ing);
      continue;
    }
    auto new_possible = decltype(possible){};
    std::set_intersection(possible.begin(), possible.end(),
                          food.ingredients.begin(), food.ingredients.end(),
                          std::back_inserter(new_possible));
    std::swap(possible, new_possible);
  }
  return possible;
}

auto eliminate(std::map<std::string, List> potentials) { // copy
  while (true) {
    auto eliminated = true;
    for (auto &p : potentials) {
      if (p.second.size() != 1) {
        eliminated = false;
      } else {
        auto elim = p.second.front();
        for (auto &q : potentials) {
          if (q.second.size() == 1)
            continue;
          auto it = std::find(q.second.begin(), q.second.end(), elim);
          if (it != q.second.end())
            q.second.erase(it);
        }
      }
    }
    if (eliminated)
      return potentials;
  }
}

auto determine_allergens(const std::set<std::string> &allergens,
                         const FoodList &foods) {
  auto potentials = std::map<std::string, List>{};
  for (auto &all : allergens)
    potentials[all] = find_always(foods, all);
  return eliminate(potentials);
}

auto puzzle_one(const FoodList &foods, List &allergens) {
  auto rval = size_t{};
  for (auto &food : foods)
    for (auto &ing : food.ingredients)
      if (std::find(allergens.begin(), allergens.end(), ing) == allergens.end())
        ++rval;
  return rval;
}

int main() {
  auto foods = read();
  std::cout << "Foods: " << foods.size() << std::endl;
  auto allergens = all_allergens(foods);
  auto ingredients = all_ingredients(foods);
  std::cout << "Allergens: " << allergens.size() << std::endl;
  std::cout << "Ingredients: " << ingredients.size() << std::endl;

  auto allergy_map = std::map<std::string, std::string>{};
  auto allergies = List{};
  for (auto &a : determine_allergens(allergens, foods)) {
    std::cout << a.first << " is " << a.second.front() << std::endl;
    allergy_map.insert(std::make_pair(a.first, a.second.front()));
    allergies.push_back(a.second.front());
  }

  std::cout << "P1: " << puzzle_one(foods, allergies) << std::endl; // 2410
  std::cout << "P2: ";
  for (auto &a : allergy_map) {
    std::cout << a.second
              << ','; // tmp,pdpgm,cdslv,zrvtg,ttkn,mkpmkx,vxzpfp,flnhl
  }
  std::cout << std::endl;
}
