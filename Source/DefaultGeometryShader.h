#ifndef DEFAULT_GEOMETRY_SHADER_H

#include "Core/Types.h"
#include "Triangle.h"

template <typename Vertex>
struct DefaultGeometryShader
{
    typedef Vertex VertexOut;

    Triangle<VertexOut> operator () (Vertex* v0, Vertex* v1, Vertex* v2, size_t idx)
    {
        Triangle<VertexOut> result = { *v0, *v1, *v2 };
        return result;
    }
};

#define DEFAULT_GEOMETRY_SHADER_H
#endif
