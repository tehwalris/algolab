#include <iostream>
#include <cassert>
#include <vector>
#include <map>
#include <deque>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/kruskal_min_spanning_tree.hpp>
#include <boost/iterator/function_output_iterator.hpp>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>

const int debug_level = 0;

#define DEBUG(min_level, x)      \
  if (debug_level >= min_level)  \
  {                              \
    std::cerr << x << std::endl; \
  }

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, boost::no_property, boost::property<boost::edge_weight_t, long>> Graph;

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Delaunay_triangulation_2<K> Triangulation;

bool testcase()
{
  int n;
  std::cin >> n;
  if (n == 0)
  {
    return false;
  }
  assert(n >= 2 && n <= 6e4);

  DEBUG(1, "\ntest case; n = " << n);

  std::vector<K::Point_2> infected_points;
  for (int i = 0; i < n; i++)
  {
    int x, y;
    std::cin >> x >> y;
    assert(abs(x) < (1 << 24) && abs(y) < (1 << 24));
    infected_points.emplace_back(x, y);
  }

  int m;
  std::cin >> m;
  assert(m >= 1 && m <= 4e4);

  std::vector<std::pair<K::Point_2, long>> healthy_points;
  for (int i = 0; i < m; i++)
  {
    int x, y;
    long d;
    std::cin >> x >> y >> d;
    assert(abs(x) < (1 << 24) && abs(y) < (1 << 24));
    assert(d >= 0 && d < (long(1) << 49));
    healthy_points.push_back(std::make_pair(K::Point_2(x, y), d));
  }

  Triangulation triangulation;
  triangulation.insert(infected_points.begin(), infected_points.end());

  const int indexed_face_count = triangulation.number_of_faces() + 1;
  Graph mst_input_graph(indexed_face_count);
  auto mst_input_weights = boost::get(boost::edge_weight, mst_input_graph);
  std::map<Triangulation::Face_handle, int> face_indices_by_handle;
  int next_free_face_index = 0;
  const int infinite_face_index = next_free_face_index++;
  auto index_from_face_handle = [&face_indices_by_handle, &next_free_face_index, &triangulation, indexed_face_count, infinite_face_index](Triangulation::Face_handle fh) {
    if (triangulation.is_infinite(fh))
    {
      DEBUG(4, "infinite face");
      return infinite_face_index;
    }

    int face_index;
    if (face_indices_by_handle.count(fh) == 1)
    {
      DEBUG(4, "existing face");
      face_index = face_indices_by_handle.at(fh);
    }
    else
    {
      DEBUG(4, "new face");
      assert(face_indices_by_handle.count(fh) == 0);
      face_index = next_free_face_index++;
      face_indices_by_handle.insert(std::make_pair(fh, face_index));
    }
    DEBUG(4, "face_index " << face_index << " limit " << indexed_face_count);
    assert(face_index >= 0 && face_index < indexed_face_count);
    return face_index;
  };
  for (auto it = triangulation.finite_edges_begin(); it != triangulation.finite_edges_end(); it++)
  {
    long weight = -CGAL::squared_distance(it->first->vertex((it->second + 1) % 3)->point(), it->first->vertex((it->second + 2) % 3)->point());
    int vertex_a = index_from_face_handle(it->first);
    int vertex_b = index_from_face_handle(it->first->neighbor(it->second));
    if (vertex_a > vertex_b)
    {
      std::swap(vertex_a, vertex_b);
    }
    assert(vertex_a < vertex_b);
    DEBUG(3, "vertex_a " << vertex_a << " vertex_b " << vertex_b << " weight " << weight);
    auto edge_result = boost::edge(vertex_a, vertex_b, mst_input_graph);
    if (edge_result.second)
    {
      long &saved_weight = mst_input_weights[edge_result.first];
      saved_weight = std::min(saved_weight, weight);
    }
    else
    {
      boost::add_edge(vertex_a, vertex_b, weight, mst_input_graph);
    }
  }

  Graph mst(indexed_face_count);
  auto add_mst_edge = [&mst, &mst_input_graph](Graph::edge_descriptor edge) {
    long weight = boost::get(boost::edge_weight_t(), mst_input_graph, edge);
    DEBUG(3, "add_mst_edge " << edge.m_source << " " << edge.m_target << " " << weight);
    boost::add_edge(edge.m_source, edge.m_target, weight, mst);
  };
  boost::kruskal_minimum_spanning_tree(mst_input_graph, boost::make_function_output_iterator(std::ref(add_mst_edge)));

  std::deque<std::pair<int, long>> bfs_queue{std::make_pair(infinite_face_index, std::numeric_limits<long>::max())};
  std::vector<long> largest_escape_by_face(indexed_face_count, 0);
  while (!bfs_queue.empty())
  {
    int prev_vertex = bfs_queue.front().first;
    long prev_largest_escape = bfs_queue.front().second;
    bfs_queue.pop_front();

    assert(largest_escape_by_face.at(prev_vertex) == 0);
    largest_escape_by_face.at(prev_vertex) = prev_largest_escape;
    DEBUG(3, "prev_vertex " << prev_vertex << " prev_largest_escape " << prev_largest_escape);

    for (auto its = boost::out_edges(prev_vertex, mst); its.first != its.second; its.first++)
    {
      int next_vertex = its.first->m_target;
      assert(next_vertex != prev_vertex);
      if (largest_escape_by_face.at(next_vertex) > 0)
      {
        continue;
      }
      DEBUG(3, "next_vertex " << next_vertex);

      long limit_of_this_edge = -boost::get(boost::edge_weight_t(), mst, *its.first);
      long next_largest_escape = std::min(prev_largest_escape, limit_of_this_edge);
      DEBUG(3, "limit_of_this_edge " << limit_of_this_edge << " next_largest_escape " << next_largest_escape)
      assert(next_largest_escape > 0);

      bfs_queue.push_back(std::make_pair(next_vertex, next_largest_escape));
    }
  }
  int min_largest_escape_by_triangle = *std::min_element(largest_escape_by_face.begin(), largest_escape_by_face.end());
  DEBUG(2, "min_largest_escape_by_triangle " << min_largest_escape_by_triangle);
  assert(min_largest_escape_by_triangle > 0);

  std::vector<bool> can_escape_by_index(m);
  for (int i = 0; i < m; i++)
  {
    auto &healthy_point = healthy_points.at(i);
    Triangulation::Face_handle face = triangulation.locate(healthy_point.first);
    int face_index = index_from_face_handle(face);
    DEBUG(2, "face_index " << face_index);
    long largest_escape = largest_escape_by_face.at(face_index);
    DEBUG(2, "largest_escape (initial) " << largest_escape);

    auto nearest = triangulation.nearest_vertex(healthy_point.first);
    DEBUG(3, "limiting using point " << nearest->point());
    largest_escape = std::min(largest_escape, 4 * long(CGAL::squared_distance(healthy_point.first, nearest->point())));

    DEBUG(2, "largest_escape (adjusted) " << largest_escape);
    can_escape_by_index.at(i) = 4 * healthy_point.second <= largest_escape;
  }

  for (bool v : can_escape_by_index)
  {
    std::cout << (v ? "y" : "n");
  }
  std::cout << "\n";

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