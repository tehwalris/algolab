#include <iostream>
#include <cassert>
#include <vector>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/depth_first_search.hpp>

const int debug_level = 0;

#define DEBUG(min_level, x)      \
  if (debug_level >= min_level)  \
  {                              \
    std::cerr << x << std::endl; \
  }

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS> Graph;

const long undefined_diff = std::numeric_limits<int>::max();

class DFSVisitor : public boost::default_dfs_visitor
{
public:
  DFSVisitor(
      std::size_t n,
      std::size_t m,
      std::vector<long> &window_diff_by_start,
      std::function<long(int)> get_value) : n(n),
                                            m(m),
                                            window_diff_by_start(window_diff_by_start),
                                            get_value(get_value)
  {
    assert(m > 0);
  }

  void discover_vertex(int vertex, const Graph &G)
  {
    const long added_value = get_value(vertex);
    DEBUG(3, "discover_vertex " << vertex << " adding " << added_value);
    values_on_path.push_back(added_value);
    vertices_on_path.push_back(vertex);
    values_in_window.insert(added_value);
    if (values_on_path.size() > m)
    {
      const long removed_value = values_on_path.at(values_on_path.size() - m - 1);
      DEBUG(3, "discover_vertex " << vertex << " removing " << removed_value);
      values_in_window.erase(values_in_window.find(removed_value));
    }
    try_save_diff();
  }

  void finish_vertex(int vertex, const Graph &G)
  {
    assert(vertices_on_path.back() == vertex);
    vertices_on_path.pop_back();
    const long removed_value = values_on_path.back();
    DEBUG(3, "finish_vertex " << vertex << " removing " << removed_value);
    values_on_path.pop_back();
    values_in_window.erase(values_in_window.find(removed_value));
    if (values_on_path.size() >= m)
    {
      const long added_value = values_on_path.at(values_on_path.size() - m);
      DEBUG(3, "finish_vertex " << vertex << " adding " << added_value);
      values_in_window.insert(added_value);
    }
  }

private:
  std::size_t n, m;
  std::vector<long> &window_diff_by_start;
  std::function<long(int)> get_value;
  std::multiset<long> values_in_window;
  std::vector<long> values_on_path;
  std::vector<int> vertices_on_path;

  void try_save_diff()
  {
    if (values_in_window.size() < m)
    {
      return;
    }
    assert(values_in_window.size() == m);
    const int start_vertex = vertices_on_path.at(vertices_on_path.size() - m);
    const long diff_in_window = *values_in_window.rbegin() - *values_in_window.begin();
    DEBUG(3, "try_save_diff " << start_vertex << " " << diff_in_window);
    long &saved_diff = window_diff_by_start.at(start_vertex);
    saved_diff = std::min(saved_diff, diff_in_window);
  }
};

void testcase()
{
  int n, m, k;
  std::cin >> n >> m >> k;
  assert(n >= 1 && n <= 1e5 && m >= 2 && m <= 1e4 && k >= 0 && k <= 1e4);

  std::vector<long> temperature_by_node(n);
  for (long &t : temperature_by_node)
  {
    std::cin >> t;
    assert(t >= 0 && t < (1L << 31));
  }

  Graph G(n);
  for (int i = 0; i < n - 1; i++)
  {
    int u, v;
    std::cin >> u >> v;
    assert(u >= 0 && u < n && v >= 0 && v < n);
    assert(u != v); // not sure if this holds
    boost::add_edge(u, v, G);
  }

  std::vector<long> window_diff_by_start(n, undefined_diff);
  DFSVisitor visitor(n, m, window_diff_by_start, [&temperature_by_node](int i) { return temperature_by_node.at(i); });
  // WARNING visitors are copied
  boost::depth_first_search(G, boost::visitor(visitor));

  bool mission_is_possible = false;
  for (int i = 0; i < n; i++)
  {
    const long diff = window_diff_by_start.at(i);
    DEBUG(2, "i " << i << " diff " << diff);
    if (diff == undefined_diff)
    {
      continue;
    }
    assert(diff >= 0);
    if (diff <= k)
    {
      if (mission_is_possible)
      {
        std::cout << " ";
      }
      else
      {
        mission_is_possible = true;
      }
      std::cout << i;
    }
  }
  std::cout << (mission_is_possible ? "\n" : "Abort mission\n");
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