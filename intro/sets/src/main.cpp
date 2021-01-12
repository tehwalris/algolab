#include <iostream>
#include <set>
#include <assert.h>

void testcase()
{
  std::set<int> set;

  int n;
  std::cin >> n;
  for (int i = 0; i < n; i++)
  {
    int a, b;
    std::cin >> a >> b;
    if (a == 0)
    {
      set.insert(b);
    }
    else if (a == 1)
    {
      set.erase(b);
    }
    else
    {
      assert(false);
    }
  }

  if (set.empty())
  {
    std::cout << "Empty\n";
  }
  else
  {
    for (auto it = set.begin(); it != set.end(); it++)
    {
      if (it != set.begin())
      {
        std::cout << ' ';
      }
      std::cout << (*it);
    }
    std::cout << '\n';
  }
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