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

void testcase()
{
  int n, m, s;
  std::cin >> n >> m >> s;
  assert(n >= 1 && m >= 0 && s >= 1 && s < n);

  Graph G(n + 1);
  auto source = boost::vertex(0, G), sink = boost::vertex(n, G);
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

  std::vector<int> num_stores_by_intersection(n, 0);
  for (int i = 0; i < s; i++)
  {
    int shop_loc;
    std::cin >> shop_loc;
    assert(shop_loc >= 0 && shop_loc < n);
    num_stores_by_intersection.at(shop_loc)++;
  }
  for (int i = 0; i < n; i++)
  {
    if (num_stores_by_intersection.at(i) > 0)
    {
      add_edge(i, sink, num_stores_by_intersection.at(i));
    }
  }

  for (int i = 0; i < m; i++)
  {
    int from, to;
    std::cin >> from >> to;
    assert(from >= 0 && from < n && to >= 0 && to < n);
    add_edge(from, to, 1);
    add_edge(to, from, 1);
  }

  int flow = boost::push_relabel_max_flow(G, source, sink);
  assert(flow >= 0 && flow <= s);
  std::cout << (flow == s ? "yes\n" : "no\n");
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