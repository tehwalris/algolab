#include <iostream>
#include <cassert>
#include <CGAL/QP_models.h>
#include <CGAL/QP_functions.h>
#include <CGAL/Gmpz.h>

typedef int IT;
typedef CGAL::Gmpz ET;
typedef CGAL::Quadratic_program<IT> Program;
typedef CGAL::Quadratic_program_solution<ET> Solution;

struct Legion
{
  int a, b, c, v;
};

int floor_to_int(const CGAL::Quotient<ET> &x)
{
  int a = std::floor(CGAL::to_double(x.numerator() / x.denominator()));
  while (a > x)
    a -= 1;
  while (a + 1 <= x)
    a += 1;
  return a;
}

const int debug_level = 0;

#define DEBUG(min_level, x)      \
  if (debug_level >= min_level)  \
  {                              \
    std::cerr << x << std::endl; \
  }

void testcase()
{
  int x_s, y_s, n;
  std::cin >> x_s >> y_s >> n;
  assert(abs(x_s) <= (1 << 24) && abs(y_s) <= (1 << 24) && n >= 1 && n <= 2e3);

  std::vector<Legion> legions(n);
  for (int i = 0; i < n; i++)
  {
    Legion &l = legions.at(i);
    std::cin >> l.a >> l.b >> l.c >> l.v;
    assert(abs(l.a) <= (1 << 24) && abs(l.b) <= (1 << 24) && abs(l.c) <= (1 << 24));
    assert(l.v >= 1 && l.v <= (1 << 8));
  }

  std::random_shuffle(legions.begin(), legions.end());

  Program lp(CGAL::EQUAL, false, 0, false, 0);
  const int var_p_x = 0, var_p_y = 1, var_t = 2;
  lp.set_c(var_t, -1);

  for (int i = 0; i < n; i++)
  {
    Legion &l = legions.at(i);
    const bool invert = long(x_s) * long(l.a) + long(y_s) * long(l.b) + long(l.c) < 0;
    DEBUG(2, "invert " << invert);

    const int coef_p_x = l.a;
    const int coef_p_y = l.b;
    const int coef_t = (invert ? 1 : -1) * l.v * sqrt(pow(l.a, 2) + pow(l.b, 2));
    const int rhs = -l.c;
    DEBUG(3, ""
                 << "coef_p_x " << coef_p_x << " "
                 << "coef_p_y " << coef_p_y << " "
                 << "coef_t " << coef_t << " "
                 << "rhs " << rhs);

    lp.set_a(var_p_x, i, coef_p_x);
    lp.set_a(var_p_y, i, coef_p_y);
    lp.set_a(var_t, i, coef_t);
    lp.set_b(i, rhs);
    lp.set_r(i, invert ? CGAL::SMALLER : CGAL::LARGER);
  }

  auto lp_opts = CGAL::Quadratic_program_options();
  lp_opts.set_pricing_strategy(CGAL::Quadratic_program_pricing_strategy::QP_BLAND);
  Solution s = CGAL::solve_quadratic_program(lp, ET(), lp_opts);
  DEBUG(2, s);
  assert(s.solves_quadratic_program(lp) && s.is_optimal());
  const int t = floor_to_int(-s.objective_value());
  assert(t >= 0);
  std::cout << t << "\n";
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