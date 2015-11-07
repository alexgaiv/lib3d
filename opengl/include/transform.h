#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#include "datatypes.h"

Matrix44f Rotate(float angleInDegrees, float x, float y, float z);
Matrix44f Scale(float x, float y, float z);
Matrix44f Translate(float x, float y, float z);
Matrix44f Frustum(float left, float right, float bottom,
	float top, float zNear, float zFar);
Matrix44f Ortho(float left, float right, float bottom,
	float top, float zNear, float zFar);
Matrix44f Perspective(float fovY, float aspect, float zNear, float zFar);

#endif // _TRANSFORM_H_