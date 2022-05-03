#pragma once
#include <array>
#include <ostream>

class Vec3
{
public:
	double x, y, z;

	Vec3() = default;

	Vec3(double x, double y, double z)
		: x(x),
		  y(y),
		  z(z)
	{
	}

	friend bool operator==(const Vec3& lhs, const Vec3& rhs)
	{
		return fabs(lhs.x - rhs.x) < 0
			&& fabs(lhs.y - rhs.y) < 0
			&& fabs(lhs.z - rhs.z) < 0;
	}

	friend bool operator!=(const Vec3& lhs, const Vec3& rhs)
	{
		return !(lhs == rhs);
	}

	friend Vec3 operator+(const Vec3& lhs, const Vec3& rhs)
	{
		return { lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z };
	}

	friend Vec3 operator-(const Vec3& lhs, const Vec3& rhs)
	{
		return { lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z };
	}

	friend double operator*(const Vec3& lhs, const Vec3& rhs)
	{
		return  lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z; 
	}

	friend Vec3 operator*(const Vec3& lhs, double k)
	{
		return  { lhs.x * k,lhs.y * k, lhs.z * k };
	}

	friend Vec3 operator/(const Vec3& lhs, double k)
	{
		return  {lhs.x / k,lhs.y / k, lhs.z / k};
	}

	void operator+=(const Vec3& rhs)
	{
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
	}

	void operator-=(const Vec3& rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;
	}


	void operator*=(double k)
	{
		x *= k;
		y *= k;
		z *= k;
	}

	void operator/=(double k)
	{
		x /= k;
		y /= k;
		z /= k;
	}



	friend std::ostream& operator<<(std::ostream& os, const Vec3& obj)
	{
		return os
			<< "{ " << obj.x
			<< ", " << obj.y
			<< ", " << obj.z << "}";
	}
};

using Point3d = Vec3;