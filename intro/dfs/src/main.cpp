#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <stack>

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
    edges.at(b).insert(a); // TODO assuming undirected graph
  }

  std::vector<int> firstVisitedAt(n, -1);
  std::vector<int> finishedAt(n, -1);
  std::stack<int> nextNodes;
  nextNodes.push(v);
  for (int timestamp = 0; !nextNodes.empty();)
  {
    int node = nextNodes.top();
    nextNodes.pop();

    if (firstVisitedAt[node] == -1)
    {
      firstVisitedAt[node] = timestamp++;

      nextNodes.push(node);

      auto neighbors = edges.at(node);
      for (auto it = neighbors.rbegin(); it != neighbors.rend(); it++)
      {
        if (firstVisitedAt[*it] == -1)
        {
          nextNodes.push(*it);
        }
      }
    }
    else if (finishedAt[node] == -1)
    {
      finishedAt[node] = timestamp++;
    }
  }

  std::array<std::vector<int>, 2> vectors{firstVisitedAt, finishedAt};
  for (auto vec : vectors)
  {
    for (auto it = vec.begin(); it != vec.end(); it++)
    {
      if (it != vec.begin())
      {
        std::cout << ' ';
      }
      std::cout << *it;
    }
    std::cout << '\n';
  }
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