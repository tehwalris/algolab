#include <iostream>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <limits>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;

bool testcase()
{
  int n;
  std::cin >> n;
  if (n == 0)
  {
    return false;
  }
  assert(n >= 1 && n <= 20'000);

  double x, y, a, b;
  std::cin >> x >> y >> a >> b;
  K::Ray_2 ray(K::Point_2(x, y), K::Point_2(a, b));

  bool did_intersect = false;
  for (int i = 0; i < n; i++)
  {
    if (did_intersect)
    {
      std::cin.ignore(std::numeric_limits<long>::max(), '\n');
    }
    else
    {
      double r, s, t, u;
      std::cin >> r >> s >> t >> u;
      K::Segment_2 segment(K::Point_2(r, s), K::Point_2(t, u));
      if (CGAL::do_intersect(ray, segment))
      {
        did_intersect = true;
        std::cin >> std::ws;
      }
    }
  }

  std::cout << (did_intersect ? "yes\n" : "no\n");
  return true;
}

int main()
{
  std::ios_base::sync_with_stdio(false);

  while (testcase())
  {
  };

  return 0;
}