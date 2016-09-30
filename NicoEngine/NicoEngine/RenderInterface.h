#pragma once

#include "defines.h"
#include "light.h"
#include "material.h"
#include "GUI.h"
#include "structs.h"
#include "Camera.h"
#include "matrix.h"

class CRenderInterface
{
public:
	// 构造函数
	CRenderInterface() : m_screenWidth(0),
		m_screenHeight(0), m_near(0), m_far(0) {}
	// 虚拟析构函数
	virtual ~CRenderInterface() {}


	// 初始化Direct3D
	virtual bool Initialize(int w, int h,
		WinHWND mainWin, bool fullScreen, NE_MS_TYPE msType) = 0;

	// 设置默认的投影矩阵
	virtual void OneTimeInit() = 0;

	virtual void Shutdown() = 0;

	// 用于将后台缓存颜色设置为指定的颜色
	virtual void SetClearCol(float r, float g, float b) = 0;

	// 启动场景的绘制
	virtual void StartRender(bool bColor, bool bDepth,
		bool bStencil) = 0;

	// 清理已经启动的场景
	virtual void ClearBuffers(bool bColor, bool bDepth,
		bool bStencil) = 0;

	// 结束场景的绘制
	virtual void EndRendering() = 0;

	// 设置立体投影矩阵
	virtual void CalculateProjMatrix(float fov, float n,
		float f) = 0;
	// 设置正交投影矩阵
	virtual void CalculateOrthoMatrix(float n, float f) = 0;

	// 创建要绘制的静态顶点缓存
	virtual int CreateStaticBuffer(VertexType, PrimType,
		int totalVerts, int totalIndices,
		int stride, void **data, unsigned int *indices,
		int *staticId) = 0;

	// 设置材质
	virtual void SetMaterial(stMaterial *mat) = 0;

	// 设置并启用光源
	virtual void SetLight(stLight *light, int index) = 0;

	// 关闭光源
	virtual void DisableLight(int index) = 0;

	// 设置透明度
	virtual void SetTransparency(RenderState state,
		TransState src, TransState dst) = 0;

	// 添加2D纹理
	virtual int AddTexture2D(char *file, int *texId) = 0;

	// 设置纹理过滤器
	virtual void SetTextureFilter(int index, int filter,
		int val) = 0;

	// 设置多纹理贴图
	virtual void SetMultiTexture() = 0;

	// 应用纹理
	virtual void ApplyTexture(int index, int texId) = 0;

	// 保存屏幕截图
	virtual void SaveScreenShot(char *file) = 0;

	// 启用点状sprite
	virtual void EnablePointSprites(float size, float min,
		float a, float b, float c) = 0;

	// 禁用点状sprite
	virtual void DisablePointSprites() = 0;


	virtual bool CreateText(char *font, int weight, bool italic,
		int size, int &id) = 0;
	virtual void DisplayText(int id, long x, long y,
		int r, int g, int b,
		char *text, ...) = 0;
	virtual void DisplayText(int id, long x, long y,
		unsigned long color,
		char *text, ...) = 0;

	// 创建新的GUI对象，并将该对象添加到链表中
	bool CreateGUI(int &id)
	{
		// 首次创建GUI系统CGUISystem对象时，创建一个含有1个元素的对象链表
		if (!m_guiList)
		{
			// m_guiList初始化为动态创建的对象数组
			m_guiList = new CGUISystem[1];

			if (!m_guiList)
			{
				return NE_FAIL;
			}
		}
		else
		{
			// 临时的GUI系统对象指针，用于保存当前对象数组的值
			CGUISystem *temp;

			// 指向动态开辟的GUI系统对象数组
			temp = new CGUISystem[m_totalGUIs + 1];

			// 将m_guiList对象数组的全部数据复制到temp中
			memcpy(temp, m_guiList, sizeof(CGUISystem) * m_totalGUIs);

			delete[] m_guiList;  // 销毁m_guiList对象数组
			m_guiList = temp;    // m_guiList对象数组获得当前的temp对象数组的全部数据
		}

		id = m_totalGUIs;   // 记录当前GUI系统对象ID
		m_totalGUIs++;      // GUI系统对象数量递增1

		return true;
	}

	// 添加背景图GUI
	virtual bool AddGUIBackdrop(int guiId, char *fileName) = 0;

	// 添加静态文本GUI
	virtual bool AddGUIStaticText(int guiId, int id,
		char *text, int x, int y, unsigned long color,
		int fontID) = 0;

	// 添加按钮GUI
	virtual bool AddGUIButton(int guiId, int id, int x,
		int y, char *up, char *over, char *down) = 0;

	//设置GUI是否可见
	virtual void SetGUIVisble(int guiId, int id, bool visible) = 0;

	// 绘制GUI
	virtual void ProcessGUI(int guiID, bool LMBDown,
		int mouseX, int mouseY,
		void(*funcPtr)(int id, int state)) = 0;

	// 释放所有的GUI对象
	void ReleaseAllGUIs()
	{
		for (int s = 0; s < m_totalGUIs; s++)
		{
			m_guiList[s].Shutdown();
		}
		m_totalGUIs = 0;
		if (m_guiList)
		{
			delete[] m_guiList;
		}
		m_guiList = NULL;
	}

	// 启用雾
	virtual void EnableFog(float start, float end,
		NE_FOG_TYPE type, unsigned long color,
		bool rangeFog) = 0;
	// 禁用雾
	virtual void DisableFog() = 0;

	// 设置细节映射
	virtual void SetDetailMapping() = 0;

	virtual int LoadUMFAsStaticBuffer(char *file, int meshIndex, int *staticID) = 0;

	virtual int LoadUMFAsStaticBuffer(char *file, int meshIndex, int staticID) = 0;

	virtual int LoadOBJAsStaticBuffer(char *file, int *staticID) = 0;

	virtual int LoadOBJAsStaticBuffer(char *file, int staticID) = 0;

	virtual int LoadXModel(char *file, int *xModelId) = 0;

	virtual int LoadXModel(char *file, int xModelId) = 0;

	virtual void ReleaseAllStaticBuffers() = 0;

	virtual void ReleaseAllXModels() = 0;

	virtual int ReleaseStaticBuffer(int staticID) = 0;

	virtual int ReleaseXModel(int xModelId) = 0;

	virtual int RenderStaticBuffer(int staticId) = 0;

	virtual int RenderXModel(int xModelId) = 0;

	virtual void GetXModelBoundingSphere(int xModelId, CVector3 *origin, float *radius) = 0;

	// 设置视图矩阵
	virtual void ApplyView(CVector3 pos, CVector3 view, CVector3 up) = 0;

	// 设置世界矩阵
	virtual void SetWorldMatrix(CMatrix4x4 *mat) = 0;

	// 根据摄象机数据计算矩阵
	virtual void CameraToViewMatrix(CCamera *cam, CMatrix4x4 *mat) = 0;

	// 获取世界矩阵，在平截头体选择计算时会使用它
	virtual void GetProjectionMatrix(CMatrix4x4 *mat) = 0;

	virtual void UpdateXAnimation(int xModelId, float time, CMatrix4x4 *mat) = 0;

protected:
	CGUISystem *m_guiList;  // GUI系统CGUISystem的对象链表(数组)
	int m_totalGUIs;        // GUI系统CGUISystem的对象数量

protected:
	int m_screenWidth;      // 屏幕宽度
	int m_screenHeight;     // 屏幕高度
	bool m_fullscreen;      // 是否渲染整个屏幕

	WinHWND m_mainHandle;   // D3D初始化函数所需要的窗口句柄

	int m_totalFonts;		//Font的数量

	float m_near;           // 投影矩阵所需的近距离值
	float m_far;            // 投影矩阵所需的远距离值
};
