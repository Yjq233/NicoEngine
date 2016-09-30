#pragma once

#define NE_FRONT    0
#define NE_BACK     1
#define NE_ON_PLANE 2
#define NE_CLIPPED  3
#define NE_CULLED   4
#define NE_VISIBLE  5

#define PI 3.14159265358979323846

// 将度转换成弧度
#define GET_RADIANS(degree) (float)(degree / 180.0f * PI)
