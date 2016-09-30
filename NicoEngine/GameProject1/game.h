#pragma once

#include "gameEngine.h"

bool GameInitialize();    // 创建引擎中的相关对象，对相关渲染进行设置
void GameLoop();          // 启动引擎中的绘制功能
void GameShutdown();      // 销毁相关内存
void GameProcessInput();
void GameReleaseAll();

bool ObjectsTestCollision(int i);
bool CharacterTestCollision(int i);
bool CharacterTestCollision(int i, CRay &ray);
bool CharacterProcess(int i);

#define X_MESH          1
#define X_ANIMATED_MESH 2


#define MISSION_COMPLETE   1
#define MISSION_FAIL       2
#define MISSION_ONGOING    3

// Holds game entity.
struct stGameObject
{
	stGameObject() : m_type(0), m_id(0), m_x(0), m_y(0), m_z(0),
		m_rotX(0), m_rotY(0), m_rotZ(0), m_visible(1) {}

	int m_type;
	int m_id;
	float m_x, m_y, m_z;
	float m_rotX, m_rotY, m_rotZ;
	int m_visible;

	CMatrix4x4 worldMat;
};

// Holds entire game world.
struct stGameWorld
{
	// Level id and skydome.
	int m_levelID;
	int m_skyBoxID;

	// Number of models and model list.
	stGameObject *m_models;
	int m_totalModels;

	// Number of artifacts and list.
	stGameObject *m_artifacts;
	int *m_artifactsCollected;
	int m_totalArtifacts;

	stGameObject *m_characters;
	int *m_characterSpot;
	int m_totalCharacters;

	// Game Statistics.
	float m_totalTime;
	float m_startTime;
	float m_currentTime;
	float m_levelStartTime;
	int m_spottedCount;
	int m_numArtifactsCollected;

	// Mission status.
	int m_missionStatus;

	// Number of enemies killed.
	int m_totalEnemiesKilled;

	stGameWorld() 
	{
		m_levelID = m_skyBoxID = -1;
		m_models = 0; m_totalModels = 0;
		m_artifacts = 0; m_totalArtifacts = 0;
		m_artifactsCollected = 0;
		m_totalTime = m_startTime = m_currentTime = 0;
		m_spottedCount = m_numArtifactsCollected = 0;
		m_levelStartTime = 0;
		m_missionStatus = MISSION_ONGOING;
		m_totalEnemiesKilled = 0;
		m_characters = 0; m_totalCharacters = 0;
		m_characterSpot = 0;
	}

	void Shutdown() 
	{
		m_levelID = -1;
		m_skyBoxID = -1;

		if (m_models) delete[] m_models;
		m_models = 0;
		m_totalModels = 0;

		if (m_artifacts) delete[] m_artifacts;
		m_artifacts = 0;

		if (m_artifactsCollected) delete[] m_artifactsCollected;
		m_artifactsCollected = 0;
		m_totalArtifacts = 0;

		m_totalTime = 0;
		m_startTime = 0;
		m_currentTime = 0;
		m_levelStartTime = 0;
		m_spottedCount = 0;
		m_numArtifactsCollected = 0;
		m_missionStatus = MISSION_ONGOING;

		m_totalEnemiesKilled = 0;

		if (m_characters) delete[] m_characters;
		m_characters = 0;

		if (m_characterSpot) delete[] m_characterSpot;
		m_characterSpot = 0;
		m_totalArtifacts = 0;
	}
};

extern bool g_LMBDown;
extern int g_mouseX;
extern int g_mouseY;

// 游戏状态
enum GAME_STATE {
	GS_MENU = 1,        // 主菜单运行状态
	GS_LEVEL,           // 某个级别的运行状态
	GS_LEVEL_1_SWITCH   // 正在加载第一个游戏级别的状态
};

extern stGameWorld g_gameWorld;
extern GAME_STATE g_gameState;

extern CRay g_viewRay;

// Weapon sound.
extern int g_shotSound;