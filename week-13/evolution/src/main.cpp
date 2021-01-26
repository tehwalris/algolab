#include <iostream>
#include <cassert>
#include <vector>
#include <map>

const int debug_level = 0;

#define DEBUG(min_level, x)      \
  if (debug_level >= min_level)  \
  {                              \
    std::cerr << x << std::endl; \
  }

const int virtual_root = -1;

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

  std::vector<int> parent_by_node(n, virtual_root);
  for (int i = 1; i < n; i++)
  {
    std::string name_s, name_p;
    std::cin >> name_s >> name_p;
    const int s = species_by_name.at(name_s), p = species_by_name.at(name_p);
    parent_by_node.at(s) = p;
    assert(age_by_node.at(s) <= age_by_node.at(p));
  }

  for (int i = 0; i < q; i++)
  {
    std::string name_s;
    int b;
    std::cin >> name_s >> b;
    assert(b >= 0 && b <= 1e9);
    const int s = species_by_name.at(name_s);
    assert(age_by_node.at(s) <= b);

    int cursor = s;
    while (true)
    {
      const int p = parent_by_node.at(cursor);
      if (p == virtual_root || age_by_node.at(p) > b)
      {
        break;
      }
      cursor = p;
    }

    int output_i = cursor; // TODO redundant variable
    if (i > 0)
    {
      std::cout << " ";
    }
    std::cout << names_by_node.at(output_i);
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