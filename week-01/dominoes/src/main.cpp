#include <iostream>

void testcase()
{
  int n;
  std::cin >> n;
  int nextToppled = 0, numToppled = 0;
  for (int i = 0; i < n; i++)
  {
    int h;
    std::cin >> h;
    if (i <= nextToppled)
    {
      int newNextToppled = i + h - 1;
      if (newNextToppled > nextToppled)
      {
        nextToppled = newNextToppled;
      }
      numToppled++;
    }
  }
  std::cout << numToppled << '\n';
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