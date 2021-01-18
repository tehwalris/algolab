#include <iostream>
#include <cassert>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Triangulation_vertex_base_with_info_2.h>
#include <CGAL/Triangulation_face_base_2.h>
#include <CGAL/Triangulation_data_structure_2.h>
#include <boost/pending/disjoint_sets.hpp>

const int debug_level = 0;

#define DEBUG(min_level, x)      \
  if (debug_level >= min_level)  \
  {                              \
    std::cerr << x << std::endl; \
  }

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Triangulation_vertex_base_with_info_2<int, K> Vb;
typedef CGAL::Triangulation_face_base_2<K> Fb;
typedef CGAL::Triangulation_data_structure_2<Vb, Fb> Tds;
typedef CGAL::Delaunay_triangulation_2<K, Tds> Triangulation;

// Note: the order that patterns evaluated in is important.
// I don't fully understand why this specific order should work though.
const std::vector<std::vector<std::vector<int>>> patterns_by_k = {
    {},
    {},
    {{0, 2}},
    {{0, 1, 1}, {0, 3}, {0, 0, 2}},
    {{0, 1, 0, 1}, {0, 2, 1}, {0, 4}, {0, 0, 2}, {0, 0, 1, 1}, {0, 0, 0, 2}},
};

std::vector<int> determine_component_sizes(const Triangulation &triangulation, double s)
{
  const int n = triangulation.number_of_vertices();

  boost::disjoint_sets_with_storage<> disjoint_sets(n);
  for (auto it = triangulation.finite_edges_begin(); it != triangulation.finite_edges_end(); it++)
  {
    if (triangulation.segment(it).squared_length() < s)
    {
      int i1 = it->first->vertex((it->second + 1) % 3)->info();
      int i2 = it->first->vertex((it->second + 2) % 3)->info();
      disjoint_sets.union_set(i1, i2);
    }
  }

  std::vector<int> component_sizes(n, 0);
  for (int i = 0; i < n; i++)
  {
    component_sizes.at(disjoint_sets.find_set(i))++;
  }

  return component_sizes;
}

int count_max_families(const std::vector<int> &component_sizes, int k)
{
  const int max_counted_size = 4;
  assert(k <= max_counted_size);
  assert(k >= 1 && k <= 4);

  std::vector<int> counts_by_size(max_counted_size + 1);
  for (const int size : component_sizes)
  {
    counts_by_size.at(std::min(size, max_counted_size))++;
  }

  for (int i = 0; i <= max_counted_size; i++)
  {
    DEBUG(3, "counts_by_size.at(" << i << ") = " << counts_by_size.at(i));
  }

  int f = 0;
  for (int i = k; i <= max_counted_size; i++)
  {
    f += counts_by_size.at(i);
    counts_by_size.at(i) = 0;
  }

  for (const std::vector<int> &pattern : patterns_by_k.at(k))
  {
    int matches = std::numeric_limits<int>::max();
    for (int i = 0; i < int(pattern.size()); i++)
    {
      int required = pattern.at(i);
      assert(required >= 0);
      if (required > 0)
      {
        matches = std::min(matches, counts_by_size.at(i) / required);
      }
    }

    if (matches > 0)
    {
      for (int i = 0; i < int(pattern.size()); i++)
      {
        const int used_count = pattern.at(i) * matches;
        assert(counts_by_size.at(i) >= used_count);
        counts_by_size.at(i) -= used_count;
      }
      f += matches;
    }
  }

  DEBUG(3, "f " << f);

  return f;
}

long find_upper_bound(std::function<bool(long)> is_match)
{
  assert(is_match(0));
  int base = 0;
  while (is_match(1L << base))
  {
    base++;
  }

  long low = base == 0 ? 0 : 1L << (base - 1);
  long high = 1L << base;
  while (low < high)
  {
    long mid = (low + high) / 2;
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

void testcase()
{
  int n, k, f_0;
  double s_0;
  std::cin >> n >> k >> f_0 >> s_0;
  assert(n >= 2 && n <= 9e4 && k >= 1 && k <= 4 && f_0 >= 2 && k * f_0 <= n && s_0 >= 0 && s_0 <= (1L << 50));

  std::vector<std::pair<K::Point_2, int>> tent_locations;
  for (int i = 0; i < n; i++)
  {
    double x, y;
    std::cin >> x >> y;
    assert(abs(x) < (1 << 24) && abs(y) < (1 << 24));
    tent_locations.push_back(std::make_pair(K::Point_2(x, y), i));
  }

  Triangulation triangulation;
  triangulation.insert(tent_locations.begin(), tent_locations.end());

  double s = find_upper_bound([&triangulation, f_0, k](long s_query) {
    return count_max_families(determine_component_sizes(triangulation, s_query), k) >= f_0;
  });

  int f = count_max_families(determine_component_sizes(triangulation, s_0), k);

  std::cout << s << " " << f << "\n";
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