#include <iostream>
#include <string>
#include <algorithm>
#include <utility>

void testcase()
{
  std::string a, b;
  std::cin >> a >> b;

  std::cout << a.length() << ' ' << b.length() << '\n';
  std::cout << a << b << '\n';

  std::string c = a;
  std::reverse(c.begin(), c.end());
  std::string d = b;
  std::reverse(d.begin(), d.end());
  std::swap(c[0], d[0]);

  std::cout << c << ' ' << d << '\n';
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