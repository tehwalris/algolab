#include <iostream>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <vector>
#include <algorithm>

const int debug_level = 0;

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;

struct Biker
{
  int id;
  double start;
  K::Direction_2 dir;
  K::Direction_2 abs_dir;
};

void testcase()
{
  int n;
  std::cin >> n;
  assert(n >= 1 && n <= 500000);

  std::vector<Biker> bikers(n);
  for (int i = 0; i < n; i++)
  {
    Biker &biker = bikers.at(i);
    biker.id = i;
    double x_1, y_1;
    std::cin >> biker.start >> x_1 >> y_1;
    assert(x_1 > 0);
    biker.dir = K::Direction_2(x_1, y_1 - biker.start);
    biker.abs_dir = K::Direction_2(abs(biker.dir.dx()), abs(biker.dir.dy()));
  }

  auto compare_earlier = [](Biker &a, Biker &b) {
    auto abs_dir_comp = CGAL::compare_angle_with_x_axis(a.abs_dir, b.abs_dir);
    if (abs_dir_comp != CGAL::Comparison_result::EQUAL)
    {
      return abs_dir_comp == CGAL::Comparison_result::SMALLER;
    }
    if ((a.dir.dy() <= 0) != (b.dir.dy() <= 0))
    {
      return a.dir.dy() > 0;
    }
    if (a.dir.dy() <= 0)
    {
      return a.start > b.start;
    }
    return a.start < b.start;
  };
  std::sort(bikers.begin(), bikers.end(), compare_earlier);

  std::vector<int> forever_ids;
  double safe_above = -INFINITY, safe_below = INFINITY;
  for (auto &biker : bikers)
  {
    if (debug_level >= 2)
    {
      std::cout << "safe above " << safe_above << " and below " << safe_below << "\n";
      std::cout << "inspecting biker " << biker.id << "\n";
    }

    if (biker.dir.dy() > 0 ? (biker.start > safe_above) : (biker.start < safe_below))
    {
      if (debug_level >= 2)
      {
        std::cout << "forever\n";
      }
      forever_ids.push_back(biker.id);
    }
    safe_below = std::min(safe_below, biker.start);
    safe_above = std::max(safe_above, biker.start);
  }

  std::sort(forever_ids.begin(), forever_ids.end());
  for (int i = 0; i < int(forever_ids.size()); i++)
  {
    if (i != 0)
    {
      std::cout << " ";
    }
    std::cout << forever_ids.at(i);
  }
  std::cout << "\n";
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