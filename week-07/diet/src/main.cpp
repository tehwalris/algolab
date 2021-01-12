#include <iostream>
#include <cassert>
#include <vector>
#include <CGAL/QP_models.h>
#include <CGAL/QP_functions.h>
#include <CGAL/Gmpz.h>

typedef int IT;
typedef CGAL::Gmpz ET;
typedef CGAL::Quadratic_program<IT> Program;
typedef CGAL::Quadratic_program_solution<ET> Solution;

const int debug_level = 0;

#define DEBUG(min_level, x)      \
  if (debug_level >= min_level)  \
  {                              \
    std::cerr << x << std::endl; \
  }

bool testcase()
{
  int n, m;
  std::cin >> n >> m;
  if (n == 0 && m == 0)
  {
    return false;
  }
  assert(n >= 1 && n <= 40 && m >= 1 && m <= 100);

  std::vector<int> min_allowed_by_nutrient(n), max_allowed_by_nutrient(n);
  for (int i = 0; i < n; i++)
  {
    int min, max;
    std::cin >> min >> max;
    assert(min >= 0 && min <= max);
    min_allowed_by_nutrient.at(i) = min;
    max_allowed_by_nutrient.at(i) = max;
  }

  std::vector<int> price_by_food(m);
  std::vector<std::vector<int>> nutrients_by_product(m, std::vector<int>(n));
  for (int i = 0; i < m; i++)
  {
    int p;
    std::cin >> p;
    assert(p >= 0 && p < (1 << 20));
    price_by_food.at(i) = p;

    for (int j = 0; j < n; j++)
    {
      int C;
      std::cin >> C;
      assert(C >= 0 && C < (1 << 20));
      nutrients_by_product.at(i).at(j) = C;
    }
  }

  Program lp(CGAL::LARGER, true, 0, false, 0);
  for (int i = 0; i < n; i++)
  {
    int eq_index = 2 * i;
    for (int j = 0; j < m; j++)
    {
      lp.set_a(j, eq_index, nutrients_by_product.at(j).at(i));
    }
    lp.set_b(eq_index, min_allowed_by_nutrient.at(i));

    eq_index++;
    for (int j = 0; j < m; j++)
    {
      lp.set_a(j, eq_index, -nutrients_by_product.at(j).at(i));
    }
    lp.set_b(eq_index, -max_allowed_by_nutrient.at(i));
  }

  for (int i = 0; i < m; i++)
  {
    lp.set_c(i, price_by_food.at(i));
  }

  Solution s = CGAL::solve_linear_program(lp, ET());
  if (s.is_infeasible())
  {
    std::cout << "No such diet.\n";
  }
  else
  {
    assert(s.is_optimal());
    std::cout << long(floor(s.objective_value_numerator().to_double() / s.objective_value_denominator().to_double())) << "\n";
  }

  return true;
}

int main()
{
  std::ios_base::sync_with_stdio(false);

  while (testcase())
  {
    // intentionally empty
  }

  return 0;
}