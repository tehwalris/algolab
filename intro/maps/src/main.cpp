#include <iostream>
#include <map>
#include <set>

void testcase()
{
  std::map<std::string, std::multiset<int>> map;

  int q;
  std::cin >> q;
  for (int i = 0; i < q; i++)
  {
    int a;
    std::string b;
    std::cin >> a >> b;

    if (a == 0)
    {
      map.erase(b);
    }
    else
    {
      if (map.count(b) == 0)
      {
        map.insert(std::make_pair(b, std::multiset<int>()));
      }
      map.at(b).insert(a);
    }
  }

  std::string s;
  std::cin >> s;
  if (map.count(s) == 0)
  {
    std::cout << "Empty\n";
  }
  else
  {
    auto set = map.at(s);
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

  int t;
  std::cin >> t;
  for (int i = 0; i < t; i++)
  {
    testcase();
  }

  return 0;
}