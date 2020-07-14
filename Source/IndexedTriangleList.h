#ifndef INDEXED_TRIANGLE_LIST

#include <vector>

template <typename Vertex>
struct IndexedTriangleList
{
    std::vector<Vertex> vertices;
    std::vector<size_t> indices;
};

#define INDEXED_TRIANGLE_LIST
#endif
