#include "game.h"
#include "gameEngine.h"
#include "menu.h"
#include "level.h"

bool g_LMBDown = false;				// �������Ƿ񱻰���
int g_mouseX = 0, g_mouseY = 0;		// ���ָ��λ��(X��Y)

							  
stGameWorld g_gameWorld;			// ��Ϸ����

GAME_STATE g_gameState = GS_MENU;	// ��Ϸ����ʱ��ʼ��Ϊ���˵�������״̬

CRay g_viewRay;

// Sound id for weapon shot.
int g_shotSound = -1;

									// ��Ϸ�����ڽ�����Ϣѭ��ǰ����س�ʼ��
bool GameInitialize()
{
	// �˵������ʼ��
	if (!InitializeMainMenu())
	{
		return false;
	}

	// Load menu sound.
	if (g_shotSound == -1)
	{
		if (!g_SoundSystem->AddSound("sounds/shot.wav", false,
			&g_shotSound)) return false;
	}


	return true;
}

void GameProcessInput()
{
	if (!g_InputSystem) return;

	// Update all devices.
	g_InputSystem->UpdateDevices();

	// Keyboard Input.
	if (g_InputSystem->KeyUp(DIK_ESCAPE))
	{
		if (g_gameState == GS_MENU)
		{
			switch (g_currentGUI)
			{
			case GUI_MAIN_SCREEN:
				PostQuitMessage(0);
				break;

			default:
				g_currentGUI = GUI_MAIN_SCREEN;
				break;
			}
		}
		else if (g_gameState == GS_LEVEL)
		{
			GameReleaseAll();

			InitializeMainMenu();
			g_SoundSystem->Play(g_menuSound);
			g_gameState = GS_MENU;
			g_currentGUI = GUI_MAIN_SCREEN;
		}
	}

	if (g_gameState == GS_LEVEL &&
		g_gameWorld.m_missionStatus == MISSION_ONGOING)
	{
		if (g_InputSystem->KeyDown(DIK_UP))
		{
			g_camera.MoveCamera(0.15f);

			for (int i = 0; i < g_gameWorld.m_totalModels; i++)
			{
				if (ObjectsTestCollision(i))
				{
					g_camera.MoveCamera(-0.15f);
					break;
				}
			}

			for (int i = 0; i < g_gameWorld.m_totalCharacters; i++)
			{
				if (CharacterProcess(i))
				{
					if (!g_gameWorld.m_characterSpot[i])
						g_gameWorld.m_spottedCount++;

					g_gameWorld.m_characterSpot[i] = 1;

					if (CharacterTestCollision(i))
					{
						g_camera.MoveCamera(-0.15f);
						break;
					}
				}
			}
		}

		if (g_InputSystem->KeyDown(DIK_DOWN))
		{
			g_camera.MoveCamera(-0.15f);

			for (int i = 0; i < g_gameWorld.m_totalModels; i++)
			{
				if (ObjectsTestCollision(i))
				{
					g_camera.MoveCamera(0.15f);
					break;
				}
			}

			for (int i = 0; i < g_gameWorld.m_totalCharacters; i++)
			{
				if (CharacterProcess(i))
				{
					if (!g_gameWorld.m_characterSpot[i])
						g_gameWorld.m_spottedCount++;

					g_gameWorld.m_characterSpot[i] = 1;

					if (CharacterTestCollision(i))
					{
						g_camera.MoveCamera(0.15f);
						break;
					}
				}
			}
		}

		if (g_InputSystem->KeyDown(DIK_LEFT))
		{
			g_camera.RotateCamera(-0.001f, 0, 1, 0);
		}

		if (g_InputSystem->KeyDown(DIK_RIGHT))
		{
			g_camera.RotateCamera(0.001f, 0, 1, 0);
		}

		// Update view ray.
		g_viewRay.m_origin = g_camera.m_pos;
		g_viewRay.m_direction = g_camera.m_view - g_camera.m_pos;
		g_viewRay.m_direction.Normal();

		if (g_InputSystem->MouseButtonDown(NE_LEFT_BUTTON))
		{
			g_SoundSystem->Play(g_shotSound);

			for (int i = 0; i < g_gameWorld.m_totalCharacters; i++)
			{
				if (g_gameWorld.m_characters[i].m_visible)
				{
					if (CharacterTestCollision(i, g_viewRay))
					{
						g_gameWorld.m_characters[i].m_visible = 0;
						g_gameWorld.m_totalEnemiesKilled++;
					}
				}
			}
		}
	}

	// Mouse Input
	POINT pos = { 0, 0 };
	GetCursorPos(&pos);
	g_mouseX = pos.x;
	g_mouseY = pos.y;

	if (g_InputSystem->MouseButtonDown(NE_LEFT_BUTTON))
		g_LMBDown = true;
	if (!g_InputSystem->MouseButtonDown(NE_LEFT_BUTTON))
		g_LMBDown = false;
}


// ��Ϸ��״̬�µ���Ⱦ�Ȳ���
void GameLoop()
{
	// ִ������ϵͳ�Ĵ���
	GameProcessInput();

	// ��Ⱦ��Ӧ��Ϸ״̬�µĳ���

	if (g_gameState == GS_LEVEL)
	{
		// ��Ⱦ�ü����µĳ���
		LevelRender();
	}
	else if (g_gameState == GS_MENU)
	{
		// ���Ʋ˵�����
		MainMenuRender();
	}
	else if (g_gameState == GS_LEVEL_1_SWITCH)
	{
		MainMenuRender();
		GameReleaseAll();

		// ��ʼ��1���������Ϸ
		if (LoadLevel("maps/level1/level1.lvl"))
		{
			g_gameWorld.m_missionStatus = MISSION_ONGOING;
			g_gameState = GS_LEVEL;
		}
		// ���������ʧ�ܣ��򷵻�GS_MENU��Ϸ״̬
		else
		{
			g_gameState = GS_MENU;
		}
	}

	if (g_gameState == GS_LEVEL)
	{
		int time = (int)(g_gameWorld.m_totalTime -
			g_gameWorld.m_currentTime);

		if (time <= 0)
			g_gameWorld.m_missionStatus = MISSION_FAIL;

		if (g_gameWorld.m_numArtifactsCollected >=
			g_gameWorld.m_totalArtifacts)
			g_gameWorld.m_missionStatus = MISSION_COMPLETE;
	}

}


void GameShutdown()
{
	GameReleaseAll();
}

void GameReleaseAll()
{
	if (!g_Render) return;

	g_Render->ReleaseAllStaticBuffers();
	g_Render->ReleaseAllXModels();
	g_Render->ReleaseAllGUIs();

	g_gameWorld.Shutdown();

	g_mainGui = -1;
	g_startGui = -1;
	g_creditsGui = -1;
	g_currentGUI = GUI_MAIN_SCREEN;

	if (!g_SoundSystem) return;

	if (g_gameState != GS_MENU && g_gameState != GS_LEVEL)
		g_SoundSystem->Stop(g_menuSound);
}

bool ObjectsTestCollision(int i)
{
	CBoundingSphere sphere;
	int index = g_gameWorld.m_models[i].m_id;

	g_Render->GetXModelBoundingSphere(index,
		&sphere.m_center, &sphere.m_radius);

	CMatrix4x4 *worldMat = &g_gameWorld.m_models[i].worldMat;
	sphere.m_center = worldMat->VectorMatrixMultiply(sphere.m_center);

	sphere.m_radius -= 0.5f;
	if (sphere.isPointInside(g_camera.m_pos))
		return true;

	return false;
}

bool CharacterTestCollision(int i)
{
	CBoundingSphere sphere;
	int index = g_gameWorld.m_characters[i].m_id;

	if (!g_gameWorld.m_characters[i].m_visible)
		return false;

	g_Render->GetXModelBoundingSphere(index,
		&sphere.m_center, &sphere.m_radius);

	CMatrix4x4 *worldMat = &g_gameWorld.m_characters[i].worldMat;
	sphere.m_center = worldMat->VectorMatrixMultiply(sphere.m_center);

	if (sphere.isPointInside(g_camera.m_pos))
		return true;

	return false;
}


bool CharacterTestCollision(int i, CRay &ray)
{
	CBoundingSphere sphere;
	int index = g_gameWorld.m_characters[i].m_id;

	if (!g_gameWorld.m_characters[i].m_visible)
		return false;

	g_Render->GetXModelBoundingSphere(index,
		&sphere.m_center, &sphere.m_radius);

	CMatrix4x4 *worldMat = &g_gameWorld.m_characters[i].worldMat;
	sphere.m_center = worldMat->VectorMatrixMultiply(sphere.m_center);

	if (ray.Intersect(sphere.m_center, sphere.m_radius, NULL))
		return true;

	return false;
}


bool CharacterProcess(int i)
{
	CBoundingSphere sphere;
	int index = g_gameWorld.m_characters[i].m_id;

	if (!g_gameWorld.m_characters[i].m_visible)
		return false;

	g_Render->GetXModelBoundingSphere(index,
		&sphere.m_center, &sphere.m_radius);

	CMatrix4x4 *worldMat = &g_gameWorld.m_characters[i].worldMat;
	sphere.m_center = worldMat->VectorMatrixMultiply(sphere.m_center);

	sphere.m_radius *= 5;

	if (sphere.isPointInside(g_camera.m_pos))
		return true;

	return false;
}
