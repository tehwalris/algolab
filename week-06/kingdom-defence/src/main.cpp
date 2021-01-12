#include <iostream>
#include <cassert>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/push_relabel_max_flow.hpp>

const int debug_level = 0;

#define DEBUG(min_level, x)      \
  if (debug_level >= min_level)  \
  {                              \
    std::cerr << x << std::endl; \
  }

typedef boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS> GraphTraits;
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, boost::no_property,
                              boost::property<boost::edge_capacity_t, long,
                                              boost::property<boost::edge_residual_capacity_t, long,
                                                              boost::property<boost::edge_reverse_t, GraphTraits::edge_descriptor>>>>
    Graph;
typedef Graph::vertex_descriptor Vertex;

template <typename T>
void print_vec(std::vector<T> &vec, std::ostream &stream)
{
  for (int i = 0; i < int(vec.size()); i++)
  {
    if (i > 0)
    {
      stream << " ";
    }
    stream << vec.at(i);
  }
  stream << "\n";
}

void testcase()
{
  int l, p;
  std::cin >> l >> p;
  assert(l >= 1 && l <= 500 && p >= 1 && p <= l * l);

  std::vector<int> start_pops_by_town(l), end_pops_by_town(l);
  int total_start_pops = 0, total_end_pops = 0;
  for (int i = 0; i < l; i++)
  {
    int g, d;
    std::cin >> g >> d;
    assert(g >= 1 && g <= 1e6 && d >= 1 && d <= 1e6);
    start_pops_by_town.at(i) = g;
    total_start_pops += g;
    end_pops_by_town.at(i) = d;
    total_end_pops += d;
  }
  DEBUG(2, "total_start_pops " << total_start_pops << " total_end_pops " << total_end_pops);

  std::vector<int> starts_by_path(p), ends_by_path(p), min_caps_by_path(p), max_caps_by_path(p);
  for (int i = 0; i < p; i++)
  {
    int f, t, c, C;
    std::cin >> f >> t >> c >> C;
    assert(f >= 0 && f < l && t >= 0 && t < l);
    assert(c >= 0 && c <= C && C <= 1e6);
    starts_by_path.at(i) = f;
    ends_by_path.at(i) = t;
    min_caps_by_path.at(i) = c;
    max_caps_by_path.at(i) = C;
  }

  if (total_end_pops > total_start_pops)
  {
    std::cout << "no\n";
    return;
  }

  int num_nodes = l + p + 2, next_free_node = 0;
  Vertex source = next_free_node++;
  Vertex sink = next_free_node++;
  auto town_at = [l, next_free_node](int i) { assert(i >= 0 && i < l);  return next_free_node + i; };
  next_free_node += l;
  auto path_mid_node_at = [p, next_free_node](int i) { assert(i >= 0 && i < p);  return next_free_node + i; };
  next_free_node += p;
  assert(next_free_node == num_nodes);
  Graph G(num_nodes);

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

  std::vector<int> demands_by_town(l);
  for (int i = 0; i < l; i++)
  {
    demands_by_town.at(i) = end_pops_by_town.at(i) - start_pops_by_town.at(i);
  }
  if (debug_level >= 2)
  {
    std::cerr << "demands_by_town before adjustment ";
    print_vec(demands_by_town, std::cerr);
  }

  for (int i = 0; i < p; i++)
  {
    int s = starts_by_path.at(i), e = ends_by_path.at(i), mid = path_mid_node_at(i), min = min_caps_by_path.at(i), max = max_caps_by_path.at(i);
    demands_by_town.at(s) += min;
    demands_by_town.at(e) -= min;
    add_edge(town_at(s), mid, max - min);
    add_edge(mid, town_at(e), max - min);
  }
  if (debug_level >= 2)
  {
    std::cerr << "demands_by_town after adjustment ";
    print_vec(demands_by_town, std::cerr);
  }

  for (int i = 0; i < l; i++)
  {
    int d = demands_by_town.at(i);
    if (d > 0)
    {
      add_edge(town_at(i), sink, d);
    }
    else if (d < 0)
    {
      add_edge(source, town_at(i), -d);
    }
  }

  int target_flow = 0;
  for (int d : demands_by_town)
  {
    if (d > 0)
    {
      target_flow += d;
    }
  }
  DEBUG(2, "target_flow " << target_flow);

  long flow = boost::push_relabel_max_flow(G, source, sink);
  DEBUG(2, "flow " << flow);
  assert(flow >= 0 && flow <= target_flow);
  std::cout << (flow == target_flow ? "yes\n" : "no\n");
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