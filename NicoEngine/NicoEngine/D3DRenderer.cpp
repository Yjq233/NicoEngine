#include "D3DRenderer.h"
#include <stdio.h>
#include <tchar.h>

// GUI控件的灵活顶点格式
#define D3DFVF_GUI (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)
// X模型顶点结构的灵活顶点格式
#define D3DFVF_MV (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1)

// 类型转换函数：从float转为unsigned long
inline unsigned long FtoDW(float val)
{
	return *((unsigned long*)&val);
}

unsigned long CreateD3DFVF(int flags)
{
	unsigned long fvf = 0;

	if (flags == GUI_FVF)
	{
		fvf = D3DFVF_GUI;
	}
	if (flags == MV_FVF)
	{
		fvf = D3DFVF_MV;
	}

	return fvf;
}

D3DMULTISAMPLE_TYPE GetD3DMultiSampleType(LPDIRECT3D9 d3d,
	NE_MS_TYPE ms, D3DDEVTYPE type, D3DFORMAT format, bool fullscreen)
{
	D3DMULTISAMPLE_TYPE t = D3DMULTISAMPLE_NONE;

	if (d3d)
	{
		switch (ms)
		{
		case NE_MS_NONE:
			t = D3DMULTISAMPLE_NONE;
			break;

		case NE_MS_SAMPLES_2:
			if (d3d->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT,
				type, format, !fullscreen, D3DMULTISAMPLE_2_SAMPLES,
				NULL) == D3D_OK) t = D3DMULTISAMPLE_2_SAMPLES;
			break;

		case NE_MS_SAMPLES_4:
			if (d3d->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT,
				type, format, !fullscreen, D3DMULTISAMPLE_2_SAMPLES,
				NULL) == D3D_OK) t = D3DMULTISAMPLE_4_SAMPLES;
			break;

		case NE_MS_SAMPLES_8:
			if (d3d->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT,
				type, format, !fullscreen, D3DMULTISAMPLE_8_SAMPLES,
				NULL) == D3D_OK) t = D3DMULTISAMPLE_8_SAMPLES;
			break;

		case NE_MS_SAMPLES_16:
			if (d3d->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT,
				type, format, !fullscreen, D3DMULTISAMPLE_16_SAMPLES,
				NULL) == D3D_OK) t = D3DMULTISAMPLE_16_SAMPLES;
			break;

		default:
			break;
		}
	}

	return t;
}

// 创建渲染系统的抽象基类对象，函数参数指向该内存块
bool CreateD3DRenderer(CRenderInterface **pObj)
{
	if (!*pObj)
	{
		// 动态开辟内存，以创建一个新的渲染对象
		// 该函数参数（抽象基类指针）指向这块内存空间（派生类对象）
		*pObj = new CD3DRenderer;
	}
	else
	{
		return false;
	}

	return true;
}


// 构造函数
CD3DRenderer::CD3DRenderer()
{
	m_Direct3D = NULL;
	m_Device = NULL;
	m_renderingScene = false;
	m_numStaticBuffers = 0;
	m_activeStaticBuffer = NE_INVALID;
	m_staticBufferList = NULL;
	m_textureList = NULL;
	m_numTextures = 0;
	m_totalFonts = 0;
	m_guiList = NULL;
	m_totalGUIs = 0;
	m_fonts = NULL;
}


// 析构函数
CD3DRenderer::~CD3DRenderer()
{
	Shutdown();
}

// 初始化Direct3D
bool CD3DRenderer::Initialize(int w, int h, WinHWND mainWin,
	bool fullScreen, NE_MS_TYPE msType)
{
	// 清理渲染系统用过的内存
	Shutdown();

	m_mainHandle = mainWin;
	if (!m_mainHandle) return false;

	m_fullscreen = fullScreen;

	D3DDISPLAYMODE mode;
	D3DCAPS9 caps;
	D3DPRESENT_PARAMETERS Params;

	ZeroMemory(&Params, sizeof(Params));

	m_Direct3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (!m_Direct3D) return false;

	if (FAILED(m_Direct3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,
		&mode))) return false;

	if (FAILED(m_Direct3D->GetDeviceCaps(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL, &caps))) return false;

	DWORD processing = 0;
	if (caps.VertexProcessingCaps != 0)
		processing = D3DCREATE_HARDWARE_VERTEXPROCESSING |
		D3DCREATE_PUREDEVICE;
	else
		processing = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	if (m_fullscreen)
	{
		Params.FullScreen_RefreshRateInHz = mode.RefreshRate;
		Params.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	}
	else
		Params.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	Params.Windowed = !m_fullscreen;
	Params.BackBufferWidth = w;
	Params.BackBufferHeight = h;
	Params.hDeviceWindow = m_mainHandle;
	Params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	Params.BackBufferFormat = mode.Format;
	Params.BackBufferCount = 1;
	Params.EnableAutoDepthStencil = TRUE;
	Params.AutoDepthStencilFormat = D3DFMT_D16;
	Params.MultiSampleType = GetD3DMultiSampleType(m_Direct3D, msType,
		D3DDEVTYPE_HAL, mode.Format, m_fullscreen);

	m_screenWidth = w;
	m_screenHeight = h;

	if (FAILED(m_Direct3D->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL, m_mainHandle, processing,
		&Params, &m_Device))) return false;

	if (m_Device == NULL) return false;

	// 设置默认的投影矩阵
	OneTimeInit();

	return true;
}


// 设置默认的投影矩阵
void CD3DRenderer::OneTimeInit()
{
	if (!m_Device) return;

	m_Device->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	CalculateProjMatrix(D3DX_PI / 4, 0.1f, 1000);
}

// 清理所有渲染系统用过的内存
void CD3DRenderer::Shutdown()
{
	// 释放所有的静态缓存
	ReleaseAllStaticBuffers();

	// 释放纹理对象列表
	for (int s = 0; s<m_numTextures; s++)
	{
		if (m_textureList[s].fileName)
		{
			delete[] m_textureList[s].fileName;   // 销毁文件名对象
			m_textureList[s].fileName = NULL;     // 指向为空
		}

		if (m_textureList[s].image)
		{
			m_textureList[s].image->Release();  // 释放纹理对象
			m_textureList[s].image = NULL;      // 纹理对象指向为空
		}
	}

	m_numTextures = 0;   // 数目赋值为0

	if (m_textureList)
	{
		// 销毁纹理对象列表内存区域
		delete[] m_textureList;
	}
	m_textureList = NULL;  // 指向为空



	// 释放所有的字体对象
	for (int s = 0; s<m_totalFonts; s++)
	{
		if (m_fonts[s])
		{
			m_fonts[s]->Release();
			m_fonts[s] = NULL;
		}
	}
	m_totalFonts = 0;   // 字体对象数量清0

	if (m_fonts)
	{
		delete[] m_fonts;   // 删除字体对象内存块
	}
	m_fonts = NULL;     // 该指针指向为空


	// 释放所有的GUI内存块
	ReleaseAllGUIs();

	// 释放所有的X模型内存块
	ReleaseAllXModels();


	if (m_Device)
	{
		m_Device->Release();
	}
	if (m_Direct3D)
	{
		m_Direct3D->Release();
	}

	m_Device = NULL;
	m_Direct3D = NULL;
}


// 用于将后台缓存颜色设置为指定的颜色
void CD3DRenderer::SetClearCol(float r, float g, float b)
{
	m_Color = D3DCOLOR_COLORVALUE(r, g, b, 1.0f);
}


// 启动场景的绘制
void CD3DRenderer::StartRender(bool bColor, bool bDepth,
	bool bStencil)
{
	if (!m_Device) return;

	unsigned int buffers = 0;
	if (bColor) buffers |= D3DCLEAR_TARGET;
	if (bDepth) buffers |= D3DCLEAR_ZBUFFER;
	if (bStencil) buffers |= D3DCLEAR_STENCIL;

	if (FAILED(m_Device->Clear(0, NULL, buffers, m_Color, 1, 0)))
		return;
	if (FAILED(m_Device->BeginScene())) return;

	m_renderingScene = true;
}


// 用于清理已经启动的场景
void CD3DRenderer::ClearBuffers(bool bColor, bool bDepth,
	bool bStencil)
{
	if (!m_Device) return;

	unsigned int buffers = 0;
	if (bColor) buffers |= D3DCLEAR_TARGET;
	if (bDepth) buffers |= D3DCLEAR_ZBUFFER;
	if (bStencil) buffers |= D3DCLEAR_STENCIL;

	if (m_renderingScene) m_Device->EndScene();
	if (FAILED(m_Device->Clear(0, NULL, buffers, m_Color, 1, 0)))
		return;

	if (m_renderingScene)
		if (FAILED(m_Device->BeginScene())) return;
}


// 结束场景的绘制
void CD3DRenderer::EndRendering()
{
	if (!m_Device) return;

	m_Device->EndScene();
	m_Device->Present(NULL, NULL, NULL, NULL);

	m_renderingScene = false;
}

// 设置静态缓存对象中的一个对象内容，并将其添加到静态缓存列表中
int CD3DRenderer::CreateStaticBuffer(
	VertexType vType,   // 正在使用的顶点类型
	PrimType primType,  // 渲染该几何图形(例如：三角形列表、顶点)的指令
	int totalVerts,     // 几何图形中的顶点总数
	int totalIndices,   // 几何图形中的索引总数
	int stride,         // 单个顶点尺寸的幅度值
	void **data,
	unsigned int *indices,
	int *staticId       // 保存静态缓存ID的指针
)
{
	void *ptr;
	int index = m_numStaticBuffers;

	if (!m_staticBufferList)
	{
		m_staticBufferList = new stD3DStaticBuffer[1];
		if (!m_staticBufferList) return NE_FAIL;
	}
	else
	{
		stD3DStaticBuffer *temp;
		temp = new stD3DStaticBuffer[m_numStaticBuffers + 1];

		memcpy(temp, m_staticBufferList,
			sizeof(stD3DStaticBuffer) * m_numStaticBuffers);

		delete[] m_staticBufferList;
		m_staticBufferList = temp;
	}

	m_staticBufferList[index].numVerts = totalVerts;
	m_staticBufferList[index].numIndices = totalIndices;
	m_staticBufferList[index].primType = primType;
	m_staticBufferList[index].stride = stride;
	m_staticBufferList[index].fvf = CreateD3DFVF(vType);

	if (totalIndices > 0)
	{
		if (FAILED(m_Device->CreateIndexBuffer(sizeof(unsigned int) *
			totalIndices, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16,
			D3DPOOL_DEFAULT,
			&m_staticBufferList[index].ibPtr,
			NULL))) return NE_FAIL;

		if (FAILED(m_staticBufferList[index].ibPtr->Lock(0, 0,
			(void**)&ptr, 0))) return NE_FAIL;

		memcpy(ptr, indices, sizeof(unsigned int) * totalIndices);
		m_staticBufferList[index].ibPtr->Unlock();
	}
	else
	{
		m_staticBufferList[index].ibPtr = NULL;
	}

	if (FAILED(m_Device->CreateVertexBuffer(totalVerts * stride,
		D3DUSAGE_WRITEONLY, m_staticBufferList[index].fvf,
		D3DPOOL_DEFAULT, &m_staticBufferList[index].vbPtr,
		NULL))) return NE_FAIL;

	if (FAILED(m_staticBufferList[index].vbPtr->Lock(0, 0,
		(void**)&ptr, 0))) return NE_FAIL;

	memcpy(ptr, data, totalVerts * stride);
	m_staticBufferList[index].vbPtr->Unlock();


	// 保存 m_numStaticBuffers的当前静态缓存ID
	*staticId = m_numStaticBuffers;
	m_numStaticBuffers++;  // 静态缓存ID值递增1

	return NE_OK;
}

// 实现将与该参数ID相关的几何图形渲染到屏幕上
int CD3DRenderer::RenderStaticBuffer(int staticId)
{
	// 检查静态缓存ID是否有效
	if (staticId >= m_numStaticBuffers) return NE_FAIL;

	if (m_activeStaticBuffer != staticId)
	{
		if (m_staticBufferList[staticId].ibPtr != NULL)
			// 设置索引缓存
			m_Device->SetIndices(m_staticBufferList[staticId].ibPtr);

		// 设置顶点数据流的输入源
		m_Device->SetStreamSource(0,
			m_staticBufferList[staticId].vbPtr, 0,
			m_staticBufferList[staticId].stride);

		// 设置顶点格式
		m_Device->SetFVF(m_staticBufferList[staticId].fvf);

		// m_activeStaticBuffer获得当前的静态缓存ID
		m_activeStaticBuffer = staticId;
	}

	if (m_staticBufferList[staticId].ibPtr != NULL)
	{
		switch (m_staticBufferList[staticId].primType)
		{
		case POINT_LIST:
			if (FAILED(m_Device->DrawPrimitive(D3DPT_POINTLIST,
				0, m_staticBufferList[staticId].numVerts)))
				return NE_FAIL;
			break;

		case TRIANGLE_LIST:
			if (FAILED(m_Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0,
				0, m_staticBufferList[staticId].numVerts / 3,
				0, m_staticBufferList[staticId].numIndices)))
				return NE_FAIL;
			break;

		case TRIANGLE_STRIP:
			if (FAILED(m_Device->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0,
				0, m_staticBufferList[staticId].numVerts / 2,
				0, m_staticBufferList[staticId].numIndices)))
				return NE_FAIL;
			break;

		case TRIANGLE_FAN:
			if (FAILED(m_Device->DrawIndexedPrimitive(D3DPT_TRIANGLEFAN, 0,
				0, m_staticBufferList[staticId].numVerts / 2,
				0, m_staticBufferList[staticId].numIndices)))
				return NE_FAIL;
			break;

		case LINE_LIST:
			if (FAILED(m_Device->DrawIndexedPrimitive(D3DPT_LINELIST, 0,
				0, m_staticBufferList[staticId].numVerts / 2,
				0, m_staticBufferList[staticId].numIndices)))
				return NE_FAIL;
			break;

		case LINE_STRIP:
			if (FAILED(m_Device->DrawIndexedPrimitive(D3DPT_LINESTRIP, 0,
				0, m_staticBufferList[staticId].numVerts,
				0, m_staticBufferList[staticId].numIndices)))
				return NE_FAIL;
			break;

		default:
			return NE_FAIL;
		}
	}
	else
	{
		switch (m_staticBufferList[staticId].primType)
		{
		case POINT_LIST:
			if (FAILED(m_Device->DrawPrimitive(D3DPT_POINTLIST,
				0, m_staticBufferList[staticId].numVerts)))
				return NE_FAIL;
			break;

		case TRIANGLE_LIST:
			if (FAILED(m_Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0,
				(int)(m_staticBufferList[staticId].numVerts / 3))))
				return NE_FAIL;
			break;

		case TRIANGLE_STRIP:
			if (FAILED(m_Device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0,
				(int)(m_staticBufferList[staticId].numVerts / 2))))
				return NE_FAIL;
			break;

		case TRIANGLE_FAN:
			if (FAILED(m_Device->DrawPrimitive(D3DPT_TRIANGLEFAN, 0,
				(int)(m_staticBufferList[staticId].numVerts / 2))))
				return NE_FAIL;
			break;

		case LINE_LIST:
			if (FAILED(m_Device->DrawPrimitive(D3DPT_LINELIST, 0,
				m_staticBufferList[staticId].numVerts / 2)))
				return NE_FAIL;
			break;

		case LINE_STRIP:
			if (FAILED(m_Device->DrawPrimitive(D3DPT_LINESTRIP, 0,
				m_staticBufferList[staticId].numVerts)))
				return NE_FAIL;
			break;

		default:
			return NE_FAIL;
		}
	}

	return NE_OK;
}



// 用于创建平行投影矩阵
void CD3DRenderer::CalculateProjMatrix(float fov, float n, float f)
{
	if (!m_Device) return;
	D3DXMATRIX projection;

	D3DXMatrixPerspectiveFovLH(&projection, fov,
		(float)m_screenWidth / (float)m_screenHeight, n, f);

	// 从视图空间(摄象机空间)到到屏幕的投影转换
	m_Device->SetTransform(D3DTS_PROJECTION, &projection);
}


// 用于创建正交投影矩阵
void CD3DRenderer::CalculateOrthoMatrix(float n, float f)
{
	if (!m_Device) return;
	D3DXMATRIX ortho;

	D3DXMatrixOrthoLH(&ortho, (float)m_screenWidth,
		(float)m_screenHeight, n, f);

	// 从视图空间(摄象机空间)到到屏幕的投影转换
	m_Device->SetTransform(D3DTS_PROJECTION, &ortho);
}

// 设置材质
void CD3DRenderer::SetMaterial(stMaterial *mat)
{
	if (!mat || !m_Device) return;

	D3DMATERIAL9 m = { mat->diffuseR, mat->diffuseG,
		mat->diffuseB, mat->diffuseA,
		mat->ambientR, mat->ambientG,
		mat->ambientB, mat->ambientA,
		mat->specularR, mat->specularG,
		mat->specularB, mat->specularA,
		mat->emissiveR, mat->emissiveG,
		mat->emissiveB, mat->emissiveA,
		mat->power
	};

	m_Device->SetMaterial(&m);
}

// 设置并启用光源
void CD3DRenderer::SetLight(stLight *light, int index)
{
	if (!light || !m_Device || index < 0) return;

	D3DLIGHT9 l;

	l.Ambient.a = light->ambientA;
	l.Ambient.r = light->ambientR;
	l.Ambient.g = light->ambientG;
	l.Ambient.b = light->ambientB;
	l.Attenuation0 = light->attenuation0;
	l.Attenuation1 = light->attenuation1;
	l.Attenuation2 = light->attenuation2;
	l.Diffuse.a = light->diffuseA;
	l.Diffuse.r = light->diffuseR;
	l.Diffuse.g = light->diffuseG;
	l.Diffuse.b = light->diffuseB;
	l.Direction.x = light->dirX;
	l.Direction.y = light->dirY;
	l.Direction.z = light->dirZ;
	l.Falloff = light->falloff;
	l.Phi = light->phi;
	l.Position.x = light->posX;
	l.Position.y = light->posY;
	l.Position.z = light->posZ;
	l.Range = light->range;
	l.Specular.a = light->specularA;
	l.Specular.r = light->specularR;
	l.Specular.g = light->specularG;
	l.Specular.b = light->specularB;
	l.Theta = light->theta;


	if (light->type == LIGHT_POINT ) l.Type = D3DLIGHT_POINT;
	else if (light->type == LIGHT_SPOT ) l.Type = D3DLIGHT_SPOT;
	else l.Type = D3DLIGHT_DIRECTIONAL;

	m_Device->SetLight(index, &l);
	m_Device->LightEnable(index, TRUE);
}


// 关闭光源
void CD3DRenderer::DisableLight(int index)
{
	if (!m_Device) return;

	m_Device->LightEnable(index, FALSE);
}

void CD3DRenderer::SetTransparency(RenderState state, TransState src,
	TransState dst)
{
	if (!m_Device) return;

	if (state == TRANSPARENCY_NONE)
	{
		m_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
		return;
	}

	if (state == TRANSPARENCY_ENABLE)
	{
		m_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);

		switch (src)
		{
		case TRANS_ZERO:
			m_Device->SetRenderState(D3DRS_SRCBLEND,
				D3DBLEND_ZERO);
			break;

		case TRANS_ONE:
			m_Device->SetRenderState(D3DRS_SRCBLEND,
				D3DBLEND_ONE);
			break;

		case TRANS_SRCCOLOR:
			m_Device->SetRenderState(D3DRS_SRCBLEND,
				D3DBLEND_SRCCOLOR);
			break;

		case TRANS_INVSRCCOLOR:
			m_Device->SetRenderState(D3DRS_SRCBLEND,
				D3DBLEND_INVSRCCOLOR);
			break;

		case TRANS_SRCALPHA:
			m_Device->SetRenderState(D3DRS_SRCBLEND,
				D3DBLEND_SRCALPHA);
			break;

		case TRANS_INVSRCALPHA:
			m_Device->SetRenderState(D3DRS_SRCBLEND,
				D3DBLEND_INVSRCALPHA);
			break;

		case TRANS_DSTALPHA:
			m_Device->SetRenderState(D3DRS_SRCBLEND,
				D3DBLEND_DESTALPHA);
			break;

		case TRANS_INVDSTALPHA:
			m_Device->SetRenderState(D3DRS_SRCBLEND,
				D3DBLEND_INVDESTALPHA);
			break;

		case TRANS_DSTCOLOR:
			m_Device->SetRenderState(D3DRS_SRCBLEND,
				D3DBLEND_DESTCOLOR);
			break;

		case TRANS_INVDSTCOLOR:
			m_Device->SetRenderState(D3DRS_SRCBLEND,
				D3DBLEND_INVDESTCOLOR);
			break;

		case TRANS_SRCALPHASAT:
			m_Device->SetRenderState(D3DRS_SRCBLEND,
				D3DBLEND_SRCALPHASAT);
			break;

		case TRANS_BOTHSRCALPHA:
			m_Device->SetRenderState(D3DRS_SRCBLEND,
				D3DBLEND_BOTHSRCALPHA);
			break;

		case TRANS_INVBOTHSRCALPHA:
			m_Device->SetRenderState(D3DRS_SRCBLEND,
				D3DBLEND_BOTHINVSRCALPHA);
			break;

		case TRANS_BLENDFACTOR:
			m_Device->SetRenderState(D3DRS_SRCBLEND,
				D3DBLEND_BLENDFACTOR);
			break;

		case TRANS_INVBLENDFACTOR:
			m_Device->SetRenderState(D3DRS_SRCBLEND,
				D3DBLEND_INVBLENDFACTOR);
			break;

		default:
			m_Device->SetRenderState(D3DRS_ALPHABLENDENABLE,
				false);
			return;
			break;
		}

		switch (dst)
		{
		case TRANS_ZERO:
			m_Device->SetRenderState(D3DRS_DESTBLEND,
				D3DBLEND_ZERO);
			break;

		case TRANS_ONE:
			m_Device->SetRenderState(D3DRS_DESTBLEND,
				D3DBLEND_ONE);
			break;

		case TRANS_SRCCOLOR:
			m_Device->SetRenderState(D3DRS_DESTBLEND,
				D3DBLEND_SRCCOLOR);
			break;

		case TRANS_INVSRCCOLOR:
			m_Device->SetRenderState(D3DRS_DESTBLEND,
				D3DBLEND_INVSRCCOLOR);
			break;

		case TRANS_SRCALPHA:
			m_Device->SetRenderState(D3DRS_DESTBLEND,
				D3DBLEND_SRCALPHA);
			break;

		case TRANS_INVSRCALPHA:
			m_Device->SetRenderState(D3DRS_DESTBLEND,
				D3DBLEND_INVSRCALPHA);
			break;

		case TRANS_DSTALPHA:
			m_Device->SetRenderState(D3DRS_DESTBLEND,
				D3DBLEND_DESTALPHA);
			break;

		case TRANS_INVDSTALPHA:
			m_Device->SetRenderState(D3DRS_DESTBLEND,
				D3DBLEND_INVDESTALPHA);
			break;

		case TRANS_DSTCOLOR:
			m_Device->SetRenderState(D3DRS_DESTBLEND,
				D3DBLEND_DESTCOLOR);
			break;

		case TRANS_INVDSTCOLOR:
			m_Device->SetRenderState(D3DRS_DESTBLEND,
				D3DBLEND_INVDESTCOLOR);
			break;

		case TRANS_SRCALPHASAT:
			m_Device->SetRenderState(D3DRS_DESTBLEND,
				D3DBLEND_SRCALPHASAT);
			break;

		case TRANS_BOTHSRCALPHA:
			m_Device->SetRenderState(D3DRS_DESTBLEND,
				D3DBLEND_BOTHSRCALPHA);
			break;

		case TRANS_INVBOTHSRCALPHA:
			m_Device->SetRenderState(D3DRS_DESTBLEND,
				D3DBLEND_BOTHINVSRCALPHA);
			break;

		case TRANS_BLENDFACTOR:
			m_Device->SetRenderState(D3DRS_DESTBLEND,
				D3DBLEND_BLENDFACTOR);
			break;

		case TRANS_INVBLENDFACTOR:
			m_Device->SetRenderState(D3DRS_DESTBLEND,
				D3DBLEND_INVBLENDFACTOR);
			break;

		default:
			m_Device->SetRenderState(D3DRS_ALPHABLENDENABLE,
				false);
			break;
		}
	}
}

// 添加2D纹理
int CD3DRenderer::AddTexture2D(
	char *file,  // 图像文件名
	int *texId   // 存储新创建的纹理对象的纹理ID
)
{
	if (!file || !m_Device) return NE_FAIL;

	// 获取文件名的字节长度
	int len = strlen(file);
	if (!len) return NE_FAIL;

	// 获取纹理对象数目
	int index = m_numTextures;

	// 如果 m_textureList对象列表为空，则表明是第一次创建
	if (!m_textureList)
	{
		// m_textureList获得了动态开辟的一段有1个元素的stD3DTexture结构数组内存块
		m_textureList = new stD3DTexture[1];

		if (!m_textureList) return NE_FAIL;
	}
	else
	{
		stD3DTexture *temp;   // 临时对象列表

							  // temp获得比现有对象列表大1位的内存块
		temp = new stD3DTexture[m_numTextures + 1];

		// 将 m_textureList内存块的数据复制到temp中
		memcpy(temp, m_textureList,
			sizeof(stD3DTexture) * m_numTextures);

		delete[] m_textureList;  // 销毁对象列表 m_textureList的内存块
		m_textureList = temp;    // 对象列表 m_textureList获得了新的对象列表数据
	}

	// 开辟fileName的内存区域
	m_textureList[index].fileName = new char[len];

	// 将图像文件名复制到 m_textureList[index].fileName
	memcpy(m_textureList[index].fileName, file, len);

	D3DCOLOR colorkey = 0xff000000;
	D3DXIMAGE_INFO info;

	// 从文件加载纹理图像
	if (D3DXCreateTextureFromFileEx(m_Device, (file), 0, 0, 0, 0,
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT,
		D3DX_DEFAULT, colorkey, &info, NULL,
		&m_textureList[index].image) != D3D_OK) return false;

	m_textureList[index].image->SetAutoGenFilterType(D3DTEXF_LINEAR);
	m_textureList[index].image->GenerateMipSubLevels();   // 生成mipmaps

	m_textureList[index].width = info.Width;    // 在对象中保存纹理图像的宽度
	m_textureList[index].height = info.Height;  // 在对象中保存纹理图像的高度

	*texId = m_numTextures;  // 保存纹理ID
	m_numTextures++;         // 增加纹理总数

	return NE_OK;
}

// 设置纹理过滤器
void CD3DRenderer::SetTextureFilter(
	int index,   // 纹理单位
	int filter,  // 滤波器模式
	int val      // 滤波器值
)
{
	if (!m_Device || index < 0) return;

	D3DSAMPLERSTATETYPE fil = D3DSAMP_MINFILTER;
	int v = D3DTEXF_POINT;

	if (filter == MIN_FILTER) fil = D3DSAMP_MINFILTER;
	if (filter == MAG_FILTER) fil = D3DSAMP_MAGFILTER;
	if (filter == MIP_FILTER) fil = D3DSAMP_MIPFILTER;

	if (val == POINT_TYPE) v = D3DTEXF_POINT;
	if (val == LINEAR_TYPE) v = D3DTEXF_LINEAR;
	if (val == ANISOTROPIC_TYPE) v = D3DTEXF_ANISOTROPIC;

	m_Device->SetSamplerState(index, fil, v);
}

// 设置多纹理贴图
void CD3DRenderer::SetMultiTexture()
{
	if (!m_Device) return;

	m_Device->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
	m_Device->SetTextureStageState(0, D3DTSS_COLOROP,
		D3DTOP_MODULATE);
	m_Device->SetTextureStageState(0, D3DTSS_COLORARG1,
		D3DTA_TEXTURE);
	m_Device->SetTextureStageState(0, D3DTSS_COLORARG2,
		D3DTA_DIFFUSE);

	m_Device->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1);
	m_Device->SetTextureStageState(1, D3DTSS_COLOROP,
		D3DTOP_MODULATE);
	m_Device->SetTextureStageState(1, D3DTSS_COLORARG1,
		D3DTA_TEXTURE);
	m_Device->SetTextureStageState(1, D3DTSS_COLORARG2,
		D3DTA_CURRENT);
}

// 应用纹理
void CD3DRenderer::ApplyTexture(int index, int texId)
{
	if (!m_Device) return;

	if (index < 0 || texId < 0)
		m_Device->SetTexture(0, NULL);
	else
		m_Device->SetTexture(0, m_textureList[texId].image);
}

// 保存屏幕截图
void CD3DRenderer::SaveScreenShot(char *file)
{
	if (!file) return;

	LPDIRECT3DSURFACE9 surface = NULL;
	D3DDISPLAYMODE disp;

	// 获得m_Device设备所指向的场景的显示模式
	m_Device->GetDisplayMode(0, &disp);

	// 创建表面
	m_Device->CreateOffscreenPlainSurface(disp.Width, disp.Height,
		D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &surface, NULL);

	// 使用渲染好的场景填充表面 surface
	m_Device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &surface);

	// 保存该表面数据为指定文件
	D3DXSaveSurfaceToFile(file, D3DXIFF_JPG, surface, NULL, NULL);

	if (surface != NULL) surface->Release();
	surface = NULL;
}

// 启用点状sprite
void CD3DRenderer::EnablePointSprites(
	float size,  // sprite的尺寸
	float min,   // 最小尺寸
	float a,     // 刻度值
	float b,     // 刻度值
	float c      // 刻度值
)
{
	if (!m_Device) return;

	m_Device->SetRenderState(D3DRS_POINTSPRITEENABLE, TRUE);    // 启用点状sprite
	m_Device->SetRenderState(D3DRS_POINTSCALEENABLE, TRUE);     // 启用点状sprite的比例
	m_Device->SetRenderState(D3DRS_POINTSIZE, FtoDW(size));     // 设置sprite的尺寸
	m_Device->SetRenderState(D3DRS_POINTSIZE_MIN, FtoDW(min));  // 设置点状sprite的最小尺寸
	m_Device->SetRenderState(D3DRS_POINTSCALE_A, FtoDW(a));     // 根据距离更改点状sprite的形状
	m_Device->SetRenderState(D3DRS_POINTSCALE_B, FtoDW(b));     // 根据距离更改点状sprite的形状
	m_Device->SetRenderState(D3DRS_POINTSCALE_C, FtoDW(c));     // 根据距离更改点状sprite的形状
}

// 禁用点状sprite
void CD3DRenderer::DisablePointSprites()
{
	m_Device->SetRenderState(D3DRS_POINTSPRITEENABLE, FALSE);   // 禁用sprite
	m_Device->SetRenderState(D3DRS_POINTSCALEENABLE, FALSE);    // 禁用sprite比例
}

bool CD3DRenderer::CreateText(char *font, int weight, bool italic,
	int size, int &id)
{
	if (!m_fonts)
	{
		m_fonts = new LPD3DXFONT[1];
		if (!m_fonts) return NE_FAIL;
	}
	else
	{
		LPD3DXFONT *temp;
		temp = new LPD3DXFONT[m_totalFonts + 1];

		memcpy(temp, m_fonts,
			sizeof(LPD3DXFONT) * m_totalFonts);

		delete[] m_fonts;
		m_fonts = temp;
	}

	if (FAILED(D3DXCreateFont(m_Device, size, 0, weight, 1, italic,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE, font,
		&m_fonts[m_totalFonts]))) return false;

	id = m_totalFonts;
	m_totalFonts++;

	return true;
}


void CD3DRenderer::DisplayText(int id, long x, long y,
	int r, int g, int b, char *text, ...)
{
	RECT FontPosition = { x, y, m_screenWidth, m_screenWidth };
	char message[1024];
	va_list argList;

	if (id >= m_totalFonts) return;

	va_start(argList, text);
	vsprintf_s(message, text, argList);
	va_end(argList);

	m_fonts[id]->DrawText(NULL, message, -1, &FontPosition,
		DT_SINGLELINE, D3DCOLOR_ARGB(255, r, g, b));
}


void CD3DRenderer::DisplayText(int id, long x, long y,
	unsigned long color, char *text, ...)
{
	RECT FontPosition = { x, y, m_screenWidth, m_screenWidth };
	char message[1024];
	va_list argList;

	if (id >= m_totalFonts) return;

	va_start(argList, text);
	vsprintf_s(message, text, argList);
	va_end(argList);

	m_fonts[id]->DrawText(NULL, message, -1, &FontPosition,
		DT_SINGLELINE, color);
}

// 添加背景图控件
bool CD3DRenderer::AddGUIBackdrop(int guiId, char *fileName)
{
	if (guiId >= m_totalGUIs)
	{
		return false;
	}

	int texID = -1, staticID = -1;

	// 添加纹理对象到纹理对象链表中
	if (!AddTexture2D(fileName, &texID))
	{
		return false;
	}

	unsigned long col = D3DCOLOR_XRGB(255, 255, 255);

	// 填充GUI顶点结构数组
	stGUIVertex obj[] =
	{
		{ (float)m_screenWidth, 0, 0, 1, col, 1, 0 },
		{ (float)m_screenWidth, (float)m_screenHeight, 0, 1, col, 1, 1 },
		{ 0, 0, 0, 1, col, 0, 0 },
		{ 0, (float)m_screenHeight, 0, 1, col, 0, 1 },
	};

	// 创建静态缓存到链表中
	if (!CreateStaticBuffer(GUI_FVF, TRIANGLE_STRIP, 4, 0,
		sizeof(stGUIVertex), (void**)&obj, NULL,
		&staticID))
	{
		return false;
	}

	// 添加背景图控件到链表，并返回布尔值
	return m_guiList[guiId].AddBackdrop(texID, staticID);
}


// 添加静态文本控件
bool CD3DRenderer::AddGUIStaticText(int guiId, int id, char *text,
	int x, int y, unsigned long color, int fontID)
{
	if (guiId >= m_totalGUIs)
	{
		return false;
	}

	// 添加静态文本控件到链表，并返回布尔值
	return m_guiList[guiId].AddStaticText(id, text, x, y, color, fontID);
}


// 添加按钮控件
bool CD3DRenderer::AddGUIButton(int guiId, int id, int x, int y,
	char *up, char *over, char *down)
{
	if (guiId >= m_totalGUIs)
	{
		return false;
	}

	int upID = -1, overID = -1, downID = -1, staticID = -1;

	// 添加按钮弹起状态的纹理对象
	if (!AddTexture2D(up, &upID))
	{
		return false;
	}
	// 添加鼠标停靠在按钮上的状态的纹理对象
	if (!AddTexture2D(over, &overID))
	{
		return false;
	}
	// 添加按钮被按下状态的纹理对象
	if (!AddTexture2D(down, &downID))
	{
		return false;
	}

	unsigned long col = D3DCOLOR_XRGB(255, 255, 255);

	int w = m_textureList[upID].width;
	int h = m_textureList[upID].height;

	// 填充GUI顶点结构
	stGUIVertex obj[] =
	{
		{ (float)(w + x), (float)(0 + y), 0, 1, col, 1, 0 },
		{ (float)(w + x), (float)(h + y), 0, 1, col, 1, 1 },
		{ (float)(0 + x), (float)(0 + y), 0, 1, col, 0, 0 },
		{ (float)(0 + x), (float)(h + y), 0, 1, col, 0, 1 },
	};

	// 创建顶点缓存到链表中
	if (!CreateStaticBuffer(GUI_FVF, TRIANGLE_STRIP, 4, 0,
		sizeof(stGUIVertex), (void**)&obj, NULL,
		&staticID))
	{
		return false;
	}

	// 添加按钮控件，并返回布尔值
	return m_guiList[guiId].AddButton(id, x, y, w, h, upID, overID, downID, staticID);
}

void CD3DRenderer::SetGUIVisble(int guiId, int id, bool visible)
{
	m_guiList[guiId].SetVisisble(id, visible);
}

// 绘制GUI控件
void CD3DRenderer::ProcessGUI(
	int guiId,      // GUI控件对象ID
	bool LMBDown,   // 鼠标左键是否被按下
	int mouseX, int mouseY,  // 鼠标的X、Y坐标
	void(*funcPtr)(int id, int state)  // 指向回调函数
)
{
	if (guiId >= m_totalGUIs || !m_Device)
	{
		return;
	}

	// gui指向GUI系统对象数组中的guiId系统对象
	CGUISystem *gui = &m_guiList[guiId];

	if (!gui)
	{
		return;
	}

	// backDrop指向获得的背景图控件对象
	stGUIControl *backDrop = gui->GetBackDrop();

	// 绘制背景图纹理
	if (backDrop && backDrop->m_visible)
	{
		ApplyTexture(0, backDrop->m_upTex);  // 设置背景图纹理
		RenderStaticBuffer(backDrop->m_listID);          // 绘制该对象纹理
		ApplyTexture(0, -1);                 // ??
	}

	// Initial button state.
	int status = NE_BUTTON_UP;

	// 遍历GUI控件对象数组，并逐个
	for (int i = 0; i<gui->GetTotalControls(); i++)
	{
		// 指向第i个GUI控件对象
		stGUIControl *pCnt = gui->GetGUIControl(i);

		if (!pCnt)
		{
			continue;
		}

		//不可见不渲染
		if (!pCnt->m_visible)continue;

		// 匹配控件对象的类型，并做出响应
		switch (pCnt->m_type)
		{
			// 静态文本控件
		case NE_GUI_STATICTEXT:
			// 绘制该文本
			DisplayText(pCnt->m_listID, pCnt->m_xPos, pCnt->m_yPos,
				pCnt->m_color, pCnt->m_text);
			break;

			// 按钮控件
		case NE_GUI_BUTTON:
			status = NE_BUTTON_UP;  // 设置按钮为弹起状态

									 // 启用纹理混合运算
			m_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

			// 设置源混合因子
			m_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);

			// 设置目标混合因子
			m_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

			// 若鼠标在按纽控件范围内
			if (mouseX > pCnt->m_xPos && mouseX < (pCnt->m_xPos + pCnt->m_width)
				&& mouseY > pCnt->m_yPos && mouseY < (pCnt->m_yPos + pCnt->m_height))
			{
				// 若鼠标左键被按下
				if (LMBDown)
				{
					// 设置为按下状态
					status = NE_BUTTON_DOWN;
				}
				else
				{
					// 设置为停靠状态
					status = NE_BUTTON_OVER;
				}
			}

			// 若按钮为弹起状态
			if (status == NE_BUTTON_UP)
			{
				// 设置弹起状态的纹理
				ApplyTexture(0, pCnt->m_upTex);
			}
			// 若鼠标为停靠状态
			if (status == NE_BUTTON_OVER)
			{
				// 设置停靠状态的纹理
				ApplyTexture(0, pCnt->m_overTex);
			}
			// 若按钮为被按下状态
			if (status == NE_BUTTON_DOWN)
			{
				// 设置按钮被按下状态的纹理
				ApplyTexture(0, pCnt->m_downTex);
			}

			// 绘制按钮
			RenderStaticBuffer(pCnt->m_listID);

			// 禁用混合运算
			m_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
			break;
		}

		// 调用回调函数，以对按钮事件做出响应
		if (funcPtr)
		{
			funcPtr(pCnt->m_id, status);
		}
	}
}

void CD3DRenderer::EnableFog(float start, float end,
	NE_FOG_TYPE type, unsigned long color, bool rangeFog)
{
	if (!m_Device) return;

	D3DCAPS9 caps;
	m_Device->GetDeviceCaps(&caps);

	// Set fog properties.
	m_Device->SetRenderState(D3DRS_FOGENABLE, true);
	m_Device->SetRenderState(D3DRS_FOGCOLOR, color);

	// Start and end dist of fog.
	m_Device->SetRenderState(D3DRS_FOGSTART, *(DWORD*)(&start));
	m_Device->SetRenderState(D3DRS_FOGEND, *(DWORD*)(&end));

	// Set based on type.
	if (type == NE_VERTEX_FOG)
		m_Device->SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR);
	else
		m_Device->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_LINEAR);

	// Can only use if hardware supports it.
	if (caps.RasterCaps & D3DPRASTERCAPS_FOGRANGE)
	{
		if (rangeFog)
			m_Device->SetRenderState(D3DRS_RANGEFOGENABLE, true);
		else
			m_Device->SetRenderState(D3DRS_RANGEFOGENABLE, false);
	}
}


void CD3DRenderer::DisableFog()
{
	if (!m_Device) return;

	// Set fog properties.
	m_Device->SetRenderState(D3DRS_FOGENABLE, false);
}

// 设置细节映射
void CD3DRenderer::SetDetailMapping()
{
	if (!m_Device) return;

	m_Device->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
	m_Device->SetTextureStageState(0, D3DTSS_COLOROP,
		D3DTOP_SELECTARG1);
	m_Device->SetTextureStageState(0, D3DTSS_COLORARG1,
		D3DTA_TEXTURE);
	m_Device->SetTextureStageState(0, D3DTSS_COLORARG2,
		D3DTA_DIFFUSE);

	m_Device->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1);
	m_Device->SetTextureStageState(1, D3DTSS_COLOROP,
		D3DTOP_ADDSIGNED);
	m_Device->SetTextureStageState(1, D3DTSS_COLORARG1,
		D3DTA_TEXTURE);
	m_Device->SetTextureStageState(1, D3DTSS_COLORARG2,
		D3DTA_CURRENT);
}

// 加载UMF文件模型到静态缓存
int CD3DRenderer::LoadUMFAsStaticBuffer(
	char *file,     // 模型文件名
	int meshIndex,  // 网格的索引
	int *staticID   // 保存静态缓存ID
)
{
	void *ptr;
	int index = m_numStaticBuffers;

	stUMFModel *model;
	int numModels = 0;
	int stride = sizeof(stModelVertex);

	// 加载UMF文件模型
	if (!LoadUMF(file, &model, &numModels))
	{
		return NE_FAIL;
	}

	if (!numModels || !model)
	{
		return NE_FAIL;
	}

	if (meshIndex < 0 || meshIndex >= numModels)
	{
		return NE_FAIL;
	}

	// 加载的首个模型的情况下
	if (!m_staticBufferList)
	{
		// m_staticBufferList指向动态开辟的只有一个元素的静态缓存链表
		m_staticBufferList = new stD3DStaticBuffer[1];

		if (!m_staticBufferList)
		{
			return NE_FAIL;
		}
	}
	// 否则在链表尾部添加该模型
	else
	{
		stD3DStaticBuffer *temp;  // 临时静态缓存指针
								  // 指向比当前链多一个元素的链表
		temp = new stD3DStaticBuffer[m_numStaticBuffers + 1];

		// 复制进temp中
		memcpy(temp, m_staticBufferList, sizeof(stD3DStaticBuffer) * m_numStaticBuffers);

		// 删除旧的链表
		delete[] m_staticBufferList;

		// 用新的链表覆盖旧的链表
		m_staticBufferList = temp;
	}

	m_staticBufferList[index].numVerts = model[meshIndex].totalVertices;
	m_staticBufferList[index].numIndices = model[meshIndex].totalFaces;
	m_staticBufferList[index].primType = TRIANGLE_LIST;
	m_staticBufferList[index].stride = stride;
	m_staticBufferList[index].fvf = CreateD3DFVF(MV_FVF);

	// 若有外观，则将其复制进临时链表ptr
	if (model[meshIndex].totalFaces > 0)
	{
		int size = model[meshIndex].totalFaces * 3;
		unsigned int *idx = new unsigned int[size];

		for (int i = 0, f = 0; i < size; i += 3)
		{
			idx[i + 0] = model[meshIndex].faces[f].indices[0];
			idx[i + 1] = model[meshIndex].faces[f].indices[1];
			idx[i + 2] = model[meshIndex].faces[f].indices[2];
			f++;
		}

		if (FAILED(m_Device->CreateIndexBuffer(sizeof(unsigned int) *
			size, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16,
			D3DPOOL_DEFAULT, &m_staticBufferList[index].ibPtr,
			NULL)))
		{
			return NE_FAIL;
		}

		if (FAILED(m_staticBufferList[index].ibPtr->Lock(0, 0,
			(void**)&ptr, 0)))
		{
			return NE_FAIL;
		}

		memcpy(ptr, idx, sizeof(unsigned int) * size);
		m_staticBufferList[index].ibPtr->Unlock();

		delete[] idx;
	}
	// 否则将索引缓存设置为NULL
	else
	{
		m_staticBufferList[index].ibPtr = NULL;
	}
	int totalVerts = model[meshIndex].totalVertices;
	stModelVertex *data = new stModelVertex[totalVerts];

	for (int i = 0; i < totalVerts; i++)
	{
		data[i].x = model[meshIndex].vertices[i].x;
		data[i].y = model[meshIndex].vertices[i].y;
		data[i].z = model[meshIndex].vertices[i].z;

		if (model[meshIndex].normals)
		{
			data[i].nx = model[meshIndex].normals[i].x;
			data[i].ny = model[meshIndex].normals[i].y;
			data[i].nz = model[meshIndex].normals[i].z;
		}
		else
		{
			data[i].nx = data[i].ny = data[i].nz = 0;
		}

		if (model[meshIndex].colors)
		{
			data[i].color = NECOLOR_ARGB(255,
				(int)model[meshIndex].colors[i].x,
				(int)model[meshIndex].colors[i].y,
				(int)model[meshIndex].colors[i].z);
		}
		else
		{
			data[i].color = NECOLOR_ARGB(255, 255, 255, 255);
		}

		if (model[meshIndex].texCoords)
		{
			data[i].tu = model[meshIndex].texCoords[i].tu;
			data[i].tv = model[meshIndex].texCoords[i].tv;
		}
		else
		{
			data[i].tu = data[i].tv = 0;
		}
	}

	if (FAILED(m_Device->CreateVertexBuffer(totalVerts * stride,
		D3DUSAGE_WRITEONLY, m_staticBufferList[index].fvf,
		D3DPOOL_DEFAULT, &m_staticBufferList[index].vbPtr,
		NULL)))
	{
		return NE_FAIL;
	}

	if (FAILED(m_staticBufferList[index].vbPtr->Lock(0, 0,
		(void**)&ptr, 0)))
	{
		return NE_FAIL;
	}

	memcpy(ptr, data, totalVerts * stride);
	m_staticBufferList[index].vbPtr->Unlock();

	if (data)
	{
		delete[] data;
	}

	data = NULL;

	FreeModel(model, numModels);

	if (model)
	{
		delete[] model;
	}

	model = NULL;

	*staticID = m_numStaticBuffers;
	m_numStaticBuffers++;

	return NE_OK;
}

int CD3DRenderer::LoadUMFAsStaticBuffer(char *file, int meshIndex, int staticID)
{
	void *ptr;

	if (staticID < 0 || staticID >= m_numStaticBuffers)
		return NE_FAIL;

	stUMFModel *model;
	int numModels = 0;
	int stride = sizeof(stModelVertex);

	if (!LoadUMF(file, &model, &numModels))
		return NE_FAIL;

	if (!numModels || !model)
		return NE_FAIL;

	if (meshIndex < 0 || meshIndex >= numModels)
		return NE_FAIL;

	ReleaseStaticBuffer(staticID);

	m_staticBufferList[staticID].numVerts = model[meshIndex].totalVertices;
	m_staticBufferList[staticID].numIndices = model[meshIndex].totalFaces;
	m_staticBufferList[staticID].primType = TRIANGLE_LIST;
	m_staticBufferList[staticID].stride = stride;
	m_staticBufferList[staticID].fvf = CreateD3DFVF(MV_FVF);

	if (model[meshIndex].totalFaces > 0)
	{
		int size = model[meshIndex].totalFaces * 3;
		unsigned int *idx = new unsigned int[size];

		for (int i = 0, f = 0; i < size; i += 3)
		{
			idx[i + 0] = model[meshIndex].faces[f].indices[0];
			idx[i + 1] = model[meshIndex].faces[f].indices[1];
			idx[i + 2] = model[meshIndex].faces[f].indices[2];
			f++;
		}

		if (FAILED(m_Device->CreateIndexBuffer(sizeof(unsigned int) *
			size, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16,
			D3DPOOL_DEFAULT, &m_staticBufferList[staticID].ibPtr,
			NULL))) return NE_FAIL;

		if (FAILED(m_staticBufferList[staticID].ibPtr->Lock(0, 0,
			(void**)&ptr, 0))) return NE_FAIL;

		memcpy(ptr, idx, sizeof(unsigned int) * size);
		m_staticBufferList[staticID].ibPtr->Unlock();

		delete[] idx;
	}
	else
	{
		m_staticBufferList[staticID].ibPtr = NULL;
	}

	int totalVerts = model[meshIndex].totalVertices;
	stModelVertex *data = new stModelVertex[totalVerts];

	for (int i = 0; i < totalVerts; i++)
	{
		data[i].x = model[meshIndex].vertices[i].x;
		data[i].y = model[meshIndex].vertices[i].y;
		data[i].z = model[meshIndex].vertices[i].z;

		if (model[meshIndex].normals)
		{
			data[i].nx = model[meshIndex].normals[i].x;
			data[i].ny = model[meshIndex].normals[i].y;
			data[i].nz = model[meshIndex].normals[i].z;
		}
		else
		{
			data[i].nx = data[i].ny = data[i].nz = 0;
		}

		if (model[meshIndex].colors)
		{
			data[i].color = NECOLOR_ARGB(255,
				(int)model[meshIndex].colors[i].x,
				(int)model[meshIndex].colors[i].y,
				(int)model[meshIndex].colors[i].z);
		}
		else
			data[i].color = NECOLOR_ARGB(255, 255, 255, 255);

		if (model[meshIndex].texCoords)
		{
			data[i].tu = model[meshIndex].texCoords[i].tu;
			data[i].tv = model[meshIndex].texCoords[i].tv;
		}
		else
		{
			data[i].tu = data[i].tv = 0;
		}
	}

	if (FAILED(m_Device->CreateVertexBuffer(totalVerts * stride,
		D3DUSAGE_WRITEONLY, m_staticBufferList[staticID].fvf,
		D3DPOOL_DEFAULT, &m_staticBufferList[staticID].vbPtr,
		NULL))) return NE_FAIL;

	if (FAILED(m_staticBufferList[staticID].vbPtr->Lock(0, 0,
		(void**)&ptr, 0))) return NE_FAIL;

	memcpy(ptr, data, totalVerts * stride);
	m_staticBufferList[staticID].vbPtr->Unlock();

	if (data) delete[] data;
	data = NULL;

	FreeModel(model, numModels);

	if (model) delete[] model;
	model = NULL;

	return NE_OK;
}

int CD3DRenderer::LoadOBJAsStaticBuffer(char * file, int * staticID)
{
	return 0;
}

int CD3DRenderer::LoadOBJAsStaticBuffer(char * file, int staticID)
{
	return 0;
}

// 加载X文件模型到链表中
int CD3DRenderer::LoadXModel(char *file, int *xModelId)
{
	if (!file)
	{
		return NE_FAIL;
	}

	if (!m_xModels)
	{
		m_xModels = new CXModel[1];
		if (!m_xModels)
		{
			return NE_FAIL;
		}
		m_xModels[0].SetDevice(m_Device);
	}
	else
	{
		CXModel *temp;
		temp = new CXModel[m_numXModels + 1];

		memcpy(temp, m_xModels, sizeof(CXModel) * m_numXModels);

		delete[] m_xModels;
		m_xModels = temp;

		m_xModels[m_numXModels].SetDevice(m_Device);
	}

	if (!m_xModels[m_numXModels].LoadXFile(file))
	{
		return NE_FAIL;
	}

	*xModelId = m_numXModels;
	m_numXModels++;

	return NE_OK;
}

int CD3DRenderer::LoadXModel(char *file, int xModelId)
{
	if (!file || xModelId < 0 || xModelId >= m_numXModels || !m_xModels)
	{
		return NE_FAIL;
	}

	// Release previous data.
	m_xModels[xModelId].Shutdown();

	// Load new data.
	if (!m_xModels[xModelId].LoadXFile(file))
	{
		return NE_FAIL;
	}

	return NE_OK;

}

// 循环系统中所有的静态缓存
// 并调用ReleaseStaticBuffer()函数释放各自的数据
void CD3DRenderer::ReleaseAllStaticBuffers()
{
	for (int s = 0; s < m_numStaticBuffers; s++)
	{
		ReleaseStaticBuffer(s);
	}

	m_numStaticBuffers = 0;
	if (m_staticBufferList)
	{
		delete[] m_staticBufferList;
	}
	m_staticBufferList = NULL;
}


// 释放所有X模型链表
void CD3DRenderer::ReleaseAllXModels()
{
	for (int s = 0; s < m_numXModels; s++)
	{
		m_xModels[s].Shutdown();
	}

	m_numXModels = 0;
	if (m_xModels)
	{
		delete[] m_xModels;
	}

	m_xModels = NULL;
}


// 释放单个静态缓存
int CD3DRenderer::ReleaseStaticBuffer(int staticID)
{
	if (staticID < 0 || staticID >= m_numStaticBuffers || !m_staticBufferList)
	{
		return NE_FAIL;
	}

	m_staticBufferList[staticID].fvf = 0;
	m_staticBufferList[staticID].numIndices = 0;
	m_staticBufferList[staticID].numVerts = 0;
	//m_staticBufferList[staticID].primType = 0;
	m_staticBufferList[staticID].stride = 0;

	if (m_staticBufferList[staticID].vbPtr)
	{
		m_staticBufferList[staticID].vbPtr->Release();
		m_staticBufferList[staticID].vbPtr = NULL;
	}

	if (m_staticBufferList[staticID].ibPtr)
	{
		m_staticBufferList[staticID].ibPtr->Release();
		m_staticBufferList[staticID].ibPtr = NULL;
	}

	return NE_OK;
}


// 释放单个X模型
int CD3DRenderer::ReleaseXModel(int xModelId)
{
	if (xModelId < 0 || xModelId >= m_numXModels || !m_xModels)
	{
		return NE_FAIL;
	}

	m_xModels[xModelId].Shutdown();

	return NE_OK;
}

// 渲染指定ID的X模型
int CD3DRenderer::RenderXModel(int xModelId)
{
	if (!m_xModels || xModelId >= m_numXModels || xModelId < 0)
	{
		return NE_FAIL;
	}

	m_Device->SetIndices(NULL);
	m_Device->SetStreamSource(0, NULL, 0, 0);
	m_Device->SetFVF(0);

	m_xModels[xModelId].Render();

	return NE_OK;
}

// 设置视图矩阵
void CD3DRenderer::ApplyView(CVector3 pos, CVector3 view, CVector3 up)
{
	if (!m_Device) return;

	// Define camera information.
	D3DXVECTOR3 cameraPos(pos.x, pos.y, pos.z);
	D3DXVECTOR3 lookAtPos(view.x, view.y, view.z);
	D3DXVECTOR3 upDir(up.x, up.y, up.z);

	// Build view matrix.
	D3DXMATRIX mat;
	D3DXMatrixLookAtLH(&mat, &cameraPos,
		&lookAtPos, &upDir);
	m_Device->SetTransform(D3DTS_VIEW, &mat);
}


// 设置世界矩阵
void CD3DRenderer::SetWorldMatrix(CMatrix4x4 *mat)
{
	if (!m_Device || !mat) return;

	m_Device->SetTransform(D3DTS_WORLD, (D3DXMATRIX*)mat->matrix);
}


// 根据摄象机数据计算矩阵
void CD3DRenderer::CameraToViewMatrix(CCamera *cam, CMatrix4x4 *mat)
{
	if (!cam || !mat) return;

	// Define camera information.
	D3DXVECTOR3 cameraPos(cam->m_pos.x, cam->m_pos.y, cam->m_pos.z);
	D3DXVECTOR3 lookAtPos(cam->m_view.x, cam->m_view.y, cam->m_view.z);
	D3DXVECTOR3 upDir(cam->m_up.x, cam->m_up.y, cam->m_up.z);

	// Build view matrix.
	D3DXMATRIX view;
	D3DXMatrixLookAtLH(&view, &cameraPos, &lookAtPos, &upDir);

	mat->matrix[0] = view._11;
	mat->matrix[1] = view._12;
	mat->matrix[2] = view._13;
	mat->matrix[3] = view._14;

	mat->matrix[4] = view._21;
	mat->matrix[5] = view._22;
	mat->matrix[6] = view._23;
	mat->matrix[7] = view._24;

	mat->matrix[8] = view._31;
	mat->matrix[9] = view._32;
	mat->matrix[10] = view._33;
	mat->matrix[11] = view._34;

	mat->matrix[12] = view._41;
	mat->matrix[13] = view._42;
	mat->matrix[14] = view._43;
	mat->matrix[15] = view._44;
}


// 获取世界矩阵，在平截头体选择计算时会使用它
void CD3DRenderer::GetProjectionMatrix(CMatrix4x4 *mat)
{
	if (!mat) return;

	mat->matrix[0] = m_projection._11;
	mat->matrix[1] = m_projection._12;
	mat->matrix[2] = m_projection._13;
	mat->matrix[3] = m_projection._14;

	mat->matrix[4] = m_projection._21;
	mat->matrix[5] = m_projection._22;
	mat->matrix[6] = m_projection._23;
	mat->matrix[7] = m_projection._24;

	mat->matrix[8] = m_projection._31;
	mat->matrix[9] = m_projection._32;
	mat->matrix[10] = m_projection._33;
	mat->matrix[11] = m_projection._34;

	mat->matrix[12] = m_projection._41;
	mat->matrix[13] = m_projection._42;
	mat->matrix[14] = m_projection._43;
	mat->matrix[15] = m_projection._44;
}
