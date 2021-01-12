#include <iostream>
#include <cassert>
#include <vector>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/push_relabel_max_flow.hpp>
#include <boost/graph/successive_shortest_path_nonnegative_weights.hpp>
#include <boost/graph/find_flow_cost.hpp>

typedef boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS> GraphTraits;
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, boost::no_property,
                              boost::property<boost::edge_capacity_t, long,
                                              boost::property<boost::edge_residual_capacity_t, long,
                                                              boost::property<boost::edge_reverse_t, GraphTraits::edge_descriptor,
                                                                              boost::property<boost::edge_weight_t, long>>>>>
    Graph;

const int max_possible_bid = 100;

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

void testcase()
{
  int n, m, s;
  std::cin >> n >> m >> s;
  assert(n >= 1 && n <= 100 && m >= 1 && m <= 100 && s >= 1 && s <= m);

  std::vector<int> limits_by_state(s);
  for (int &limit : limits_by_state)
  {
    std::cin >> limit;
    assert(limit >= 0 && limit <= n);
  }

  std::vector<int> state_by_site(m);
  for (int &state : state_by_site)
  {
    std::cin >> state;
    assert(state >= 1 && state <= s);
    state--;
  }

  std::vector<std::vector<int>> bids_by_buyer(n, std::vector<int>(m));
  for (std::vector<int> &bids : bids_by_buyer)
  {
    for (int &bid : bids)
    {
      std::cin >> bid;
      assert(bid >= 1 && bid <= max_possible_bid);
    }
  }

  int next_free_node = 0;
  const int node_source = next_free_node++;
  const int node_sink = next_free_node++;
  const auto get_node_for_buyer = [next_free_node, n](int buyer) {
    assert(buyer >= 0 && buyer < n);
    return next_free_node + buyer;
  };
  next_free_node += n;
  const auto get_node_for_site = [next_free_node, m](int site) {
    assert(site >= 0 && site < m);
    return next_free_node + site;
  };
  next_free_node += m;
  const auto get_node_for_state = [next_free_node, s](int state) {
    assert(state >= 0 && state < s);
    return next_free_node + state;
  };
  next_free_node += s;
  const int num_nodes = next_free_node;

  Graph G(num_nodes);
  EdgeAdder adder(G);

  for (int i = 0; i < n; i++)
  {
    adder.add_edge(node_source, get_node_for_buyer(i), 1, 0);
    for (int j = 0; j < m; j++)
    {
      const int bid = bids_by_buyer.at(i).at(j);
      adder.add_edge(get_node_for_buyer(i), get_node_for_site(j), 1, -bid + max_possible_bid);
    }
  }

  for (int i = 0; i < m; i++)
  {
    adder.add_edge(get_node_for_site(i), get_node_for_state(state_by_site.at(i)), 1, 0);
  }

  for (int i = 0; i < s; i++)
  {
    adder.add_edge(get_node_for_state(i), node_sink, limits_by_state.at(i), 0);
  }

  const int flow = boost::push_relabel_max_flow(G, node_source, node_sink);
  assert(flow >= std::min(s, std::min(n, m)) && flow <= std::min(n, m));
  boost::successive_shortest_path_nonnegative_weights(G, node_source, node_sink);
  const int profit = -(boost::find_flow_cost(G) - max_possible_bid * flow);
  assert(profit > 0);
  std::cout << flow << " " << profit << "\n";
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