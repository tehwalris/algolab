#include <iostream>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/kruskal_min_spanning_tree.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <algorithm>

typedef boost::adjacency_list<
    boost::vecS,
    boost::vecS,
    boost::undirectedS,
    boost::no_property,
    boost::property<boost::edge_weight_t, int>>
    Graph;
typedef boost::graph_traits<Graph>::edge_descriptor Edge;

void testcase()
{
  int n, m;
  std::cin >> n >> m;
  assert(n >= 0 && n <= 100);
  assert(m >= 0 && m <= n * (n - 1) / 2);

  Graph G(n);
  for (int i = 0; i < m; i++)
  {
    int a, b, w;
    std::cin >> a >> b >> w;
    boost::add_edge(a, b, w, G);
  }

  std::vector<Edge> spanning_tree;
  boost::kruskal_minimum_spanning_tree(G, std::back_inserter(spanning_tree));

  auto weights = boost::get(boost::edge_weight, G);
  int sum_of_weights = 0;
  for (auto edge : spanning_tree)
  {
    sum_of_weights += weights[edge];
  }

  std::vector<int> distances(n);
  boost::dijkstra_shortest_paths(G, boost::vertex(0, G), boost::distance_map(boost::make_iterator_property_map(distances.begin(), boost::get(boost::vertex_index, G))));

  std::cout
      << sum_of_weights
      << " "
      << *std::max_element(distances.begin(), distances.end())
      << "\n";
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