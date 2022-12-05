#include <algorithm>
#include <iostream>
#include <list>
#include <vector>

using List = std::list<int>;
const auto INPUT = List{1, 6, 7, 2, 4, 8, 3, 5, 9};

auto take(List &list) {
  auto rval = list.front();
  list.pop_front();
  return rval;
}

// find the next number below the start number that's not in our list
auto find_not_in(const List &three, int start) {
  while (std::find(three.begin(), three.end(), start) != three.end())
    --start;
  return start;
}

// prepare a cache between the number on the cup, and its iterator
auto cache = std::vector<List::iterator>{};
auto size = size_t{};
auto prep_cache(List &list) {
  size = list.size();
  cache = std::vector<List::iterator>(size + 1);
  for (auto it = list.begin(); it != list.end(); ++it)
    cache[*it] = it;
}

auto shuffle(List &list) {
  // take three cups to the right of the first one
  auto current = take(list);
  auto three = List{};
  for (auto i = 0; i < 3; ++i)
    three.push_back(take(list));
  list.push_front(current);

  // select a destination cup
  auto dest = find_not_in(three, current - 1);
  if (dest == 0)
    dest = find_not_in(three, size);

  // place them to the right of the destination
  auto it = cache[dest];
  auto prev = it++;
  list.insert(it, three.begin(), three.end());
  // cache update
  ++prev;
  for (auto i = 0; i < 3; ++i) {
    cache[*prev] = prev;
    ++prev;
  }

  // select a new destination cup
  list.push_back(take(list)); // reference preserved, cache ok
}

auto puzzle_one() {
  auto input = INPUT;
  prep_cache(input);
  for (auto i = 1; i <= 100; ++i)
    shuffle(input);
  while (input.front() != 1)
    input.push_back(take(input));
  for (auto i : input) {
    if (i == 1)
      continue;
    std::cout << i;
  }
  std::cout << std::endl;
}

auto puzzle_two() {
  auto input = INPUT;
  while (input.size() < 1000000)
    input.push_back(input.size() + 1);
  prep_cache(input);
  for (auto i = 1; i <= 10000000; ++i)
    shuffle(input);
  while (input.front() != 1)
    input.push_back(take(input));
  auto one = take(input);
  auto p1 = size_t(take(input));
  auto p2 = size_t(take(input));
  std::cout << p1 * p2 << std::endl;
}

int main() {
  puzzle_one(); // 38756249
  puzzle_two(); // 21986479838
}
