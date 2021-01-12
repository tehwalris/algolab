#include <iostream>
#include <cassert>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/max_cardinality_matching.hpp>
#include <string>

const int debug_level = 0;

#define DEBUG(min_level, x)      \
  if (debug_level >= min_level)  \
  {                              \
    std::cerr << x << std::endl; \
  }

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS> Graph;

void testcase()
{
  int w, h;
  std::cin >> w >> h;
  assert(w >= 1 && w <= 50 && h >= 1 && h <= 50);

  std::vector<std::vector<bool>> should_tile_at_cell(h, std::vector<bool>(w, false));
  int num_nodes = w * h;
  int num_nodes_to_cover = 0;
  for (int y = 0; y < h; y++)
  {
    std::string row;
    std::cin >> row;
    assert(int(row.size()) == w);
    for (int x = 0; x < w; x++)
    {
      if (row.at(x) == '.')
      {
        num_nodes_to_cover++;
        should_tile_at_cell.at(y).at(x) = true;
      }
    }
  }

  if (num_nodes_to_cover % 2 != 0)
  {
    DEBUG(2, "fast path");
    std::cout << "no\n";
    return;
  }

  Graph G(num_nodes);
  auto node_at_y_x = [w, h, num_nodes](int y, int x) -> Graph::vertex_descriptor {
    assert(x >= 0 && x < w && y >= 0 && y < h);
    int node = y * w + x;
    assert(node >= 0 && node < num_nodes);
    return node;
  };

  for (int y = 0; y < h; y++)
  {
    for (int x = 0; x < w; x++)
    {
      if (!should_tile_at_cell.at(y).at(x))
      {
        continue;
      }
      if (y > 0 && should_tile_at_cell.at(y - 1).at(x))
      {
        DEBUG(3, "adding vertical edge " << y << " " << x);
        boost::add_edge(node_at_y_x(y - 1, x), node_at_y_x(y, x), G);
      }
      if (x > 0 && should_tile_at_cell.at(y).at(x - 1))
      {
        DEBUG(3, "adding horizontal edge " << y << " " << x);
        boost::add_edge(node_at_y_x(y, x - 1), node_at_y_x(y, x), G);
      }
    }
  }

  std::vector<Graph::vertex_descriptor> mate(num_nodes);
  assert(boost::checked_edmonds_maximum_cardinality_matching(G, mate.data()));
  int num_nodes_covered = 2 * boost::matching_size(G, mate.data());
  DEBUG(2, "num_nodes_covered " << num_nodes_covered);
  assert(num_nodes_covered >= 0 && num_nodes_covered <= num_nodes_to_cover);
  std::cout << (num_nodes_covered == num_nodes_to_cover ? "yes\n" : "no\n");
}

int main()
{
  std::ios_base::sync_with_stdio(false);

  int t;
  std::cin >> t;
  assert(t >= 0 && t <= 100);
  for (int i = 0; i < t; i++)
  {
    testcase();
  }

  return 0;
}