#include <iostream>
#include <vector>
#include <cassert>
#include <map>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/max_cardinality_matching.hpp>

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS> Graph;

void testcase()
{
  int n, c, f;
  std::cin >> n >> c >> f;
  assert(n >= 2 && n <= 400 && n % 2 == 0);
  assert(c >= 1 && c <= 100);
  assert(f >= 0);

  std::map<std::string, int> ids_by_c;
  std::vector<std::vector<int>> cs_by_student(n);
  std::vector<std::vector<bool>> student_masks_by_c;
  for (int i = 0; i < n; i++)
  {
    std::vector<int> &cs_this_student = cs_by_student.at(i);
    for (int j = 0; j < c; j++)
    {
      std::string c_name;
      std::cin >> c_name;
      assert(c_name.size() <= 20);
      if (ids_by_c.count(c_name) == 0)
      {
        ids_by_c.insert(std::make_pair(c_name, ids_by_c.size()));
        student_masks_by_c.emplace_back(std::vector<bool>(n, false));
      }
      int c_id = ids_by_c.at(c_name);

      cs_this_student.push_back(c_id);
      student_masks_by_c.at(c_id).at(i) = true;
    }
  }

  Graph G(n);
  for (int i = 0; i < n; i++)
  {
    for (int j = i + 1; j < n; j++)
    {
      int overlap_count_lower_bound = 0;
      for (auto c_id : cs_by_student.at(i))
      {
        if (student_masks_by_c.at(c_id).at(j))
        {
          overlap_count_lower_bound++;
        }
        if (overlap_count_lower_bound > f)
        {
          break;
        }
      }

      if (overlap_count_lower_bound > f)
      {
        boost::add_edge(i, j, G);
      }
    }
  }

  std::vector<Graph::vertex_descriptor> mate(n);
  bool success = boost::checked_edmonds_maximum_cardinality_matching(G, &mate[0]);
  assert(success);

  bool all_matched = true;
  for (auto its = boost::vertices(G); its.first != its.second; its.first++)
  {
    if (mate[*its.first] == Graph::null_vertex())
    {
      all_matched = false;
    }
  }

  std::cout << (all_matched ? "not optimal" : "optimal") << "\n";
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