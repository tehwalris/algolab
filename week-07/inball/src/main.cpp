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
  int n;
  std::cin >> n;
  if (n == 0)
  {
    return false;
  }
  int d;
  std::cin >> d;
  assert(n >= 1 && n <= 1e3 && d >= 1 && d <= 10);

  Program lp(CGAL::SMALLER, false, 0, false, 0);
  const int R = d;
  for (int i = 0; i < n; i++)
  {
    int norm = 0;
    std::vector<int> as(d);
    for (int j = 0; j < d; j++)
    {
      int a;
      std::cin >> a;
      assert(abs(a) <= (1 << 10));
      as.at(j) = a;
      norm += a * a;
      assert(norm >= 0);
    }
    norm = int(sqrt(double(norm)));

    int b;
    std::cin >> b;
    assert(abs(b) <= (1 << 10));

    for (int j = 0; j < d; j++)
    {
      lp.set_a(j, i, -as.at(j));
    }
    lp.set_a(R, i, norm);
    lp.set_b(i, b);
  }

  lp.set_l(R, true, 0);
  lp.set_c(R, -1);

  Solution s = CGAL::solve_linear_program(lp, ET());
  if (s.is_infeasible())
  {
    std::cout << "none\n";
  }
  else if (s.is_unbounded())
  {
    std::cout << "inf\n";
  }
  else
  {
    assert(s.is_optimal());
    std::cout << long(floor(-s.objective_value_numerator().to_double() / s.objective_value_denominator().to_double())) << "\n";
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