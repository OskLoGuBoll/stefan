
template <size_t N>
PointCloud<N>::PointCloud(std::string const& filePath)
: mesh{}, tree{}, insideTester{nullptr}
{
    LoadMesh(filePath);
    BuildAABBTree();
    RandomSampling();
}

template <size_t N>
int PointCloud<N>::LoadMesh(std::string const& filePath)
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

template <size_t N>
void PointCloud<N>::BuildAABBTree()
{
    tree = Tree{faces(mesh).first, faces(mesh).second, mesh};
    tree.accelerate_distance_queries();
    insideTester = new PointInside{tree};

}

template <size_t N>
void PointCloud<N>::RandomSampling()
{
    BoundingBox bbox = tree.bbox();
    std::mt19937 rng;
    std::uniform_real_distribution<double> x_dist(bbox.xmin(), bbox.xmax());
    std::uniform_real_distribution<double> y_dist(bbox.ymin(), bbox.ymax());
    std::uniform_real_distribution<double> z_dist(bbox.zmin(), bbox.zmax());

    int j{};
    for (int i = 0; i < N; ++i) {
        Point p(x_dist(rng), y_dist(rng), z_dist(rng));
        if ((*insideTester)(p) == CGAL::ON_BOUNDED_SIDE) {
            pointCloud[j] = vec3{p.x(), p.y(), p.z()};
            j++;
        }
    }
}

template <size_t N>
void PointCloud<N>::SaveToFile(std::string const& filePath)
{
    std::ofstream out {filePath};
    for (auto const& p : pointCloud) 
    {
        out << p.x << " " << p.y << " " << p.z << "\n";
    }
}
