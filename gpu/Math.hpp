#pragma once

#include <string.h> //memset

namespace gapi
{
	const int g_numSamples = 1;
	struct Pixel
	{
		Pixel(float r = 0.0f, float g = 0.0f, float b = 0.0f)
			: r(r)
			, g(g)
			, b(b)
		{
		}

		float r;
		float g;
		float b;

		
	};

	struct FB
	{
		Pixel finalColor;
		Pixel samples[g_numSamples];
		float samplesZ[g_numSamples];
	};

	struct Point3
	{
		Point3(float x = 0.0f, float y = 0.0f, float z = 0.0f)
			: x(x)
			, y(y)
			, z(z)
		{
		}

		Point3(const Point3& p)
			: x(p.x)
			, y(p.y)
			, z(p.z)
		{
		}

		Point3& operator=(const Point3& rhs)
		{
			x = rhs.x;
			y = rhs.y;
			z = rhs.z;
			return *this;
		}

		Point3& operator/=(float n)
		{
			x /= n;
			y /= n;
			z /= n;
			return *this;
		}

		Point3& operator*=(float n)
		{
			x *= n;
			y *= n;
			z *= n;
			return *this;
		}

		Point3 operator+(const Point3& rhs)
		{
			Point3 p(*this);
			p.x += rhs.x;
			p.y += rhs.y;
			p.z += rhs.z;
			return p;
		}

		Point3 operator-(const Point3& rhs)
		{
			Point3 p(*this);
			p.x -= rhs.x;
			p.y -= rhs.y;
			p.z -= rhs.z;
			return p;
		}

		Point3 operator*(float n)
		{
			Point3 p(*this);
			p.x *= n;
			p.y *= n;
			p.z *= n;
			return p;
		}

		Point3& operator+=(float n)
		{
			x += n;
			y += n;
			z += n;
			return *this;
		}
		float x;
		float y;
		float z;
	};

	struct Point4
	{
		Point4(float x = 0.0f, float y = 0.0f, float z = 0.0f, float w = 1.0f)
			: x(x)
			, y(y)
			, z(z)
			, w(w)
		{
		}

		Point4(const Point4& p)
			: x(p.x)
			, y(p.y)
			, z(p.z)
			, w(p.w)
		{
		}

		Point4& operator=(const Point4& rhs)
		{
			x = rhs.x;
			y = rhs.y;
			z = rhs.z;
			w = rhs.w;
			return *this;
		}

		Point4& operator=(const Point3& rhs)
		{
			x = rhs.x;
			y = rhs.y;
			z = rhs.z;
			w = 1.0f;
			return *this;
		}

		Point4& operator/=(float n)
		{
			x /= n;
			y /= n;
			z /= n;
			w /= n;
			return *this;
		}
		Point4 operator/(float n)
		{
			Point4 p(*this);
			p.x /= n;
			p.y /= n;
			p.z /= n;
			p.w /= n;
			return p;
		}

		Point4& operator*=(float n)
		{
			x *= n;
			y *= n;
			z *= n;
			w *= n;
			return *this;
		}

		Point4 operator+(const Point4& rhs)
		{
			Point4 p(*this);
			p.x += rhs.x;
			p.y += rhs.y;
			p.z += rhs.z;
			p.w += rhs.w;
			return p;
		}

		Point4 operator-(const Point4& rhs)
		{
			Point4 p(*this);
			p.x -= rhs.x;
			p.y -= rhs.y;
			p.z -= rhs.z;
			p.w -= rhs.w;
			return p;
		}

		Point4 operator*(float n)
		{
			Point4 p(*this);
			p.x *= n;
			p.y *= n;
			p.z *= n;
			p.w *= n;
			return p;
		}

		Point4& operator+=(float n)
		{
			x += n;
			y += n;
			z += n;
			w += n;
			return *this;
		}

		float& operator[](int i)
		{
			switch (i)
			{
			case 0:
				return x;
				break;
			case 1:
				return y;
				break;
			case 2:
				return z;
				break;
			case 3:
				return w;
				break;
			default:
				return x;
				break;
			}
		}

		float x;
		float y;
		float z;
		float w;
	};


	struct Point2
	{
		Point2(float x = 0.0f, float y = 0.0f)
			: x(x)
			, y(y)
		{
		}

		Point2(const Point2& p)
			: x(p.x)
			, y(p.y)
		{
		}

		Point2& operator=(const Point2& rhs)
		{
			x = rhs.x;
			y = rhs.y;
			return *this;
		}

		Point2& operator/=(float n)
		{
			x /= n;
			y /= n;
			return *this;
		}

		Point2& operator*=(float n)
		{
			x *= n;
			y *= n;
			return *this;
		}

		Point2 operator+(const Point2& rhs)
		{
			Point2 p(*this);
			p.x += rhs.x;
			p.y += rhs.y;

			return p;
		}

		Point2 operator-(const Point2& rhs)
		{
			Point2 p(*this);
			p.x -= rhs.x;
			p.y -= rhs.y;

			return p;
		}

		Point2 operator*(float n)
		{
			Point2 p(*this);
			p.x *= n;
			p.y *= n;
			return p;
		}

		Point2& operator+=(float n)
		{
			x += n;
			y += n;
			return *this;
		}

		float x;
		float y;
	};
	struct P
	{
		P(int x = 0, int y = 0)
			:x(x)
			, y(y)
			, needShade(false)
			, numSamplesCovered(0)
		{
			memset(samplesCovered, false, sizeof(bool) * g_numSamples);
		}
		P operator-(const P& rhs)
		{
			P p(x, y);
			p.x -= rhs.x;
			p.y -= rhs.y;

			return p;
		}
		int numSamplesCovered;
		int x, y;
		float realX, realY;
		bool needShade;
		bool samplesCovered[g_numSamples];
	};
	inline float dot(const Point4& a, const Point4&b)
	{
		return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
	}
	inline float dot(const Point3& a, const Point3&b)
	{
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}
	inline float dot(const Point2& a, const Point2&b)
	{
		return a.x * b.x + a.y * b.y;
	}
	inline float dot(const P& a, const P&b)
	{
		return a.x * b.x + a.y * b.y;
	}

	inline int dot(const int& a, const int&b)
	{
		return a * b;
	}
	struct Matrix4x4
	{
		Point4 rows[4];

		void setIdentity()
		{
			rows[0] = Point4(1.0f, 0.0f, 0.0f, 0.0f);
			rows[1] = Point4(0.0f, 1.0f, 0.0f, 0.0f);
			rows[2] = Point4(0.0f, 0.0f, 1.0f, 0.0f);
			rows[3] = Point4(0.0f, 0.0f, 0.0f, 1.0f);
		}

		void setScale(float x, float y, float z)
		{
			setIdentity();
			rows[0][0] = x;
			rows[1][1] = y;
			rows[2][2] = z;
		}

		Point4 operator*(const Point4& rhs)
		{
			Point4 ret;
			ret.x = dot(rows[0], rhs);
			ret.y = dot(rows[1], rhs);
			ret.z = dot(rows[2], rhs);
			ret.w = dot(rows[3], rhs);
			return ret;
		}

		float& operator()(int i, int j)
		{
			return rows[i][j];
		}

	};


	template< typename T> inline T getFromBarycentric(T& p, T& pp0, T& pp1, float a, float b)
	{
		return p + pp0*a + pp1*b;
	}
	template< typename T> inline T getFromBarycentric2(T& p1, T& p2, T& p3, float a, float b, float c)
	{
		return p1*a + p2*b + p3*c;
	}

	template< typename T> inline void Barycentric(T& p, T& a, T& b, T& c, float &u, float &v, float& w)
	{
		T v0 = b - a, v1 = c - a, v2 = p - a;
		float d00 = dot(v0, v0);
		float d01 = dot(v0, v1);
		float d11 = dot(v1, v1);
		float d20 = dot(v2, v0);
		float d21 = dot(v2, v1);
		float denom = d00 * d11 - d01 * d01;
		v = (d11 * d20 - d01 * d21) / denom;
		w = (d00 * d21 - d01 * d20) / denom;

		u = 1.0f - v - w;
	}
}