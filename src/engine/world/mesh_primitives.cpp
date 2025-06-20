#include "world/mesh_primitives.h"
#include "mathlib/math_constants.h"
#include <cmath>

namespace geometry {

void CreateCubeMesh(std::vector<float>& verts, std::vector<unsigned int>& indices, const Vector& size) {
    float sx = size.x * 0.5f, sy = size.y * 0.5f, sz = size.z * 0.5f;

    constexpr int VERT_COUNT = 8;
    constexpr int INDEX_COUNT = 36;

    float cubeVerts[] = {
        -sx, -sy, -sz,  sx, -sy, -sz,  sx,  sy, -sz,  -sx,  sy, -sz,
        -sx, -sy,  sz,  sx, -sy,  sz,  sx,  sy,  sz,  -sx,  sy,  sz
    };

    unsigned int cubeIndices[] = {
        0,1,2, 2,3,0,      // front
        1,5,6, 6,2,1,      // right
        5,4,7, 7,6,5,      // back
        4,0,3, 3,7,4,      // left
        3,2,6, 6,7,3,      // top
        4,5,1, 1,0,4       // bottom
    };

    verts.assign(cubeVerts, cubeVerts + VERT_COUNT * 3);
    indices.assign(cubeIndices, cubeIndices + INDEX_COUNT);
}

void CreatePlaneMesh(std::vector<float>& verts, std::vector<unsigned int>& indices, const Vector& size) {
    float sx = size.x * 0.5f, sz = size.z * 0.5f;

    constexpr int VERT_COUNT = 4;
    constexpr int INDEX_COUNT = 6;

    float planeVerts[] = {
        -sx, 0, -sz,  sx, 0, -sz,  sx, 0, sz,  -sx, 0, sz
    };

    unsigned int planeIndices[] = {
        0,1,2, 2,3,0
    };

    verts.assign(planeVerts, planeVerts + VERT_COUNT * 3);
    indices.assign(planeIndices, planeIndices + INDEX_COUNT);
}

void CreateSphereMesh(std::vector<float>& verts, std::vector<unsigned int>& indices, float radius, int slices, int stacks) {
    verts.clear();
    indices.clear();

    for (int stack = 0; stack <= stacks; ++stack) {
        float phi = (float)stack / stacks * math::PI;
        float y = radius * cosf(phi);
        float r = radius * sinf(phi);

        for (int slice = 0; slice <= slices; ++slice) {
            float theta = (float)slice / slices * 2.0f * math::PI;
            float x = r * cosf(theta);
            float z = r * sinf(theta);
            verts.push_back(x);
            verts.push_back(y);
            verts.push_back(z);
        }
    }

    for (int stack = 0; stack < stacks; ++stack) {
        for (int slice = 0; slice < slices; ++slice) {
            int first = stack * (slices + 1) + slice;
            int second = first + slices + 1;

            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);

            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }
}

} // namespace geometry