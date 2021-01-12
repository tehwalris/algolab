#include <iostream>
#include <cassert>
#include <vector>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/successive_shortest_path_nonnegative_weights.hpp>
#include <boost/graph/find_flow_cost.hpp>

typedef boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS> GraphTraits;
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, boost::no_property,
                              boost::property<boost::edge_capacity_t, long,
                                              boost::property<boost::edge_residual_capacity_t, long,
                                                              boost::property<boost::edge_reverse_t, GraphTraits::edge_descriptor,
                                                                              boost::property<boost::edge_weight_t, long>>>>>
    Graph;

const int debug_level = 0;

#define DEBUG(min_level, x)      \
  if (debug_level >= min_level)  \
  {                              \
    std::cerr << x << std::endl; \
  }

class EdgeAdder
{
  Graph &G;

public:
  explicit EdgeAdder(Graph &G) : G(G) {}
  void add_edge(int from, int to, long capacity, long cost)
  {
    auto c_map = boost::get(boost::edge_capacity, G);
    auto r_map = boost::get(boost::edge_reverse, G);
    auto w_map = boost::get(boost::edge_weight, G);
    const Graph::edge_descriptor e = boost::add_edge(from, to, G).first;
    const Graph::edge_descriptor rev_e = boost::add_edge(to, from, G).first;
    c_map[e] = capacity;
    c_map[rev_e] = 0; // reverse edge has no capacity!
    r_map[e] = rev_e;
    r_map[rev_e] = e;
    w_map[e] = cost;
    w_map[rev_e] = -cost;
  }
};

const long max_priority = 1 << 7;

struct Mission
{
  int x, y, q;
};

void testcase()
{
  int n, m, l;
  std::cin >> n >> m >> l;
  assert(n >= 2 && n <= 3e2 && m >= 1 && m <= 1e5 && l >= 1 && l <= 1e2);

  std::vector<Mission> missions(m);
  for (Mission &mission : missions)
  {
    std::cin >> mission.x >> mission.y >> mission.q;
    assert(mission.x >= 0 && mission.x < mission.y && mission.y <= n - 1 && mission.q >= 1 && mission.q <= max_priority);
  }

  int next_free_node = 0;
  const int node_source = next_free_node++;
  const auto get_node_for_stop = [next_free_node, n](int stop) {
    assert(stop >= 0 && stop < n);
    return next_free_node + stop;
  };
  next_free_node += n;
  const int node_sink = get_node_for_stop(n - 1);
  const auto get_node_for_mission = [next_free_node, m](int mission) {
    assert(mission >= 0 && mission < m);
    return next_free_node + mission;
  };
  next_free_node += m;
  const int num_nodes = next_free_node;

  Graph G(num_nodes);
  EdgeAdder adder(G);

  adder.add_edge(node_source, get_node_for_stop(0), l, 0);

  for (int i = 1; i < n; i++)
  {
    adder.add_edge(get_node_for_stop(i - 1), get_node_for_stop(i), l, max_priority);
  }

  for (int i = 0; i < m; i++)
  {
    const Mission &mission = missions.at(i);
    adder.add_edge(get_node_for_stop(mission.x), get_node_for_mission(i), 1, max_priority * (mission.y - mission.x) - mission.q);
    adder.add_edge(get_node_for_mission(i), get_node_for_stop(mission.y), 1, 0);
  }

  boost::successive_shortest_path_nonnegative_weights(G, node_source, node_sink);
  const int priority_sum = -(boost::find_flow_cost(G) - l * (n - 1) * max_priority);
  DEBUG(1, "priority_sum " << priority_sum);
  assert(priority_sum > 0);
  std::cout << priority_sum << "\n";
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