#include "world/geometry_loader.h"

void CreatePlaneMesh(std::vector<float>& verts, std::vector<unsigned int>& indices, const Vector& size) {
    float sx = size.x * 0.5f, sz = size.z * 0.5f;

    float planeVerts[] = {
        -sx, 0, -sz,  sx, 0, -sz,  sx, 0, sz,  -sx, 0, sz
    };

    unsigned int planeIndices[] = {
        0,1,2, 2,3,0
    };

    verts.assign(planeVerts, planeVerts + 12);
    indices.assign(planeIndices, planeIndices + 6);
}
