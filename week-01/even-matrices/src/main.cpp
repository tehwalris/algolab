#include <iostream>
#include <vector>

void printVec(std::vector<int> vec)
{
  for (auto it = vec.begin(); it < vec.end(); it++)
  {
    if (it != vec.begin())
    {
      std::cout << ' ';
    }
    std::cout << *it;
  }
  std::cout << '\n';
}

int countEvenIntervals(std::vector<int> values)
{
  int parity = 0, even = 1, odd = 0;
  for (auto v : values)
  {
    parity = (parity + v) % 2;
    if (parity == 0)
    {
      even++;
    }
    else
    {
      odd++;
    }
  }
  return (even * (even - 1)) / 2 + (odd * (odd - 1)) / 2;
}

void testcase()
{
  int n;
  std::cin >> n;
  std::vector<std::vector<int>> cumM{std::vector<int>(n, 0)};
  for (int i = 0; i < n; i++)
  {
    auto row = cumM.back();
    for (int j = 0; j < n; j++)
    {
      int v;
      std::cin >> v;
      row[j] += v;
    }
    cumM.push_back(row);
  }

  int evenIntervals = 0;
  for (int i1 = 0; i1 < n; i1++)
  {
    for (int i2 = i1 + 1; i2 <= n; i2++)
    {
      auto diff = cumM[i2];
      for (int j = 0; j < n; j++)
      {
        diff[j] -= cumM[i1][j];
      }

      evenIntervals += countEvenIntervals(diff);
    }
  }

  std::cout << evenIntervals << '\n';
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