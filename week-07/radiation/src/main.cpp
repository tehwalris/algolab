#include <iostream>
#include <cassert>
#include <algorithm>
#include <CGAL/QP_models.h>
#include <CGAL/QP_functions.h>
#include <CGAL/Gmpz.h>

const int debug_level = 0;

#define DEBUG(min_level, x)      \
  if (debug_level >= min_level)  \
  {                              \
    std::cerr << x << std::endl; \
  }

typedef CGAL::Gmpz IT;
typedef CGAL::Gmpz ET;
typedef CGAL::Quadratic_program<IT> Program;
typedef CGAL::Quadratic_program_solution<ET> Solution;

const int max_dim = 30;
const int space_dim = 3;
const int not_found = -1;

typedef std::vector<std::vector<std::vector<IT>>> PowCache;

struct Point
{
  std::array<int, space_dim> coords;
  bool healthy;
};

void add_dim_equations(Program &lp, int target_dim, int &next_free_var, std::vector<Point> &points, PowCache &pow_cache)
{
  std::vector<int> pows(space_dim, 0);
  bool done = false;
  while (!done)
  {
    for (int i = 0; i < space_dim; i++)
    {
      int &v = pows.at(i);
      if (v == target_dim)
      {
        v = 0;
        if (i + 1 == space_dim)
        {
          done = true;
        }
      }
      else
      {
        assert(v < target_dim);
        v++;
        break;
      }
    }

    int pow_sum = 0;
    for (int i = 0; i < space_dim; i++)
    {
      pow_sum += pows.at(i);
    }
    if (pow_sum != target_dim)
    {
      continue;
    }

    for (int i = 0; i < int(points.size()); i++)
    {
      IT coef(1);
      for (int j = 0; j < space_dim; j++)
      {
        coef *= pow_cache.at(i).at(j).at(pows.at(j));
      }
      lp.set_a(next_free_var, i, coef);
    }

    next_free_var++;
  }
}

bool solvable_with_dim(int target_dim, std::vector<Point> &points, PowCache &pow_cache)
{
  Program lp(CGAL::EQUAL, false, 0, false, 0);
  auto lp_options = CGAL::Quadratic_program_options();
  lp_options.set_pricing_strategy(CGAL::Quadratic_program_pricing_strategy::QP_BLAND);

  int next_free_var = 0;
  for (int i = 0; i <= target_dim; i++)
  {
    add_dim_equations(lp, i, next_free_var, points, pow_cache);
  }

  for (int i = 0; i < int(points.size()); i++)
  {
    Point &p = points.at(i);
    lp.set_b(i, p.healthy ? -1 : 1);
    lp.set_r(i, p.healthy ? CGAL::SMALLER : CGAL::LARGER);
  }

  Solution solution = CGAL::solve_linear_program(lp, ET(), lp_options);
  return !solution.is_infeasible();
}

int binary_search(int min_possible, int max_possible, std::function<bool(int)> is_target_or_higher)
{
  int low = min_possible, high = max_possible + 1;
  int lowest_found_at = not_found;
  while (low < high)
  {
    int i = (low + high) / 2;
    if (is_target_or_higher(i))
    {
      high = i;
      assert(lowest_found_at == not_found || lowest_found_at > i);
      lowest_found_at = i;
    }
    else
    {
      low = i + 1;
    }
  }
  return lowest_found_at;
}

// based on https://github.com/timethy/eth-16hs-algolab/blob/master/week12/radiation/radiation_robin.cpp
int binary_search_fast(int min_possible, int max_possible, std::function<bool(int)> is_target_or_higher)
{
  int low = 1, high = 2;
  while (high <= max_possible && !is_target_or_higher(high))
  {
    low = high + 1;
    high *= 2;
  }
  high = std::min(high, max_possible);
  low = std::min(low, high);

  DEBUG(2, "low " << low << " high " << high);
  return binary_search(low, high, is_target_or_higher);
}

void testcase()
{
  int h, t;
  std::cin >> h >> t;
  const int n = h + t;
  assert(n >= 1 && n <= 50);

  std::vector<Point> points(n);
  for (int i = 0; i < n; i++)
  {
    Point &p = points.at(i);
    p.healthy = i < h;
    for (int j = 0; j < space_dim; j++)
    {
      int v;
      std::cin >> v;
      assert(abs(v) <= (1 << 10));
      p.coords.at(j) = v;
    }
  }

  if (h == n || t == n)
  {
    std::cout << "0\n";
    return;
  }

  PowCache pow_cache(n, std::vector<std::vector<IT>>(space_dim, std::vector<IT>(max_dim + 1)));
  for (int i = 0; i < n; i++)
  {
    for (int j = 0; j < space_dim; j++)
    {
      int coord = points.at(i).coords.at(j);
      IT power(1);
      pow_cache.at(i).at(j).at(0) = power;
      for (int k = 1; k <= max_dim; k++)
      {
        power *= coord;
        pow_cache.at(i).at(j).at(k) = power;
      }
    }
  }

  int required_dims = binary_search_fast(1, max_dim, [&points, &pow_cache](int i) {
    DEBUG(2, "evaluated at " << i);
    return solvable_with_dim(i, points, pow_cache);
  });
  if (required_dims >= 0)
  {
    std::cout << required_dims << "\n";
  }
  else
  {
    assert(required_dims == not_found);
    std::cout << "Impossible!\n";
  }
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