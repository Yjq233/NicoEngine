#pragma once

#include "vector.h"
#include "ray.h"
#include "plane.h"
#include "mathDefines.h"


class CPolygon
{
public:
	CPolygon() : m_flag(0) { }

	void SetFlag(unsigned int fl) { m_flag = fl; }
	unsigned int GetFlag() { return m_flag; }
	void Copy(CPolygon &pol);

	void Clip(CPlane &pl, CPolygon *front, CPolygon *back);

	bool Intersect(CRay &ray, bool cull, float *dist);

	unsigned int m_flag;
	CVector3 m_vertexList[3];
};
