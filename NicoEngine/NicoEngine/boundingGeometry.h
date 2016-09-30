#pragma once

#include "mathLibrary.h"

// 边界几何图形基类
class CBoundingBase
{
public:
	CBoundingBase() {}
	virtual ~CBoundingBase() {}

	virtual void CreateFromPoints(CVector3 *pointList, int numPoints) = 0;

	// 测试一个点是否和一个边界几何图形发生碰撞
	virtual bool isPointInside(CVector3 &v) = 0;

	virtual bool Intersect(CRay ray, float *dist) = 0;
	virtual bool Intersect(CRay ray, float length, float *dist) = 0;

	// 计算每个框边外的平面(6个平面)
	virtual void GetPlanes(CPlane *planes) = 0;

	// 查看射线是否真的在边界几何图形中
	virtual bool isRayInside(CRay &ray, float length) = 0;
};



// 边界框类
class CBoundingBox : public CBoundingBase
{
public:
	CBoundingBox() {}
	~CBoundingBox() {}

	void CreateFromPoints(CVector3 *pointList, int numPoints);
	bool isPointInside(CVector3 &v);

	bool Intersect(CRay ray, float *dist);
	bool Intersect(CRay ray, float length, float *dist);

	void GetPlanes(CPlane *planes);
	bool isRayInside(CRay &ray, float length);

	CVector3 m_min, m_max;
};



// 边界球
class CBoundingSphere : public CBoundingBase
{
public:
	CBoundingSphere() : m_radius(0) {}
	~CBoundingSphere() {}

	void CreateFromPoints(CVector3 *pointList, int numPoints);
	bool isPointInside(CVector3 &v);

	bool Intersect(CRay ray, float *dist);
	bool Intersect(CRay ray, float length, float *dist);

	void GetPlanes(CPlane *planes) {}
	bool isRayInside(CRay &ray, float length);

	CVector3 m_center;
	float m_radius;
};


// 检测两个方框之间的碰撞
bool BoxToBoxIntersect(CBoundingBox &bb1, CBoundingBox &bb2);

// 检测两个球体之间的碰撞
bool SphereToSphereIntersect(CBoundingSphere &bs1, CBoundingSphere &bs2);
