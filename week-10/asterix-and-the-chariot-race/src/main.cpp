#include <iostream>
#include <cassert>
#include <vector>
#include <iomanip>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/depth_first_search.hpp>

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS> Graph;

const int debug_level = 0;

#define DEBUG(min_level, x)      \
  if (debug_level >= min_level)  \
  {                              \
    std::cerr << x << std::endl; \
  }

class DPCell
{
public:
  int set, unset_uncovered, unset_covered;
};

class DPVisitor : public boost::default_dfs_visitor
{
public:
  DPVisitor(std::vector<DPCell> &dp_table, const std::vector<int> &costs_by_city) : dp_table(dp_table), costs_by_city(costs_by_city) {}

  void finish_vertex(int vertex, const Graph &G)
  {
    int cheapest_set_child = -1, cheapest_set_child_cost = std::numeric_limits<int>::max();
    for (auto it = boost::out_edges(vertex, G); it.first != it.second; it.first++)
    {
      const int i = it.first->m_target;
      const DPCell &child_cell = dp_table.at(i);
      const int cost = child_cell.set - std::min(child_cell.set, child_cell.unset_uncovered);
      if (cheapest_set_child == -1 || cheapest_set_child_cost > cost)
      {
        cheapest_set_child = i;
        cheapest_set_child_cost = cost;
      }
    }

    DPCell &parent_cell = dp_table.at(vertex);
    parent_cell.set = costs_by_city.at(vertex);
    parent_cell.unset_covered = 0;

    if (cheapest_set_child == -1)
    {
      parent_cell.unset_uncovered = std::numeric_limits<int>::max();
    }
    else
    {
      assert(cheapest_set_child >= 0);
      parent_cell.unset_uncovered = 0;
      for (auto it = boost::out_edges(vertex, G); it.first != it.second; it.first++)
      {
        const int i = it.first->m_target;
        const DPCell &child_cell = dp_table.at(i);
        parent_cell.set += std::min(child_cell.set, child_cell.unset_covered);
        parent_cell.unset_uncovered += i == cheapest_set_child ? child_cell.set : std::min(child_cell.set, child_cell.unset_uncovered);
        parent_cell.unset_covered += std::min(child_cell.set, child_cell.unset_uncovered);
      }
    }

    DEBUG(2, "finish_vertex(" << vertex << "): " << parent_cell.set << " " << parent_cell.unset_uncovered << " " << parent_cell.unset_covered);
  }

  int get_result()
  {
    const DPCell &cell = dp_table.at(0);
    DEBUG(2, "result " << cell.set << " " << cell.unset_uncovered << " " << cell.unset_covered)
    return std::min(cell.set, cell.unset_uncovered);
  }

private:
  std::vector<DPCell> &dp_table;
  const std::vector<int> &costs_by_city;
};

void testcase()
{
  int n;
  std::cin >> n;
  assert(n >= 1 && n <= 1e5);

  std::vector<std::pair<int, int>> stages(n - 1);
  for (std::pair<int, int> &s : stages)
  {
    std::cin >> s.first >> s.second;
    assert(s.first != s.second && s.first >= 0 && s.first < n && s.second >= 0 && s.second < n);
  }

  std::vector<int> costs_by_city(n);
  for (int &c : costs_by_city)
  {
    std::cin >> c;
    assert(c >= 0 && c <= (1 << 14));
  }

  Graph G(n);
  for (const std::pair<int, int> &s : stages)
  {
    boost::add_edge(s.first, s.second, G);
  }

  std::vector<DPCell> dp_table(n);
  DPVisitor visitor(dp_table, costs_by_city);
  // WARNING: Boost copies the visitor, we have to save the DP table externally, otherwise we will never get the filled-out copy.
  boost::depth_first_search(G, boost::visitor(visitor));
  std::cout << visitor.get_result() << "\n";
}

int main()
{
  std::ios_base::sync_with_stdio(false);
  std::cout << std::fixed << std::setprecision(0);

  int t;
  std::cin >> t;
  for (int i = 0; i < t; i++)
  {
    testcase();
    DEBUG(1, "");
  }

  return 0;
}