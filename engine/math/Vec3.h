#pragma once

#include <ostream>

class Vec3f
{
public:
	float x, y, z;

	Vec3f() = default;

	//Vec3(const vec4& vec): x(vec.x()), y(vec.y()), z(vec.z())
	//{		
	//}

	Vec3f(float x, float y, float z)
		: x(x),
		  y(y),
		  z(z)
	{
	}

	Vec3f operator-() const
	{
		return  { -x, -y, -z };
	}

	friend bool operator==(const Vec3f& lhs, const Vec3f& rhs)
	{
		return fabs(lhs.x - rhs.x) < 0
			&& fabs(lhs.y - rhs.y) < 0
			&& fabs(lhs.z - rhs.z) < 0;
	}

	friend bool operator!=(const Vec3f& lhs, const Vec3f& rhs)
	{
		return !(lhs == rhs);
	}

	friend Vec3f operator+(const Vec3f& lhs, const Vec3f& rhs)
	{
		return { lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z };
	}

	friend Vec3f operator-(const Vec3f& lhs, const Vec3f& rhs)
	{
		return { lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z };
	}

	friend Vec3f operator*(const Vec3f& lhs, const Vec3f& rhs)
	{
		return  { lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z };
	}

	friend Vec3f operator*(const Vec3f& lhs, float k)
	{
		return  { lhs.x * k,lhs.y * k, lhs.z * k };
	}

	friend Vec3f operator*(float k, const Vec3f& lhs)
	{
		return  { lhs.x * k,lhs.y * k, lhs.z * k };
	}

	friend Vec3f operator/(const Vec3f& lhs, float k)
	{
		return  {lhs.x / k,lhs.y / k, lhs.z / k};
	}

	void operator+=(const Vec3f& rhs)
	{
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
	}

	void operator-=(const Vec3f& rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;
	}


	void operator*=(float k)
	{
		x *= k;
		y *= k;
		z *= k;
	}

	void operator/=(float k)
	{
		x /= k;
		y /= k;
		z /= k;
	}



	friend std::ostream& operator<<(std::ostream& os, const Vec3f& obj)
	{
		return os
			<< "{ " << obj.x
			<< ", " << obj.y
			<< ", " << obj.z << "}";
	}
};

using Point3d = Vec3f;

inline float dot(const Vec3f& rv, const Vec3f& lv)
{
	return rv.x * lv.x + rv.y * lv.y + rv.z * lv.z;
}

inline float length_squared(const Vec3f& vec)
{
	return vec.x * vec.x + vec.y * vec.y + vec.z * vec.z;
}

inline float length(const Vec3f& vec)
{
	return sqrt(length_squared(vec));
}

inline Vec3f normalize(const Vec3f& vec)
{
	Vec3f norm{};
	float l = length(vec);
	norm.x = vec.x / l;
	norm.y = vec.y / l;
	norm.z = vec.z / l;

	return norm;
}