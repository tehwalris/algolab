#include <iostream>
#include <vector>

void testcase()
{
  int n;
  std::cin >> n;
  std::vector<int> vec;
  for (int i = 0; i < n; i++)
  {
    int v;
    std::cin >> v;
    vec.push_back(v);
  }

  int d;
  std::cin >> d;
  vec.erase(vec.begin() + d);

  int a, b;
  std::cin >> a >> b;
  vec.erase(vec.begin() + a, vec.begin() + b + 1);

  if (vec.empty())
  {
    std::cout << "Empty\n";
  }
  else
  {
    for (auto it = vec.begin(); it < vec.end(); it++)
    {
      if (it != vec.begin())
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