#include "menu.h"
#include "gameEngine.h"
#include "main.h"
#include "game.h"

// GUI�ؼ�ID
int g_mainGui = -1;     // ���˵�����
int g_startGui = -1;    // ��ʼ�˵�����
int g_creditsGui = -1;  // ��Ļ�˵�����
int g_loadingGui = -1;  // ���ؽ���
int g_currentGUI = GUI_MAIN_SCREEN;   // ��ǰ�˵�����

// Sound id.
int g_menuSound = -1;
int g_buttonSound = -1;

// ���˵������ʼ��
bool InitializeMainMenu()
{
	// Create gui screens.
	if (!g_Render->CreateGUI(g_mainGui)) return false;
	if (!g_Render->CreateGUI(g_startGui)) return false;
	if (!g_Render->CreateGUI(g_creditsGui)) return false;
	if (!g_Render->CreateGUI(g_loadingGui)) return false;

	// Load backdrops.
	if (!g_Render->AddGUIBackdrop(g_mainGui, "menu/mainMenu.jpg"))
		return false;
	if (!g_Render->AddGUIBackdrop(g_startGui, "menu/startMenu.jpg"))
		return false;
	if (!g_Render->AddGUIBackdrop(g_creditsGui,
		"menu/creditsMenu.jpg")) return false;
	if (!g_Render->AddGUIBackdrop(g_loadingGui,
		"menu/loadingMenu.jpg")) return false;

	if (!g_Render->AddGUIStaticText(g_mainGui, STATIC_TEXT_ID,
		"Version: 1.0", PERCENT_OF(WIN_WIDTH, 0.85),
		PERCENT_OF(WIN_WIDTH, 0.05),
		NECOLOR_ARGB(255, 255, 255, 255), g_arialID)) return false;

	if (!g_Render->AddGUIButton(g_mainGui, BUTTON_START_ID,
		PERCENT_OF(WIN_WIDTH, 0.05), PERCENT_OF(WIN_HEIGHT, 0.40),
		"menu/startUp.png", "menu/StartOver.png",
		"menu/startDown.png")) return false;

	if (!g_Render->AddGUIButton(g_mainGui, BUTTON_CREDITS_ID,
		PERCENT_OF(WIN_WIDTH, 0.05), PERCENT_OF(WIN_HEIGHT, 0.50),
		"menu/creditsUp.png", "menu/creditsOver.png",
		"menu/creditsDown.png")) return false;

	if (!g_Render->AddGUIButton(g_mainGui, BUTTON_QUIT_ID,
		PERCENT_OF(WIN_WIDTH, 0.05), PERCENT_OF(WIN_HEIGHT, 0.60),
		"menu/quitUp.png", "menu/quitOver.png",
		"menu/quitDown.png")) return false;

	if (!g_Render->AddGUIButton(g_mainGui, BUTTON_MUSIC_ON_ID,
		PERCENT_OF(WIN_WIDTH, 0.85), PERCENT_OF(WIN_HEIGHT, 0.30),
		"menu/musicon.jpg", "menu/musicon.jpg",
		"menu/musicon.jpg")) return false;

	if (!g_Render->AddGUIButton(g_mainGui, BUTTON_MUSIC_OFF_ID,
		PERCENT_OF(WIN_WIDTH, 0.85), PERCENT_OF(WIN_HEIGHT, 0.50),
		"menu/musicoff.jpg", "menu/musicoff.jpg",
		"menu/musicoff.jpg")) return false;

	g_Render->SetGUIVisble(g_mainGui, BUTTON_MUSIC_OFF_ID, false);


	// Set start screen elements.
	if (!g_Render->AddGUIButton(g_startGui, BUTTON_LEVEL_1_ID,
		PERCENT_OF(WIN_WIDTH, 0.1), PERCENT_OF(WIN_HEIGHT, 0.15),
		"menu/level1Up.png", "menu/level1Over.png",
		"menu/level1Down.png")) return false;

	if (!g_Render->AddGUIButton(g_startGui, BUTTON_BACK_ID,
		PERCENT_OF(WIN_WIDTH, 0.1), PERCENT_OF(WIN_HEIGHT, 0.80),
		"menu/backUp.png", "menu/backOver.png",
		"menu/backDown.png")) return false;


	// Set credits screen elements.
	if (!g_Render->AddGUIStaticText(g_creditsGui, STATIC_TEXT_ID,
		"Game Design -",
		PERCENT_OF(WIN_WIDTH, 0.1), PERCENT_OF(WIN_HEIGHT, 0.15),
		NECOLOR_ARGB(255, 255, 255, 255),
		g_arialID)) return false;

	if (!g_Render->AddGUIStaticText(g_creditsGui, STATIC_TEXT_ID,
		"              Allen Sherrod",
		PERCENT_OF(WIN_WIDTH, 0.1), PERCENT_OF(WIN_HEIGHT, 0.20),
		NECOLOR_ARGB(255, 255, 255, 255),
		g_arialID)) return false;

	if (!g_Render->AddGUIStaticText(g_creditsGui, STATIC_TEXT_ID,
		"Programming -",
		PERCENT_OF(WIN_WIDTH, 0.1), PERCENT_OF(WIN_HEIGHT, 0.25),
		NECOLOR_ARGB(255, 255, 255, 255),
		g_arialID)) return false;

	if (!g_Render->AddGUIStaticText(g_creditsGui, STATIC_TEXT_ID,
		"              Allen Sherrod",
		PERCENT_OF(WIN_WIDTH, 0.1), PERCENT_OF(WIN_HEIGHT, 0.30),
		NECOLOR_ARGB(255, 255, 255, 255),
		g_arialID)) return false;

	if (!g_Render->AddGUIStaticText(g_creditsGui, STATIC_TEXT_ID,
		"Sound -",
		PERCENT_OF(WIN_WIDTH, 0.1), PERCENT_OF(WIN_HEIGHT, 0.35),
		NECOLOR_ARGB(255, 255, 255, 255),
		g_arialID)) return false;

	if (!g_Render->AddGUIStaticText(g_creditsGui, STATIC_TEXT_ID,
		"              Allen Sherrod",
		PERCENT_OF(WIN_WIDTH, 0.1), PERCENT_OF(WIN_HEIGHT, 0.40),
		NECOLOR_ARGB(255, 255, 255, 255),
		g_arialID)) return false;

	if (!g_Render->AddGUIStaticText(g_creditsGui, STATIC_TEXT_ID,
		"Level Design -",
		PERCENT_OF(WIN_WIDTH, 0.1), PERCENT_OF(WIN_HEIGHT, 0.45),
		NECOLOR_ARGB(255, 255, 255, 255),
		g_arialID)) return false;

	if (!g_Render->AddGUIStaticText(g_creditsGui, STATIC_TEXT_ID,
		"              Allen Sherrod",
		PERCENT_OF(WIN_WIDTH, 0.1), PERCENT_OF(WIN_HEIGHT, 0.50),
		NECOLOR_ARGB(255, 255, 255, 255),
		g_arialID)) return false;

	if (!g_Render->AddGUIStaticText(g_creditsGui, STATIC_TEXT_ID,
		"Speical Thanks -",
		PERCENT_OF(WIN_WIDTH, 0.1), PERCENT_OF(WIN_HEIGHT, 0.55),
		NECOLOR_ARGB(255, 255, 255, 255),
		g_arialID)) return false;

	if (!g_Render->AddGUIStaticText(g_creditsGui, STATIC_TEXT_ID,
		"              Jenifer Niles",
		PERCENT_OF(WIN_WIDTH, 0.1), PERCENT_OF(WIN_HEIGHT, 0.60),
		NECOLOR_ARGB(255, 255, 255, 255),
		g_arialID)) return false;

	if (!g_Render->AddGUIStaticText(g_creditsGui, STATIC_TEXT_ID,
		"              Bryan Davidson",
		PERCENT_OF(WIN_WIDTH, 0.1), PERCENT_OF(WIN_HEIGHT, 0.65),
		NECOLOR_ARGB(255, 255, 255, 255),
		g_arialID)) return false;

	if (!g_Render->AddGUIStaticText(g_creditsGui, STATIC_TEXT_ID,
		"              Charles River Media",
		PERCENT_OF(WIN_WIDTH, 0.1), PERCENT_OF(WIN_HEIGHT, 0.70),
		NECOLOR_ARGB(255, 255, 255, 255),
		g_arialID)) return false;

	if (!g_Render->AddGUIStaticText(g_creditsGui, STATIC_TEXT_ID,
		"              Readers of this book",
		PERCENT_OF(WIN_WIDTH, 0.1), PERCENT_OF(WIN_HEIGHT, 0.75),
		NECOLOR_ARGB(255, 255, 255, 255),
		g_arialID)) return false;

	if (!g_Render->AddGUIButton(g_creditsGui, BUTTON_BACK_ID,
		PERCENT_OF(WIN_WIDTH, 0.1), PERCENT_OF(WIN_HEIGHT, 0.80),
		"menu/backUp.png", "menu/backOver.png",
		"menu/backDown.png")) return false;

	return true;
}

// �˵�����Ļص���������ProcessGUI()�������ã�
void MainMenuCallback(int id, int state)
{
	switch (id)
	{
		// ��ʼ��ť�����µ����
	case BUTTON_START_ID:
		if (state == NE_BUTTON_DOWN)
		{
			g_SoundSystem->Play(g_buttonSound);
			g_currentGUI = GUI_START_SCREEN;
		}
		break;

		// ��Ļ��ť�����µ����
	case BUTTON_CREDITS_ID:
		if (state == NE_BUTTON_DOWN)
		{
			g_SoundSystem->Play(g_buttonSound);
			g_currentGUI = GUI_CREDITS_SCREEN;
		}
		break;

		// ���ذ�ť�����µ����
	case BUTTON_BACK_ID:
		if (state == NE_BUTTON_DOWN)
		{
			g_SoundSystem->Play(g_buttonSound);
			g_currentGUI = GUI_MAIN_SCREEN;
		}
		break;

		// �˳���ť�����µ����
	case BUTTON_QUIT_ID:
		if (state == NE_BUTTON_DOWN)
		{
			g_SoundSystem->Play(g_buttonSound);
			PostQuitMessage(0);
		}
		break;
		// �����رհ�ť����
	case BUTTON_MUSIC_OFF_ID:
		if (state == NE_BUTTON_DOWN)
		{
			g_SoundSystem->Play(g_menuSound);
			g_Render->SetGUIVisble(g_mainGui, BUTTON_MUSIC_ON_ID, true);
			g_Render->SetGUIVisble(g_mainGui, BUTTON_MUSIC_OFF_ID, false);
		}
		break;
		// ��������ť����
	case BUTTON_MUSIC_ON_ID:
		if (state == NE_BUTTON_DOWN)
		{
			g_SoundSystem->Stop(g_menuSound);
			g_Render->SetGUIVisble(g_mainGui, BUTTON_MUSIC_ON_ID, false);
			g_Render->SetGUIVisble(g_mainGui, BUTTON_MUSIC_OFF_ID, true);
		}
		break;

		// ����1��ť�����µ����
	case BUTTON_LEVEL_1_ID:
		if (state == NE_BUTTON_DOWN) 
		{
			g_currentGUI = GUI_LOADING_SCREEN;
			g_gameState = GS_LEVEL_1_SWITCH;
		}
		break;
	}
}

void MainMenuRender()
{
	if (!g_Render) return;

	g_Render->StartRender(1, 1, 0);

	if (g_currentGUI == GUI_MAIN_SCREEN)
		g_Render->ProcessGUI(g_mainGui, g_LMBDown, g_mouseX,
			g_mouseY, MainMenuCallback);
	else if (g_currentGUI == GUI_START_SCREEN)
		g_Render->ProcessGUI(g_startGui, g_LMBDown, g_mouseX,
			g_mouseY, MainMenuCallback);
	else if(g_currentGUI == GUI_CREDITS_SCREEN)
		g_Render->ProcessGUI(g_creditsGui, g_LMBDown, g_mouseX,
			g_mouseY, MainMenuCallback);
	else if (g_currentGUI == GUI_LOADING_SCREEN)
		g_Render->ProcessGUI(g_loadingGui, g_LMBDown, g_mouseX,
			g_mouseY, MainMenuCallback);

	g_Render->EndRendering();
}