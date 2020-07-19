#ifndef DEFAULT_VERTEX_SHADER_H

#include <glm/glm.hpp>

template <typename Vertex>
struct DefaultVertexShader
{
    typedef Vertex VertexOut;

    void BindModelMatrix(const glm::mat4& m)
    {
        model = m;
    }

    VertexOut operator () (const Vertex& v) const
    {
        VertexOut result;
        result.position = glm::vec3(model * glm::vec4(v.position, 1.f));
        result.CopyAttributesFrom(v);
        return result;
    }

    glm::mat4 model;
};

#define DEFAULT_VERTEX_SHADER_H
#endif
