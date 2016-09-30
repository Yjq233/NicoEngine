#pragma once

#include "../NicoEngine/engine.h"

#pragma comment(lib, "../x64/Debug/NicoEngine.lib")

extern CInputInterface *g_InputSystem;
extern CSoundSystemInterface *g_SoundSystem;
extern CRenderInterface *g_Render;

extern int g_arialID;
extern int g_arial24ID;
extern int g_arial36ID;

bool InitializeEngine();   // �����ʼ������ز���
void ShutdownEngine();     // ��������ʹ�õ��ڴ�