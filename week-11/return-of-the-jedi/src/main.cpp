#include <iostream>
#include <cassert>
#include <boost/graph/adjacency_matrix.hpp>
#include <boost/graph/prim_minimum_spanning_tree.hpp>

const int debug_level = 0;

#define DEBUG(min_level, x)      \
  if (debug_level >= min_level)  \
  {                              \
    std::cerr << x << std::endl; \
  }

typedef std::tuple<int, int, int> RawEdgeWeight;
typedef boost::adjacency_matrix<boost::directedS, boost::no_property, boost::property<boost::edge_weight_t, long>> Graph;

struct MstEdge
{
  int cost, from, to;
};

void testcase()
{
  int n, root_i;
  std::cin >> n >> root_i;
  assert(n >= 3 && n <= 1e3 && root_i >= 1 && root_i <= n);
  root_i--;

  Graph G(n);
  std::vector<std::vector<int>> edge_costs(n, std::vector<int>(n));
  for (int i = 0; i < n; i++)
  {
    for (int j = i + 1; j < n; j++)
    {
      int cost;
      std::cin >> cost;
      assert(cost >= 1 && cost <= (1 << 20));

      edge_costs.at(i).at(j) = cost;
      edge_costs.at(j).at(i) = cost;

      boost::add_edge(i, j, long(cost) << 20 | long(n - j) << 10 | long(n - i), G);
      boost::add_edge(j, i, long(cost) << 20 | long(n - i) << 10 | long(n - j), G);
    }
  }

  std::vector<Graph::vertex_descriptor> predecessors(n);
  boost::prim_minimum_spanning_tree(G, predecessors.data(), boost::root_vertex(root_i));

  auto weights = boost::get(boost::edge_weight, G);
  int leia_cost = 0;
  for (int i = 0; i < n; i++)
  {
    const Graph::vertex_descriptor p = predecessors.at(i);
    if (int(p) == i)
    {
      DEBUG(3, "same predecessor " << i);
      assert(i == root_i);
      continue;
    }

    const int cost = edge_costs.at(p).at(i);
    DEBUG(3, "p " << p << " to i " << i << " costs " << cost);
    assert(cost >= 1 && cost <= (1 << 20));
    leia_cost += cost;
  }
  DEBUG(2, "leia_cost " << leia_cost);

  auto max_cost_between_endpoints = [root_i](const std::vector<std::pair<int, int>> &path_a, const std::vector<std::pair<int, int>> &path_b) {
    auto it_a = path_a.rbegin(), it_b = path_b.rbegin();
    while (it_a != path_a.rend() && it_b != path_b.rend() && it_a->first == it_b->first)
    {
      it_a++;
      it_b++;
    }
    int max_cost = 0;
    if (it_a != path_a.rend())
    {
      max_cost = std::max(max_cost, it_a->second);
    }
    if (it_b != path_b.rend())
    {
      max_cost = std::max(max_cost, it_b->second);
    }
    return max_cost;
  };

  std::vector<std::vector<std::pair<int, int>>> paths_to_root(n);
  for (int i = 0; i < n; i++)
  {
    std::vector<std::pair<int, int>> &path = paths_to_root.at(i);
    int j = i;
    int max_cost = 0;
    while (true)
    {
      const int p = predecessors.at(j);
      if (p == j)
      {
        break;
      }
      const int cost = edge_costs.at(p).at(j);
      max_cost = std::max(max_cost, cost);
      path.push_back(std::make_pair(j, max_cost));
      j = p;
    }
  }

  int min_total_cost = std::numeric_limits<int>::max();
  for (int from = 0; from < n; from++)
  {
    for (int to = from + 1; to < n; to++)
    {
      DEBUG(3, "considering adding edge " << from << " " << to);

      if (int(predecessors.at(from)) == to || int(predecessors.at(to)) == from)
      {
        DEBUG(3, "edge already in mst");
        continue;
      }

      const int added_cost = edge_costs.at(from).at(to);
      const int removed_cost = max_cost_between_endpoints(paths_to_root.at(from), paths_to_root.at(to));
      DEBUG(3, "added_cost " << added_cost << " removed_cost " << removed_cost);
      assert(added_cost >= removed_cost);

      const int this_total_cost = leia_cost - removed_cost + added_cost;
      if (this_total_cost < min_total_cost)
      {
        min_total_cost = this_total_cost;
        DEBUG(3, "lowered min_total_cost " << min_total_cost);
      }
    }
  }

  std::cout << min_total_cost << "\n";
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