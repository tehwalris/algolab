#include <iostream>
#include <cassert>
#include <vector>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Exact_predicates_exact_constructions_kernel_with_sqrt.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Triangulation_vertex_base_with_info_2.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel IK;
typedef CGAL::Exact_predicates_exact_constructions_kernel_with_sqrt EK;
typedef CGAL::Triangulation_vertex_base_with_info_2<int, IK> VertexBase;
typedef CGAL::Triangulation_face_base_2<IK> FaceBase;
typedef CGAL::Triangulation_data_structure_2<VertexBase, FaceBase> TriangulationDataStructure;
typedef CGAL::Delaunay_triangulation_2<IK, TriangulationDataStructure> Triangulation;

const int debug_level = 0;

#define DEBUG(min_level, x)      \
  if (debug_level >= min_level)  \
  {                              \
    std::cerr << x << std::endl; \
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

double ceil_to_double(const EK::FT &x)
{
  double a = std::floor(CGAL::to_double(x));
  while (a < x)
    a += 1;
  while (a - 1 >= x)
    a -= 1;
  return a;
}

int ceil_hours_from_sq_dist(double sq_dist)
{
  EK::FT hours = CGAL::sqrt((CGAL::sqrt(EK::FT(sq_dist)) - 1) / 2);
  return int(ceil_to_double(hours));
}

bool testcase()
{
  int n;
  std::cin >> n;
  if (n == 0)
  {
    return false;
  }
  assert(n >= 1 && n <= 1e5);

  std::vector<int> dish_boundaries(4);
  for (int i = 0; i < 4; i++)
  {
    int v;
    std::cin >> v;
    assert(abs(v) < (1 << 25));
    dish_boundaries.at(i) = v;
  }
  assert(dish_boundaries.at(0) <= dish_boundaries.at(2) && dish_boundaries.at(1) <= dish_boundaries.at(3));

  std::vector<std::pair<IK::Point_2, int>> bacteria_centers;
  for (int i = 0; i < n; i++)
  {
    int x, y;
    std::cin >> x >> y;
    assert(abs(x) < (1 << 24) && abs(y) < (1 << 24));
    assert(dish_boundaries.at(0) <= x && x <= dish_boundaries.at(2) && dish_boundaries.at(1) <= x && x <= dish_boundaries.at(3));
    bacteria_centers.push_back(std::make_pair(IK::Point_2(x, y), i));
  }

  std::vector<double> death_sq_dists(n);
  for (int i = 0; i < n; i++)
  {
    IK::Point_2 &center = bacteria_centers.at(i).first;
    int closest_dist = std::min({
        center.x() - dish_boundaries.at(0),
        dish_boundaries.at(2) - center.x(),
        center.y() - dish_boundaries.at(1),
        dish_boundaries.at(3) - center.y(),
    });
    assert(closest_dist >= 0);
    death_sq_dists.at(i) = pow(double(2 * closest_dist), 2);
  }

  Triangulation triangulation;
  int num_unique_centers = triangulation.insert(bacteria_centers.begin(), bacteria_centers.end());
  assert(num_unique_centers == n);
  (void)num_unique_centers;

  for (auto it = triangulation.finite_edges_begin(); it != triangulation.finite_edges_end(); it++)
  {
    Triangulation::Vertex_handle v1 = it->first->vertex((it->second + 1) % 3);
    Triangulation::Vertex_handle v2 = it->first->vertex((it->second + 2) % 3);
    double new_sq_dist = CGAL::squared_distance(v1->point(), v2->point());
    for (auto &v : {v1, v2})
    {
      double &old_sq_dist = death_sq_dists.at(v->info());
      old_sq_dist = std::min(old_sq_dist, new_sq_dist);
    }
  }

  if (debug_level >= 2)
  {
    print_vec(death_sq_dists, std::cerr);
  }

  std::sort(death_sq_dists.begin(), death_sq_dists.end());
  std::cout << ceil_hours_from_sq_dist(death_sq_dists.front()) << " "
            << ceil_hours_from_sq_dist(death_sq_dists.at(n / 2)) << " "
            << ceil_hours_from_sq_dist(death_sq_dists.back()) << "\n";

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