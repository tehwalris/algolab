#include <iostream>
#include <cassert>
#include <vector>
#include <limits>

const int debug_level = 0;

#define DEBUG(min_level, x)      \
  if (debug_level >= min_level)  \
  {                              \
    std::cerr << x << std::endl; \
  }

typedef std::pair<int, int> Cell;
typedef std::vector<Cell> Table;

const int dp_impossible = -1;

struct Beverage
{
  int c, v;
};

void testcase()
{
  int n, k;
  std::cin >> n >> k;
  assert(n >= 1 && n <= 100 && k >= 1 && k <= 1e4);

  std::vector<Beverage> beverages(n);
  for (Beverage &b : beverages)
  {
    std::cin >> b.c >> b.v;
    assert(b.c >= 1 && b.c <= 1e4);
    assert(b.v >= 1 && b.v <= 1e4);
  }

  Table dp_table(k + 1, std::make_pair(dp_impossible, 0));
  dp_table.at(0) = std::make_pair(0, 0);
  for (int i_beverage = 0; i_beverage < n; i_beverage++)
  {
    const Beverage &b = beverages.at(i_beverage);
    Table next_dp_table = dp_table;
    for (int i_col = k; i_col >= 0; i_col--)
    {
      const Cell &source_cell = next_dp_table.at(i_col);
      if (source_cell.first == dp_impossible)
      {
        continue;
      }
      Cell new_value = std::make_pair(source_cell.first + b.c, source_cell.second - 1);
      Cell &target_cell = next_dp_table.at(std::min(i_col + b.v, k));
      if (target_cell.first == dp_impossible || new_value < target_cell)
      {
        target_cell = new_value;
      }
    }
    dp_table = next_dp_table;
  }

  std::vector<int> filler_table(2 * k + 1, dp_impossible);
  filler_table.at(0) = 0;
  for (int i_col = 1; i_col < int(filler_table.size()); i_col++)
  {
    int &target_cell = filler_table.at(i_col);
    for (int i_beverage = 0; i_beverage < n; i_beverage++)
    {
      const Beverage &b = beverages.at(i_beverage);
      if (i_col - b.v < 0)
      {
        continue;
      }
      const int source_cell = filler_table.at(i_col - b.v);
      if (source_cell == dp_impossible)
      {
        continue;
      }
      int new_value = source_cell + b.c;
      if (target_cell == dp_impossible || new_value < target_cell)
      {
        target_cell = new_value;
      }
    }
  }
  int cheapest_for_at_least_this = dp_impossible;
  for (int i_col = int(filler_table.size() - 1); i_col >= 0; i_col--)
  {
    int &v = filler_table.at(i_col);
    if (cheapest_for_at_least_this == dp_impossible)
    {
      cheapest_for_at_least_this = v;
    }
    else if (v == dp_impossible || v > cheapest_for_at_least_this)
    {
      v = cheapest_for_at_least_this;
    }
    else
    {
      cheapest_for_at_least_this = v;
    }
  }

  const int cheapest_price = filler_table.at(k);
  int most_beverages = 0;
  for (int i_col = 1; i_col <= k; i_col++)
  {
    const Cell &variety_cell = dp_table.at(i_col);
    if (variety_cell.first == dp_impossible || variety_cell.first > cheapest_price)
    {
      continue;
    }
    const int filler_cost = filler_table.at(k - i_col);
    if (filler_cost == dp_impossible)
    {
      continue;
    }
    const int mixed_price = variety_cell.first + filler_cost;
    if (mixed_price > cheapest_price)
    {
      continue;
    }
    assert(mixed_price == cheapest_price);
    most_beverages = std::max(most_beverages, -variety_cell.second);
  }

  std::cout << cheapest_price << " " << most_beverages << "\n";
}

int main()
{
  std::ios_base::sync_with_stdio(false);

  int t;
  std::cin >> t;
  for (int i = 0; i < t; i++)
  {
    testcase();
    DEBUG(1, "");
  }

  return 0;
}