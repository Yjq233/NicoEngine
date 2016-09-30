#include "gameEngine.h"
#include "main.h"
#include "menu.h"

CInputInterface *g_InputSystem = NULL;   // 输入系统的抽象基类
CSoundSystemInterface *g_SoundSystem = NULL;
CRenderInterface *g_Render = NULL;// 渲染基类的对象指针 g_Render

int g_arialID = -1;			  // 字体类型ID
int g_arial24ID = -1;		  // 字体类型ID
int g_arial36ID = -1;		  // 字体类型ID

// 创建渲染系统对象，并初始化Direct3D
bool InitializeEngine()
{
	if (!CreateD3DRenderer(&g_Render)) return false;   

	if (!g_Render->Initialize(WIN_WIDTH, WIN_HEIGHT,
		g_hwnd, FULLSCREEN, NE_MS_SAMPLES_4)) return false;

	g_Render->SetClearCol(0, 0, 0);
	g_Render->CalculateProjMatrix((float)PI / 4, 0.1f, 10000.0f);

	// 为要显示的文本创建Arial字体
	if (!g_Render->CreateText("Arial", 0, true, 18, g_arialID))
	{
		return false;
	}
	if (!g_Render->CreateText("Arial", 0, true, 24, g_arial24ID))
	{
		return false;
	}
	if (!g_Render->CreateText("Arial", 0, true, 36, g_arial36ID))
	{
		return false;
	}

	// 动态开辟输入系统CDirectInputSystem类对象
	if (!CreateDIInput(&g_InputSystem, g_hwnd, g_hInstance, false))
	{
		return false;
	}

	// 输入系统初始化
	if (!g_InputSystem->Initialize())
	{
		return false;
	}

	// 动态开辟CreateDIInput类对象
	if (!CreateDMSound(&g_SoundSystem))
	{
		return false;
	}

	if (!g_SoundSystem->Initialize(g_hwnd))
	{
		return false;
	}

	// Load menu sound.
	if (!g_SoundSystem->AddSound("sounds/button.wav", false,
		&g_buttonSound)) return false;
	if (!g_SoundSystem->AddSound("sounds/menu.wav", true,
		&g_menuSound)) return false;

	//// Play the sound.
	g_SoundSystem->Play(g_menuSound);

	return true;
}


// 引擎的相关内存销毁操作
void ShutdownEngine()
{
	if (g_Render)
	{
		g_Render->Shutdown();
		delete g_Render;
		g_Render = NULL;
	}

	if (g_InputSystem)
	{
		g_InputSystem->Shutdown();
		delete g_InputSystem;
		g_InputSystem = NULL;
	}

	if (g_SoundSystem)
	{
		g_SoundSystem->Shutdown();
		delete g_SoundSystem;
		g_SoundSystem = NULL;
	}
}
