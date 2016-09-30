#pragma once

#include"Vector.h"

#define STRIAGHT_PATH 0
#define CURVE_PATH 1


class CPath
{
   public:
      CPath();
      CPath(int Type, CVector3 start, CVector3 cnt1,
            CVector3 cnt2, CVector3 end);
      ~CPath();

      void SetPath(int Type, CVector3 start, CVector3 cnt1,
                   CVector3 cnt2, CVector3 end);
      void Shutdown();


      int m_type;          // Type of path this is.

      CVector3 m_startPos; // Start location of this path.
      CVector3 m_control1; // Control point one (only for curve paths).
      CVector3 m_control2; // Control point two (only for curve paths).
      CVector3 m_endPos;   // Destination location.

      float m_start;       // Starting lenght for this path animation.
      float m_total;       // Total length distance of this animation path.

      CPath *m_next;       // Pointer to the next path in our link list.
};


class CRoute
{
   public:
      CRoute();
      ~CRoute();

      bool AddPath(int Type, CVector3 start, CVector3 cnt1,
                   CVector3 cnt2, CVector3 end);


      void GetPosition(float time, CVector3 &objPos);

      CVector3 CalcBezierCurvePos(CVector3 start, CVector3 c1,
                                  CVector3 c2, CVector3 end,
                                  float Scalar);
      CVector3 CalcStriaghtPath(CVector3 start,
                                CVector3 end, float Scalar);
      void Shutdown();


      CPath *m_path;
      float m_startTime;
};