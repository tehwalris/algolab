#include <iostream>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/prim_minimum_spanning_tree.hpp>
#include <limits>
#include <cassert>
#include <map>

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, boost::no_property, boost::property<boost::edge_weight_t, int>> Graph;

void testcase()
{
  int n, e, s, source_node, target_node;
  std::cin >> n >> e >> s >> source_node >> target_node;
  assert(n >= 1 && n <= 500 && e >= 1 && s >= 1 && s <= 10);
  assert(source_node >= 0 && source_node < n);
  assert(target_node >= 0 && target_node < n);

  Graph G(n);
  auto shared_weights = boost::get(boost::edge_weight, G);
  int infinite_weight = std::numeric_limits<int>::max();
  std::vector<std::map<Graph::edge_descriptor, int>> weights_by_species(s);
  for (int i = 0; i < e; i++)
  {
    int a, b;
    std::cin >> a >> b;
    Graph::edge_descriptor edge = boost::add_edge(a, b, infinite_weight, G).first;

    for (int j = 0; j < s; j++)
    {
      int w;
      std::cin >> w;
      weights_by_species.at(j).insert(std::make_pair(edge, w));
    }
  }

  for (int i = 0; i < s; i++)
  {
    int hive_location;
    std::cin >> hive_location;
    auto species_weight_map = boost::make_assoc_property_map(weights_by_species.at(i));
    std::vector<Graph::vertex_descriptor> predecessors(n);
    boost::prim_minimum_spanning_tree(G, boost::make_iterator_property_map(predecessors.begin(), boost::get(boost::vertex_index, G)), boost::weight_map(species_weight_map).root_vertex(hive_location));

    for (Graph::vertex_descriptor a = 0; a < Graph::vertex_descriptor(n); a++)
    {
      Graph::vertex_descriptor b = predecessors.at(a);
      if (a == b)
      {
        continue;
      }
      assert(boost::edge(a, b, G).second);
      Graph::edge_descriptor edge = boost::edge(a, b, G).first;
      int &min_weight = shared_weights[edge];
      min_weight = std::min({min_weight, species_weight_map[edge]});
    }
  }

  Graph G_finite(n);
  for (auto edge_iterators = boost::edges(G); edge_iterators.first != edge_iterators.second; edge_iterators.first++)
  {
    auto edge = *edge_iterators.first;
    int w = shared_weights[edge];
    assert(w >= 0);
    if (w != infinite_weight)
    {
      boost::add_edge(edge.m_source, edge.m_target, w, G_finite);
    }
  }

  std::vector<int> distances(n);
  boost::dijkstra_shortest_paths(G_finite, boost::vertex(source_node, G_finite), boost::distance_map(boost::make_iterator_property_map(distances.begin(), boost::get(boost::vertex_index, G_finite))));

  std::cout << distances.at(target_node) << "\n";
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