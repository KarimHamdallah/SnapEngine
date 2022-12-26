#pragma once
#include <stdint.h>

class vec2
{
public:
	vec2() : x(0.0f), y(0.0f) {}
	vec2(float x_val, float y_val) : x(x_val), y(y_val) {}
	vec2(float val) : x(val), y(val) {}

private:
	union
	{
		float elements[2];
		struct { float x, y; };
	};


public:
	inline float operator[](uint32_t index) { return elements[index]; }

	inline void operator += (const vec2& other) { x += other.x; y += other.y; }
	inline void operator *= (const vec2& other) { x *= other.x; y *= other.y; }
	inline void operator /= (const vec2& other) { x /= other.x; y /= other.y; }


	inline void operator += (float other) { x += other; y += other; }
	inline void operator *= (float other) { x *= other; y *= other; }
	inline void operator /= (float other) { x /= other; y /= other; }


	inline vec2& operator + (const vec2& other) { return vec2(x + other.x, y + other.y); }
	inline vec2& operator - (const vec2& other) { return vec2(x - other.x, y - other.y); }
	inline vec2& operator * (const vec2& other) { return vec2(x * other.x, y * other.y); }
	inline vec2& operator / (const vec2& other) { return vec2(x / other.x, y / other.y); }

	inline vec2& operator + (float other) { return vec2(x + other, y + other); }
	inline vec2& operator - (float other) { return vec2(x - other, y - other); }
	inline vec2& operator * (float other) { return vec2(x * other, y * other); }
	inline vec2& operator / (float other) { return vec2(x / other, y / other); }
};

class vec3
{
public:
	vec3() : x(0.0f), y(0.0f), z(0.0f) {}
	vec3(float x_val, float y_val, float z_val) : x(x_val), y(y_val), z(z_val) {}
	vec3(float val) : x(val), y(val), z(val) {}

private:
	union
	{
		float elements[3];
		struct { float x, y, z; };
		struct { float r, g, b; };
	};

public:
	inline float operator[](uint32_t index) { return elements[index]; }

	inline void operator += (const vec3& other) { x += other.x; y += other.y; z += other.z; }
	inline void operator *= (const vec3& other) { x *= other.x; y *= other.y; z *= other.z; }
	inline void operator /= (const vec3& other) { x /= other.x; y /= other.y; z /= other.z; }


	inline void operator += (float other) { x += other; y += other; z += other; }
	inline void operator *= (float other) { x *= other; y *= other; z *= other; }
	inline void operator /= (float other) { x /= other; y /= other; z /= other; }


	inline vec3& operator + (const vec3& other) { return vec3(x + other.x, y + other.y, z + other.z); }
	inline vec3& operator - (const vec3& other) { return vec3(x - other.x, y - other.y, z - other.z); }
	inline vec3& operator * (const vec3& other) { return vec3(x * other.x, y * other.y, z * other.z); }
	inline vec3& operator / (const vec3& other) { return vec3(x / other.x, y / other.y, z / other.z); }

	inline vec3& operator + (float other) { return vec3(x + other, y + other, z + other); }
	inline vec3& operator - (float other) { return vec3(x - other, y - other, z - other); }
	inline vec3& operator * (float other) { return vec3(x * other, y * other, z * other); }
	inline vec3& operator / (float other) { return vec3(x / other, y / other, z / other); }
};