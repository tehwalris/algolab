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

typedef std::vector<int> Row;
typedef std::vector<Row> Table;

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

  Table dp_table(n + 1, Row(k + 1, dp_impossible));
  dp_table.at(0).at(0) = 0;
  for (int i_beverage = 0; i_beverage < n; i_beverage++)
  {
    const Beverage &b = beverages.at(i_beverage);
    for (int i_row = i_beverage; i_row >= 0; i_row--)
    {
      for (int i_col = k; i_col >= 0; i_col--)
      {
        const int old_cell = dp_table.at(i_row).at(i_col);
        if (old_cell == dp_impossible)
        {
          continue;
        }
        for (int i_col_new = i_col + b.v, added_cost = b.c; i_col_new < k + b.v; i_col_new += b.v, added_cost += b.c)
        {
          int &new_cell = dp_table.at(i_row + 1).at(std::min(i_col_new, k));
          if (new_cell == dp_impossible || new_cell > old_cell + added_cost)
          {
            new_cell = old_cell + added_cost;
          }
        }
      }
    }
  }

  int cheapest_cost = std::numeric_limits<int>::max();
  int most_beverages_at_cheapest = 0;
  for (int i = n; i >= 0; i--)
  {
    const int cell = dp_table.at(i).at(k);
    if (cell != dp_impossible && cell < cheapest_cost)
    {
      cheapest_cost = cell;
      most_beverages_at_cheapest = i;
    }
  }

  std::cout << cheapest_cost << " " << most_beverages_at_cheapest << "\n";
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