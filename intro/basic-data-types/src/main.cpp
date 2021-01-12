#include <iostream>
#include <string>
#include <iomanip>

void testcase()
{
  int the_int;
  std::cin >> the_int;
  long the_long;
  std::cin >> the_long;
  std::string the_string;
  std::cin >> the_string;
  double the_double;
  std::cin >> the_double;

  std::cout << std::setprecision(2) << std::fixed << the_int << ' ' << the_long << ' ' << the_string << ' ' << the_double << '\n';
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