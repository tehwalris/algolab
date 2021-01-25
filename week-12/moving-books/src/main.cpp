#include <iostream>
#include <cassert>
#include <vector>
#include <algorithm>
#include <queue>
#include <map>
#include <set>

const int debug_level = 0;

#define DEBUG(min_level, x)      \
  if (debug_level >= min_level)  \
  {                              \
    std::cerr << x << std::endl; \
  }

void testcase()
{
  int n, m;
  std::cin >> n >> m;
  assert(n >= 1 && n <= 3e5 && m >= 1 && m <= 3e5);

  std::vector<int> strengths(n);
  for (int &s : strengths)
  {
    std::cin >> s;
    assert(s >= 1 && s <= 5e5);
  }

  std::vector<int> weights(m);
  for (int &m : weights)
  {
    std::cin >> m;
    assert(m >= 1 && m <= 5e5);
  }

  std::sort(strengths.begin(), strengths.end());
  std::sort(weights.begin(), weights.end());

  std::map<int, int> active_queues;
  std::set<int> active_people;
  {
    int j = 0;
    for (int i = 0; i < n; i++)
    {
      active_people.insert(i);
      const int s = strengths.at(i);
      int queue = 0;
      while (j < int(weights.size()))
      {
        const int w = weights.at(j);
        if (w > s)
        {
          break;
        }
        queue++;
        j++;
      }
      if (queue > 0)
      {
        active_queues.insert(std::make_pair(i, queue));
      }
    }
  }

  int rounds = 0;
  for (int carried_boxes = 0; carried_boxes < m;)
  {
    const int old_carried_boxes = carried_boxes;
    rounds++;

    std::vector<int> just_stopped_people;
    for (int i : active_people)
    {
      auto it = active_queues.upper_bound(i);
      if (it == active_queues.begin())
      {
        just_stopped_people.push_back(i);
        continue;
      }
      it--;
      assert(it->first <= i);

      assert(it->second > 0);
      it->second--;
      if (it->second == 0)
      {
        active_queues.erase(it);
      }
      carried_boxes++;
    }

    for (int i : just_stopped_people)
    {
      active_people.erase(i);
    }

    if (carried_boxes == old_carried_boxes)
    {
      std::cout << "impossible\n";
      return;
    }
  }

  assert(rounds >= 1 && rounds >= m / n && rounds <= m);
  std::cout << (rounds * 3 - 1) << "\n";
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