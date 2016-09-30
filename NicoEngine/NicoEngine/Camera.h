#pragma once

#include <math.h>
#include "Vector.h"

#define UP     0.03f             // Forward speed.
#define DOWN   -0.03f            // Backward speed.
#define LEFT   -0.03f            // Left speed.
#define RIGHT  0.03f             // Right speed.


class CCamera
{
   public:
	   CCamera() {}

      void SetCamera(float x, float y, float z,
                     float xv, float yv, float zv,
                     float xu, float yu, float zu);

      void MoveCamera(float amount);
      void RotateCamera(float angle, float X, float Y, float Z);

      CVector3 m_pos, m_view, m_up;
};