#pragma once

#include<math.h>

class CVector3
{
public:
	CVector3();
	CVector3(float X, float Y, float Z);

	void operator=(CVector3 &v);
	CVector3 operator-(CVector3 &v);
	CVector3 operator+(CVector3 &v);
	CVector3 operator*(CVector3 &v);
	CVector3 operator/(CVector3 &v);

	CVector3 operator+(float f);
	CVector3 operator-(float f);
	CVector3 operator*(float f);
	CVector3 operator/(float f);

	void operator +=(CVector3 &v);
	void operator -=(CVector3 &v);
	void operator *=(CVector3 &v);
	void operator /=(CVector3 &v);
	void operator +=(float f);
	void operator -=(float f);
	void operator *=(float f);
	void operator /=(float f);

	void CrossProduct(CVector3 &v1, CVector3 &v2);
	float DotProduct3(CVector3 &v1);
	float GetLength();
	void Normal();
	void Normalize(CVector3 &v1, CVector3 &v2, CVector3 &v3);

	float x, y, z;
};
