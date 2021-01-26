#include <iostream>
#include <cassert>
#include <vector>
#include <tuple>
#include <algorithm>
#include <queue>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Triangulation_vertex_base_with_info_2.h>
#include <CGAL/Triangulation_face_base_2.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Triangulation_vertex_base_with_info_2<int, K> Vb;
typedef CGAL::Triangulation_face_base_2<K> Fb;
typedef CGAL::Triangulation_data_structure_2<Vb, Fb> Tds;
typedef CGAL::Delaunay_triangulation_2<K, Tds> Triangulation;

const int debug_level = 0;

#define DEBUG(min_level, x)      \
  if (debug_level >= min_level)  \
  {                              \
    std::cerr << x << std::endl; \
  }

typedef std::vector<std::pair<K::Point_2, int>> Planets;

const int unknown_component = -1;

std::vector<int> find_connected_components(const int n, const double r_sq, const Planets::const_iterator planets_begin, const Planets::const_iterator planets_end)
{
  Triangulation triangulation;
  triangulation.insert(planets_begin, planets_end);
  DEBUG(3, "using " << planets_end - planets_begin << " planets");

  // BFS on short enough edges
  std::vector<int> components(n, unknown_component);
  for (auto root_it = triangulation.finite_vertices_begin(); root_it != triangulation.finite_vertices_end(); root_it++)
  {
    if (components.at(root_it->info()) != unknown_component)
    {
      continue;
    }
    const int component = root_it->info();
    DEBUG(3, "component " << component);
    components.at(root_it->info()) = component;

    std::queue<Triangulation::Vertex_handle> queue;
    queue.push(root_it);
    while (!queue.empty())
    {
      const Triangulation::Vertex_handle current_vertex = queue.front();
      queue.pop();

      const Triangulation::Edge_circulator first_edge = current_vertex->incident_edges();
      Triangulation::Edge_circulator edge = first_edge;
      do
      {
        const Triangulation::Vertex_handle cw_vertex = edge->first->vertex(triangulation.cw(edge->second));
        const Triangulation::Vertex_handle ccw_vertex = edge->first->vertex(triangulation.ccw(edge->second));
        const Triangulation::Vertex_handle next_vertex = cw_vertex == current_vertex ? ccw_vertex : cw_vertex;
        assert(next_vertex != current_vertex);

        if (!triangulation.is_infinite(next_vertex) && components.at(next_vertex->info()) == unknown_component && triangulation.segment(edge).squared_length() <= r_sq)
        {
          DEBUG(3, "adding vertex " << next_vertex->info());
          components.at(next_vertex->info()) = component;
          queue.push(next_vertex);
        }

        edge++;
      } while (edge != first_edge);
    }
  }

  return components;
}

int count_largest_component(const std::vector<int> &components)
{
  const int n = components.size();
  std::vector<int> size_by_component(n, 0);
  for (const int c : components)
  {
    if (c == unknown_component)
    {
      continue;
    }
    assert(c >= 0 && c < n);
    size_by_component.at(c)++;
  }
  return *std::max_element(size_by_component.begin(), size_by_component.end());
}

int find_upper_bound(std::function<bool(int)> is_match, int initial_value)
{
  assert(is_match(initial_value));
  int base = 0;
  while (is_match(initial_value + (1 << base)))
  {
    base++;
  }

  int low = base == 0 ? initial_value : (initial_value + (1 << (base - 1)));
  int high = initial_value + (1 << base);
  assert(is_match(low) && !is_match(high));
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

  assert(low > initial_value);
  assert(is_match(low - 1) && !is_match(low));
  return low - 1;
}

void testcase()
{
  int n, r;
  std::cin >> n >> r;
  assert(n >= 1 && n <= 3e4 && r >= 1 && r <= (1 << 24));
  const double r_sq = pow(double(r), 2);

  Planets planets(n);
  for (int i = 0; i < n; i++)
  {
    int x, y;
    std::cin >> x >> y;
    assert(abs(x) < (1 << 24) && abs(y) < (1 << 24));
    planets.at(i).first = K::Point_2(x, y);
    planets.at(i).second = i;
  }

  const auto is_possible = [n, r_sq, &planets](int k) {
    DEBUG(2, "is_possible(" << k << ")");
    assert(k >= 1);
    const int available_planets = n - k;
    if (available_planets < k)
    {
      DEBUG(2, "fast exit");
      return false;
    }
    const int largest_size = count_largest_component(find_connected_components(n, r_sq, planets.begin() + k, planets.end()));
    DEBUG(2, "largest_size " << largest_size);
    return largest_size >= k;
  };
  const int best_k = find_upper_bound(is_possible, 1);
  std::cout << best_k << "\n";
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