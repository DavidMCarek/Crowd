#pragma once

#include "vec3.h"

#define DEGREES_PER_RADIAN 0.01745329251

class mat4
{
public:
	mat4(float a,
		float b, 
		float c, 
		float d, 
		float e, 
		float f, 
		float g, 
		float h, 
		float i, 
		float j, 
		float k, 
		float l, 
		float m, 
		float n, 
		float o, 
		float p);
	~mat4();

	float mat[16];

	
};

mat4 makeCameraMatrix(vec3 camPos, vec3 camTar, vec3 upVec);
mat4 operator*(const mat4 &u, const mat4 &v);
mat4 getRotY(float rotationAngle);
mat4 getTranslation(vec3 translation);
mat4 perspective_projection(float fov, float aspect, float near, float far);
mat4 scale_mat(float scaleFactor);