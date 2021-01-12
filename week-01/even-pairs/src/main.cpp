#include <iostream>
#include <assert.h>

void testcase()
{
  int n;
  std::cin >> n;
  int parity = 0, even = 1, odd = 0;
  for (int i = 0; i < n; i++)
  {
    int v;
    std::cin >> v;
    assert(v == 0 || v == 1);
    parity = (parity + v) % 2;
    if (parity == 0)
    {
      even++;
    }
    else
    {
      odd++;
    }
  }
  std::cout << (even * (even - 1)) / 2 + (odd * (odd - 1)) / 2 << '\n';
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