#include <iostream>
#include <cassert>
#include <vector>
#include <bitset>
#include <CGAL/QP_models.h>
#include <CGAL/QP_functions.h>
#include <CGAL/Gmpz.h>

const int debug_level = 0;

#define DEBUG(min_level, x)      \
  if (debug_level >= min_level)  \
  {                              \
    std::cerr << x << std::endl; \
  }

typedef int IT;
typedef CGAL::Gmpz ET;

typedef CGAL::Quadratic_program<IT> Program;
typedef CGAL::Quadratic_program_solution<ET> Solution;

const int max_contours = 1e6;
typedef std::bitset<max_contours> ContourMembership;

class Point
{
public:
  double x, y;
};

class Warehouse : public Point
{
public:
  int s, a;
};

class Stadium : public Point
{
public:
  int d, u;
};

class Contour : public Point
{
public:
  double r_sq;
};

double floor_to_double(const CGAL::Quotient<ET> &x)
{
  double a = std::floor(CGAL::to_double(x));
  while (a > x)
    a -= 1;
  while (a + 1 <= x)
    a += 1;
  return a;
}

inline bool is_point_valid(const Point &p)
{
  return abs(p.x) < (1 << 24) && abs(p.y) < (1 << 24);
}

inline double calc_sq_dist(const Point &a, const Point &b)
{
  return std::pow(a.x - b.x, 2) + std::pow(a.y - b.y, 2);
}

inline bool contour_contains_point(const Contour &c, const Point &p)
{
  DEBUG(3, "calc_sq_dist(c, p) " << calc_sq_dist(c, p) << " c.r_sq " << c.r_sq);
  return calc_sq_dist(c, p) <= c.r_sq;
}

void testcase()
{
  int n, m, c;
  std::cin >> n >> m >> c;
  assert(n >= 1 && n <= 200 && m >= 1 && m <= 20 && c >= 0 && c <= max_contours);

  int max_supply_any_wareshouse = 0;
  std::vector<Warehouse> warehouses(n);
  for (Warehouse &w : warehouses)
  {
    std::cin >> w.x >> w.y >> w.s >> w.a;
    assert(is_point_valid(w) && w.s >= 0 && w.s <= 1e5 && w.a >= 0 && w.a <= 100);
    max_supply_any_wareshouse = std::max(max_supply_any_wareshouse, w.s);
  }

  std::vector<Stadium> stadiums(m);
  for (Stadium &s : stadiums)
  {
    std::cin >> s.x >> s.y >> s.d >> s.u;
    assert(is_point_valid(s) && s.d >= 0 && s.d <= 1e5 && s.u >= 0 && s.u <= 1e5);
  }

  std::vector<std::vector<int>> estimated_revenues(n, std::vector<int>(m));
  for (std::vector<int> &row : estimated_revenues)
  {
    for (int &v : row)
    {
      std::cin >> v;
      assert(v >= -10 && v <= 10);
    }
  }

  std::vector<Contour> contours(c);
  for (Contour &c : contours)
  {
    double r;
    std::cin >> c.x >> c.y >> r;
    assert(is_point_valid(c) && r >= 0 && r <= (1 << 24));
    c.r_sq = std::pow(r, 2);
  }

  std::vector<ContourMembership> contour_membership_by_warehouse(n);
  for (int iw = 0; iw < n; iw++)
  {
    const Warehouse &w = warehouses.at(iw);
    ContourMembership &membership = contour_membership_by_warehouse.at(iw);
    for (int ic = 0; ic < c; ic++)
    {
      const Contour &c = contours.at(ic);
      if (contour_contains_point(c, w))
      {
        membership.set(ic);
      }
    }
    DEBUG(2, "iw " << iw << " membership.count() " << membership.count());
  }

  std::vector<ContourMembership> contour_membership_by_stadium(m);
  for (int is = 0; is < m; is++)
  {
    const Stadium &s = stadiums.at(is);
    ContourMembership &membership = contour_membership_by_stadium.at(is);
    for (int ic = 0; ic < c; ic++)
    {
      const Contour &c = contours.at(ic);
      if (contour_contains_point(c, s))
      {
        membership.set(ic);
      }
    }
    DEBUG(2, "is " << is << " membership.count() " << membership.count());
  }

  // WARNING Scaled by 100 to be integers
  std::vector<std::vector<int>> adjusted_revenues(n, std::vector<int>(m));
  for (int iw = 0; iw < n; iw++)
  {
    const ContourMembership &membership_w = contour_membership_by_warehouse.at(iw);
    for (int is = 0; is < m; is++)
    {
      const ContourMembership &membership_s = contour_membership_by_stadium.at(is);
      const int crossed_count = (membership_w ^ membership_s).count();
      DEBUG(2, "iw " << iw << " is " << is << " crossed_count " << crossed_count);
      adjusted_revenues.at(iw).at(is) = estimated_revenues.at(iw).at(is) * 100 - crossed_count;
    }
  }

  Program lp(CGAL::SMALLER, true, 0, true, max_supply_any_wareshouse);
  int next_eq_index = 0;
  auto get_lp_var = [n, m](int iw, int is) {
    assert(iw >= 0 && iw < n && is >= 0 && is < m);
    return iw * m + is;
  };

  // Check: Warehouses have enough supply
  for (int iw = 0; iw < n; iw++)
  {
    for (int is = 0; is < m; is++)
    {
      lp.set_a(get_lp_var(iw, is), next_eq_index, 1);
    }
    lp.set_b(next_eq_index, warehouses.at(iw).s);
    next_eq_index++;
  }

  // Check: Not too much alcohol in stadiums (in percent-liters)
  for (int is = 0; is < m; is++)
  {
    for (int iw = 0; iw < n; iw++)
    {
      lp.set_a(get_lp_var(iw, is), next_eq_index, warehouses.at(iw).a);
    }
    lp.set_b(next_eq_index, 100 * stadiums.at(is).u);
    next_eq_index++;
  }

  // Check: Exactly the right amount of liquid in stadiums
  for (int is = 0; is < m; is++)
  {
    for (int iw = 0; iw < n; iw++)
    {
      lp.set_a(get_lp_var(iw, is), next_eq_index, 1);
    }
    lp.set_b(next_eq_index, stadiums.at(is).d);
    lp.set_r(next_eq_index, CGAL::EQUAL);
    next_eq_index++;
  }

  // Maximize revenue
  for (int iw = 0; iw < n; iw++)
  {
    for (int is = 0; is < m; is++)
    {
      lp.set_c(get_lp_var(iw, is), -adjusted_revenues.at(iw).at(is));
    }
  }

  Solution solution = CGAL::solve_linear_program(lp, ET());
  if (solution.is_optimal())
  {
    std::cout << floor_to_double(solution.objective_value() / -100) << "\n";
  }
  else
  {
    std::cout << "RIOT!\n";
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