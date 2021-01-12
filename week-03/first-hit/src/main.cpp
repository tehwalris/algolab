#include <iostream>
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <limits>
#include <algorithm>

const int debug_level = 0;

typedef CGAL::Exact_predicates_exact_constructions_kernel K;

double floor_to_double(const K::FT &x)
{
  double a = std::floor(CGAL::to_double(x));
  while (a > x)
  {
    a -= 1;
  }
  while (a + 1 <= x)
  {
    a += 1;
  }
  return a;
}

bool testcase()
{
  int n;
  std::cin >> n;
  if (n == 0)
  {
    return false;
  }
  assert(n >= 1 && n <= 30'000);

  double x, y, a, b;
  std::cin >> x >> y >> a >> b;
  K::Point_2 ray_origin(x, y);
  K::Ray_2 ray(ray_origin, K::Point_2(a, b));

  std::vector<K::Segment_2> walls;
  for (int i = 0; i < n; i++)
  {
    double r, s, t, u;
    std::cin >> r >> s >> t >> u;
    K::Segment_2 wall(K::Point_2(r, s), K::Point_2(t, u));
    walls.push_back(wall);
  }

  std::random_shuffle(walls.begin(), walls.end());

  bool did_intersect = false;
  K::Point_2 closest_intersection;

  auto inspect_hit_point = [&did_intersect, &closest_intersection, &ray_origin](const K::Point_2 &hit_point) {
    if (!did_intersect || CGAL::has_smaller_distance_to_point(ray_origin, hit_point, closest_intersection))
    {
      if (debug_level >= 2)
      {
        std::cout << "closer\n";
      }
      closest_intersection = hit_point;
      did_intersect = true;
    }
  };

  for (auto wall : walls)
  {
    K::Segment_2 ray_segment(ray_origin, closest_intersection);
    if (!(did_intersect ? CGAL::do_intersect(ray_segment, wall) : CGAL::do_intersect(ray, wall)))
    {
      continue;
    }

    auto generic_intersection = did_intersect ? CGAL::intersection(ray_segment, wall) : CGAL::intersection(ray, wall);
    if (const K::Point_2 *hit_point = boost::get<K::Point_2>(&*generic_intersection))
    {
      if (debug_level >= 2)
      {
        std::cout << "hit point " << *hit_point << "\n";
      }
      inspect_hit_point(*hit_point);
    }
    else if (const K::Segment_2 *hit_segment = boost::get<K::Segment_2>(&*generic_intersection))
    {
      if (debug_level >= 2)
      {
        std::cout << "hit segment " << *hit_segment << "\n";
      }
      inspect_hit_point(hit_segment->source());
      inspect_hit_point(hit_segment->target());
    }
    else
    {
      assert(false);
    }
  }

  if (did_intersect)
  {
    std::cout << long(floor_to_double(closest_intersection.x())) << " " << long(floor_to_double(closest_intersection.y())) << "\n";
  }
  else
  {
    std::cout << "no\n";
  }
  return true;
}

int main()
{
  std::ios_base::sync_with_stdio(false);

  std::srand(0);

  while (testcase())
  {
  };

  return 0;
}