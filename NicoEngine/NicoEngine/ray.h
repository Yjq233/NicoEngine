#pragma once

#include "mathLibrary.h"

class CRay
{
public:
	CRay() {}

	bool Intersect(CVector3 &pos, float radius, float *dist);
	bool Intersect(CVector3 &p1, CVector3 &p2, CVector3 &p3,
		bool cull, float *dist);
	bool Intersect(CVector3 &p1, CVector3 &p2, CVector3 &p3,
		bool cull, float length, float *dist);
	bool Intersect(CPlane &pl, bool cull,
		CVector3 *intersectPoint, float *dist);
	bool Intersect(CPlane &pl, bool cull, float length,
		CVector3 *intersectPoint, float *dist);

	CVector3 m_origin;
	CVector3 m_direction;
};