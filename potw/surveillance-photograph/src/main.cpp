#include <iostream>
#include <cassert>
#include <vector>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/push_relabel_max_flow.hpp>

const int debug_level = 0;

#define DEBUG(min_level, x)      \
  if (debug_level >= min_level)  \
  {                              \
    std::cerr << x << std::endl; \
  }

typedef boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS> GraphTraits;
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, boost::no_property,
                              boost::property<boost::edge_capacity_t, long,
                                              boost::property<boost::edge_residual_capacity_t, long,
                                                              boost::property<boost::edge_reverse_t, GraphTraits::edge_descriptor>>>>
    Graph;
typedef GraphTraits::vertex_descriptor Vertex;
typedef GraphTraits::edge_descriptor Edge;

void testcase()
{
  int n, m, k, l;
  std::cin >> n >> m >> k >> l;
  assert(n >= 1 && n <= 500 && m >= 0 && m <= 1e4 && k >= 1 && k <= 500 && l >= 1 && l <= 500);

  std::vector<int> station_counts_by_loc(n, 0);
  for (int i = 0; i < k; i++)
  {
    int x;
    std::cin >> x;
    assert(x >= 0 && x < n);
    station_counts_by_loc.at(x)++;
  }

  std::vector<int> photo_counts_by_loc(n, 0);
  for (int i = 0; i < l; i++)
  {
    int x;
    std::cin >> x;
    assert(x >= 0 && x < n);
    photo_counts_by_loc.at(x)++;
  }

  std::vector<std::vector<int>> alt_road_counts(n, std::vector<int>(n, 0));
  for (int i = 0; i < m; i++)
  {
    int x, y;
    std::cin >> x >> y;
    assert(x >= 0 && x < n && y >= 0 && y < n && x != y);
    alt_road_counts.at(x).at(y)++;
  }

  const int max_possible_flow = std::min(k, l);

  const int num_vertices = 2 + 2 * n;
  int next_free_vertex = 0;
  const Vertex source = next_free_vertex++;
  const Vertex sink = next_free_vertex++;
  auto intersection_at = [n, next_free_vertex](int i, bool post_photo_layer) -> Vertex {
    assert(i >= 0 && i < n);
    return next_free_vertex + (post_photo_layer ? n : 0) + i;
  };
  next_free_vertex += 2 * n;
  assert(next_free_vertex == num_vertices);
  Graph G(num_vertices);

  auto add_edge = [&G](int from, int to, long capacity) {
    auto c_map = boost::get(boost::edge_capacity, G);
    auto r_map = boost::get(boost::edge_reverse, G);
    const auto e = boost::add_edge(from, to, G).first;
    const auto rev_e = boost::add_edge(to, from, G).first;
    c_map[e] = capacity;
    c_map[rev_e] = 0;
    r_map[e] = rev_e;
    r_map[rev_e] = e;
  };

  for (int i = 0; i < n; i++)
  {
    int count = station_counts_by_loc.at(i);
    if (count > 0)
    {
      add_edge(source, intersection_at(i, false), count);
      add_edge(intersection_at(i, true), sink, count);
    }
  }

  for (int i = 0; i < n; i++)
  {
    int count = photo_counts_by_loc.at(i);
    if (count > 0)
    {
      add_edge(intersection_at(i, false), intersection_at(i, true), count);
    }
  }

  for (int i = 0; i < n; i++)
  {
    for (int j = 0; j < n; j++)
    {
      int count = alt_road_counts.at(i).at(j);
      if (count == 0)
      {
        continue;
      }
      assert(count > 0);

      DEBUG(2, "alt roads between " << i << " and " << j << ": " << count);

      add_edge(intersection_at(i, false), intersection_at(j, false), max_possible_flow);
      add_edge(intersection_at(i, true), intersection_at(j, true), count);
    }
  }

  int flow = boost::push_relabel_max_flow(G, source, sink);
  assert(flow >= 0 && flow <= max_possible_flow);
  std::cout << flow << "\n";
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