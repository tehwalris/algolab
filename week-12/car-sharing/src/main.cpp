#include <iostream>
#include <cassert>
#include <vector>
#include <iomanip>
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

struct Booking
{
  int s, t, d, a, p;
};

const int max_possible_profit = 100;

void testcase()
{
  int n, s;
  std::cin >> n >> s;
  assert(n >= 1 && n <= 10000 && s >= 2 && s <= 10);

  int total_cars = 0;
  std::vector<int> initial_cars_by_station(s);
  for (int &l : initial_cars_by_station)
  {
    std::cin >> l;
    assert(l >= 0 && l <= 100);
    total_cars += l;
  }

  std::vector<Booking> bookings(n);
  for (Booking &b : bookings)
  {
    std::cin >> b.s >> b.t >> b.d >> b.a >> b.p;
    assert(b.s >= 1 && b.s <= s && b.t >= 1 && b.t <= s);
    b.s--;
    b.t--;
    assert(b.d >= 0 && b.d < b.a && b.a <= 100000);
    assert(b.p >= 1 && b.p <= max_possible_profit);
  }

  const auto time_to_index = [](int time) {
    // TODO handle less restricted times
    return std::min(time, 10000) / 30;
  };
  const int num_time_slots = time_to_index(10000) + 1;

  int next_free_node = 0;
  const int node_source = next_free_node++;
  const int node_target = next_free_node++;
  const auto get_station_time_node = [next_free_node, s, num_time_slots, &time_to_index](const int i_time, const int i_station) {
    assert(i_station >= 0 && i_station < s && i_time >= 0 && i_time < num_time_slots);
    return next_free_node + num_time_slots * i_station + i_time;
  };
  next_free_node += s * num_time_slots;
  const auto get_trip_node = [next_free_node, n](const int i_trip) {
    assert(i_trip >= 0 && i_trip < n);
    return next_free_node + i_trip;
  };
  next_free_node += n;
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
  };

  for (int i_station = 0; i_station < s; i_station++)
  {
    add_edge(node_source, get_station_time_node(0, i_station), initial_cars_by_station.at(i_station), 0);
    add_edge(get_station_time_node(num_time_slots - 1, i_station), node_target, total_cars, 0);

    for (int i_time = 1; i_time < num_time_slots; i_time++)
    {
      add_edge(get_station_time_node(i_time - 1, i_station), get_station_time_node(i_time, i_station), total_cars, max_possible_profit);
    }
  }

  for (int i_trip = 0; i_trip < n; i_trip++)
  {
    Booking &b = bookings.at(i_trip);
    const int i_d = time_to_index(b.d), i_a = time_to_index(b.a);
    assert(i_d < i_a);
    add_edge(get_station_time_node(i_d, b.s), get_trip_node(i_trip), 1, max_possible_profit * (i_a - i_d) - b.p);
    add_edge(get_trip_node(i_trip), get_station_time_node(i_a, b.t), 1, 0);
  }

  boost::successive_shortest_path_nonnegative_weights(G, node_source, node_target);
  const int flow_cost = boost::find_flow_cost(G);
  const int total_profit = total_cars * (num_time_slots - 1) * max_possible_profit - flow_cost;
  std::cout << total_profit << "\n";
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