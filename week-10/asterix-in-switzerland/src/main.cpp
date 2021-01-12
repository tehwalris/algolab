#include <iostream>
#include <cassert>
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

struct DebtRelation
{
  int i, j, d;
};

void testcase()
{
  int n, m;
  std::cin >> n >> m;
  assert(n >= 1 && n <= 1e3 && m >= 0 && m <= 1e5);

  std::vector<int> balances_by_province(n);
  for (int &v : balances_by_province)
  {
    std::cin >> v;
    assert(abs(v) <= (1 << 20));
  }

  std::vector<DebtRelation> debt_relations(m);
  for (DebtRelation &r : debt_relations)
  {
    std::cin >> r.i >> r.j >> r.d;
    assert(r.i != r.j && r.i >= 0 && r.i < n && r.j >= 0 && r.j < n && r.d > 0 && r.d <= (1 << 20));
  }

  Graph G(n + 2);
  const int node_source = n;
  const int node_sink = n + 1;
  auto add_edge = [&G](int from, int to, long capacity) {
    auto c_map = boost::get(boost::edge_capacity, G);
    auto r_map = boost::get(boost::edge_reverse, G);
    const Graph::edge_descriptor e = boost::add_edge(from, to, G).first;
    const Graph::edge_descriptor rev_e = boost::add_edge(to, from, G).first;
    c_map[e] = capacity;
    c_map[rev_e] = 0; // reverse edge has no capacity!
    r_map[e] = rev_e;
    r_map[rev_e] = e;
  };

  long base_flow = 0;
  for (int i = 0; i < n; i++)
  {
    const int balance = balances_by_province.at(i);
    if (balance >= 0)
    {
      DEBUG(3, "non-negative balance " << balance << " i " << i);
      add_edge(node_source, i, balance);
      base_flow += balance;
    }
    else
    {
      add_edge(i, node_sink, -balance);
    }
  }

  for (const DebtRelation &r : debt_relations)
  {
    add_edge(r.i, r.j, r.d);
  }

  long flow = boost::push_relabel_max_flow(G, node_source, node_sink);
  DEBUG(1, "flow " << flow << " base_flow " << base_flow);
  std::cout << (flow < base_flow ? "yes" : "no") << "\n";
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