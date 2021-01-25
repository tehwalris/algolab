#include <iostream>
#include <cassert>
#include <vector>
#include <CGAL/QP_models.h>
#include <CGAL/QP_functions.h>
#include <CGAL/Gmpz.h>

const int debug_level = 0;

#define DEBUG(min_level, x)      \
  if (debug_level >= min_level)  \
  {                              \
    std::cerr << x << std::endl; \
  }

typedef long IT;
typedef CGAL::Gmpz ET;
typedef CGAL::Quadratic_program<IT> Program;
typedef CGAL::Quadratic_program_solution<ET> Solution;

const long infinite_s = -1;

void testcase()
{
  int n, m;
  long s;
  std::cin >> n >> m >> s;
  assert(n >= 0 && n <= 500 && m >= 0 && m <= 500 && s >= -1 && s < (1L << 50));

  std::vector<std::pair<int, int>> houses(n + m);
  for (std::pair<int, int> &h : houses)
  {
    std::cin >> h.first >> h.second;
    assert(abs(h.first) < (1 << 24) && abs(h.second) < (1 << 24));
  }

  int next_equation = 0;
  const int sewage_sum_equation = next_equation++;
  const auto get_water_distance_equation = [next_equation, n, m](int house_i, bool lower_bound) {
    assert(house_i >= 0 && house_i < n + m);
    return next_equation + lower_bound * (n + m) + house_i;
  };
  next_equation += 2 * (n + m);
  const auto get_sewage_side_equation = [next_equation, n, m](int house_i) {
    assert(house_i >= 0 && house_i < n + m);
    return next_equation + house_i;
  };
  next_equation += n + m;

  const int var_b = 0, var_c1 = 1, var_c2 = 2, var_l = 3;

  Program lp(CGAL::SMALLER, false, 0, false, 0);

  int eq;
  long signed_sum_x = 0, signed_sum_y = 0, signed_sum_c1 = 0;
  for (int i = 0; i < n + m; i++)
  {
    const bool is_noble = i < n;
    const int x = houses.at(i).first, y = houses.at(i).second;

    const int sign = is_noble ? -1 : 1;
    signed_sum_x += sign * x;
    signed_sum_y += sign * y;
    signed_sum_c1 += sign;

    // Noble houses on left of sewage, common houses on right
    eq = get_sewage_side_equation(i);
    lp.set_a(var_b, eq, y);
    lp.set_a(var_c1, eq, 1);
    lp.set_b(eq, -x);
    lp.set_r(eq, is_noble ? CGAL::SMALLER : CGAL::LARGER);

    // Limit length of longest water pipe (to minimize it below)
    for (const bool lower_bound : {true, false})
    {
      eq = get_water_distance_equation(i, lower_bound);
      lp.set_a(var_b, eq, -x);
      lp.set_a(var_c2, eq, 1);
      lp.set_a(var_l, eq, lower_bound ? 1 : -1);
      lp.set_b(eq, -y);
      lp.set_r(eq, lower_bound ? CGAL::LARGER : CGAL::SMALLER);
    }
  }

  // Minimize length of longest water pipe
  lp.set_c(var_l, 1);

  Solution solution = CGAL::solve_linear_program(lp, ET());
  if (solution.is_infeasible())
  {
    std::cout << "Yuck!\n";
    return;
  }

  if (s != infinite_s)
  {
    // Total length of sewage pipes is not too long
    eq = sewage_sum_equation;
    lp.set_a(var_b, eq, signed_sum_y);
    lp.set_a(var_c1, eq, signed_sum_c1);
    lp.set_b(eq, s - signed_sum_x);
  }

  solution = CGAL::solve_linear_program(lp, ET());
  if (solution.is_infeasible())
  {
    std::cout << "Bankrupt!\n";
  }
  else
  {
    assert(solution.is_optimal());
    std::cout << std::ceil(CGAL::to_double(solution.objective_value())) << "\n";
  }
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