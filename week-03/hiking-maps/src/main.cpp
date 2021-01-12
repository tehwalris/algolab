#include <iostream>
#include <assert.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <vector>
#include <array>

const int debug_level = 1;

const int triangle_size = 6;

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef std::array<K::Point_2, triangle_size> Triangle;

// orient_triangle reorders the points that define a triangle
// so that all points in sequence describe a counter-clockwise
// convex polygon.
void orient_triangle(Triangle &triangle, bool assert_already_oriented = false)
{
  bool did_reorient = false;

  auto get = [&triangle](int i) -> K::Point_2 & { return triangle.at(i % triangle_size); };
  for (int i = 0; i < triangle_size; i += 2)
  {
    CGAL::Orientation o = CGAL::orientation(get(i), get(i + 1), get(i + 2));
    assert(o != CGAL::Orientation::COLLINEAR);
    if (o == CGAL::Orientation::RIGHT_TURN)
    {
      std::swap(get(i), get(i + 1));
      did_reorient = true;
    }
  }

  CGAL::Orientation o = CGAL::orientation(get(0), get(2), get(4));
  assert(o != CGAL::Orientation::COLLINEAR);
  if (o == CGAL::Orientation::RIGHT_TURN)
  {
    std::swap(get(2), get(4));
    std::swap(get(3), get(5));
    did_reorient = true;
  }

  if (assert_already_oriented)
  {
    assert(!did_reorient);
  }
  else
  {
    if (debug_level >= 2)
    {
      std::cout << "did_reorient " << did_reorient << "\n";
    }
    if (debug_level >= 1)
    {
      orient_triangle(triangle, true);
    }
  }
}

bool is_outside_triangle(const Triangle &triangle, const K::Point_2 &p)
{
  for (int i = 0; i < triangle_size; i += 2)
  {
    if (CGAL::right_turn(triangle.at(i), triangle.at(i + 1), p))
    {
      return true;
    }
  }
  return false;
}

void testcase()
{
  // - determine which segments are contained in which triangles
  // - use sliding window to find shortest window
  //   - track number of triangles that each line is contained in

  int m, n;
  std::cin >> m >> n;
  assert(m >= 2 && m <= 2000);
  assert(n >= 2 && n <= 2000);

  std::vector<K::Point_2> path_points;
  for (int i_point = 0; i_point < m; i_point++)
  {
    int x, y;
    std::cin >> x >> y;
    path_points.push_back(K::Point_2(x, y));
  }

  std::vector<Triangle> triangles(n);
  for (int i_triangle = 0; i_triangle < n; i_triangle++)
  {
    Triangle &triangle = triangles.at(i_triangle);
    for (int i_point = 0; i_point < triangle_size; i_point++)
    {
      int x, y;
      std::cin >> x >> y;
      triangle.at(i_point) = K::Point_2(x, y);
    }
    orient_triangle(triangle);
  }

  std::vector<std::vector<int>> segments_by_triangle(n);
  for (int i_triangle = 0; i_triangle < n; i_triangle++)
  {
    Triangle &triangle = triangles.at(i_triangle);
    std::vector<int> &segments = segments_by_triangle.at(i_triangle);
    for (int i_segment = 0; i_segment < m - 1; i_segment++)
    {
      if (!is_outside_triangle(triangle, path_points.at(i_segment)) && !is_outside_triangle(triangle, path_points.at(i_segment + 1)))
      {
        segments.push_back(i_segment);
      }
    }
  }

  int i_start = 0, i_end = 0;
  std::vector<int> times_covered_by_segment(m - 1, 0);
  int min_cost = std::numeric_limits<int>::max();
  int covered_segments = 0;
  while (i_end < n)
  {
    assert(i_start >= 0 && i_start <= i_end);

    for (int i_segment : segments_by_triangle.at(i_end))
    {
      int &times_covered = times_covered_by_segment.at(i_segment);
      if (times_covered == 0)
      {
        covered_segments++;
        assert(covered_segments <= m - 1);
      }
      times_covered++;
    }
    i_end++;

    while (i_start < i_end)
    {
      bool all_redundant = true;
      for (int i_segment : segments_by_triangle.at(i_start))
      {
        int times_covered = times_covered_by_segment.at(i_segment);
        assert(times_covered >= 1);
        if (times_covered == 1)
        {
          all_redundant = false;
          break;
        }
      }

      if (!all_redundant)
      {
        break;
      }

      for (int i_segment : segments_by_triangle.at(i_start))
      {
        times_covered_by_segment.at(i_segment)--;
      }
      i_start++;
    }

    assert(i_start >= 0 && i_start <= i_end);

    if (covered_segments == m - 1)
    {
      int cost = i_end - i_start;
      assert(cost >= 1 && cost <= n);
      min_cost = std::min({cost, min_cost});
    }
  }
  assert(min_cost != std::numeric_limits<int>::max());
  std::cout << min_cost << "\n";
}

int main()
{
  std::ios_base::sync_with_stdio(false);

  int c;
  std::cin >> c;
  assert(c >= 0 && c <= 60);
  for (int i = 0; i < c; i++)
  {
    testcase();
  }

  return 0;
}