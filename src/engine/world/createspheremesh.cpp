#include <vector>
#include <cmath>

// Generate a UV sphere mesh with given radius, slices (longitude), and stacks (latitude)
// Currently generates only positions and indices (no normals or texture coords yet)
// Future: add normals and UVs for lighting and materials support
void CreateSphereMesh(std::vector<float>& verts, std::vector<unsigned int>& indices, float radius, int slices, int stacks) {
	if (slices <= 0) slices = 32;
	if (stacks <= 0) stacks = 16;
	
	verts.clear();
	indices.clear();

    // Vertices
    for (int stack = 0; stack <= stacks; ++stack) {
        float phi = (float)stack / stacks * 3.14159265f; // 0 to pi

        for (int slice = 0; slice <= slices; ++slice) {
            float theta = (float)slice / slices * 2.0f * 3.14159265f; // 0 to 2pi

            // Spherical to Cartesian
            float x = radius * sinf(phi) * cosf(theta);
            float y = radius * cosf(phi);
            float z = radius * sinf(phi) * sinf(theta);

            // Position only: x, y, z
            verts.push_back(x);
            verts.push_back(y);
            verts.push_back(z);

            // TODO: Add normals here (for lighting)
            // TODO: Add UV coordinates here (for texturing/materials)
        }
    }

    // Indices for triangles
    for (int stack = 0; stack < stacks; ++stack) {
        for (int slice = 0; slice < slices; ++slice) {
            int first = (stack * (slices + 1)) + slice;
            int second = first + slices + 1;

            // Two triangles per quad on sphere surface
            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);

            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }
}