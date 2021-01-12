#include <iostream>
#include <vector>
#include <assert.h>

const int dp_not_calculated = -1;
const int max_n = 1000;
const int max_m = 500;
const bool debug = false;

using DpTable = std::vector<std::vector<int>>;

int guaranteed_profit_memo(DpTable &dp_table, const std::vector<int> &coin_values, int m, int others_ahead, int i, int j);

int guaranteed_profit_memo_inner(DpTable &dp_table, const std::vector<int> &coin_values, int m, int others_ahead, int i, int j)
{
  assert(i <= j);
  assert(others_ahead >= 0 && others_ahead < m);

  int remaining_count = j - i + 1;
  if (remaining_count == 1)
  {
    return others_ahead > 0 ? 0 : coin_values[i];
  }

  int next_others_ahead = others_ahead - 1;
  if (next_others_ahead < 0)
  {
    next_others_ahead = m - 1;
  }

  if (others_ahead == 0)
  {
    auto take_left = guaranteed_profit_memo(dp_table, coin_values, m, next_others_ahead, i + 1, j) + coin_values[i];
    auto take_right = guaranteed_profit_memo(dp_table, coin_values, m, next_others_ahead, i, j - 1) + coin_values[j];
    return std::max(take_left, take_right);
  }
  else
  {
    auto give_left = guaranteed_profit_memo(dp_table, coin_values, m, next_others_ahead, i + 1, j);
    auto give_right = guaranteed_profit_memo(dp_table, coin_values, m, next_others_ahead, i, j - 1);
    return std::min(give_left, give_right);
  }
}

int guaranteed_profit_memo(DpTable &dp_table, const std::vector<int> &coin_values, int m, int others_ahead, int i, int j)
{
  assert(i >= 0 && uint(i) < dp_table.size());
  assert(j >= 0 && uint(j) < dp_table.size());

  // others_ahead is not relevant for caching, since it is fully determined by (i - j) and the initial k

  if (dp_table[i][j] == dp_not_calculated)
  {
    if (debug)
    {
      std::cout << "DEBUG calc " << i << ' ' << j << '\n';
    }
    dp_table[i][j] = guaranteed_profit_memo_inner(dp_table, coin_values, m, others_ahead, i, j);
  }
  return dp_table[i][j];
}

DpTable make_dp_table(int n, int m)
{
  std::vector<int> empty_row(n, dp_not_calculated);
  DpTable dp_table;
  for (int i = 0; i < n; i++)
  {
    dp_table.push_back(empty_row);
  }
  return dp_table;
}

void clear_dp_table(DpTable &dp_table, int n, int m)
{
  for (int i = 0; i < n; i++)
  {
    for (int j = 0; j < n; j++)
    {
      dp_table[i][j] = dp_not_calculated;
    }
  }
}

void testcase(DpTable &dp_table)
{
  int n, m, k;
  std::cin >> n >> m >> k;
  assert(n <= max_n);
  assert(m <= max_m);

  std::vector<int> coin_values;
  for (int i = 0; i < n; i++)
  {
    int v;
    std::cin >> v;
    coin_values.push_back(v);
  }

  clear_dp_table(dp_table, n, m);

  std::cout << guaranteed_profit_memo(dp_table, coin_values, m, k, 0, n - 1) << '\n';
}

int main()
{
  std::ios_base::sync_with_stdio(false);

  DpTable dp_table = make_dp_table(max_n, max_m);

  int t;
  std::cin >> t;
  for (int i = 0; i < t; i++)
  {
    testcase(dp_table);
  }

  return 0;
}