#include"Camera.h"

void CCamera::SetCamera(float x, float y, float z,
                        float xv, float yv, float zv,
                        float xu, float yu, float zu)
{
   m_pos.x = x; m_pos.y = y; m_pos.z = z;
   m_view.x = xv; m_view.y = yv; m_view.z = zv;
   m_up.x = xu; m_up.y = yu; m_up.z = zu;
}


void CCamera::MoveCamera(float amount)
{
   CVector3 dir = m_view - m_pos;
   dir.Normal();

   // Move the camera.
	m_pos += dir * amount;

   // Move the view along with the position.
   m_view += dir * amount;
}


void CCamera::RotateCamera(float angle, float X, float Y, float Z)
{
   float ca = (float)cos(angle);
   float sa = (float)sin(angle);

	// Get direction.
	CVector3 dir = m_view - m_pos;
	dir.Normal();

   CVector3 dir2;

	// Calculate the new X position.
	dir2.x = (ca + (1 - ca) * X) * dir.x;
	dir2.x += ((1 - ca) * X * Y - Z * sa)* dir.y;
	dir2.x += ((1 - ca) * X * Z + Y * sa) * dir.z;

	// Calculate the new Y position.
	dir2.y = ((1 - ca) * X * Y + Z * sa) * dir.x;
	dir2.y += (ca + (1 - ca) * Y) * dir.y;
	dir2.y += ((1 - ca) * Y * Z - X * sa) * dir.z;

	// Calculate the new Z position.
	dir2.z = ((1 - ca) * X * Z - Y * sa) * dir.x;
	dir2.z += ((1 - ca) * Y * Z + X * sa) * dir.y;
	dir2.z += (ca + (1 - ca) * Z) * dir.z;


	// Apply to view.
	m_view = m_pos + dir2;
}