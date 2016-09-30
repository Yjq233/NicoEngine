#include "gameEngine.h"
#include "main.h"
#include "menu.h"

CInputInterface *g_InputSystem = NULL;   // ����ϵͳ�ĳ������
CSoundSystemInterface *g_SoundSystem = NULL;
CRenderInterface *g_Render = NULL;// ��Ⱦ����Ķ���ָ�� g_Render

int g_arialID = -1;			  // ��������ID
int g_arial24ID = -1;		  // ��������ID
int g_arial36ID = -1;		  // ��������ID

// ������Ⱦϵͳ���󣬲���ʼ��Direct3D
bool InitializeEngine()
{
	if (!CreateD3DRenderer(&g_Render)) return false;   

	if (!g_Render->Initialize(WIN_WIDTH, WIN_HEIGHT,
		g_hwnd, FULLSCREEN, NE_MS_SAMPLES_4)) return false;

	g_Render->SetClearCol(0, 0, 0);
	g_Render->CalculateProjMatrix((float)PI / 4, 0.1f, 10000.0f);

	// ΪҪ��ʾ���ı�����Arial����
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

	// ��̬��������ϵͳCDirectInputSystem�����
	if (!CreateDIInput(&g_InputSystem, g_hwnd, g_hInstance, false))
	{
		return false;
	}

	// ����ϵͳ��ʼ��
	if (!g_InputSystem->Initialize())
	{
		return false;
	}

	// ��̬����CreateDIInput�����
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


// ���������ڴ����ٲ���
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
