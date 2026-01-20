#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "math.hpp"






class Camera
{
public:
    Camera();
    ~Camera();

    void MoveLeft();
    void MoveRight();
    void MoveForward();
    void MoveBackward();
    
    void UpdateCamera();
    void SetSpeed(float speed) {m_CameraSpeed = speed;};
    inline Vec3 GetCameraUp() const {return m_CameraUp;}
    inline Vec3 GetCameraRight() const {return m_CameraRight;}
    Vec3 GetLocation() const {return m_Location;}
    Vec3 GetView() const {return m_ViewDir;}
private:
    Vec3 m_Location;
    Vec3 m_ViewDir;
    float m_CameraSpeed;
    Vec3 m_CameraUp;
    Vec3 m_CameraRight;


    Vec3 CalculateRight();
    Vec3 CalculateUp();
};

Vec3 WorldToCamera(const Vec3& p, Camera* camera);

Triangle3D Triangle_WorldToCamera(const Triangle3D& tri, Camera* camera);


#endif