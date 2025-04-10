#include "pointCloud.h"

PointCloud::PointCloud()
: pointCloud{}, radii{}, colors{}, centerPosition{}, mesh{}, tree{}, insideTester{nullptr}
{}

PointCloud::PointCloud(std::string const& filePath, double const resolution)
: pointCloud{}, radii{}, colors{}, centerPosition{}, mesh{}, tree{}, insideTester{nullptr}
{
    LoadMesh(filePath);
    BuildAABBTree();
    Sampling(resolution);
    SaveToFile(filePath);
}

PointCloud::PointCloud(std::string const& filePath)
: pointCloud{}, radii{}, colors{}, centerPosition{}, mesh{}, tree{}, insideTester{nullptr}
{
    std::ifstream ifs {filePath};
    std::string line {};

    while (std::getline(ifs, line))
    {
        std::istringstream iss {line};

        vec3 vertex {};

        iss >> vertex.x >> vertex.y >> vertex.z;

        pointCloud.push_back(vertex);
        colors.push_back(vec4{0,0.3,1,1});
        radii.push_back(100);
    }
}

PointCloud::~PointCloud()
{
    delete insideTester;
}

PointCloud::PointCloud(PointCloud const& other)
: pointCloud{other.pointCloud}, radii{other.radii}, colors{other.colors},
    centerPosition{other.centerPosition}, mesh{}, tree{}, insideTester{nullptr}
{}

PointCloud& PointCloud::operator=(PointCloud const& other)
{
    pointCloud = other.pointCloud;
    radii = other.radii;
    colors = other.colors;
    centerPosition = other.centerPosition;

    return *this;
}

PointCloud::PointCloud(PointCloud&& other)
: pointCloud{other.pointCloud}, radii{other.radii}, colors{other.colors},
    centerPosition{other.centerPosition}, mesh{}, tree{}, insideTester{nullptr}
{}

PointCloud& PointCloud::operator=(PointCloud&& other)
{
    pointCloud = other.pointCloud;
    radii = other.radii;
    colors = other.colors;
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
                    pointCloud.push_back(vec3{p.x(), p.y(), p.z()});
                    colors.push_back(vec4{0,0.3,1,1});
                    radii.push_back(100);
                }
            }
        }
    }
}

void PointCloud::SaveToFile(std::string const& filePath)
{
    std::ofstream out {filePath + ".xyz"};
    for (auto const& p : pointCloud) 
    {
        out << p.x << " " << p.y << " " << p.z << "\n";
    }
}
