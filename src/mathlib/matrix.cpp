Mat4 Mat4::Translation(const Vec3& t) {
    Mat4 out = Mat4::Identity();
    out[3][0] = t.x;
    out[3][1] = t.y;
    out[3][2] = t.z;
    return out;
}