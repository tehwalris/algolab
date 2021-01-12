#include <iostream>
#include <cassert>
#include <vector>
#include <deque>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Triangulation_data_structure_2.h>
#include <CGAL/Triangulation_vertex_base_with_info_2.h>
#include <CGAL/Triangulation_face_base_2.h>
#include <boost/graph/adjacency_list.hpp>

const int debug_level = 0;

#define DEBUG(min_level, x)      \
  if (debug_level >= min_level)  \
  {                              \
    std::cerr << x << std::endl; \
  }

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Triangulation_vertex_base_with_info_2<int, K> TVB;
typedef CGAL::Triangulation_face_base_2<K> TFB;
typedef CGAL::Triangulation_data_structure_2<TVB, TFB> TriangulationDataStructure;
typedef CGAL::Delaunay_triangulation_2<K, TriangulationDataStructure> Triangulation;
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS> Graph;

K::Point_2 read_point()
{
  int x, y;
  std::cin >> x >> y;
  assert(abs(x) < (1 << 24) && abs(y) < (1 << 24));
  return K::Point_2(x, y);
}

inline Triangulation::Vertex_handle vertex_from_edge(Triangulation::Edge &edge, int i)
{
  assert(i == 0 || i == 1);
  return edge.first->vertex((edge.second + 1 + i) % 3);
}

inline Triangulation::Vertex_handle other_vertex_from_edge(Triangulation::Edge &edge, Triangulation::Vertex_handle &v)
{
  auto vertex_a = vertex_from_edge(edge, 0);
  auto vertex_b = vertex_from_edge(edge, 1);
  if (vertex_a == v)
  {
    return vertex_b;
  }
  else if (vertex_b == v)
  {
    return vertex_a;
  }
  assert(false);
  __builtin_unreachable();
}

struct NetworkAnalysis
{
  NetworkAnalysis(int n) : valid(false), component_map(n, -1){};

  bool valid;
  std::vector<int> component_map;
};

NetworkAnalysis analyze_network(std::vector<std::pair<K::Point_2, int>> radio_stations, Triangulation &all_stations_triangulation, std::function<bool(double)> is_close_enough)
{
  const int n = radio_stations.size();
  NetworkAnalysis network_analysis(n);

  std::vector<Triangulation::Vertex_handle> all_stations_triangulation_handles(n);
  for (auto it = all_stations_triangulation.finite_vertices_begin(); it != all_stations_triangulation.finite_vertices_end(); it++)
  {
    all_stations_triangulation_handles.at(it->info()) = it;
  }

  std::deque<std::pair<int, int>> queue;
  std::vector<int> color_by_station(n, -1);
  int next_connected_component = 0;
  for (int queue_starter = 0; queue_starter < n; queue_starter++)
  {
    if (network_analysis.component_map.at(queue_starter) >= 0)
    {
      continue;
    }
    queue.push_back(std::make_pair(queue_starter, 0));
    const int this_connected_component = next_connected_component++;
    network_analysis.component_map.at(queue_starter) = this_connected_component;

    while (!queue.empty())
    {
      int prev_index = queue.front().first;
      int prev_color = queue.front().second;
      queue.pop_front();
      K::Point_2 &prev_point = radio_stations.at(prev_index).first;

      assert(color_by_station.at(prev_index) == -1);
      color_by_station.at(prev_index) = prev_color;

      auto &prev_vertex = all_stations_triangulation_handles.at(prev_index);
      Triangulation::Edge_circulator c = all_stations_triangulation.incident_edges(prev_vertex);
      do
      {
        if (all_stations_triangulation.is_infinite(c))
        {
          continue;
        }

        auto next_vertex = other_vertex_from_edge(*c, prev_vertex);
        int next_index = next_vertex->info();
        K::Point_2 &next_point = next_vertex->point();
        int next_color = 1 - prev_color;

        if (network_analysis.component_map.at(next_index) == -1 && is_close_enough(CGAL::squared_distance(prev_point, next_point)))
        {
          network_analysis.component_map.at(next_index) = this_connected_component;
          queue.push_front(std::make_pair(next_index, next_color));
        }
      } while (++c != all_stations_triangulation.incident_edges(prev_vertex));
    }
  }

  assert(*std::min_element(network_analysis.component_map.begin(), network_analysis.component_map.end()) >= 0);

  std::vector<std::vector<K::Point_2>> point_vectors_by_color(2);
  for (int i = 0; i < n; i++)
  {
    int color = color_by_station.at(i);
    point_vectors_by_color.at(color).push_back(radio_stations.at(i).first);
  }

  for (auto &points_this_color : point_vectors_by_color)
  {
    Triangulation color_triangulation;
    color_triangulation.insert(points_this_color.begin(), points_this_color.end());
    for (auto it = color_triangulation.finite_edges_begin(); it != color_triangulation.finite_edges_end(); it++)
    {
      if (is_close_enough(color_triangulation.segment(it).squared_length()))
      {
        return network_analysis;
      }
    }
  }

  network_analysis.valid = true;
  return network_analysis;
}

void testcase()
{
  int n, m, r;
  std::cin >> n >> m >> r;
  assert(n >= 1 && n <= 9e4 && m >= 1 && m <= 9e4 && r > 0 && r < (1 << 24));

  const double r_squared = pow(double(r), 2);
  const auto is_close_enough = [r_squared](double sqlen) { return sqlen <= r_squared; };

  std::vector<std::pair<K::Point_2, int>> radio_stations;
  for (int i = 0; i < n; i++)
  {
    radio_stations.push_back(std::make_pair(read_point(), i));
  }

  std::vector<std::pair<K::Point_2, K::Point_2>> clues;
  for (int i = 0; i < m; i++)
  {
    auto p1 = read_point();
    auto p2 = read_point();
    clues.push_back(std::make_pair(p1, p2));
  }

  Triangulation all_stations_triangulation;
  all_stations_triangulation.insert(radio_stations.begin(), radio_stations.end());

  NetworkAnalysis network_analysis = analyze_network(radio_stations, all_stations_triangulation, is_close_enough);
  DEBUG(2, "network_analysis.valid " << network_analysis.valid);

  const auto network_component_from_radio_set = [&is_close_enough, &all_stations_triangulation, &network_analysis](K::Point_2 &radio_set) -> int {
    auto nearest_station = all_stations_triangulation.nearest_vertex(radio_set);
    double set_station_sq_dist = CGAL::squared_distance(radio_set, nearest_station->point());
    if (!is_close_enough(set_station_sq_dist))
    {
      return -1;
    }
    return network_analysis.component_map.at(nearest_station->info());
  };

  std::vector<bool> can_transmit_by_clue(m, false);
  if (network_analysis.valid)
  {
    for (int i = 0; i < m; i++)
    {
      DEBUG(3, "clue " << i);
      auto &clue = clues.at(i);
      if (is_close_enough(CGAL::squared_distance(clue.first, clue.second)))
      {
        can_transmit_by_clue.at(i) = true;
      }
      else
      {
        int first_component = network_component_from_radio_set(clue.first);
        if (first_component >= 0 && first_component == network_component_from_radio_set(clue.second))
        {
          can_transmit_by_clue.at(i) = true;
        }
      }
    }
  }

  for (bool can_transmit : can_transmit_by_clue)
  {
    std::cout << (can_transmit ? "y" : "n");
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