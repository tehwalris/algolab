#include <iostream>
#include <cassert>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/push_relabel_max_flow.hpp>

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS> GraphTraits;
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, boost::no_property,
                              boost::property<boost::edge_capacity_t, int,
                                              boost::property<boost::edge_residual_capacity_t, int,
                                                              boost::property<boost::edge_reverse_t, GraphTraits::edge_descriptor>>>>
    Graph;

void testcase()
{
  int m, n, k, c;
  std::cin >> m >> n >> k >> c;
  assert(m >= 0 && m <= 50 && n >= 0 && n <= 50);
  assert(k >= 0 && k <= m * n);
  assert(c >= 0 && c <= 4);

  int total_nodes = 2 + 2 * m * n + m * (n + 1) + n * (m + 1);
  Graph G(total_nodes);
  int next_free_node = 0;
  Graph::vertex_descriptor source_node = next_free_node++;
  Graph::vertex_descriptor sink_node = next_free_node++;
  auto node_from_intersection = [next_free_node, m, n](int col, int row, bool source_part) -> Graph::vertex_descriptor {
    assert(col >= 0 && col < m && row >= 0 && row < n);
    return next_free_node + 2 * (col * n + row) + (source_part ? 1 : 0);
  };
  next_free_node += 2 * m * n;
  auto node_from_col_hall = [next_free_node, m, n](int col, int hall) -> Graph::vertex_descriptor {
    assert(col >= 0 && col < m && hall >= 0 && hall <= n);
    return next_free_node + col * (n + 1) + hall;
  };
  next_free_node += m * (n + 1);
  auto node_from_row_hall = [next_free_node, m, n](int row, int hall) -> Graph::vertex_descriptor {
    assert(row >= 0 && row < n && hall >= 0 && hall <= m);
    return next_free_node + row * (m + 1) + hall;
  };
  next_free_node += n * (m + 1);
  assert(next_free_node == total_nodes);

  auto capacity_map = boost::get(boost::edge_capacity, G);
  auto reverse_map = boost::get(boost::edge_reverse, G);
  auto add_edge = [&G, &capacity_map, &reverse_map, total_nodes](int from, int to, int capacity) {
    assert(from >= 0 && from < total_nodes && to >= 0 && to < total_nodes);
    Graph::edge_descriptor e = boost::add_edge(from, to, G).first;
    Graph::edge_descriptor rev_e = boost::add_edge(to, from, G).first;
    capacity_map[e] = capacity;
    capacity_map[rev_e] = 0;
    reverse_map[e] = rev_e;
    reverse_map[rev_e] = e;
  };

  for (int col = 0; col < m; col++)
  {
    add_edge(node_from_col_hall(col, 0), sink_node, 1);
    add_edge(node_from_col_hall(col, n), sink_node, 1);
    for (int i = 0; i < n; i++)
    {
      auto intersection_source = node_from_intersection(col, i, true);
      auto intersection_target = node_from_intersection(col, i, false);
      for (int hall : {i, i + 1})
      {
        auto hall_node = node_from_col_hall(col, hall);
        add_edge(hall_node, intersection_source, 1);
        add_edge(intersection_target, hall_node, 1);
      }
    }
  }

  for (int row = 0; row < n; row++)
  {
    add_edge(node_from_row_hall(row, 0), sink_node, 1);
    add_edge(node_from_row_hall(row, m), sink_node, 1);
    for (int i = 0; i < m; i++)
    {
      auto intersection_source = node_from_intersection(i, row, true);
      auto intersection_target = node_from_intersection(i, row, false);
      for (int hall : {i, i + 1})
      {
        auto hall_node = node_from_row_hall(row, hall);
        add_edge(hall_node, intersection_source, 1);
        add_edge(intersection_target, hall_node, 1);
      }
    }
  }

  for (int col = 0; col < m; col++)
  {
    for (int row = 0; row < n; row++)
    {
      add_edge(node_from_intersection(col, row, true), node_from_intersection(col, row, false), c);
    }
  }

  for (int i = 0; i < k; i++)
  {
    int x, y;
    std::cin >> x >> y;
    assert(x >= 0 && x < m && y >= 0 && y < n);
    add_edge(source_node, node_from_intersection(x, y, true), 1);
  }

  int flow = boost::push_relabel_max_flow(G, source_node, sink_node);
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
