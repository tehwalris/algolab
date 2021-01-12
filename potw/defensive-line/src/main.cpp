#include <iostream>
#include <vector>
#include <cassert>

const int debug_level = 0;

const int dp_undefined = -1;

void testcase()
{
  int n, m, k;
  std::cin >> n >> m >> k;
  assert(n >= 1 && n <= 100000);
  assert(m >= 1 && m <= 100);
  assert(k >= 1 && k <= 1000000000);

  std::vector<int> defense_values(n);
  for (int i = 0; i < n; i++)
  {
    int v;
    std::cin >> v;
    assert(v >= 1 && v < 10000);
    defense_values.at(i) = v;
  }

  std::vector<int> window_length_by_last(n);
  int i_start = 0, i_end = 0;
  int window_sum = 0;
  while (i_end < n)
  {
    assert(i_start >= 0 && i_start <= i_end);

    window_sum += defense_values.at(i_end);
    i_end++;

    while (window_sum > k && i_start < i_end)
    {
      window_sum -= defense_values.at(i_start);
      i_start++;
    }

    if (window_sum == k)
    {
      if (debug_level >= 3)
      {
        std::cout << "found window " << i_start << " " << i_end << "\n";
      }
      assert(i_end > 0);
      window_length_by_last.at(i_end - 1) = i_end - i_start;
    }
  }

  std::vector<std::vector<int>> dp_table;
  auto get = [&dp_table, m, n](int i, int j) -> int {
    assert(i >= -1 && i < m && j >= -1 && j < n);
    if (i == -1)
    {
      return 0;
    }
    if (j == -1)
    {
      return dp_undefined;
    }
    return dp_table.at(i).at(j);
  };
  for (int i = 0; i < m; i++)
  {
    dp_table.emplace_back(std::vector<int>(n, dp_undefined));
    for (int j = 0; j < n; j++)
    {
      int best_without_extension = get(i, j - 1);
      if (best_without_extension != dp_undefined)
      {
        dp_table.at(i).at(j) = std::max(get(i, j), best_without_extension);
      }

      int l = window_length_by_last.at(j);
      if (l != 0)
      {
        int best_before_extension = get(i - 1, j - l);
        if (best_before_extension != dp_undefined)
        {
          dp_table.at(i).at(j) = std::max(get(i, j), best_before_extension + l);
        }
      }

      if (debug_level >= 3)
      {
        std::cout << dp_table.at(i).at(j) << " ";
      }
    }
    if (debug_level >= 3)
    {
      std::cout << "\n";
    }
  }

  int max_legal_attack = dp_table.back().back();
  if (max_legal_attack == dp_undefined)
  {
    std::cout << "fail\n";
  }
  else
  {
    assert(max_legal_attack > 0);
    std::cout << max_legal_attack << "\n";
  }
}

int main()
{
  std::ios_base::sync_with_stdio(false);

  int t;
  std::cin >> t;
  for (int i = 0; i < t; i++)
  {
    testcase();
  }

  return 0;
}