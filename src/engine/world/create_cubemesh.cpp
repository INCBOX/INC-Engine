#include "world/geometry_loader.h"

void CreateCubeMesh(std::vector<float>& verts, std::vector<unsigned int>& indices, const Vector& size) {
    float sx = size.x * 0.5f, sy = size.y * 0.5f, sz = size.z * 0.5f;

    float cubeVerts[] = {
        -sx, -sy, -sz,  sx, -sy, -sz,  sx,  sy, -sz,  -sx,  sy, -sz,
        -sx, -sy,  sz,  sx, -sy,  sz,  sx,  sy,  sz,  -sx,  sy,  sz
    };

    unsigned int cubeIndices[] = {
        0,1,2, 2,3,0,
        1,5,6, 6,2,1,
        5,4,7, 7,6,5,
        4,0,3, 3,7,4,
        3,2,6, 6,7,3,
        4,5,1, 1,0,4
    };

    verts.assign(cubeVerts, cubeVerts + 8 * 3);
    indices.assign(cubeIndices, cubeIndices + 36);
}
