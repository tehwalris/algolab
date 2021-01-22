#include <iostream>
#include <cassert>
#include <queue>
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Triangulation_vertex_base_2.h>
#include <CGAL/Triangulation_face_base_with_info_2.h>
#include <CGAL/Triangulation_data_structure_2.h>
#include <CGAL/Delaunay_triangulation_2.h>

typedef CGAL::Exact_predicates_exact_constructions_kernel K;
typedef CGAL::Triangulation_vertex_base_2<K> Vb;
typedef CGAL::Triangulation_face_base_with_info_2<int, K> Fb;
typedef CGAL::Triangulation_data_structure_2<Vb, Fb> Tds;
typedef CGAL::Delaunay_triangulation_2<K, Tds> Triangulation;

const int debug_level = 0;

#define DEBUG(min_level, x)      \
  if (debug_level >= min_level)  \
  {                              \
    std::cerr << x << std::endl; \
  }

K::FT square(const K::FT &ft)
{
  return ft * ft;
}

const K::FT inf_width = 16 * square(K::FT(1L << 50));

struct Balloon
{
  K::Point_2 loc;
  double s;
};

class QueueElement
{
public:
  QueueElement(const K::FT &width, const Triangulation::Face_handle &face) : width(width), face(face){};

  K::FT width;
  Triangulation::Face_handle face;
};

// sort by descending width
bool operator<(const QueueElement &a, const QueueElement &b)
{
  // IMPORTANT This function must also order by face, otherwise set lookups will not work correctly
  return a.width > b.width || (a.width == b.width && a.face->info() < b.face->info());
}

class Testcase
{
public:
  void
  run()
  {
    std::cin >> n >> m >> r;
    assert(n >= 1 && n <= 4e4 && m >= 1 && m <= 9e4 && r >= 1 && r < (1L << 50));

    tree_locations.reserve(n);
    for (int i = 0; i < n; i++)
    {
      double x, y;
      std::cin >> x >> y;
      assert(abs(x) < (1L << 50) && abs(y) < (1L << 50));
      tree_locations.emplace_back(x, y);
    }

    balloons.resize(m);
    for (Balloon &b : balloons)
    {
      double x, y;
      std::cin >> x >> y >> b.s;
      assert(abs(x) < (1L << 50) && abs(y) < (1L << 50));
      assert(b.s >= 0 && b.s < (1L << 50));
      b.loc = K::Point_2(x, y);
    }

    triangulation.insert(tree_locations.begin(), tree_locations.end());
    int next_free_face_index = 0;
    for (auto it = triangulation.all_faces_begin(); it != triangulation.all_faces_end(); it++)
    {
      it->info() = next_free_face_index++;
    }
    number_of_faces = next_free_face_index;

    calculate_square_exit_diameters();

    for (const Balloon &b : balloons)
    {
      std::cout << (can_launch_balloon(b) ? "y" : "n");
    }

    std::cout << "\n";
  }

private:
  int n, m;
  double r;
  std::vector<K::Point_2> tree_locations;
  std::vector<Balloon> balloons;
  Triangulation triangulation;
  int number_of_faces;
  std::vector<K::FT> square_exit_diameter_by_face;

  void calculate_square_exit_diameters()
  {
    // https://en.wikipedia.org/wiki/Widest_path_problem
    // More or less Dijkstra from some infinite face, but with min width along the path instead of distance

    std::set<QueueElement> queue{QueueElement(inf_width, triangulation.infinite_face())};
    std::vector<bool> finished_by_face(number_of_faces, false);
    square_exit_diameter_by_face.resize(number_of_faces, 0);
    square_exit_diameter_by_face.at(queue.begin()->face->info()) = queue.begin()->width;
    while (!queue.empty())
    {
      const K::FT prev_width = queue.begin()->width;
      const Triangulation::Face_handle prev_face = queue.begin()->face;
      queue.erase(queue.begin());
      assert(!finished_by_face.at(prev_face->info()));
      finished_by_face.at(prev_face->info()) = true;

      DEBUG(3, "current face " << prev_face->info() << " " << prev_width);

      for (int i = 0; i < 3; i++)
      {
        const Triangulation::Face_handle next_face = prev_face->neighbor(i);
        DEBUG(3, "next_face->info() " << next_face->info());
        if (finished_by_face.at(next_face->info()))
        {
          continue;
        }

        const K::FT new_width = std::min(
            prev_width,
            (triangulation.is_infinite(prev_face) && triangulation.is_infinite(next_face))
                ? inf_width
                : triangulation.segment(prev_face, i).squared_length());
        K::FT &saved_width = square_exit_diameter_by_face.at(next_face->info());
        if (new_width <= saved_width)
        {
          continue;
        }

        DEBUG(3, "improve face " << next_face->info() << " " << saved_width << " " << new_width);

        const auto it_to_remove = queue.find(QueueElement(saved_width, next_face));
        if (it_to_remove != queue.end())
        {
          queue.erase(it_to_remove);
        }
        queue.insert(QueueElement(new_width, next_face));
        saved_width = new_width;
      }
    }

    assert(*std::min_element(finished_by_face.begin(), finished_by_face.end()));
  }

  bool can_launch_balloon(const Balloon &b)
  {
    const K::FT short_critical_dist = square(K::FT(r) + K::FT(b.s));
    const K::FT long_critical_dist = 4 * short_critical_dist;

    const Triangulation::Vertex_handle tree = triangulation.nearest_vertex(b.loc);
    const K::FT center_sq_dist = CGAL::squared_distance(tree->point(), b.loc);
    if (center_sq_dist >= long_critical_dist)
    {
      // balloon can immediately be launched
      return true;
    }
    if (center_sq_dist < short_critical_dist)
    {
      // balloon is immediately punctured
      return false;
    }

    const Triangulation::Face_handle face = triangulation.locate(b.loc);
    DEBUG(3, "face " << face->info());
    const K::FT bottleneck = square_exit_diameter_by_face.at(face->info());
    DEBUG(3, "bottleneck " << bottleneck << " long_critical_dist " << long_critical_dist);
    if (bottleneck >= long_critical_dist)
    {
      return true;
    }

    return false;
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