#include <iostream>
#include <cassert>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/successive_shortest_path_nonnegative_weights.hpp>
#include <boost/graph/find_flow_cost.hpp>

typedef boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS> GraphTraits;
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, boost::no_property,
                              boost::property<boost::edge_capacity_t, long,
                                              boost::property<boost::edge_residual_capacity_t, long,
                                                              boost::property<boost::edge_reverse_t, GraphTraits::edge_descriptor,
                                                                              boost::property<boost::edge_weight_t, long>>>>>
    Graph;

const int debug_level = 0;

#define DEBUG(min_level, x)      \
  if (debug_level >= min_level)  \
  {                              \
    std::cerr << x << std::endl; \
  }

struct Guide
{
  int x, y, d, e;
};

const int impossible = -1;

int find_upper_bound(std::function<bool(int)> is_match)
{
  assert(is_match(0));
  int exponent = 0;
  while (is_match(1 << exponent))
  {
    exponent++;
  }

  int low = exponent == 0 ? 0 : (1 << (exponent - 1));
  int high = 1 << exponent;
  while (low < high)
  {
    const int mid = (low + high) / 2;
    if (is_match(mid))
    {
      low = mid + 1;
    }
    else
    {
      high = mid;
    }
  }

  assert(is_match(low - 1) && !is_match(low));
  return low - 1;
}

class Testcase
{
public:
  void run()
  {
    std::cin >> c >> g >> b >> k >> a;
    assert(c >= 2 && c <= 1e3);
    assert(g >= 0 && g <= 5e3);
    assert(b >= 0 && b <= 1e9);
    assert(k >= 0 && k < c && a >= 0 && a < c && k != a);

    int total_elephants = 0;
    guides.resize(g);
    for (int i = 0; i < g; i++)
    {
      Guide &gd = guides.at(i);
      std::cin >> gd.x >> gd.y >> gd.d >> gd.e;
      assert(gd.x >= 0 && gd.x < c && gd.y >= 0 && gd.y < c && gd.d >= 1 && gd.d <= 1e3 && gd.e >= 1 && gd.e <= 1e3);
      total_elephants += gd.e;
    }

    // fast path for test set 2
    {
      const std::pair<int, int> result = calculate_cost(total_elephants);
      const int suitcases = result.first, cost = result.second;
      if (cost <= b)
      {
        std::cout << suitcases << "\n";
        return;
      }
    }

    const int max_suitcases = find_upper_bound([this](int target_suitcases) {
      assert(target_suitcases >= 0);
      const std::pair<int, int> result = calculate_cost(target_suitcases);
      const int suitcases = result.first, cost = result.second;
      return suitcases == target_suitcases && cost <= b;
    });
    std::cout << max_suitcases << "\n";
  }

private:
  int c, g, b, k, a;
  std::vector<Guide> guides;

  std::pair<int, int> calculate_cost(const int max_suitcases)
  {
    assert(max_suitcases >= 0);

    int next_free_node = 0;
    const int node_source = next_free_node++;
    const auto get_node_for_city = [next_free_node, this](int i) {
      assert(i >= 0 && i < c);
      return next_free_node + i;
    };
    next_free_node += c;
    const auto get_node_for_guide = [next_free_node, this](int i) {
      assert(i >= 0 && i < g);
      return next_free_node + i;
    };
    next_free_node += g;
    const int num_nodes = next_free_node;
    Graph G(num_nodes);

    const auto add_edge = [&G](int from, int to, long capacity, long cost) {
      auto c_map = boost::get(boost::edge_capacity, G);
      auto r_map = boost::get(boost::edge_reverse, G);
      auto w_map = boost::get(boost::edge_weight, G);
      const Graph::edge_descriptor e = boost::add_edge(from, to, G).first;
      const Graph::edge_descriptor rev_e = boost::add_edge(to, from, G).first;
      c_map[e] = capacity;
      c_map[rev_e] = 0;
      r_map[e] = rev_e;
      r_map[rev_e] = e;
      w_map[e] = cost;
      w_map[rev_e] = -cost;
      return e;
    };

    const Graph::edge_descriptor source_edge = add_edge(node_source, get_node_for_city(k), max_suitcases, 0);

    for (int i = 0; i < g; i++)
    {
      Guide &gd = guides.at(i);
      add_edge(get_node_for_city(gd.x), get_node_for_guide(i), gd.e, gd.d);
      add_edge(get_node_for_guide(i), get_node_for_city(gd.y), gd.e, 0);
    }

    boost::successive_shortest_path_nonnegative_weights(G, node_source, get_node_for_city(a));

    const int flow = boost::get(boost::edge_capacity, G)[source_edge] - boost::get(boost::edge_residual_capacity, G)[source_edge];
    assert(flow >= 0 && flow <= max_suitcases);
    const int cost = boost::find_flow_cost(G);
    assert(cost >= 0);
    return std::make_pair(flow, cost);
  }
};

int main()
{
  std::ios_base::sync_with_stdio(false);

  int t;
  std::cin >> t;
  for (int i = 0; i < t; i++)
  {
    Testcase().run();
    DEBUG(1, "");
  }

  return 0;
}