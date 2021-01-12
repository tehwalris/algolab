#include <iostream>
#include <cassert>
#include <vector>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Triangulation_vertex_base_with_info_2.h>
#include <CGAL/Triangulation_face_base_2.h>
#include <CGAL/Triangulation_data_structure_2.h>
#include <boost/pending/disjoint_sets.hpp>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Triangulation_vertex_base_with_info_2<int, K> Vb;
typedef CGAL::Triangulation_face_base_2<K> Fb;
typedef CGAL::Triangulation_data_structure_2<Vb, Fb> Tds;
typedef CGAL::Delaunay_triangulation_2<K, Tds> Triangulation;

const int debug_level = 0;

#define DEBUG(min_level, x)      \
  if (debug_level >= min_level)  \
  {                              \
    std::cerr << x << std::endl; \
  }

struct TreeEdge
{
  int i1, i2;
  double sq_dist;
};

struct GraphProblem
{
  std::vector<int> nearest_tree_by_bone;
  std::vector<double> bone_tree_sq_dists;
  std::vector<TreeEdge> tree_edges;
};

typedef std::vector<std::pair<K::Point_2, int>> LocationVec;

int binary_search_first(std::function<bool(int)> is_target)
{
  int upper_power = 0;
  while (true)
  {
    int i = 1 << upper_power;
    if (is_target(i))
    {
      break;
    }
    upper_power++;
  }

  int low = upper_power == 0 ? 0 : (1 << (upper_power - 1));
  int high = 1 << upper_power;
  while (low < high)
  {
    int mid = (low + high) / 2;
    if (is_target(mid))
    {
      high = mid;
    }
    else
    {
      low = mid + 1;
    }
  }

  assert(!is_target(high - 1) && is_target(high));
  return high;
}

LocationVec read_locations(int n)
{
  LocationVec locations;
  for (int i = 0; i < n; i++)
  {
    long x, y;
    std::cin >> x >> y;
    assert(abs(x) < (1 << 24) && abs(y) < (1 << 24));
    locations.push_back(std::make_pair(K::Point_2(x, y), i));
  }
  return locations;
}

GraphProblem graph_problem_from_locations(const LocationVec &tree_locations, const LocationVec &bone_locations)
{
  Triangulation triangulation;
  triangulation.insert(tree_locations.begin(), tree_locations.end());

  GraphProblem g;

  for (const auto &bone_location : bone_locations)
  {
    Triangulation::Vertex_handle vertex = triangulation.nearest_vertex(bone_location.first);
    g.nearest_tree_by_bone.push_back(vertex->info());
    g.bone_tree_sq_dists.push_back(CGAL::squared_distance(vertex->point(), bone_location.first));
  }

  for (auto it = triangulation.finite_edges_begin(); it != triangulation.finite_edges_end(); it++)
  {
    TreeEdge e;
    e.i1 = it->first->vertex((it->second + 1) % 3)->info();
    e.i2 = it->first->vertex((it->second + 2) % 3)->info();
    if (e.i1 > e.i2)
    {
      std::swap(e.i1, e.i2);
    }
    e.sq_dist = triangulation.segment(it).squared_length();
    g.tree_edges.push_back(e);
  }

  DEBUG(2, "g.tree_edges.size() " << g.tree_edges.size());

  return g;
}

int count_reachable_bones(int n, double s, const GraphProblem &graph_problem)
{
  const int m = graph_problem.nearest_tree_by_bone.size();
  assert(int(graph_problem.bone_tree_sq_dists.size()) == m);

  std::vector<int> ds_rank(n);
  std::vector<int> ds_parent(n);
  boost::disjoint_sets<int *, int *> ds(ds_rank.data(), ds_parent.data());
  for (int i = 0; i < n; i++)
  {
    ds.make_set(i);
  }

  for (const auto &edge : graph_problem.tree_edges)
  {
    DEBUG(2, "tree edge " << edge.i1 << " " << edge.i2 << " " << edge.sq_dist);
    if (edge.sq_dist <= s)
    {
      DEBUG(2, "union");
      ds.union_set(edge.i1, edge.i2);
    }
  }

  std::vector<int> bones_per_component(n, 0);
  for (int i = 0; i < m; i++)
  {
    if (4 * graph_problem.bone_tree_sq_dists.at(i) <= s)
    {
      bones_per_component.at(ds.find_set(graph_problem.nearest_tree_by_bone.at(i)))++;
    }
  }

  return *std::max_element(bones_per_component.begin(), bones_per_component.end());
}

void testcase()
{
  int n, m, k;
  double s;
  std::cin >> n >> m >> s >> k;
  assert(n >= 1 && n <= 4e4);
  assert(m >= 1 && m <= 4e4);
  assert(s >= 1 && s <= (1L << 51));
  assert(k >= 1 && k <= m);

  auto tree_locations = read_locations(n);
  auto bone_locations = read_locations(m);

  GraphProblem graph_problem = graph_problem_from_locations(tree_locations, bone_locations);

  int a = count_reachable_bones(n, s, graph_problem);

  std::vector<double> interesting_sq_dists;
  for (const auto &e : graph_problem.tree_edges)
  {
    interesting_sq_dists.push_back(e.sq_dist);
  }
  for (const auto &sq_dist : graph_problem.bone_tree_sq_dists)
  {
    interesting_sq_dists.push_back(4 * sq_dist);
  }
  std::sort(interesting_sq_dists.begin(), interesting_sq_dists.end());

  int critical_i = binary_search_first([&interesting_sq_dists, &graph_problem, n, k](int i) {
    assert(i >= 0);
    if (i >= int(interesting_sq_dists.size()))
    {
      return true;
    }
    return count_reachable_bones(n, interesting_sq_dists.at(i), graph_problem) >= k;
  });
  double q = interesting_sq_dists.at(critical_i);

  std::cout << a << " " << q << "\n";
}

int main()
{
  std::ios_base::sync_with_stdio(false);
  std::cout << std::fixed << std::setprecision(0);

  int t;
  std::cin >> t;
  for (int i = 0; i < t; i++)
  {
    testcase();
  }

  return 0;
}