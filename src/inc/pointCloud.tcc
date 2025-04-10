
template <size_t N>
PointCloud<N>::PointCloud(std::string filePath)
: mesh{}, tree{}, insideTester{nullptr}
{
    std::cout<<"00"<<std::endl;

    LoadMesh("assets/models/groundspherecopy.obj");
    BuildAABBTree();
    RandomSampling();
}

template <size_t N>
int PointCloud<N>::LoadMesh(std::string filePath)
{
    std::ifstream input(filePath);
    if (!CGAL::IO::read_OBJ(input, mesh)) {
        std::cerr << "❌ Failed to parse mesh from: " << filePath << std::endl;
        std::cout << "CGAL version: " << CGAL_VERSION_NR << std::endl;
    }

    for (auto v : mesh.vertices()) {
        auto pt = mesh.point(v);
        std::cout << "Vertex: " << pt << std::endl;
    }
    
    for (auto f : mesh.faces()) {
        std::cout << "Face: ";
        for (auto v : vertices_around_face(mesh.halfedge(f), mesh)) {
            std::cout << v << " ";
        }
        std::cout << std::endl;
    }
    

    if (mesh.is_empty()) {
        std::cerr << "❌ Mesh loaded but is empty!" << std::endl;
    }
    
    std::cout << "✅ Mesh loaded from " << filePath << ", #vertices = "
              << mesh.number_of_vertices() << ", #faces = "
              << mesh.number_of_faces() << std::endl;


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
            pointCloud[j]=p;
            j++;
        }
    }
}

template <size_t N>
void PointCloud<N>::SaveToFile(std::string filePath)
{
    std::ofstream out(filePath);
    for (const Point& p : pointCloud) 
    {
        out << p.x() << " " << p.y() << " " << p.z() << "\n";
    }
}