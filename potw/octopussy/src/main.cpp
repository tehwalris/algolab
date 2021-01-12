#include <iostream>
#include <cassert>
#include <vector>
#include <limits>
#include <algorithm>

const int debug_level = 0;

#define DEBUG(min_level, x)      \
  if (debug_level >= min_level)  \
  {                              \
    std::cout << x << std::endl; \
  }

template <typename T>
void print_vec(std::vector<T> &vec, std::ostream &stream)
{
  for (int i = 0; i < int(vec.size()); i++)
  {
    if (i > 0)
    {
      stream << " ";
    }
    stream << vec.at(i);
  }
  stream << "\n";
}

void testcase()
{
  int n;
  std::cin >> n;
  assert(n >= 1 && n < (1 << 16));
  assert(__builtin_popcount(n + 1) == 1);

  std::vector<int> blow_time(n, std::numeric_limits<int>::max());
  for (int i = 0; i < n; i++)
  {
    int t;
    std::cin >> t;
    assert(t >= 1 && t <= (1 << 30));

    t = std::min(t, blow_time.at(i));
    DEBUG(3, "i " << i << " t " << t);

    blow_time.at(i) = t;
    if (i <= (n - 3) / 2)
    {
      blow_time.at(2 * i + 1) = t - 1;
      blow_time.at(2 * i + 2) = t - 1;
    }
  }

  if (debug_level >= 2)
  {
    print_vec(blow_time, std::cerr);
  }

  std::sort(blow_time.begin(), blow_time.end());
  for (int i = 0; i < int(blow_time.size()); i++)
  {
    if (blow_time.at(i) <= i)
    {
      std::cout << "no\n";
      return;
    }
  }

  std::cout << "yes\n";
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