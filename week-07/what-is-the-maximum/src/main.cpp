#include <iostream>
#include <cassert>
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

void testcase(int p)
{
  assert(p == 1 || p == 2);

  int a, b;
  std::cin >> a >> b;
  assert(a >= 0 && a <= 100 && b >= -100 && b <= 100);

  Program lp = p == 1 ? Program(CGAL::SMALLER, true, 0, false, 0) : Program(CGAL::LARGER, false, 0, true, 0);

  const IT X = 0, Y = 1, Z = 2;
  const bool use_Z = p == 2;
  int next_eq_index = 0;
  auto add_eq = [&next_eq_index, &lp, X, Y, Z, use_Z](IT X_coef, IT Y_coef, IT Z_coef, IT rhs) {
    lp.set_a(X, next_eq_index, X_coef);
    lp.set_a(Y, next_eq_index, Y_coef);
    if (use_Z)
    {
      lp.set_a(Z, next_eq_index, Z_coef);
    }
    else
    {
      assert(Z_coef == 0);
    }
    lp.set_b(next_eq_index, rhs);

    next_eq_index++;
  };

  if (p == 1)
  {
    add_eq(1, 1, 0, 4);
    add_eq(4, 2, 0, a * b);
    add_eq(-1, 1, 0, 1);
    lp.set_c(X, a);
    lp.set_c(Y, -b);
  }
  else
  {
    add_eq(1, 1, 0, -4);
    add_eq(4, 2, 1, -(a * b));
    add_eq(-1, 1, 0, -1);
    lp.set_c(X, a);
    lp.set_c(Y, b);
    lp.set_c(Z, 1);
  }

  Solution s = CGAL::solve_linear_program(lp, ET());
  if (s.is_infeasible())
  {
    std::cout << "no\n";
  }
  else if (s.is_unbounded())
  {
    std::cout << "unbounded\n";
  }
  else
  {
    assert(s.is_optimal());
    int v = ceil(s.objective_value_numerator().to_double() / s.objective_value_denominator().to_double());
    if (p == 1)
    {
      v = -v;
    }
    std::cout << v << "\n";
  }
}

int main()
{
  std::ios_base::sync_with_stdio(false);

  while (true)
  {
    int p;
    std::cin >> p;
    if (p == 0)
    {
      break;
    }
    testcase(p);
  }

  return 0;
}