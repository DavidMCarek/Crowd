#pragma once
class vec3
{
public:
	vec3(float x, float y, float z);
	~vec3();
	float x;
	float y;
	float z;

	vec3 cross(const vec3 &v);
	vec3 normalize();
	float length();
};

vec3 operator-(const vec3 &u, const vec3 &v);