#include <iostream>
#include <cassert>
#include <vector>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/push_relabel_max_flow.hpp>

typedef boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS> GraphTraits;
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, boost::no_property,
                              boost::property<boost::edge_capacity_t, long,
                                              boost::property<boost::edge_residual_capacity_t, long,
                                                              boost::property<boost::edge_reverse_t, GraphTraits::edge_descriptor>>>>
    Graph;

const int debug_level = 0;

#define DEBUG(min_level, x)      \
  if (debug_level >= min_level)  \
  {                              \
    std::cerr << x << std::endl; \
  }

struct Limb
{
public:
  int a, b, c;

  Limb(int a, int b, int c) : a(a), b(b), c(c){};
};

class EdgeAdder
{
  Graph &G;

public:
  explicit EdgeAdder(Graph &G) : G(G) {}
  void add_edge(int from, int to, long capacity)
  {
    auto c_map = boost::get(boost::edge_capacity, G);
    auto r_map = boost::get(boost::edge_reverse, G);
    const Graph::edge_descriptor e = boost::add_edge(from, to, G).first;
    const Graph::edge_descriptor rev_e = boost::add_edge(to, from, G).first;
    c_map[e] = capacity;
    c_map[rev_e] = 0; // reverse edge has no capacity!
    r_map[e] = rev_e;
    r_map[rev_e] = e;
  }
};

void testcase()
{
  int n, m;
  std::cin >> n >> m;
  assert(n >= 2 && n <= 200 && m >= 0 && m <= 5000);

  std::vector<std::vector<int>> limb_index_by_a_b(n, std::vector<int>(n, -1));
  std::vector<Limb> limbs;
  for (int i = 0; i < m; i++)
  {
    int a, b, c;
    std::cin >> a >> b >> c;
    assert(a >= 0 && a < n && b >= 0 && b < n && a != b && c >= 1 && c <= 1000);

    int &limb_index = limb_index_by_a_b.at(a).at(b);
    if (limb_index == -1)
    {
      limb_index = limbs.size();
      limbs.emplace_back(a, b, 0);
    }

    Limb &limb = limbs.at(limb_index);
    assert(limb.a == a && limb.b == b);
    limb.c += c;
  }

  int next_free_node = 0;
  const auto get_node_for_figure = [next_free_node, n](int i) {
    assert(i >= 0 && i < n);
    return next_free_node + i;
  };
  next_free_node += n;
  const int num_nodes = next_free_node;

  Graph G(num_nodes);
  EdgeAdder adder(G);
  auto c_map = boost::get(boost::edge_capacity, G);
  auto rc_map = boost::get(boost::edge_residual_capacity, G);

  for (const Limb &limb : limbs)
  {
    adder.add_edge(get_node_for_figure(limb.a), get_node_for_figure(limb.b), limb.c);
  }

  int min_cut_cost_ever = std::numeric_limits<int>::max();
  for (int source_figure = 0; source_figure < n; source_figure++)
  {
    const int target_figure = (source_figure + 1) % n;

    const int node_source = get_node_for_figure(source_figure);
    const int node_target = get_node_for_figure(target_figure);

    boost::push_relabel_max_flow(G, node_source, node_target);

    std::vector<bool> visited_by_node(num_nodes, false);
    visited_by_node.at(node_source) = true;
    std::deque<int> queue{node_source};
    while (!queue.empty())
    {
      const int node = queue.front();
      queue.pop_front();
      for (auto it = boost::out_edges(node, G); it.first != it.second; it.first++)
      {
        const int next_node = boost::target(*it.first, G);
        if (rc_map[*it.first] > 0 && !visited_by_node.at(next_node))
        {
          visited_by_node.at(next_node) = true;
          queue.push_back(next_node);
        }
      }
    }

    int min_cut_cost = 0;
    for (int i = 0; i < num_nodes; i++)
    {
      if (!visited_by_node.at(i))
      {
        continue;
      }
      for (auto it = boost::out_edges(i, G); it.first != it.second; it.first++)
      {
        const int j = boost::target(*it.first, G);
        if (!visited_by_node.at(j))
        {
          min_cut_cost += c_map[*it.first];
        }
      }
    }
    assert(min_cut_cost >= 0);
    min_cut_cost_ever = std::min(min_cut_cost_ever, min_cut_cost);
  }

  std::cout << min_cut_cost_ever << "\n";
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