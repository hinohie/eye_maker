#ifndef __VECTOR_2F_H__
#define __VECTOR_2F_H__

#include <algorithm>
#include <vector>
#include <math.h>
using namespace std;
#define E 0.000001
typedef pair<int, int> pii;

struct vector3f{
	double x,y,z;
	vector3f(){x=y=z=0;}
	vector3f(double p){x=y=z=p;}
	vector3f(double nx,double ny,double nz){x=nx;y=ny;z=nz;}
	friend vector3f operator +(vector3f p,vector3f q)
	{
		vector3f r = vector3f(p.x+q.x, p.y+q.y, p.z+q.z);
		return r;
	}
	friend vector3f operator *(double p,vector3f q)
	{
		vector3f r = vector3f(p*q.x, p*q.y, p*q.z);
		return r;
	}
	friend vector3f operator *(vector3f q,double p)
	{
		vector3f r = vector3f(p*q.x, p*q.y, p*q.z);
		return r;
	}
	friend vector3f operator /(vector3f q,double p)
	{
		vector3f r = vector3f(q.x/p, q.y/p, q.z/p);
		return r;
	}
	friend double operator *(vector3f p,vector3f q)
	{
		double r = (p.x*q.x + p.y*q.y+ p.z*q.z);
		return r;
	}
	friend vector3f operator -(vector3f p,vector3f q)
	{
		vector3f r = vector3f(p.x-q.x, p.y-q.y, p.z-q.z);
		return r;
	}
	friend vector3f operator %(vector3f p,vector3f q)
	{
		vector3f r = vector3f(p.y*q.z - p.z*q.y, p.z*q.x - p.x*q.z, p.x*q.y - p.y*q.x);
		return r;
	}
	void operator += (const vector3f &p)			{ x += p.x;  y += p.y; z += p.z;}
	void operator -= (const vector3f &p)			{ x -= p.x;  y -= p.y; z -= p.z;}
	void operator *= (double c)						{ x *= c;  y *= c; z *= c;}
	void operator /= (double c)						{ x /= c;  y /= c; z /= c;}

	double dist()
	{
		return sqrt(x*x+y*y+z*z);
	}
	double dist(vector3f p)
	{
		vector3f r = *this - p;
		return r.dist();
	}
	void normalize()
	{
		double w = dist();
		if(w < E) return;
		x /= w;
		y /= w;
		z /= w;
	}
};
struct vector2f{
	double x,y;
	vector2f(){x=y=0;}
	vector2f(double p){x=y=p;}
	vector2f(double nx,double ny){x=nx;y=ny;}
	vector2f(int nx,int ny){x=nx;y=ny;}
	friend vector2f operator +(vector2f p,vector2f q)
	{
		vector2f r = vector2f(p.x+q.x, p.y+q.y);
		return r;
	}
	friend vector2f operator *(double p,vector2f q)
	{
		vector2f r = vector2f(p*q.x, p*q.y);
		return r;
	}
	friend vector2f operator *(vector2f q,double p)
	{
		vector2f r = vector2f(p*q.x, p*q.y);
		return r;
	}
	friend vector2f operator /(vector2f q,double p)
	{
		vector2f r = vector2f(q.x/p, q.y/p);
		return r;
	}
	friend double operator *(vector2f p,vector2f q)
	{
		double r = (p.x*q.x + p.y*q.y);
		return r;
	}
	friend vector2f operator -(vector2f p,vector2f q)
	{
		vector2f r = vector2f(p.x-q.x, p.y-q.y);
		return r;
	}
	void operator += (const vector2f &p)			{ x += p.x;  y += p.y;}
	void operator -= (const vector2f &p)			{ x -= p.x;  y -= p.y;}
	void operator *= (double c)						{ x *= c;  y *= c;}
	void operator /= (double c)						{ x /= c;  y /= c;}

	double dist()
	{
		return sqrt(x*x+y*y);
	}
	double dist(vector2f p)
	{
		vector2f r = *this - p;
		return r.dist();
	}
	void normalize()
	{
		double w = dist();
		if(w < E) return;
		x /= w;
		y /= w;
	}
};
class vector2d {
public:
	int x,y;
	vector2d(int x,int y)
	{
		this->x = x;
		this->y = y;
	}
	vector2d()
	{
		this->x = 0;
		this->y = 0;
	}

	const vector2d operator - (const vector2d s) const
	{
		vector2d t(x - s.x, y - s.y);
		return t;
	}
	const vector2d operator + (const vector2d s) const
	{
		vector2d t(x + s.x, y + s.y);
		return t;
	}
	friend bool operator <(vector2d &p, vector2d &q)
	{
		if(p.x < q.x)return true;
		else if(p.x==q.x && p.y < q.y) return true;
		return false;
	}

	int dist(vector2d s)
	{
		int xx = x - s.x;
		int yy = y - s.y;
		if(xx<0)xx=-xx;
		if(yy<0)yy=-yy;
		return xx+yy;
	}
};

#endif