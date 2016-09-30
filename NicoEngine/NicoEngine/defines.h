#pragma once

#include <windows.h>

// Boolean values.
#define NE_INFINITE		   -1
#define NE_INVALID		   -1    // ����ʹ�õĵ�ǰ��̬�������
#define NE_OK				1    // ��Ϊ�����ķ���ֵʹ��
#define NE_FAIL				0    // ͬ UGP_OK

// Window handle (need new way if porting to Mac and OpenGL).
#define WinHWND HWND

typedef long VertexType;

enum PrimType
{
	// һЩ�Զ����ͼԪ����
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

// ��Դ����
#define LIGHT_POINT 1			// ���Դ
#define LIGHT_DIRECTIONAL 2		// �����
#define LIGHT_SPOT 3			// �۹�� 

// һЩ��Ⱦ״̬
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
	TRANSPARENCY_NONE,     // ����͸����
	TRANSPARENCY_ENABLE    // ����͸����
};

// ͸������Ⱦ״ֵ̬
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

// ����������Ĺ���ģʽ
enum TextureState
{
	MIN_FILTER,   // ��С��
	MAG_FILTER,   // �Ŵ���
	MIP_FILTER    // mipmap������
};

// �������������
enum FilterType
{
	POINT_TYPE,        // ��������
	LINEAR_TYPE,       // �����������
	ANISOTROPIC_TYPE   // ���������������
};

// ����ȷ��GUIԪ������Ļ�ϵ�λ��
#define PERCENT_OF(a, b) (int)(a * b)

// GUI�ؼ����ε������ʽ
#define GUI_FVF      2    // GUI�ؼ����ε������ʽ
#define MV_FVF       3    // X�ļ�ģ�͵������ʽ

// GUI�ؼ�����
#define NE_GUI_STATICTEXT 1    // ��̬�ı��ؼ�
#define NE_GUI_BUTTON     2    // ��ť�ؼ�
#define NE_GUI_BACKDROP   3    // ����ͼ�ؼ�

// ����״̬
#define NE_BUTTON_UP      1    // ����ڰ�ť�ϵ���
#define NE_BUTTON_OVER    2    // ͣ���ڰ�ť��
#define NE_BUTTON_DOWN    3    // ����ڰ�ť�ϰ���

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


#define NE_LEFT_BUTTON      0    // ������
#define NE_RIGHT_BUTTON		1    // ����Ҽ�