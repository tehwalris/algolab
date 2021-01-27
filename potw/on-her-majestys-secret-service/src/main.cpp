#include <iostream>
#include <cassert>
#include <iomanip>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/push_relabel_max_flow.hpp>

const int debug_level = 0;

#define DEBUG(min_level, x)      \
  if (debug_level >= min_level)  \
  {                              \
    std::cerr << x << std::endl; \
  }

typedef boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS> GraphTraits;
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, boost::no_property,
                              boost::property<boost::edge_capacity_t, int,
                                              boost::property<boost::edge_residual_capacity_t, int,
                                                              boost::property<boost::edge_reverse_t, GraphTraits::edge_descriptor,
                                                                              boost::property<boost::edge_weight_t, int>>>>>
    Graph;

const int unreachable_distance = std::numeric_limits<int>::max();

bool check_shelter_reachable_in_t(const std::vector<std::vector<int>> &distance_matrix, const int t)
{
  const int a = distance_matrix.size();
  const int s = distance_matrix.at(0).size(); // HACK This might be double the s in testcase()

  int next_free_node = 0;
  const int node_source = next_free_node++;
  const int node_sink = next_free_node++;
  const auto get_agent_node = [next_free_node, a](int i) {
    assert(i >= 0 && i < a);
    return next_free_node + i;
  };
  next_free_node += a;
  const auto get_shelter_node = [next_free_node, s](int i) {
    assert(i >= 0 && i < s);
    return next_free_node + i;
  };
  next_free_node += s;
  const int num_nodes = next_free_node;
  Graph G(num_nodes);

  const auto add_edge = [&G](int from, int to) {
    DEBUG(5, "add_edge(" << from << ", " << from << ")");
    auto c_map = boost::get(boost::edge_capacity, G);
    auto r_map = boost::get(boost::edge_reverse, G);
    const Graph::edge_descriptor e = boost::add_edge(from, to, G).first;
    const Graph::edge_descriptor rev_e = boost::add_edge(to, from, G).first;
    c_map[e] = 1;
    c_map[rev_e] = 0;
    r_map[e] = rev_e;
    r_map[rev_e] = e;
  };

  for (int i_a = 0; i_a < a; i_a++)
  {
    add_edge(node_source, get_agent_node(i_a));
  }
  for (int i_s = 0; i_s < s; i_s++)
  {
    add_edge(get_shelter_node(i_s), node_sink);
  }
  for (int i_a = 0; i_a < a; i_a++)
  {
    for (int i_s = 0; i_s < s; i_s++)
    {
      const int dist = distance_matrix.at(i_a).at(i_s);
      if (dist != unreachable_distance && dist <= t)
      {
        add_edge(get_agent_node(i_a), get_shelter_node(i_s));
      }
    }
  }

  const int flow = boost::push_relabel_max_flow(G, node_source, node_sink);
  DEBUG(3, "t " << t << " flow " << flow);
  assert(flow >= 0 && flow <= a && flow <= s);
  return flow == a;
}

int find_lower_bound(std::function<bool(int)> is_match)
{
  if (is_match(0))
  {
    return 0;
  }

  int base = 0;
  while (!is_match(1 << base))
  {
    base++;
  }

  int low = base == 0 ? 0 : 1 << (base - 1);
  int high = 1 << base;
  assert(!is_match(low) && is_match(high));
  while (low < high)
  {
    const int mid = (low + high) / 2;
    if (is_match(mid))
    {
      high = mid;
    }
    else
    {
      low = mid + 1;
    }
  }
  assert(!is_match(low - 1) && is_match(low));
  return low;
}

void testcase()
{
  int n, m, a, s, c, d;
  std::cin >> n >> m >> a >> s >> c >> d;
  assert(n >= 1 && n <= 1e3 && m >= 0 && m <= 5e3);
  assert(a >= 1 && a <= 1e2 && s >= 1 && s <= 1e2);
  assert(c >= 1 && c <= 2);
  assert(d >= 1 && d <= 1e3);

  Graph G(n);
  const auto add_edge = [&G](int from, int to, int weight) {
    auto w_map = boost::get(boost::edge_weight, G);
    const auto e_res = boost::add_edge(from, to, G);
    const Graph::edge_descriptor e = e_res.first;
    const bool is_new = e_res.second;
    w_map[e] = is_new ? weight : std::min(w_map[e], weight);
  };
  for (int i = 0; i < m; i++)
  {
    std::string w_string;
    int x, y, z;
    std::cin >> w_string >> x >> y >> z;
    assert(w_string == "S" || w_string == "L");
    assert(x >= 0 && x < n && y >= 0 && y < n && z >= 1 && z <= 1e4);
    const bool is_lift = w_string == "L";

    add_edge(x, y, z);
    if (is_lift)
    {
      add_edge(y, x, z);
    }
  }

  std::vector<int> nodes_by_agent(a);
  for (int &node : nodes_by_agent)
  {
    std::cin >> node;
    assert(node >= 0 && node < n);
  }

  std::vector<int> nodes_by_shelter(s);
  for (int &node : nodes_by_shelter)
  {
    std::cin >> node;
    assert(node >= 0 && node < n);
  }

  std::vector<std::vector<int>> distance_matrix(a, std::vector<int>(c * s, unreachable_distance));
  std::vector<int> temp_distances(n);
  auto temp_distance_map = boost::make_iterator_property_map(temp_distances.begin(), boost::get(boost::vertex_index, G));
  for (int i_a = 0; i_a < a; i_a++)
  {
    boost::dijkstra_shortest_paths(G, nodes_by_agent.at(i_a), boost::distance_map(temp_distance_map).distance_inf(unreachable_distance));
    for (int i_s = 0; i_s < s; i_s++)
    {
      const int dist = temp_distances.at(nodes_by_shelter.at(i_s));
      if (dist != unreachable_distance)
      {
        for (int i_c = 0; i_c < c; i_c++)
        {
          const int virtual_i_s = i_c * s + i_s;
          distance_matrix.at(i_a).at(virtual_i_s) = dist + i_c * d;
          DEBUG(4, "distance_matrix.at(" << i_a << ").at(" << virtual_i_s << ") = " << distance_matrix.at(i_a).at(virtual_i_s));
        }
      }
    }
  }

  int t = find_lower_bound([&distance_matrix](const int test_t) {
    return check_shelter_reachable_in_t(distance_matrix, test_t);
  });
  std::cout << t + d << "\n";
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