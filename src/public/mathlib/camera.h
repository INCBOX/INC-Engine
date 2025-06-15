class Camera {
public:
    Vec3 position;
    Vec3 target;
    Vec3 up;

    Mat4 GetView() const {
        return Mat4::LookAt(position, target, up);
    }
};