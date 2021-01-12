#include <iostream>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <cassert>

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, boost::no_property, boost::property<boost::edge_weight_t, int>> Graph;

void testcase()
{
  int n, m, k, x, y;
  std::cin >> n >> m >> k >> x >> y;
  assert(n >= 1 && n <= 1e4 && m >= 0 && m <= 1e5 && k >= 1 && k <= 10);
  assert(x >= 0 && x < n && y >= 0 && y < n);

  int total_nodes = n * (k + 1);
  auto node_at = [n, k](int index, int layer) {
    assert(index >= 0 && index < n && layer >= 0 && layer <= k);
    return n * layer + index;
  };

  Graph G(total_nodes);
  auto weights = boost::get(boost::edge_weight, G);
  auto add_or_shorten_edge = [&G, weights](int from, int to, int weight) {
    Graph::edge_descriptor test;
    auto existing = boost::edge(from, to, G);
    if (!existing.second || weights[existing.first] > weight)
    {
      boost::add_edge(from, to, weight, G);
    }
  };

  for (int i = 0; i < m; i++)
  {
    int a, b, c, d;
    std::cin >> a >> b >> c >> d;
    assert(a != b && a >= 0 && a < n && b >= 0 && b < n && c >= 1 && c < (2 << 9) && (d == 0 || d == 1));

    for (int j = 0; j <= k; j++)
    {
      if (d == 0 || j == k) // normal road or extra river
      {
        add_or_shorten_edge(node_at(a, j), node_at(b, j), c);
      }
      else // required river
      {
        add_or_shorten_edge(node_at(a, j), node_at(b, j + 1), c);
        add_or_shorten_edge(node_at(a, j + 1), node_at(b, j), c);
      }
    }
  }

  std::vector<int> distances(total_nodes);
  boost::dijkstra_shortest_paths(G, boost::vertex(node_at(x, 0), G), boost::distance_map(boost::make_iterator_property_map(distances.begin(), boost::get(boost::vertex_index, G))));

  std::cout << distances.at(node_at(y, k)) << "\n";
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