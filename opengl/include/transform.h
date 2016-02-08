#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#include "datatypes.h"

Matrix44f Rotate(float angleInDegrees, float x, float y, float z);
Matrix44f Scale(float x, float y, float z);
Matrix44f Translate(float x, float y, float z);
Matrix44f Frustum(float left, float right, float bottom, float top, float zNear, float zFar);
Matrix44f Ortho(float left, float right, float bottom, float top, float zNear, float zFar);
Matrix44f Ortho2D(float left, float right, float bottom, float top);
Matrix44f Perspective(float fovY, float aspect, float zNear, float zFar);
Matrix44f LookAt(const Vector3f &eye, const Vector3f &center, const Vector3f &up);

Vector3f Project(Vector3f objPos, const Matrix44f &modelview, const Matrix44f &projection, int viewport[4]);
Vector3f UnProject(Vector3f winPos, const Matrix44f &modelview, const Matrix44f &projection, int viewport[4]);

#endif // _TRANSFORM_H_