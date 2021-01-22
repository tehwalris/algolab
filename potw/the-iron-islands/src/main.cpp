#include <iostream>
#include <cassert>
#include <vector>
#include <map>

const int debug_level = 0;

#define DEBUG(min_level, x)      \
  if (debug_level >= min_level)  \
  {                              \
    std::cerr << x << std::endl; \
  }

void testcase()
{
  int n, k, w;
  std::cin >> n >> k >> w;
  assert(n >= 1 && n <= 5e4 && k >= 1 && k <= 1e6 && w >= 1 && w < n);

  std::vector<int> men_by_island(n);
  for (int &c : men_by_island)
  {
    std::cin >> c;
    assert(c >= 1 && c <= 1e6);
  }

  std::vector<std::vector<int>> waterways(w);
  for (std::vector<int> &waterway : waterways)
  {
    int l;
    std::cin >> l;
    assert(l >= 1 && l <= n);
    waterway.reserve(l);

    for (int i = 0; i < l; i++)
    {
      int r;
      std::cin >> r;
      assert(i == 0 ? (r == 0) : (r >= 1 && r < n));
      waterway.push_back(r);
    }
  }

  int max_conquered_islands = 0;

  for (const std::vector<int> &waterway : waterways)
  {
    const int l = waterway.size();
    int ia = 0, ib = 0, men = 0;
    while (ib < l)
    {
      while (men < k && ib < l)
      {
        men += men_by_island.at(waterway.at(ib));
        ib++;
      }
      while (men > k)
      {
        men -= men_by_island.at(waterway.at(ia));
        ia++;
      }
      assert(ia <= ib && men >= 0);
      if (men == k)
      {
        max_conquered_islands = std::max(max_conquered_islands, ib - ia);
        men -= men_by_island.at(waterway.at(ia));
        ia++;
      }
    }
  }

  const int pyke_men = men_by_island.at(0);
  std::map<int, int> max_islands_by_men; // for trips starting from Pyke
  for (const std::vector<int> &waterway : waterways)
  {
    const int l = waterway.size();
    for (int i = 0, men = 0; i < l; i++)
    {
      men += men_by_island.at(waterway.at(i));
      const int remaining_men = k - men + pyke_men;
      if (remaining_men < 0)
      {
        break;
      }
      const auto it = max_islands_by_men.find(remaining_men);
      if (it != max_islands_by_men.end())
      {
        max_conquered_islands = std::max(max_conquered_islands, i + it->second);
      }
    }
    for (int i = 0, men = 0; i < l; i++)
    {
      men += men_by_island.at(waterway.at(i));
      max_islands_by_men.insert(std::make_pair(men, i + 1));
      auto it = max_islands_by_men.find(men);
      it->second = std::max(it->second, i + 1);
    }
  }

  std::cout << max_conquered_islands << "\n";
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