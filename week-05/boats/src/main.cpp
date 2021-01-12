#include <iostream>
#include <cassert>
#include <vector>
#include <limits>
#include <algorithm>
#include <queue>

const int debug_level = 0;

const int dp_undefined = -1;

void testcase()
{
  int n;
  std::cin >> n;
  assert(n >= 1 && n <= 1e5);

  std::vector<int> boat_lengths(n), ring_positions(n);
  for (int i = 0; i < n; i++)
  {
    int l, p;
    std::cin >> l >> p;
    assert(l >= 1 && l <= 1e6 && p >= 1 && p <= 1e6);
    boat_lengths.at(i) = l;
    ring_positions.at(i) = p;
  }

  std::vector<int> boat_earliest_starts(n);
  for (int i = 0; i < n; i++)
  {
    boat_earliest_starts.at(i) = ring_positions.at(i) - boat_lengths.at(i);
  }

  std::vector<int> indices_sorted_by_earliest_start(n), indices_sorted_by_ring_position(n);
  for (int i = 0; i < n; i++)
  {
    indices_sorted_by_earliest_start.at(i) = i;
    indices_sorted_by_ring_position.at(i) = i;
  }
  std::sort(indices_sorted_by_earliest_start.begin(), indices_sorted_by_earliest_start.end(), [&boat_earliest_starts, &ring_positions](int a, int b) {
    return boat_earliest_starts.at(a) < boat_earliest_starts.at(b) || (boat_earliest_starts.at(a) == boat_earliest_starts.at(b) && ring_positions.at(a) < ring_positions.at(b));
  });
  std::sort(indices_sorted_by_ring_position.begin(), indices_sorted_by_ring_position.end(), [&ring_positions](int a, int b) {
    return ring_positions.at(a) < ring_positions.at(b);
  });

  std::vector<int> cur_length_limited_indices;
  int next_ring_limited = 0, next_length_limited = 0, cur_pos = std::numeric_limits<int>::min(), tied_boats = 0, last_chosen_i = -1;
  while (true)
  {
    if (debug_level >= 2)
    {
      std::cout << "next_length_limited " << next_length_limited << " cur_pos " << cur_pos << " tied_boats " << tied_boats << std::endl;
    }

    int chosen_i = -1;
    int chosen_next_pos;
    while (next_ring_limited < n)
    {
      int i = indices_sorted_by_ring_position.at(next_ring_limited);
      next_ring_limited++;
      if (ring_positions.at(i) >= cur_pos && i != last_chosen_i)
      {
        chosen_i = i;
        chosen_next_pos = std::max(cur_pos + boat_lengths.at(i), ring_positions.at(i));
        break;
      }
    }
    if (chosen_i == -1)
    {
      break;
    }
    assert(chosen_i >= 0 && chosen_i < n && chosen_next_pos >= 1);
    if (debug_level >= 3)
    {
      std::cout << "chosen_i " << chosen_i << " chosen_next_pos " << chosen_next_pos << std::endl;
    }

    if (debug_level >= 2)
    {
      std::cout << "cur_length_limited_indices.size() before add " << cur_length_limited_indices.size() << std::endl;
    }

    while (next_length_limited < n && boat_earliest_starts.at(indices_sorted_by_earliest_start.at(next_length_limited)) <= (tied_boats > 0 ? chosen_next_pos : cur_pos))
    {
      cur_length_limited_indices.push_back(indices_sorted_by_earliest_start.at(next_length_limited));
      next_length_limited++;
    }

    if (debug_level >= 2)
    {
      std::cout << "cur_length_limited_indices.size() before erase " << cur_length_limited_indices.size() << std::endl;
    }

    cur_length_limited_indices.erase(std::remove_if(cur_length_limited_indices.begin(), cur_length_limited_indices.end(), [&ring_positions, cur_pos, last_chosen_i](int i) { return i == last_chosen_i || ring_positions.at(i) < cur_pos; }), cur_length_limited_indices.end());

    if (debug_level >= 2)
    {
      std::cout << "cur_length_limited_indices.size() before minimum " << cur_length_limited_indices.size() << std::endl;
    }

    for (auto i : cur_length_limited_indices)
    {
      int next_pos = std::max(cur_pos + boat_lengths.at(i), ring_positions.at(i));
      if (debug_level >= 3)
      {
        std::cout << "i " << i << " next_pos " << next_pos << std::endl;
      }
      if (next_pos < chosen_next_pos || (next_pos == chosen_next_pos && ring_positions.at(i) < ring_positions.at(chosen_i)))
      {
        chosen_i = i;
        chosen_next_pos = next_pos;
        if (debug_level >= 2)
        {
          std::cout << "chosen_i " << chosen_i << std::endl;
        }
      }
    }
    cur_pos = chosen_next_pos;
    last_chosen_i = chosen_i;
    tied_boats++;
  }

  assert(tied_boats >= 1 && tied_boats <= n);
  std::cout << tied_boats << "\n";
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