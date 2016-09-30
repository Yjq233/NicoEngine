#pragma once

#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include "RenderInterface.h"
#include "UMFLoader.h"
#include "OBJLoader.h"
#include "XMesh.h"

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

// 纹理信息结构
struct stD3DTexture
{
	stD3DTexture() : fileName(0), image(0), width(0), height(0) {}

	char *fileName;            // 指明文件名
	int width, height;         // 纹理的宽度和高度
	LPDIRECT3DTEXTURE9 image;  // 纹理对象
};

// 静态缓存结构
struct stD3DStaticBuffer
{
	stD3DStaticBuffer() : vbPtr(0), ibPtr(0), numVerts(0),
		numIndices(0), stride(0), fvf(0),
		primType(NULL_TYPE) {}

	LPDIRECT3DVERTEXBUFFER9 vbPtr;   // 顶点缓存
	LPDIRECT3DINDEXBUFFER9 ibPtr;    // 索引缓存(三角形索引)
	int numVerts;        // 对顶点的计数
	int numIndices;      // 对索引的计数
	int stride;          // 单个顶点尺寸的幅度值
	unsigned long fvf;   // Direct3D顶点FVF
	PrimType primType;   // 在渲染静态缓存时要用的图元类型
};


class CD3DRenderer : public CRenderInterface
{
public:
	CD3DRenderer();
	~CD3DRenderer();

	// 初始化Direct3D
	bool Initialize(int w, int h, WinHWND mainWin,
		bool fullScreen, NE_MS_TYPE msType);
	void Shutdown();

	// 用于将后台缓存颜色设置为指定的颜色
	void SetClearCol(float r, float g, float b);

	// 启动场景的绘制
	void StartRender(bool bColor, bool bDepth, bool bStencil);

	// 清理已经启动的场景
	void ClearBuffers(bool bColor, bool bDepth, bool bStencil);

	// 结束场景的绘制
	void EndRendering();

	// 用于创建平行投影矩阵
	void CalculateProjMatrix(float fov, float n, float f);
	// 用于创建正交投影矩阵
	void CalculateOrthoMatrix(float n, float f);

	// 设置静态缓存对象中的一个对象内容，并将其添加到静态缓存列表中
	int CreateStaticBuffer(VertexType, PrimType,
		int totalVerts, int totalIndices,
		int stride, void **data, unsigned int *indices,
		int *staticId);

	// 设置材质
	void SetMaterial(stMaterial *mat);

	// 设置并启用光源
	void SetLight(stLight *light, int index);

	// 关闭光源
	void DisableLight(int index);

	// 设置透明度
	void SetTransparency(RenderState state, TransState src,
		TransState dst);

	// 添加2D纹理
	int AddTexture2D(char *file, int *texId);

	// 设置纹理过滤器
	void SetTextureFilter(int index, int filter, int val);

	// 设置多纹理贴图
	void SetMultiTexture();

	// 应用纹理
	void ApplyTexture(int index, int texId);

	// 保存屏幕截图
	void SaveScreenShot(char *file);

	// 启用点状sprite
	void EnablePointSprites(float size, float min, float a,
		float b, float c);

	// 禁用点状sprite
	void DisablePointSprites();

	bool CreateText(char *font, int weight, bool italic,
		int size, int &id);
	void DisplayText(int id, long x, long y, int r,
		int g, int b, char *text, ...);
	void DisplayText(int id, long x, long y,
		unsigned long color, char *text, ...);

	// 添加背景图GUI
	bool AddGUIBackdrop(int guiId, char *fileName);

	// 添加静态文本GUI
	bool AddGUIStaticText(int guiId, int id, char *text,
		int x, int y, unsigned long color, int fontID);

	// 添加按钮GUI
	bool AddGUIButton(int guiId, int id, int x, int y, char *up, char *over, char *down);

	//设置GUI是否可见
	void SetGUIVisble(int guiId, int id, bool visible);

	// 绘制GUI
	void ProcessGUI(int guiID, bool LMBDown, int mouseX, int mouseY,
		void(*funcPtr)(int id, int state));

	// 启用雾
	void EnableFog(float start, float end, NE_FOG_TYPE type,
		unsigned long color, bool rangeFog);
	// 禁用雾
	void DisableFog();

	// 设置细节映射
	void SetDetailMapping();

public:
	int LoadUMFAsStaticBuffer(char *file, int meshIndex, int *staticID);
	int LoadUMFAsStaticBuffer(char *file, int meshIndex, int staticID);
	int LoadOBJAsStaticBuffer(char *file, int *staticID);
	int LoadOBJAsStaticBuffer(char *file, int staticID);
	int LoadXModel(char *file, int *xModelId);
	int LoadXModel(char *file, int xModelId);

	void ReleaseAllStaticBuffers();
	void ReleaseAllXModels();
	int ReleaseStaticBuffer(int staticID);
	int ReleaseXModel(int xModelId);

	int RenderStaticBuffer(int staticId);
	int RenderXModel(int xModelId);

	// 设置视图矩阵
	void ApplyView(CVector3 pos, CVector3 view, CVector3 up);

	// 设置世界矩阵
	void SetWorldMatrix(CMatrix4x4 *mat);

	// 根据摄象机数据计算矩阵
	void CameraToViewMatrix(CCamera *cam, CMatrix4x4 *mat);

	// 获取世界矩阵，在平截头体选择计算时会使用它
	void GetProjectionMatrix(CMatrix4x4 *mat);

	void GetXModelBoundingSphere(int xModelId, CVector3 *origin, float *radius)
	{
		if (xModelId < 0 || xModelId >= m_numXModels || !m_xModels)
			return;

		D3DXVECTOR3 center;
		float r;

		m_xModels[xModelId].GetBoundingSphere(&center, &r);

		if (origin)
		{
			origin->x = center.x;
			origin->y = center.y;
			origin->z = center.z;
		}

		if (radius)
			*radius = r;
	}

	void UpdateXAnimation(int xModelId, float time, CMatrix4x4 *mat)
	{
		if (xModelId < 0 || xModelId >= m_numXModels || !m_xModels)
			return;
		if (!mat) return;

		m_xModels[xModelId].Update(time, (D3DXMATRIX*)&mat->matrix);
	}



private:
	D3DXMATRIX m_projection;

private:
	CXModel *m_xModels;   // X模型类对象链表指针
	int m_numXModels;     // 模型数量


private:
	stD3DTexture *m_textureList;
	int m_numTextures;

private:
	// 设置默认的投影矩阵
	void OneTimeInit();


private:
	D3DCOLOR m_Color;             // 清除色
	LPDIRECT3D9 m_Direct3D;       // Direct3D9接口
	LPDIRECT3DDEVICE9 m_Device;   // Direct3D设备对象
	bool m_renderingScene;        // 是否是正在渲染的场景

	LPD3DXFONT *m_fonts;

	stD3DStaticBuffer *m_staticBufferList;  // 保存所有静态缓存的数组链表
	int m_numStaticBuffers;       // 静态缓存数目的计数器
	int m_activeStaticBuffer;     // 正在使用的当前静态缓存变量，该变量用于避免设置已经设置过的静态缓存
};


// 动态创建渲染基类对象，函数参数指向该内存块
bool CreateD3DRenderer(CRenderInterface **pObj);