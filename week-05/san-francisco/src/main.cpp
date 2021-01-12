#include <iostream>
#include <cassert>
#include <vector>

const int debug_level = 0;

#define DEBUG(min_level, x)      \
  if (debug_level >= min_level)  \
  {                              \
    std::cout << x << std::endl; \
  }

const long unreachable_points = -1;

void testcase()
{
  int n, m, k;
  long x;
  std::cin >> n >> m >> x >> k;
  assert(n >= 2 && n <= 1e3 && m >= 1 && m <= 4e3);
  assert(x >= 1 && x <= 1e14 && k >= 1 && k <= 4e3);

  std::vector<std::vector<std::pair<int, long>>> edges(m, std::vector<std::pair<int, long>>());
  for (int i = 0; i < m; i++)
  {
    int u, v;
    long p;
    std::cin >> u >> v >> p;
    assert(u >= 0 && u < n && v >= 0 && v < n);
    assert(p >= 0 && p < (long(1) << 31));
    edges.at(u).push_back(std::make_pair(v, p));
  }

  std::vector<long> last_row(n, unreachable_points);
  last_row.at(0) = 0;
  for (int i = 0; i < k; i++)
  {
    DEBUG(2, "i " << i);
    std::vector<long> next_row(n, unreachable_points);
    for (int j = 0; j < n; j++)
    {
      DEBUG(2, "j " << j);
      long last_points = last_row.at(j);
      if (last_points == unreachable_points)
      {
        DEBUG(2, "unreachable");
        continue;
      }
      for (auto &edge : edges.at(j))
      {
        DEBUG(2, "edge to " << edge.first << " value " << edge.second);
        int effective_target = edges.at(edge.first).empty() ? 0 : edge.first;
        long &next_points = next_row.at(effective_target);
        next_points = std::max(next_points, last_points + edge.second);
        DEBUG(2, "next_points " << next_points);
        if (next_points >= x)
        {
          std::cout << i + 1 << "\n";
          return;
        }
      }
    }
    last_row = next_row;
  }

  std::cout << "Impossible\n";
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