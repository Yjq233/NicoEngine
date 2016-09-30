#pragma once

#include <windows.h>

#define WINDOW_CLASS    TEXT("NicoGame")	// 窗口类型名
#define WINDOW_NAME     TEXT("Nico")		// 窗口标题名
#define WIN_WIDTH       800                 // 窗口模式的宽度
#define WIN_HEIGHT      600                 // 窗口模式的高度
#define FULLSCREEN      0                   // 是否全屏

extern HWND g_hwnd;
extern HINSTANCE g_hInstance;