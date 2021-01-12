#include <iostream>
#include <bitset>
#include <vector>
#include <assert.h>
#include <algorithm>
#include <deque>
#include <limits>

const int max_parasols = 1'000'000;
const int beach_size = 2 * max_parasols + 1;
const int beach_middle = beach_size / 2;
const int max_near_distance = 100;

void testcase()
{
  std::bitset<beach_size> parasols;

  int n;
  std::cin >> n;
  assert(n >= 1 && n <= max_parasols);
  for (int i = 0; i < n; i++)
  {
    int v;
    std::cin >> v;
    v += beach_middle;
    assert(v >= 0 && v < beach_size);
    parasols.set(v);
  }

  std::vector<int> nearby_customers_by_pos(beach_size, 0);
  std::vector<int> longest_walk_by_pos(beach_size, -1);
  std::deque<int> nearby_customers;
  for (int bar = -max_near_distance; bar < beach_size + max_near_distance; bar++)
  {
    int new_customer_pos = bar + max_near_distance;
    assert(new_customer_pos >= 0);
    if (new_customer_pos < beach_size && parasols.test(new_customer_pos))
    {
      nearby_customers.push_front(new_customer_pos);
    }

    int old_customer_pos = bar - max_near_distance - 1;
    assert(old_customer_pos < beach_size);
    if (old_customer_pos >= 0 && parasols.test(old_customer_pos))
    {
      nearby_customers.pop_back();
    }

    if (bar >= 0 && bar < beach_size)
    {
      nearby_customers_by_pos.at(bar) = nearby_customers.size();

      if (nearby_customers.size() > 0)
      {
        longest_walk_by_pos.at(bar) = std::max(abs(nearby_customers.front() - bar), abs(nearby_customers.back() - bar));
      }
    }
  }

  int max_nearby_customers = *std::max_element(nearby_customers_by_pos.begin(), nearby_customers_by_pos.end());
  assert(max_nearby_customers >= 1);
  int min_relevant_walk = std::numeric_limits<int>::max();
  for (int i = 0; i < beach_size; i++)
  {
    if (nearby_customers_by_pos.at(i) == max_nearby_customers)
    {
      min_relevant_walk = std::min({min_relevant_walk, longest_walk_by_pos.at(i)});
    }
  }
  assert(min_relevant_walk >= 0 && min_relevant_walk < std::numeric_limits<int>::max());
  std::cout << max_nearby_customers << " " << min_relevant_walk << "\n";

  bool needs_space = false;
  for (int i = 0; i < beach_size; i++)
  {
    if (nearby_customers_by_pos.at(i) == max_nearby_customers && longest_walk_by_pos.at(i) == min_relevant_walk)
    {
      if (needs_space)
      {
        std::cout << " ";
      }
      std::cout << i - beach_middle;
      needs_space = true;
    }
  }
  std::cout << "\n";
}

int main()
{
  std::ios_base::sync_with_stdio(false);

  int n;
  std::cin >> n;
  for (int i = 0; i < n; i++)
  {
    testcase();
  }

  return 0;
}