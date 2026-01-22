#include "camera.hpp"
#include "math.hpp"
const Vec3 WorldUp{0.0f, 1.0f, 0.0f};

Camera::Camera() : m_CameraSpeed(0.5f)
{
    m_Location = Vec3{0.0f, 0.0f, 0.0f};
    m_ViewDir = Vec3{0.0f, 0.0f, -1.0f};
}

Camera::~Camera()
{
}

void Camera::MoveLeft()
{
    m_Location -= (CalculateRight()*m_CameraSpeed);
}

void Camera::MoveRight()
{
    m_Location += (CalculateRight()*m_CameraSpeed);
}

void Camera::MoveForward()
{
    PrintVec3(m_Location);
    m_Location += (m_ViewDir*m_CameraSpeed);
}

void Camera::MoveBackward()
{
    PrintVec3(m_Location);
    m_Location -= (m_ViewDir*m_CameraSpeed);
}

void Camera::UpdateCamera()
{
    m_CameraRight = CalculateRight();
    m_CameraUp = CalculateUp();
    // m_ViewDir = CalculateView();
}

Vec3 Camera::CalculateRight()
{
    return NormalizeVec3(CrossProduct(m_ViewDir, WorldUp));
}

Vec3 Camera::CalculateUp()
{
    return NormalizeVec3(CrossProduct(m_ViewDir, CalculateRight()));
}

Vec3 WorldToCamera(const Vec3 &p, Camera *camera)
{
    Vec3 rel = p - camera->GetLocation();

    return {
        DotProduct(rel, camera->GetCameraRight()),     // camera X
        DotProduct(rel, camera->GetCameraUp()),        // camera Y
        DotProduct(rel, camera->GetView())             // camera Z
    };
}

Triangle3D Triangle_WorldToCamera(const Triangle3D &tri, Camera *camera)
{
    Triangle3D triN;
    triN.v0 = WorldToCamera(tri.v0, camera);
    triN.v1 = WorldToCamera(tri.v1, camera);
    triN.v2 = WorldToCamera(tri.v2, camera);
    triN.Normal = NormalizeVec3(CrossProduct(triN.v1 - triN.v0, triN.v2 - triN.v0));
    // NormalizeVec3(WorldToCamera(CrossProduct(tri.v1 - tri.v0, tri.v2 - tri.v0), camera));
    return triN;
}