#include <iostream>
#include <cassert>
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

struct Balloon
{
  K::Point_2 loc;
  double s;
};

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
    for (auto it = triangulation.finite_faces_begin(); it != triangulation.finite_faces_end(); it++)
    {
      it->info() = next_free_face_index++;
    }
    number_of_faces = next_free_face_index;
    launchable_by_face.resize(number_of_faces, false);

    std::vector<int> sorted_balloon_indices(m); // largest radius first
    for (int i = 0; i < m; i++)
    {
      sorted_balloon_indices.at(i) = i;
    }
    std::sort(sorted_balloon_indices.begin(), sorted_balloon_indices.end(), [this](const int ia, const int ib) {
      return balloons.at(ia).s > balloons.at(ib).s;
    });

    std::vector<bool> can_launch_by_balloon(m);
    for (const int i : sorted_balloon_indices)
    {
      can_launch_by_balloon.at(i) = try_launch_balloon(balloons.at(i));
    }

    for (const bool can_launch : can_launch_by_balloon)
    {
      std::cout << (can_launch ? "y" : "n");
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
  std::vector<bool> launchable_by_face;

  bool try_launch_balloon(const Balloon &b)
  {
    const K::FT short_critical_dist = square(K::FT(r) + K::FT(b.s));
    const K::FT critical_dist = 4 * short_critical_dist;

    const Triangulation::Vertex_handle tree = triangulation.nearest_vertex(b.loc);
    const K::FT center_sq_dist = CGAL::squared_distance(tree->point(), b.loc);
    if (center_sq_dist >= critical_dist)
    {
      // balloon can immediately be launched
      return true;
    }
    if (center_sq_dist < short_critical_dist)
    {
      // balloon is immediately punctured
      return false;
    }

    std::vector<int> newly_launchable_face_indices;
    const auto update_launchable = [&newly_launchable_face_indices, this]() {
      for (const int i : newly_launchable_face_indices)
      {
        assert(i >= 0 && i < number_of_faces);
        assert(!launchable_by_face.at(i));
        launchable_by_face.at(i) = true;
      }
    };

    std::vector<bool> ever_queued_faces(number_of_faces, false);
    std::deque<Triangulation::Face_handle> dfs_queue{triangulation.locate(b.loc)};
    if (triangulation.is_infinite(dfs_queue.front()) || launchable_by_face.at(dfs_queue.front()->info()))
    {
      return true;
    }
    ever_queued_faces.at(dfs_queue.front()->info()) = true;
    newly_launchable_face_indices.push_back(dfs_queue.front()->info());
    while (!dfs_queue.empty())
    {
      Triangulation::Face_handle face = dfs_queue.front();
      dfs_queue.pop_front();

      if (triangulation.is_infinite(face))
      {
        // balloon can be moved out of the forest
        update_launchable();
        return true;
      }

      for (int i = 0; i < 3; i++)
      {
        Triangulation::Face_handle next_face = face->neighbor(i);
        const bool is_infinite = triangulation.is_infinite(next_face);
        const int next_face_i = next_face->info(); // only valid if !is_infinite
        if (!is_infinite && ever_queued_faces.at(next_face_i))
        {
          continue;
        }
        if (triangulation.segment(face, i).squared_length() < critical_dist)
        {
          continue;
        }
        if (!is_infinite && launchable_by_face.at(next_face_i))
        {
          // a balloon at least this big was moved through this triangle and eventually launched
          update_launchable();
          return true;
        }
        dfs_queue.push_back(next_face);
        if (!is_infinite)
        {
          ever_queued_faces.at(next_face_i) = true;
          newly_launchable_face_indices.push_back(next_face_i);
        }
      }
    }

    // TODO
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