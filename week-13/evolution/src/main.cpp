#include <iostream>
#include <cassert>
#include <vector>
#include <map>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/depth_first_search.hpp>

const int debug_level = 0;

#define DEBUG(min_level, x)      \
  if (debug_level >= min_level)  \
  {                              \
    std::cerr << x << std::endl; \
  }

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS> Graph;

class DfsVisitor : public boost::default_dfs_visitor
{
public:
  DfsVisitor(std::vector<std::vector<int>> &jump_pointers_by_node) : jump_pointers_by_node(jump_pointers_by_node){};

  void discover_vertex(int node, const Graph &G)
  {
    for (int i = 1; i <= int(path.size()); i = i << 1)
    {
      jump_pointers_by_node.at(node).push_back(path.at(path.size() - i));
    }
    path.push_back(node);
  }

  void finish_vertex(int node, const Graph &G)
  {
    assert(!path.empty() && path.back() == node);
    path.pop_back();
  }

private:
  std::vector<int> path;
  std::vector<std::vector<int>> &jump_pointers_by_node;
};

void testcase()
{
  int n, q;
  std::cin >> n >> q;
  assert(n >= 1 && n <= 5e4 && q >= 1 && q <= 5e4);

  std::map<std::string, int> species_by_name;
  std::vector<std::string> names_by_node(n);
  std::vector<int> age_by_node(n);
  for (int i = 0; i < n; i++)
  {
    std::string name;
    int age;
    std::cin >> name >> age;
    assert(name.size() >= 1 && name.size() <= 10 && age >= 0 && age <= 1e9);
    species_by_name[name] = i;
    names_by_node.at(i) = name;
    age_by_node.at(i) = age;
  }

  Graph G(n);
  std::vector<bool> has_parent_by_node(n, false);
  for (int i = 1; i < n; i++)
  {
    std::string name_s, name_p;
    std::cin >> name_s >> name_p;
    const int s = species_by_name.at(name_s), p = species_by_name.at(name_p);
    assert(age_by_node.at(s) <= age_by_node.at(p));
    boost::add_edge(p, s, G);
    assert(!has_parent_by_node.at(s));
    has_parent_by_node.at(s) = true;
  }

  const int root = std::find(has_parent_by_node.begin(), has_parent_by_node.end(), false) - has_parent_by_node.begin();
  std::vector<std::vector<int>> jump_pointers_by_node(n);
  boost::depth_first_search(G, boost::root_vertex(root).visitor(DfsVisitor(jump_pointers_by_node)));

  for (int i = 0; i < q; i++)
  {
    std::string name_s;
    int b;
    std::cin >> name_s >> b;
    assert(b >= 0 && b <= 1e9);
    const int s = species_by_name.at(name_s);
    assert(age_by_node.at(s) <= b);

    int cursor = s;
    int j = std::numeric_limits<int>::max();
    while (true)
    {
      j = std::min(int(jump_pointers_by_node.at(cursor).size() - 1), j);
      if (j < 0)
      {
        break;
      }
      const int p = jump_pointers_by_node.at(cursor).at(j);
      if (age_by_node.at(p) > b)
      {
        j--;
        continue;
      }
      cursor = p;
    }

    if (i > 0)
    {
      std::cout << " ";
    }
    std::cout << names_by_node.at(cursor);
  }
  std::cout << "\n";
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