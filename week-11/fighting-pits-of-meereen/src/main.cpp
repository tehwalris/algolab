#include <iostream>
#include <cassert>
#include <array>
#include <vector>
#include <math.h>
#include <algorithm>

const int debug_level = 0;

#define DEBUG(min_level, x)      \
  if (debug_level >= min_level)  \
  {                              \
    std::cerr << x << std::endl; \
  }

const int max_possible_m = 3;
const int max_possible_k = 4;
const int no_fighter = max_possible_k;
const int fighter_bits = 3;
const int excitement_per_fighter = 1000;
const int dp_undefined = -1;

typedef std::array<int, max_possible_m - 1> EntranceQueue;

struct State
{
  EntranceQueue left_queue, right_queue;
  int imbalance;
};

typedef std::vector<int> DpRow;                            // one entry per encoded state storing the highest total excitement
typedef std::vector<std::pair<State, int>> ValidStatesRow; // one entry corresponding to each cell of DpRow (pair.first is decoded state, pair.second is encoded state)

class Testcase
{
public:
  void run()
  {
    std::cin >> n >> k >> m;
    assert(n >= 1 && n <= 5e3 && k >= 2 && k <= max_possible_k && m >= 2 && m <= max_possible_m);

    max_abs_imbalance = ceil(log2(m * 1000 + 1));
    abs_imbalance_bits = ceil(log2(max_abs_imbalance + 1));
    assert(abs_imbalance_bits >= 1 && ((max_abs_imbalance >> abs_imbalance_bits) == 0));

    fighters.reserve(n);
    for (int i = 0; i < n; i++)
    {
      int fighter;
      std::cin >> fighter;
      assert(fighter >= 0 && fighter <= max_possible_k);
      fighters.push_back(fighter);
    }

    const int bits_per_state = fighter_bits * (max_possible_m - 1) * 2 + abs_imbalance_bits + 1;
    assert(bits_per_state <= 31);
    const int num_reserved_states = 1 << bits_per_state;

    const auto try_improve_state_cost = [this](DpRow &row, ValidStatesRow &valid_states, const State &new_state, const int new_excitement) {
      const int encoded_new_state = encode_state(new_state);
      int &x = row.at(encoded_new_state);
      if (x == dp_undefined)
      {
        valid_states.push_back(std::make_pair(new_state, encoded_new_state));
      }
      assert(dp_undefined < new_excitement);
      x = std::max(x, new_excitement);
    };

    DpRow prev_row(num_reserved_states, dp_undefined);
    ValidStatesRow prev_valid_states;
    State initial_state;
    initial_state.left_queue.fill(no_fighter);
    initial_state.right_queue.fill(no_fighter);
    initial_state.imbalance = 0;
    try_improve_state_cost(prev_row, prev_valid_states, initial_state, 0);

    DpRow next_row(num_reserved_states, dp_undefined);
    ValidStatesRow next_valid_states;
    for (int i = 0; i < n; i++)
    {
      const int next_fighter = fighters.at(i);
      DEBUG(2, "i " << i << " next_fighter " << next_fighter);
      for (const std::pair<State, int> &prev_state_entry : prev_valid_states)
      {
        const int j = prev_state_entry.second;
        const int prev_total_excitement = prev_row.at(j);
        prev_row.at(j) = dp_undefined;
        DEBUG(2, "j " << j << " prev_total_excitement " << prev_total_excitement);
        assert(prev_total_excitement != dp_undefined && prev_total_excitement >= 0);

        const State &prev_state = prev_state_entry.first;

        State left_state = prev_state;
        left_state.imbalance--;
        const int left_excitement = calculate_excitement(left_state.left_queue, next_fighter, left_state.imbalance);
        if (left_excitement >= 0)
        {
          push_to_queue(left_state.left_queue, next_fighter);
          try_improve_state_cost(next_row, next_valid_states, left_state, prev_total_excitement + left_excitement);
        }

        State right_state = prev_state;
        right_state.imbalance++;
        const int right_excitement = calculate_excitement(right_state.right_queue, next_fighter, right_state.imbalance);
        if (right_excitement >= 0)
        {
          push_to_queue(right_state.right_queue, next_fighter);
          try_improve_state_cost(next_row, next_valid_states, right_state, prev_total_excitement + right_excitement);
        }
      }

      std::swap(prev_row, next_row);
      std::swap(prev_valid_states, next_valid_states);
      next_valid_states.clear();
    }

    const int max_possible_excitement = *std::max_element(prev_row.begin(), prev_row.end());
    assert(max_possible_excitement > 0);
    std::cout << max_possible_excitement << "\n";
  }

private:
  int n, k, m;
  std::vector<int> fighters;
  int max_abs_imbalance;
  int abs_imbalance_bits;

  inline int encode_state(const State &s)
  {
    int output = 0;

    for (const EntranceQueue &queue : {s.left_queue, s.right_queue})
    {
      for (const int fighter : queue)
      {
        assert(fighter >= 0 && fighter <= max_possible_k || fighter == no_fighter);
        assert((fighter >> fighter_bits) == 0);
        output = (output << fighter_bits) | fighter;
      }
    }

    assert(abs(s.imbalance) >> abs_imbalance_bits == 0);
    output = (output << abs_imbalance_bits) | abs(s.imbalance);

    output = (output << 1) | (s.imbalance < 0);

    assert(output >= 0);
    return output;
  }

  inline void push_to_queue(EntranceQueue &queue, int fighter)
  {
    for (int i = m - 2; i >= 1; i--)
    {
      queue.at(i) = queue.at(i - 1);
    }
    queue.at(0) = fighter;
  }

  inline int calculate_excitement(EntranceQueue &old_queue, int next_fighter, int new_imbalance)
  {
    std::array<bool, max_possible_k> seen_fighter_type;
    seen_fighter_type.fill(false);
    seen_fighter_type.at(next_fighter) = true;
    for (int i = 0; i < m - 1; i++)
    {
      int fighter = old_queue.at(i);
      if (fighter != no_fighter)
      {
        seen_fighter_type.at(fighter) = true;
      }
    }

    int num_seen_fighter_types = 0;
    for (const bool seen : seen_fighter_type)
    {
      if (seen)
      {
        num_seen_fighter_types++;
      }
    }

    return num_seen_fighter_types * excitement_per_fighter - (1 << abs(new_imbalance));
  }
};

int main()
{
  std::ios_base::sync_with_stdio(false);

  int t;
  std::cin >> t;
  for (int i = 0; i < t; i++)
  {
    Testcase().run();
    DEBUG(1, "");
  }

  return 0;
}