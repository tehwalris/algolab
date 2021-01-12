#include <iostream>
#include <cassert>
#include <vector>
#include <boost/graph/adjacency_list.hpp>
#include <boost/iterator/function_output_iterator.hpp>
#include <boost/graph/biconnected_components.hpp>

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS> Graph;

void testcase()
{
  int n, m;
  std::cin >> n >> m;
  assert(n >= 0 && n <= 30000 && m >= 0 && m <= 30000);

  Graph G(n + m);
  std::vector<std::pair<int, int>> edges;
  for (int i = 0; i < m; i++)
  {
    int start, end;
    std::cin >> start >> end;
    if (start > end)
    {
      std::swap(start, end);
    }
    assert(start >= 0 && start < n && end >= 0 && end < n && start < end);
    edges.push_back(std::make_pair(start, end));
    boost::add_edge(start, n + i, G);
    boost::add_edge(n + i, end, G);
  }

  std::vector<std::pair<int, int>> critical_bridges;
  auto on_articulation = [n, &critical_bridges, &edges](int vertex) {
    if (vertex >= n)
    {
      critical_bridges.push_back(edges.at(vertex - n));
    }
  };
  boost::articulation_points(G, boost::make_function_output_iterator(on_articulation));

  std::cout << critical_bridges.size() << "\n";
  std::sort(critical_bridges.begin(), critical_bridges.end());
  for (auto bridge : critical_bridges)
  {
    std::cout << bridge.first << " " << bridge.second << "\n";
  }
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