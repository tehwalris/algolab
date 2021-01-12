#include <iostream>
#include <cassert>
#include <vector>
#include <algorithm>
#include <map>
#include <set>
#include <limits>

const int debug_level = 0;
#define DEBUG(min_level, x)      \
  if (debug_level >= min_level)  \
  {                              \
    std::cerr << x << std::endl; \
  }

const int max_jedi_in_some_segment = 10;

typedef std::pair<int, int> Segment;
typedef std::pair<std::vector<Segment>, std::vector<Segment>> FlatProblem;

int find_min_location(int n, int m, const std::vector<Segment> &circular_jedi_segments)
{
  assert(int(circular_jedi_segments.size()) == n);

  int min_location = 1, jedi_at_min_location = 0;
  std::vector<int> sorted_starts, sorted_ends;
  for (auto &segment : circular_jedi_segments)
  {
    assert(segment.first >= 1 && segment.first <= m);
    assert(segment.second >= 1 && segment.second <= m);
    if (segment.first <= segment.second)
    {
      if (segment.first == 1)
      {
        jedi_at_min_location++;
      }
      else
      {
        sorted_starts.push_back(segment.first);
      }
      if (segment.second != m)
      {
        sorted_ends.push_back(segment.second);
      }
    }
    else
    {
      sorted_starts.push_back(segment.first);
      sorted_ends.push_back(segment.second);
      jedi_at_min_location++;
    }
  }
  std::sort(sorted_starts.begin(), sorted_starts.end());
  std::sort(sorted_ends.begin(), sorted_ends.end());

  std::vector<int> candidate_locations;
  int last_candidate_location = 1;
  for (auto right_after_location : sorted_starts)
  {
    int location = right_after_location - 1;
    assert(location >= 1 && location <= m);
    if (location != last_candidate_location)
    {
      candidate_locations.push_back(location);
      last_candidate_location = location;
    }
  }
  if (candidate_locations.empty() || candidate_locations.back() != m)
  {
    candidate_locations.push_back(m);
  }

  int jedi_at_location = jedi_at_min_location;
  std::vector<int>::iterator sorted_starts_it = sorted_starts.begin(), sorted_ends_it = sorted_ends.begin();
  for (auto location : candidate_locations)
  {
    while (sorted_starts_it != sorted_starts.end() && *sorted_starts_it <= location)
    {
      jedi_at_location++;
      sorted_starts_it++;
    }
    while (sorted_ends_it != sorted_ends.end() && *sorted_ends_it < location)
    {
      jedi_at_location--;
      sorted_ends_it++;
    }
    if (jedi_at_location < jedi_at_min_location)
    {
      min_location = location;
      jedi_at_min_location = jedi_at_location;
    }
  }

  DEBUG(2, "min_location " << min_location << " jedi_at_min_location " << jedi_at_min_location);

  assert(min_location >= 1 && min_location <= m);
  assert(jedi_at_min_location <= max_jedi_in_some_segment);

  return min_location;
}

FlatProblem flatten_jedi_problem(int n, int m, const std::vector<Segment> &circular_jedi_segments)
{
  assert(int(circular_jedi_segments.size()) == n);

  int flatten_from = find_min_location(n, m, circular_jedi_segments);

  FlatProblem return_value;
  std::vector<Segment> &flat_jedi_segments = return_value.first;
  std::vector<Segment> &unflattenable_jedi_segments = return_value.second;
  for (int i = 0; i < n; i++)
  {
    const Segment &circular_segment = circular_jedi_segments.at(i);

    flat_jedi_segments.emplace_back();
    Segment &flat_segment = flat_jedi_segments.back();

    flat_segment.first = circular_segment.first - flatten_from - 1;
    if (flat_segment.first < 0)
    {
      flat_segment.first += m;
    }
    assert(flat_segment.first >= 0 && flat_segment.first < m);

    flat_segment.second = circular_segment.second - flatten_from - 1;
    if (flat_segment.second < 0)
    {
      flat_segment.second += m;
    }
    assert(flat_segment.second >= 0 && flat_segment.second < m);

    if (flat_segment.first > flat_segment.second)
    {
      unflattenable_jedi_segments.push_back(flat_segment);
      flat_jedi_segments.pop_back();
    }
  }
  assert(unflattenable_jedi_segments.size() <= max_jedi_in_some_segment);

  return return_value;
}

int solve_flat_jedi_problem(int n, int m, FlatProblem &flat_problem)
{
  std::vector<Segment> &flat_jedi_segments = flat_problem.first;
  std::vector<Segment> &unflattenable_jedi_segments = flat_problem.second;
  assert(int(flat_jedi_segments.size() + unflattenable_jedi_segments.size()) == n);

  std::sort(flat_jedi_segments.begin(), flat_jedi_segments.end(), [](Segment a, Segment b) { return a.second < b.second; });

  auto solve = [&flat_jedi_segments](int next_free, int first_blocked, int used_jedi) {
    for (auto &segment : flat_jedi_segments)
    {
      if (segment.first < next_free || segment.second >= first_blocked)
      {
        continue;
      }
      used_jedi++;
      next_free = segment.second + 1;
    }
    return used_jedi;
  };

  int max_used_jedi = solve(0, m, 0);
  for (Segment &blocked_segment : unflattenable_jedi_segments)
  {
    int used_jedi = solve(blocked_segment.second + 1, blocked_segment.first, 1);
    max_used_jedi = std::max(max_used_jedi, used_jedi);
  }
  return max_used_jedi;
}

void testcase()
{
  int n, m;
  std::cin >> n >> m;
  DEBUG(1, "n " << n << " m " << m);
  assert(n >= 1 && n <= 5e4 && m >= 1 && m <= 1e9);

  std::vector<Segment> circular_jedi_segments(n);
  for (int i = 0; i < n; i++)
  {
    int a, b;
    std::cin >> a >> b;
    assert(a >= 1 && a <= m && b >= 1 && b <= m);
    circular_jedi_segments.at(i) = std::make_pair(a, b);
  }

  FlatProblem flat_problem = flatten_jedi_problem(n, m, circular_jedi_segments);
  std::cout << solve_flat_jedi_problem(n, m, flat_problem) << "\n";
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