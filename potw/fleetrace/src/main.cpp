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

struct BoatSailorPair
{
  int b, s, c;
};

const int max_possible_spectacle = 50;

void testcase()
{
  int b, s, p;
  std::cin >> b >> s >> p;
  assert(b >= 1 && b <= 500 && s >= 1 && s <= 500 && p >= 1 && p <= b * s && p <= 5000);

  std::vector<BoatSailorPair> boat_sailor_pairs(p);
  for (BoatSailorPair &pair : boat_sailor_pairs)
  {
    std::cin >> pair.b >> pair.s >> pair.c;
    assert(pair.b >= 0 && pair.b < b && pair.s >= 0 && pair.s < s && pair.c >= 1 && pair.c <= max_possible_spectacle);
  }

  int next_node_index = 0;
  const int node_source = next_node_index++;
  const int node_target = next_node_index++;
  const auto get_node_boat = [next_node_index, b](int i) {
    assert(i >= 0 && i < b);
    return next_node_index + i;
  };
  next_node_index += b;
  const auto get_node_sailor = [next_node_index, s](int i) {
    assert(i >= 0 && i < s);
    return next_node_index + i;
  };
  next_node_index += s;
  const int num_nodes = next_node_index;
  Graph G(num_nodes);

  const auto add_edge = [&G](int from, int to, long capacity, long cost) {
    auto c_map = boost::get(boost::edge_capacity, G);
    auto r_map = boost::get(boost::edge_reverse, G);
    auto w_map = boost::get(boost::edge_weight, G);
    const Graph::edge_descriptor e = boost::add_edge(from, to, G).first;
    const Graph::edge_descriptor rev_e = boost::add_edge(to, from, G).first;
    c_map[e] = capacity;
    c_map[rev_e] = 0;
    r_map[e] = rev_e;
    r_map[rev_e] = e;
    w_map[e] = cost;
    w_map[rev_e] = -cost;
  };

  for (int i = 0; i < b; i++)
  {
    add_edge(node_source, get_node_boat(i), 1, 0);
    add_edge(get_node_boat(i), node_target, 1, max_possible_spectacle);
  }

  for (int i = 0; i < s; i++)
  {
    add_edge(get_node_sailor(i), node_target, 1, 0);
  }

  for (BoatSailorPair &pair : boat_sailor_pairs)
  {
    add_edge(get_node_boat(pair.b), get_node_sailor(pair.s), 1, max_possible_spectacle - pair.c);
  }

  boost::successive_shortest_path_nonnegative_weights(G, node_source, node_target);
  const int flow_cost = boost::find_flow_cost(G);
  const int spectacle_sum = b * max_possible_spectacle - flow_cost;
  assert(spectacle_sum > 0);
  std::cout << spectacle_sum << "\n";
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