#include <iostream>
#include <vector>
#include <assert.h>
#include <algorithm>
#include <functional>

void testcase()
{
  int n;
  std::cin >> n;
  std::vector<int> values(n);
  for (int i = 0; i < n; i++)
  {
    std::cin >> values[i];
  }

  int x;
  std::cin >> x;
  if (x == 0)
  {
    std::sort(values.begin(), values.end());
  }
  else if (x == 1)
  {
    std::sort(values.begin(), values.end(), std::greater<int>());
  }
  else
  {
    assert(false);
  }

  for (auto it = values.begin(); it < values.end(); it++)
  {
    if (it != values.begin())
    {
      std::cout << ' ';
    }
    std::cout << *it;
  }
  std::cout << '\n';
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