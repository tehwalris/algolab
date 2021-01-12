#include <iostream>
#include <cassert>
#include <vector>
#include <bitset>
#include <algorithm>

const int debug_level = 0;

#define DEBUG(min_level, x)      \
  if (debug_level >= min_level)  \
  {                              \
    std::cout << x << std::endl; \
  }

const long big_number = long(1) << 50;
const long too_many_movements = -1;

struct Movement
{
  long d;
  long t;

public:
  inline Movement() : Movement(0, 0){};

  inline Movement(long _d, long _t)
  {
    this->d = _d;
    this->t = _t;
  }

  friend inline bool operator<(const Movement &a, const Movement &b)
  {
    return a.t < b.t || (a.t == b.t && a.d < b.d);
  }
};

template <typename T>
void print_vec(std::vector<T> &vec, std::ostream &stream)
{
  for (int i = 0; i < int(vec.size()); i++)
  {
    if (i > 0)
    {
      stream << " ";
    }
    stream << vec.at(i);
  }
  stream << "\n";
}

std::vector<std::vector<Movement>> list_best_combined_movements(const std::vector<Movement> &movements, long T)
{
  int n = int(movements.size());
  int num_combos = 1 << n;
  std::vector<std::vector<Movement>> combined_movements(n + 1);
  for (int mask = 0; mask < num_combos; mask++)
  {
    Movement combined(0, 0);
    for (int i = 0; i < n; i++)
    {
      if (mask & (1 << i))
      {
        const Movement &movement = movements.at(i);
        combined.d += movement.d;
        combined.t += movement.t;
      }
    }
    if (combined.t < T)
    {
      combined_movements.at(__builtin_popcount(mask)).push_back(combined);
    }
  }

  std::vector<std::vector<Movement>> best_combined_movements;
  for (auto &group : combined_movements)
  {
    std::vector<Movement> modified_group;

    if (!group.empty())
    {
      long best_d = 0, last_t = -1;
      std::sort(group.begin(), group.end());
      for (Movement &movement : group)
      {
        if (movement.t != last_t)
        {
          if (last_t >= 0)
          {
            modified_group.push_back(Movement(best_d, last_t));
          }
          last_t = movement.t;
        }
        best_d = std::max(best_d, movement.d);
      }
      assert(last_t >= 0);
      modified_group.push_back(Movement(best_d, last_t));
    }

    best_combined_movements.push_back(modified_group);
  }

  return best_combined_movements;
}

std::vector<long> get_furtherest_by_num_movements(const std::vector<Movement> &movements, long D, long T)
{
  int n = int(movements.size());
  auto combined_movements_a = list_best_combined_movements(std::vector<Movement>(movements.begin(), movements.begin() + n / 2), T);
  auto combined_movements_b = list_best_combined_movements(std::vector<Movement>(movements.begin() + n / 2, movements.end()), T);

  for (auto &group_b : combined_movements_b)
  {
    DEBUG(3, "group");
    for (auto &b : group_b)
    {
      DEBUG(3, "b.d " << b.d << " b.t " << b.t);
    }
  }

  std::vector<long> furtherest_by_num_movements(n + 1, too_many_movements);
  for (int num_a = 0; num_a < int(combined_movements_a.size()); num_a++)
  {
    std::vector<Movement> &group_a = combined_movements_a.at(num_a);
    for (int num_b = 0; num_b < int(combined_movements_b.size()); num_b++)
    {
      std::vector<Movement> &group_b = combined_movements_b.at(num_b);
      if (group_b.empty())
      {
        continue;
      }
      long &best = furtherest_by_num_movements.at(num_a + num_b);
      DEBUG(3, "num_a " << num_a << " num_b " << num_b);

      for (Movement &a : group_a)
      {
        DEBUG(3, "a.d " << a.d << " a.t " << a.t);
        auto b = std::upper_bound(group_b.begin(), group_b.end(), Movement(0, T - a.t));
        DEBUG(3, "index of b " << b - group_b.begin());
        if (b != group_b.end())
        {
          DEBUG(3, "b->d " << b->d << " b->t " << b->t);
        }
        if (b != group_b.begin())
        {
          b--;
        }
        DEBUG(3, "b->d " << b->d << " b->t " << b->t);
        if (a.t + b->t >= T)
        {
          continue;
        }
        best = std::max(best, a.d + b->d);
      }
    }
  }
  return furtherest_by_num_movements;
}

void testcase()
{
  int n, m;
  long D, T;
  std::cin >> n >> m >> D >> T;
  assert(n >= 1 && n <= 30 && m >= 0 && m <= 1e5);
  assert(D >= 1 && D < big_number && T >= 1 && T < big_number);

  std::vector<Movement> movements(n);
  for (int i = 0; i < n; i++)
  {
    Movement &movement = movements.at(i);
    std::cin >> movement.d >> movement.t;
    assert(movement.d >= 1 && movement.d < big_number && movement.t >= 1 && movement.t < big_number);
  }

  std::vector<long> extra_distances_by_gulps(m + 1);
  extra_distances_by_gulps.at(0) = 0;
  for (int gulps = 1; gulps <= m; gulps++)
  {
    long s;
    std::cin >> s;
    assert(s >= 1 && s < big_number);
    if (gulps > 1)
    {
      assert(s >= extra_distances_by_gulps.at(gulps - 1));
    }
    extra_distances_by_gulps.at(gulps) = s;
  }

  std::vector<long> furtherest_by_num_movements = get_furtherest_by_num_movements(movements, D, T);
  if (debug_level >= 2)
  {
    print_vec(furtherest_by_num_movements, std::cerr);
  }

  auto works_with_gulps = [n, D, &furtherest_by_num_movements, &extra_distances_by_gulps](int gulps) {
    long extra_per_step = extra_distances_by_gulps.at(gulps);
    for (int num_movements = 1; num_movements <= n; num_movements++)
    {
      long d = furtherest_by_num_movements.at(num_movements);
      if (d == too_many_movements)
      {
        continue;
      }
      // HACK add in steps to avoid overflow
      for (int j = 0; j < num_movements && d < D; j++)
      {
        d += extra_per_step;
      }
      if (d >= D)
      {
        return true;
      }
    }
    return false;
  };

  long lb = 0, ub = m, ever_worked = works_with_gulps(0);
  while (lb < ub)
  {
    DEBUG(2, "lb " << lb << " ub " << ub);
    int i = (lb + ub) / 2;
    assert(i >= lb && i <= ub);
    if (works_with_gulps(i))
    {
      ub = i;
      ever_worked = true;
    }
    else
    {
      lb = i + 1;
    }
  }
  DEBUG(2, "done at lb " << lb << " ub " << ub);
  if (ever_worked)
  {
    assert(lb == ub);
    std::cout << lb << "\n";
  }
  else
  {
    std::cout << "Panoramix captured\n";
  }
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