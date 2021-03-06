
#ifndef __CAMERA_H__
#define __CAMERA_H__


#include "Vector.h"
namespace lux{
class FrustumGrid;
class Camera
{

  public:

    friend class FrustumGrid;

    Camera();
   ~Camera(){}

    void setEyeViewUp( const Vector& eye, const Vector& view, const Vector& up );
    const Vector& eye() const  { return position; }
    const Vector& view() const { return axis_view; }
    const Vector& up() const   { return axis_up; }

    // view direction of a pixel at the fractional position x,y.
    // Nominally 0 <= x <= 1 and 0 <= y <= 1 for the primary fov,
    // but the values can extend beyond that
    const Vector view( const double x, const double y ) const;

    void setFov( const double fov );
    const double& fov() const { return FOV; }

    void setAspectRatio( const double ar );
    const double& aspectRatio() const { return aspect_ratio; }

    void setNearPlane( const double n ){ near = n; }
    const double& nearPlane() const { return near; }

    void setFarPlane( const double n ){ far = n; }
    const double& farPlane() const { return far; }

  private:
 
    double FOV, aspect_ratio;
    double htanfov, vtanfov;
    double near, far;

    Vector position;
    Vector axis_right, axis_up, axis_view;



};
}

#endif


