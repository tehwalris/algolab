#include <iostream>
#include <cassert>
#include <vector>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <iomanip>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Delaunay_triangulation_2<K> Triangulation;

const int debug_level = 0;

#define DEBUG(min_level, x)      \
  if (debug_level >= min_level)  \
  {                              \
    std::cerr << x << std::endl; \
  }

bool testcase()
{
  int n;
  std::cin >> n;
  if (n == 0)
  {
    return false;
  }
  assert(n >= 1 && n <= 110000);

  std::vector<K::Point_2> old_locations;
  for (int i = 0; i < n; i++)
  {
    int x, y;
    std::cin >> x >> y;
    assert(abs(x) < (1 << 24) && abs(y) < (1 << 24));
    old_locations.emplace_back(x, y);
  }

  Triangulation triangulation;
  triangulation.insert(old_locations.begin(), old_locations.end());

  int m;
  std::cin >> m;
  assert(m >= 1 && m <= 110000);

  for (int i = 0; i < m; i++)
  {
    int x, y;
    std::cin >> x >> y;
    assert(abs(x) < (1 << 24) && abs(y) < (1 << 24));
    K::Point_2 new_location(x, y);

    auto old_location = triangulation.nearest_vertex(new_location)->point();

    std::cout << std::fixed << std::setprecision(0) << CGAL::squared_distance(new_location, old_location) << "\n";
  }

  return true;
}

int main()
{
  std::ios_base::sync_with_stdio(false);

  while (testcase())
  {
  }

  return 0;
}