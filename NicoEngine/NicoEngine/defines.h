#pragma once

#include <windows.h>

// Boolean values.
#define NE_INFINITE		   -1
#define NE_INVALID		   -1    // 正在使用的当前静态缓存变量
#define NE_OK				1    // 作为函数的返回值使用
#define NE_FAIL				0    // 同 UGP_OK

// Window handle (need new way if porting to Mac and OpenGL).
#define WinHWND HWND

typedef long VertexType;

enum PrimType
{
	// 一些自定义的图元类型
	NULL_TYPE,
	POINT_LIST,
	TRIANGLE_LIST,
	TRIANGLE_STRIP,
	TRIANGLE_FAN,
	LINE_LIST,
	LINE_STRIP
};

// Color defines.
#define NECOLOR_ARGB(a,r,g,b) ((unsigned long)((((a)&0xff)<<24)|\
                               (((r)&0xff)<<16)|(((g)&0xff)<<8)|\
                               ((b)&0xff)))

// 光源类型
#define LIGHT_POINT 1			// 点光源
#define LIGHT_DIRECTIONAL 2		// 方向光
#define LIGHT_SPOT 3			// 聚光灯 

// 一些渲染状态
enum RenderState
{
	CULL_NONE,
	CULL_CW,
	CULL_CCW,
	DEPTH_NONE,
	DEPTH_READONLY,
	DEPTH_READWRITE,
	SHADE_POINTS,
	SHADE_SOLIDTRI,
	SHADE_WIRETRI,
	SHADE_WIREPOLY,
	TRANSPARENCY_NONE,     // 禁用透明度
	TRANSPARENCY_ENABLE    // 启用透明度
};

// 透明度渲染状态值
enum TransState
{
	TRANS_ZERO = 1,
	TRANS_ONE,
	TRANS_SRCCOLOR,
	TRANS_INVSRCCOLOR,
	TRANS_SRCALPHA,
	TRANS_INVSRCALPHA,
	TRANS_DSTALPHA,
	TRANS_INVDSTALPHA,
	TRANS_DSTCOLOR,
	TRANS_INVDSTCOLOR,
	TRANS_SRCALPHASAT,
	TRANS_BOTHSRCALPHA,
	TRANS_INVBOTHSRCALPHA,
	TRANS_BLENDFACTOR,
	TRANS_INVBLENDFACTOR
};

// 纹理过滤器的过滤模式
enum TextureState
{
	MIN_FILTER,   // 缩小率
	MAG_FILTER,   // 放大率
	MIP_FILTER    // mipmap纹理级别
};

// 纹理过滤器类型
enum FilterType
{
	POINT_TYPE,        // 最近点采样
	LINEAR_TYPE,       // 线性纹理过滤
	ANISOTROPIC_TYPE   // 各向异性纹理过滤
};

// 用于确定GUI元素在屏幕上的位置
#define PERCENT_OF(a, b) (int)(a * b)

// GUI控件矩形的灵活顶点格式
#define GUI_FVF      2    // GUI控件矩形的灵活顶点格式
#define MV_FVF       3    // X文件模型的灵活顶点格式

// GUI控件类型
#define NE_GUI_STATICTEXT 1    // 静态文本控件
#define NE_GUI_BUTTON     2    // 按钮控件
#define NE_GUI_BACKDROP   3    // 背景图控件

// 鼠标的状态
#define NE_BUTTON_UP      1    // 左键在按钮上弹起
#define NE_BUTTON_OVER    2    // 停靠在按钮上
#define NE_BUTTON_DOWN    3    // 左键在按钮上按下

// Multi-sampling.
enum NE_MS_TYPE
{
	NE_MS_NONE,
	NE_MS_SAMPLES_2,
	NE_MS_SAMPLES_4,
	NE_MS_SAMPLES_8,
	NE_MS_SAMPLES_16
};

// Fog.
enum NE_FOG_TYPE
{
	NE_VERTEX_FOG,
	NE_PIXEL_FOG
};

// Keyboard keys size. 
#define KEYS_SIZE 256


#define NE_LEFT_BUTTON      0    // 鼠标左键
#define NE_RIGHT_BUTTON		1    // 鼠标右键