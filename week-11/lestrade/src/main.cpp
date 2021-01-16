#include <iostream>
#include <cassert>
#include <vector>
#include <CGAL/QP_models.h>
#include <CGAL/QP_functions.h>
#include <CGAL/Gmpz.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Triangulation_vertex_base_with_info_2.h>
#include <CGAL/Triangulation_face_base_2.h>

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

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Triangulation_vertex_base_with_info_2<int, K> Vb;
typedef CGAL::Triangulation_face_base_2<K> Fb;
typedef CGAL::Triangulation_data_structure_2<Vb, Fb> Tds;
typedef CGAL::Delaunay_triangulation_2<K, Tds> Triangulation;

struct GangMember
{
  int x, y, u, v, w;
};

struct Agent
{
  int x, y, z;
};

const int no_spying = -1;

void testcase()
{
  int z, u, v, w;
  std::cin >> z >> u >> v >> w;
  for (const int value : {z, u, v, w})
  {
    assert(value >= 0 && value <= (1 << 24));
    (void)value;
  }

  int a, g;
  std::cin >> a >> g;
  assert(a >= 1 && a <= 4000 && g >= 1 && g <= 9e4);

  std::vector<GangMember> gang_members(g);
  for (GangMember &gm : gang_members)
  {
    std::cin >> gm.x >> gm.y >> gm.u >> gm.v >> gm.w;
    assert(abs(gm.x) < (1 << 24) && abs(gm.y) < (1 << 24));
    for (const int value : {gm.u, gm.v, gm.w})
    {
      assert(value >= 0 && value <= (1 << 10));
      (void)value;
    }
  }

  std::vector<Agent> agents(a);
  for (Agent &ag : agents)
  {
    std::cin >> ag.x >> ag.y >> ag.z;
    assert(abs(ag.x) < (1 << 24) && abs(ag.y) < (1 << 24));
    assert(ag.z >= 0 && ag.z <= (1 << 10));
  }

  Triangulation triangulation;
  std::vector<std::pair<K::Point_2, int>> indexed_gang_member_points;
  for (int i = 0; i < g; i++)
  {
    const GangMember &gm = gang_members.at(i);
    indexed_gang_member_points.push_back(std::make_pair(K::Point_2(gm.x, gm.y), i));
  }
  triangulation.insert(indexed_gang_member_points.begin(), indexed_gang_member_points.end());

  std::vector<int> spying_cost_by_gang_member(g, std::numeric_limits<int>::max());
  std::vector<int> variable_by_gang_member(g, no_spying);
  int next_free_variable = 0;
  for (int i = 0; i < a; i++)
  {
    const Agent &ag = agents.at(i);
    const int j = triangulation.nearest_vertex(K::Point_2(ag.x, ag.y))->info();
    int &variable = variable_by_gang_member.at(j);
    if (variable == no_spying)
    {
      variable_by_gang_member.at(j) = next_free_variable++;
    }
    int &cost = spying_cost_by_gang_member.at(j);
    cost = std::min(cost, ag.z);
  }

  Program lp(CGAL::LARGER, true, 0, true, 24);
  for (int i = 0; i < g; i++)
  {
    const int variable = variable_by_gang_member.at(i);
    if (variable == no_spying)
    {
      continue;
    }
    const int cost = spying_cost_by_gang_member.at(i);
    const GangMember &gm = gang_members.at(i);
    lp.set_a(variable, 0, gm.u);
    lp.set_a(variable, 1, gm.v);
    lp.set_a(variable, 2, gm.w);
    lp.set_a(variable, 3, cost);
  }
  lp.set_b(0, u);
  lp.set_b(1, v);
  lp.set_b(2, w);
  lp.set_b(3, z);
  lp.set_r(3, CGAL::SMALLER);

  Solution s = CGAL::solve_linear_program(lp, ET());
  std::cout << (s.is_infeasible() ? "H" : "L") << "\n";
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