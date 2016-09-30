#pragma once

// GUI控件的顶点结构
struct stGUIVertex
{
	float x, y, z, rhw;
	unsigned long color;
	float tu, tv;
};

// 模型顶点结构
struct stModelVertex
{
	float x, y, z;         // 模型顶点坐标
	float nx, ny, nz;      // 顶点法线
	unsigned long color;   // 顶点颜色值
	float tu, tv;          // 纹理坐标
};
