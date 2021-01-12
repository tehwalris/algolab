#include <iostream>
#include <map>
#include <vector>
#include <assert.h>
#include <algorithm>

const int debug_level = 1;

const int red_meeple = 0;
const int black_meeple = 1;
const int dp_unknown = -1;

typedef std::multimap<int, int> AdjacencyList;
typedef std::vector<std::vector<std::vector<int>>> DpTable;

DpTable dp_table;

inline int get_other_meeple(int meeple)
{
  assert(meeple == red_meeple || meeple == black_meeple);
  return meeple == red_meeple ? black_meeple : red_meeple;
}

int calc_ideal_path_length(AdjacencyList &fw_transitions, int starting_pos, int starting_player, int long_player);

int _calc_ideal_path_length(AdjacencyList &fw_transitions, int starting_pos, int starting_player, int long_player)
{
  auto transitions = fw_transitions.equal_range(starting_pos);
  if (transitions.first == transitions.second)
  {
    return 0;
  }
  std::vector<int> choice_lengths;
  for (auto it = transitions.first; it != transitions.second; it++)
  {
    choice_lengths.push_back(calc_ideal_path_length(fw_transitions, it->second, get_other_meeple(starting_player), long_player));
  }
  if (starting_player == long_player)
  {
    return *std::max_element(choice_lengths.begin(), choice_lengths.end()) + 1;
  }
  else
  {
    return *std::min_element(choice_lengths.begin(), choice_lengths.end()) + 1;
  }
}

int calc_ideal_path_length(AdjacencyList &fw_transitions, int starting_pos, int starting_player, int long_player)
{
  int result = dp_table.at(long_player).at(starting_player).at(starting_pos);
  if (result == dp_unknown)
  {
    result = _calc_ideal_path_length(fw_transitions, starting_pos, starting_player, long_player);
    dp_table.at(long_player).at(starting_player).at(starting_pos) = result;
  }
  return result;
}

void init_dp_table(int n)
{
  dp_table.clear();
  for (int long_player = red_meeple; long_player <= black_meeple; long_player++)
  {
    dp_table.push_back(std::vector<std::vector<int>>());
    for (int starting_player = red_meeple; starting_player <= black_meeple; starting_player++)
    {
      dp_table.back().push_back(std::vector<int>(n, dp_unknown));
    }
  }
}

int game_length_from_path_length(int path_length, int meeple_color)
{
  int i = 1, t = 1, cur_player = red_meeple;
  while (true)
  {
    if (debug_level >= 3)
    {
      std::cout << "i = " << i << ", t = " << t << ", cur_player = " << cur_player << "\n";
    }
    if (t == path_length && cur_player == meeple_color)
    {
      return i;
    }

    cur_player = get_other_meeple(cur_player);
    i++;

    if (debug_level >= 3)
    {
      std::cout << "i = " << i << ", t = " << t << ", cur_player = " << cur_player << "\n";
    }
    if (t == path_length && cur_player == meeple_color)
    {
      return i;
    }

    t++;
    i++;
  }
}

void testcase()
{
  int n, m, red_start, black_start;
  std::cin >> n >> m >> red_start >> black_start;
  assert(n >= 2 && m >= 1);
  assert(red_start >= 1 && red_start <= n - 1);
  assert(black_start >= 1 && black_start <= n - 1);
  red_start--;
  black_start--;

  AdjacencyList fw_transitions, bw_transitions;
  for (int i = 0; i < m; i++)
  {
    int u, v;
    std::cin >> u >> v;
    assert(u >= 1 && u < v && v <= n);
    u--;
    v--;
    fw_transitions.insert(std::make_pair(u, v));
    bw_transitions.insert(std::make_pair(v, u));
  }
  assert(int(fw_transitions.size()) == m && int(bw_transitions.size()) == m);

  if (debug_level >= 3)
  {
    std::cout << "fw_transitions\n";
    for (auto entry : fw_transitions)
    {
      std::cout << entry.first << ' ' << entry.second << '\n';
    }
    std::cout << "bw_transitions\n";
    for (auto entry : bw_transitions)
    {
      std::cout << entry.first << ' ' << entry.second << '\n';
    }
  }

  init_dp_table(n);
  int red_path_length = calc_ideal_path_length(fw_transitions, red_start, red_meeple, black_meeple);
  int black_path_length = calc_ideal_path_length(fw_transitions, black_start, black_meeple, red_meeple);
  if (debug_level >= 2)
  {
    std::cout << "red_path_length " << red_path_length << "\n";
    std::cout << "black_path_length " << black_path_length << "\n";
  }

  int red_game_length = game_length_from_path_length(red_path_length, red_meeple);
  int black_game_length = game_length_from_path_length(black_path_length, black_meeple);
  if (debug_level >= 2)
  {
    std::cout << "red_game_length " << red_game_length << "\n";
    std::cout << "black_game_length " << black_game_length << "\n";
  }
  assert(red_game_length != black_game_length);

  int winner = red_game_length < black_game_length ? red_meeple : black_meeple;
  std::cout << winner << "\n";
}

int main()
{
  std::ios_base::sync_with_stdio(false);

  int n;
  std::cin >> n;
  for (int i = 0; i < n; i++)
  {
    if (debug_level >= 2)
    {
      std::cout << "test case " << i << '\n';
    }
    testcase();
  }

  return 0;
}