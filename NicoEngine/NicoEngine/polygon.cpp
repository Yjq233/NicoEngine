#include "polygon.h"

void CPolygon::Copy(CPolygon &pol)
{
	m_vertexList[0] = pol.m_vertexList[0];
	m_vertexList[1] = pol.m_vertexList[1];
	m_vertexList[2] = pol.m_vertexList[2];
}

bool CPolygon::Intersect(CRay &ray, bool cull, float *dist)
{
	if (!ray.Intersect(m_vertexList[0], m_vertexList[1],
		m_vertexList[2], false, dist)) return false;

	if (!cull)
	{
		if (!ray.Intersect(m_vertexList[2], m_vertexList[1],
			m_vertexList[0], false, dist)) return false;
	}

	return true;
}
