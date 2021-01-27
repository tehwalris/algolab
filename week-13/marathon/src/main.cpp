#include <iostream>
#include <cassert>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/push_relabel_max_flow.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>

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
                                                              boost::property<boost::edge_reverse_t, GraphTraits::edge_descriptor,
                                                                              boost::property<boost::edge_weight_t, long>>>>>
    Graph;

void testcase()
{
  int n, m, s, f;
  std::cin >> n >> m >> s >> f;
  assert(n >= 2 && n <= 1e3 && m >= 0 && m <= 5e3);
  const auto is_valid_node = [n](int i) { return i >= 0 && i < n; };
  (void)is_valid_node;
  assert(is_valid_node(s) && is_valid_node(f) && s != f);

  Graph dijkstra_graph(n);
  const auto add_dijkstra_edge = [&dijkstra_graph](int a, int b, long capacity, long distance) {
    assert(a != b);
    auto w_map = boost::get(boost::edge_weight, dijkstra_graph);
    auto c_map = boost::get(boost::edge_capacity, dijkstra_graph);
    const Graph::edge_descriptor e = boost::add_edge(a, b, dijkstra_graph).first;
    const Graph::edge_descriptor rev_e = boost::add_edge(b, a, dijkstra_graph).first;
    w_map[e] = distance;
    w_map[rev_e] = distance;
    c_map[e] = capacity;
    c_map[rev_e] = capacity;
  };

  for (int i = 0; i < m; i++)
  {
    int a, b, c, d;
    std::cin >> a >> b >> c >> d;
    assert(is_valid_node(a) && is_valid_node(b));
    assert(c >= 1 && c <= 1e4 && d >= 1 && d <= 1e4);
    if (a != b)
    {
      add_dijkstra_edge(a, b, c, d);
    }
  }

  std::vector<int> source_distances_by_node(n);
  boost::dijkstra_shortest_paths(dijkstra_graph, s, boost::distance_map(boost::make_iterator_property_map(source_distances_by_node.begin(), boost::get(boost::vertex_index, dijkstra_graph))));

  Graph flow_graph(n);
  const auto add_flow_edge = [&flow_graph](int from, int to, long capacity) {
    auto c_map = boost::get(boost::edge_capacity, flow_graph);
    auto r_map = boost::get(boost::edge_reverse, flow_graph);
    const Graph::edge_descriptor e = boost::add_edge(from, to, flow_graph).first;
    const Graph::edge_descriptor rev_e = boost::add_edge(to, from, flow_graph).first;
    c_map[e] = capacity;
    c_map[rev_e] = 0;
    r_map[e] = rev_e;
    r_map[rev_e] = e;
  };

  for (auto its = boost::edges(dijkstra_graph); its.first != its.second; its.first++)
  {
    const Graph::edge_descriptor edge = *its.first;
    if (source_distances_by_node.at(edge.m_target) - source_distances_by_node.at(edge.m_source) == boost::get(boost::edge_weight, dijkstra_graph)[edge])
    {
      add_flow_edge(edge.m_source, edge.m_target, boost::get(boost::edge_capacity, dijkstra_graph)[edge]);
    }
  }

  const int flow = boost::push_relabel_max_flow(flow_graph, s, f);
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
    DEBUG(1, "");
  }

  return 0;
}