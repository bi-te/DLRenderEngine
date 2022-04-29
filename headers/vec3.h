#pragma once
#include <array>
#include <ostream>

class vec3
{
private:
	int32_t x, y, z;

public:
	vec3(int32_t x, int32_t y, int32_t z)
		: x(x),
		  y(y),
		  z(z)
	{
	}

	int32_t length_squared()
	{
		return x * x + y * y + z * z;
	}

	int32_t length()
	{
		return sqrt(length_squared());
	}

	friend bool operator==(const vec3& lhs, const vec3& rhs)
	{
		return lhs.x == rhs.x
			&& lhs.y == rhs.y
			&& lhs.z == rhs.z;
	}

	friend bool operator!=(const vec3& lhs, const vec3& rhs)
	{
		return !(lhs == rhs);
	}

	friend vec3 operator+(const vec3& lhs, const vec3& rhs)
	{
		return { lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z };
	}

	friend vec3 operator-(const vec3& lhs, const vec3& rhs)
	{
		return { lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z };
	}

	friend int32_t operator*(const vec3& lhs, const vec3& rhs)
	{
		return  lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z; 
	}

	friend vec3 operator*(const vec3& lhs, int32_t k)
	{
		return  { lhs.x * k,lhs.y * k, lhs.z * k };
	}

	friend vec3 operator/(const vec3& lhs, int32_t k)
	{
		return  {lhs.x / k,lhs.y / k, lhs.z / k};
	}

	friend std::ostream& operator<<(std::ostream& os, const vec3& obj)
	{
		return os
			<< "{ " << obj.x
			<< ", " << obj.y
			<< ", " << obj.z << "}";
	}
};

using point3d = vec3;