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
	// ���캯��
	CRenderInterface() : m_screenWidth(0),
		m_screenHeight(0), m_near(0), m_far(0) {}
	// ������������
	virtual ~CRenderInterface() {}


	// ��ʼ��Direct3D
	virtual bool Initialize(int w, int h,
		WinHWND mainWin, bool fullScreen, NE_MS_TYPE msType) = 0;

	// ����Ĭ�ϵ�ͶӰ����
	virtual void OneTimeInit() = 0;

	virtual void Shutdown() = 0;

	// ���ڽ���̨������ɫ����Ϊָ������ɫ
	virtual void SetClearCol(float r, float g, float b) = 0;

	// ���������Ļ���
	virtual void StartRender(bool bColor, bool bDepth,
		bool bStencil) = 0;

	// �����Ѿ������ĳ���
	virtual void ClearBuffers(bool bColor, bool bDepth,
		bool bStencil) = 0;

	// ���������Ļ���
	virtual void EndRendering() = 0;

	// ��������ͶӰ����
	virtual void CalculateProjMatrix(float fov, float n,
		float f) = 0;
	// ��������ͶӰ����
	virtual void CalculateOrthoMatrix(float n, float f) = 0;

	// ����Ҫ���Ƶľ�̬���㻺��
	virtual int CreateStaticBuffer(VertexType, PrimType,
		int totalVerts, int totalIndices,
		int stride, void **data, unsigned int *indices,
		int *staticId) = 0;

	// ���ò���
	virtual void SetMaterial(stMaterial *mat) = 0;

	// ���ò����ù�Դ
	virtual void SetLight(stLight *light, int index) = 0;

	// �رչ�Դ
	virtual void DisableLight(int index) = 0;

	// ����͸����
	virtual void SetTransparency(RenderState state,
		TransState src, TransState dst) = 0;

	// ���2D����
	virtual int AddTexture2D(char *file, int *texId) = 0;

	// �������������
	virtual void SetTextureFilter(int index, int filter,
		int val) = 0;

	// ���ö�������ͼ
	virtual void SetMultiTexture() = 0;

	// Ӧ������
	virtual void ApplyTexture(int index, int texId) = 0;

	// ������Ļ��ͼ
	virtual void SaveScreenShot(char *file) = 0;

	// ���õ�״sprite
	virtual void EnablePointSprites(float size, float min,
		float a, float b, float c) = 0;

	// ���õ�״sprite
	virtual void DisablePointSprites() = 0;


	virtual bool CreateText(char *font, int weight, bool italic,
		int size, int &id) = 0;
	virtual void DisplayText(int id, long x, long y,
		int r, int g, int b,
		char *text, ...) = 0;
	virtual void DisplayText(int id, long x, long y,
		unsigned long color,
		char *text, ...) = 0;

	// �����µ�GUI���󣬲����ö�����ӵ�������
	bool CreateGUI(int &id)
	{
		// �״δ���GUIϵͳCGUISystem����ʱ������һ������1��Ԫ�صĶ�������
		if (!m_guiList)
		{
			// m_guiList��ʼ��Ϊ��̬�����Ķ�������
			m_guiList = new CGUISystem[1];

			if (!m_guiList)
			{
				return NE_FAIL;
			}
		}
		else
		{
			// ��ʱ��GUIϵͳ����ָ�룬���ڱ��浱ǰ���������ֵ
			CGUISystem *temp;

			// ָ��̬���ٵ�GUIϵͳ��������
			temp = new CGUISystem[m_totalGUIs + 1];

			// ��m_guiList���������ȫ�����ݸ��Ƶ�temp��
			memcpy(temp, m_guiList, sizeof(CGUISystem) * m_totalGUIs);

			delete[] m_guiList;  // ����m_guiList��������
			m_guiList = temp;    // m_guiList���������õ�ǰ��temp���������ȫ������
		}

		id = m_totalGUIs;   // ��¼��ǰGUIϵͳ����ID
		m_totalGUIs++;      // GUIϵͳ������������1

		return true;
	}

	// ��ӱ���ͼGUI
	virtual bool AddGUIBackdrop(int guiId, char *fileName) = 0;

	// ��Ӿ�̬�ı�GUI
	virtual bool AddGUIStaticText(int guiId, int id,
		char *text, int x, int y, unsigned long color,
		int fontID) = 0;

	// ��Ӱ�ťGUI
	virtual bool AddGUIButton(int guiId, int id, int x,
		int y, char *up, char *over, char *down) = 0;

	//����GUI�Ƿ�ɼ�
	virtual void SetGUIVisble(int guiId, int id, bool visible) = 0;

	// ����GUI
	virtual void ProcessGUI(int guiID, bool LMBDown,
		int mouseX, int mouseY,
		void(*funcPtr)(int id, int state)) = 0;

	// �ͷ����е�GUI����
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

	// ������
	virtual void EnableFog(float start, float end,
		NE_FOG_TYPE type, unsigned long color,
		bool rangeFog) = 0;
	// ������
	virtual void DisableFog() = 0;

	// ����ϸ��ӳ��
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

	// ������ͼ����
	virtual void ApplyView(CVector3 pos, CVector3 view, CVector3 up) = 0;

	// �����������
	virtual void SetWorldMatrix(CMatrix4x4 *mat) = 0;

	// ������������ݼ������
	virtual void CameraToViewMatrix(CCamera *cam, CMatrix4x4 *mat) = 0;

	// ��ȡ���������ƽ��ͷ��ѡ�����ʱ��ʹ����
	virtual void GetProjectionMatrix(CMatrix4x4 *mat) = 0;

	virtual void UpdateXAnimation(int xModelId, float time, CMatrix4x4 *mat) = 0;

protected:
	CGUISystem *m_guiList;  // GUIϵͳCGUISystem�Ķ�������(����)
	int m_totalGUIs;        // GUIϵͳCGUISystem�Ķ�������

protected:
	int m_screenWidth;      // ��Ļ���
	int m_screenHeight;     // ��Ļ�߶�
	bool m_fullscreen;      // �Ƿ���Ⱦ������Ļ

	WinHWND m_mainHandle;   // D3D��ʼ����������Ҫ�Ĵ��ھ��

	int m_totalFonts;		//Font������

	float m_near;           // ͶӰ��������Ľ�����ֵ
	float m_far;            // ͶӰ���������Զ����ֵ
};
