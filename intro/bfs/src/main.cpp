#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <deque>

void testcase()
{
  int n, m, v;
  std::cin >> n >> m >> v;

  std::map<int, std::set<int>> edges;
  for (int i = 0; i < n; i++)
  {
    edges.insert(std::make_pair(i, std::set<int>()));
  }
  for (int i = 0; i < m; i++)
  {
    int a, b;
    std::cin >> a >> b;
    edges.at(a).insert(b);
    edges.at(b).insert(a);
  }

  std::vector<int> nodeDistances(n, -1);
  std::deque<std::pair<int, int>> nextNodes;
  nextNodes.push_back(std::make_pair(v, 0));
  while (!nextNodes.empty())
  {
    int node = nextNodes.front().first;
    int distance = nextNodes.front().second;
    nextNodes.pop_front();

    if (nodeDistances[node] != -1)
    {
      continue;
    }

    nodeDistances[node] = distance;

    auto neighbors = edges.at(node);
    for (auto it = neighbors.rbegin(); it != neighbors.rend(); it++)
    {
      nextNodes.push_back(std::make_pair(*it, distance + 1));
    }
  }

  for (auto it = nodeDistances.begin(); it != nodeDistances.end(); it++)
  {
    if (it != nodeDistances.begin())
    {
      std::cout << ' ';
    }
    std::cout << *it;
  }
  std::cout << '\n';
}

int main()
{
  std::ios_base::sync_with_stdio(false);

  int n;
  std::cin >> n;
  for (int i = 0; i < n; i++)
  {
    testcase();
  }

  return 0;
}