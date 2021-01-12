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

const std::vector<std::pair<int, int>> attack_offsets{
    {-1, -2},
    {-1, 2},
    {1, -2},
    {1, 2},
    {-2, -1},
    {-2, 1},
    {2, -1},
    {2, 1},
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
  int n;
  std::cin >> n;
  assert(n >= 1 && n <= (1 << 6));

  const auto square_is_in_range = [n](int i, int j) { return i >= 0 && i < n && j >= 0 && j < n; };
  const auto square_is_white = [](int i, int j) { return (i + j) % 2 == 0; };

  std::vector<std::vector<bool>> is_present_by_field(n, std::vector<bool>(n));
  for (int i = 0; i < n; i++)
  {
    for (int j = 0; j < n; j++)
    {
      int raw;
      std::cin >> raw;
      assert(raw == 0 || raw == 1);
      is_present_by_field.at(i).at(j) = bool(raw);
    }
  }

  int next_free_node = 0;
  const int node_source = next_free_node++;
  const int node_sink = next_free_node++;
  const auto get_node_for_square = [next_free_node, n, square_is_in_range](int i, int j) {
    assert(square_is_in_range(i, j));
    return next_free_node + i * n + j;
  };
  next_free_node += n * n;
  const int num_nodes = next_free_node;

  Graph G(num_nodes);
  EdgeAdder adder(G);
  auto rc_map = boost::get(boost::edge_residual_capacity, G);

  for (int i = 0; i < n; i++)
  {
    for (int j = 0; j < n; j++)
    {
      if (!is_present_by_field.at(i).at(j))
      {
        continue;
      }

      if (square_is_white(i, j))
      {
        adder.add_edge(node_source, get_node_for_square(i, j), 1);
      }
      else
      {
        adder.add_edge(get_node_for_square(i, j), node_sink, 1);
        continue;
      }

      for (const auto offset : attack_offsets)
      {
        const int attack_i = i + offset.first;
        const int attack_j = j + offset.second;
        if (!square_is_in_range(attack_i, attack_j) || !is_present_by_field.at(attack_i).at(attack_j))
        {
          continue;
        }
        assert(!square_is_white(attack_i, attack_j));
        adder.add_edge(get_node_for_square(i, j), get_node_for_square(attack_i, attack_j), 1);
      }
    }
  }

  boost::push_relabel_max_flow(G, node_source, node_sink);

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

  int max_knights = 0;
  for (int i = 0; i < n; i++)
  {
    for (int j = 0; j < n; j++)
    {
      if (is_present_by_field.at(i).at(j) && square_is_white(i, j) == visited_by_node.at(get_node_for_square(i, j)))
      {
        max_knights++;
      }
    }
  }

  std::cout << max_knights << "\n";
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