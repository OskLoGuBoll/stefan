#ifndef POINTCLOUD_H
#define POINTCLOUD_H

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/Side_of_triangle_mesh.h>
#include <CGAL/AABB_tree.h>
#include <CGAL/AABB_face_graph_triangle_primitive.h>
#include <CGAL/IO/OBJ.h>

#include <vector>
#include <fstream>
#include <random>
#include <string>
#include <iostream>

#include "VectorUtils4.h"
#include "assetManager.h"
#include "TriangulateOBJ.h"


template <size_t N>
class PointCloud
{
public:
    typedef CGAL::Simple_cartesian<double>                  Kernel;
    typedef Kernel::Point_3                                 Point;
    typedef CGAL::Surface_mesh<Point>                       Mesh;
    typedef CGAL::AABB_face_graph_triangle_primitive<Mesh>  Primitive;
    typedef CGAL::AABB_traits<Kernel, Primitive>            AABBTraits;
    typedef CGAL::AABB_tree<AABBTraits>                     Tree;
    typedef CGAL::Side_of_triangle_mesh<Mesh, Kernel>       PointInside;
    typedef CGAL::Bbox_3                                    BoundingBox;

    PointCloud(std::string const& filePath);

    ~PointCloud() = default;
    PointCloud(const PointCloud& other) = default;
    PointCloud& operator=(const PointCloud& other) = default;
    PointCloud(PointCloud&& other) = default;
    PointCloud& operator=(PointCloud&& other) = default;
    void SaveToFile(std::string const&);

private:
    int LoadMesh(std::string const&);
    void BuildAABBTree();
    void RandomSampling();

    std::array<vec3, N> pointCloud;
    std::vector<float> radii;
    std::vector<vec4> colors;

    Mesh mesh;
    Tree tree;
    PointInside* insideTester = nullptr;

};

#include "pointCloud.tcc"
#endif
