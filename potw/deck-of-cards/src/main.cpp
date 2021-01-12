#include <iostream>
#include <vector>
#include <cmath>
#include <limits>
#include <assert.h>
#include <algorithm>

void testcase()
{
  int n, k;
  std::cin >> n >> k;
  assert(n > 0);

  std::vector<int> cardValuesCum;
  int cumSum = 0;
  for (int i = 0; i < n; i++)
  {
    int v;
    std::cin >> v;
    cumSum += v;
    cardValuesCum.push_back(cumSum);
  }

  int bestValue = std::numeric_limits<int>::max(), bestI = -1, bestJ = -1;
  for (int i = 0; i < n; i++)
  {
    auto target = k;
    if (i > 0)
    {
      target += cardValuesCum[i - 1];
    }
    if (target < 0)
    {
      // HACK there was overflow, ignore it just in case
      continue;
    }

    auto closestHigh = std::lower_bound(cardValuesCum.begin() + i, cardValuesCum.end(), target);
    std::vector<std::vector<int>::iterator> candidates;

    if (closestHigh != cardValuesCum.begin() + i)
    {
      auto closest = closestHigh - 1;

      int solutionValue = *closest;
      if (i > 0)
      {
        solutionValue -= cardValuesCum[i - 1];
      }
      solutionValue = abs(k - solutionValue);

      if (solutionValue < bestValue)
      {
        bestValue = solutionValue;
        bestI = i;
        bestJ = closest - cardValuesCum.begin();
      }
    }

    {
      auto closest = closestHigh;

      int solutionValue = *closest;
      if (i > 0)
      {
        solutionValue -= cardValuesCum[i - 1];
      }
      solutionValue = abs(k - solutionValue);

      if (solutionValue < bestValue)
      {
        bestValue = solutionValue;
        bestI = i;
        bestJ = closest - cardValuesCum.begin();
      }
    }
  }

  std::cout << bestI << ' ' << bestJ << '\n';
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