#include "pointCloud.h"

#include <filesystem>
namespace fs = std::filesystem;

PointCloud::PointCloud()
: pointCloud{}, centerPosition{0,1,0}, mesh{}, tree{}, insideTester{nullptr}
{}

PointCloud::PointCloud(std::string const& filePath, double const resolution)
: pointCloud{}, centerPosition{0,1,0}, mesh{}, tree{}, insideTester{nullptr}
{
    fs::directory_entry file {filePath};
    fs::directory_entry xyzFile {"assets/pointClouds/" + file.path().stem().string() + ".xyz"};

    if (fs::is_regular_file(xyzFile))
    {
        LoadFromFile(xyzFile.path().string());
    }
    else
    {
        LoadMesh(filePath);
        BuildAABBTree();
        Sampling(resolution);

        SaveToFile(xyzFile.path().string());
    }
}

PointCloud::PointCloud(std::string const& filePath)
: pointCloud{}, centerPosition{}, mesh{}, tree{}, insideTester{nullptr}
{
    LoadFromFile(filePath);
}

PointCloud::~PointCloud()
{
    delete insideTester;
}

PointCloud::PointCloud(PointCloud const& other)
: pointCloud{other.pointCloud}, centerPosition{other.centerPosition},
    mesh{}, tree{}, insideTester{nullptr}
{}

PointCloud& PointCloud::operator=(PointCloud const& other)
{
    pointCloud = other.pointCloud;
    centerPosition = other.centerPosition;

    return *this;
}

PointCloud::PointCloud(PointCloud&& other)
: pointCloud{other.pointCloud}, centerPosition{other.centerPosition},
    mesh{}, tree{}, insideTester{nullptr}
{}

PointCloud& PointCloud::operator=(PointCloud&& other)
{
    pointCloud = other.pointCloud;
    centerPosition = other.centerPosition;

    return *this;
}

int PointCloud::LoadMesh(std::string const& filePath)
{
    std::string target{filePath + ".triangulated.obj"};

    obj::Triangulate obj {};

	const auto triangulated = obj.triangulate(filePath, target);

	if(!triangulated)
    {
        std::cerr << "Failed to triangulate mesh: " << filePath << std::endl;
        return 1;
    }

    std::ifstream input{target};
    
    if (!CGAL::IO::read_OBJ(input, mesh)) {
        std::cerr << "❌ Failed to parse mesh from: " << target << std::endl;
        std::cout << "CGAL version: " << CGAL_VERSION_NR << std::endl;
        return 1;
    }

    if (mesh.is_empty()) {
        std::cerr << "❌ Mesh loaded but is empty!" << std::endl;
        return 1;
    }

    return 0;
}

void PointCloud::BuildAABBTree()
{
    tree = Tree{faces(mesh).first, faces(mesh).second, mesh};
    tree.accelerate_distance_queries();
    insideTester = new PointInside{tree};

}

void PointCloud::Sampling(double const resolution)
{
    BoundingBox bbox = tree.bbox();

    double stepSize {std::min({bbox.xmax() - bbox.xmin(), bbox.ymax() - bbox.ymin(), bbox.zmax() - bbox.zmin()})/resolution};

    std::cout << "Stepsize: " << stepSize << std::endl;

    for (double x {bbox.xmin()}; x < bbox.xmax(); x+=stepSize)
    {
        for (double y {bbox.ymin()}; y < bbox.ymax(); y+=stepSize)
        {
            for (double z {bbox.zmin()}; z < bbox.zmax(); z+=stepSize)
            {
                Point p(x, y, z);
                if ((*insideTester)(p) == CGAL::ON_BOUNDED_SIDE) {
                    pointCloud.push_back(vec4{p.x(), p.y(), p.z(), 0});
                }
            }
        }
    }
}

void PointCloud::SaveToFile(std::string const& filePath)
{
    std::ofstream out {filePath};
    for (unsigned int i {}; i < pointCloud.size(); i++) 
    {
        out << pointCloud[i].x << " " << pointCloud[i].y << " " << pointCloud[i].z << "\n";
    }
}

void PointCloud::LoadFromFile(std::string const& filePath)
{
    std::ifstream ifs {filePath};
    std::string line {};

    while (std::getline(ifs, line))
    {
        std::istringstream iss {line};

        vec4 vertex {};

        iss >> vertex.x >> vertex.y >> vertex.z;
        pointCloud.push_back(vertex);
    }
}
