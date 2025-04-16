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
#include <string>
#include <iostream>

#include "VectorUtils4.h"
#include "TriangulateOBJ.h"

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

    PointCloud();
    PointCloud(std::string const&, double const);
    PointCloud(std::string const&);

    ~PointCloud();
    PointCloud(PointCloud const&);
    PointCloud& operator=(PointCloud const&);
    PointCloud(PointCloud&&);
    PointCloud& operator=(PointCloud&&);

    void SaveToFile(std::string const&);

private:
    void LoadFromFile(std::string const&);

    int LoadMesh(std::string const&);
    void BuildAABBTree();
    void Sampling(double const);

protected:
    std::vector<vec4> pointCloud;

    vec3 centerPosition;

    Mesh mesh;
    Tree tree;
    PointInside* insideTester;

};

#endif
