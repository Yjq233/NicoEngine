#include "main.h"
#include "game.h"
#include "gameEngine.h"

#pragma comment(lib, "winmm.lib")

HWND g_hwnd;
HINSTANCE g_hInstance;   // 程序的当前实例的句柄

// 窗口函数
LRESULT WINAPI MsgProc(HWND hd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
	case WM_DESTROY:
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
		break;
	}

	return DefWindowProc(hd, msg, wp, lp);
}

// 游戏入口函数
int WINAPI WinMain(HINSTANCE h, HINSTANCE p, LPSTR cmd, int show)
{
	g_hInstance = h;	//为全局句柄变量赋值

	// Register the window class
	// 设置窗口类型(属性)
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc,
		0L, 0L, GetModuleHandle(NULL), NULL, NULL,
		NULL, NULL, WINDOW_CLASS, NULL };

	// 注册该窗口
	RegisterClassEx(&wc);

	// Create the application's window
	if (FULLSCREEN)
	{
		// 若“FULLSCREEN”为非0值，则全屏显示
		g_hwnd = CreateWindowEx(NULL, WINDOW_CLASS, WINDOW_NAME,
			WS_POPUP | WS_SYSMENU | WS_VISIBLE, 0, 0,
			WIN_WIDTH, WIN_HEIGHT,
			NULL, NULL, h, NULL);
	}
	else
	{
		// 若“FULLSCREEN”的值为0，则窗口显示
		g_hwnd = CreateWindowEx(NULL, WINDOW_CLASS, WINDOW_NAME,
			WS_OVERLAPPEDWINDOW | WS_VISIBLE, 0,
			0, WIN_WIDTH, WIN_HEIGHT,
			NULL, NULL, h, NULL);
	}

	if (g_hwnd)
	{
		// Show the window
		ShowWindow(g_hwnd, SW_SHOWDEFAULT);   // 显示wc窗口
		UpdateWindow(g_hwnd);                 // 更新wc窗口
	}

	// 创建渲染系统对象，初始化Direct3D，返回布尔值
	if (InitializeEngine())
	{
		// 先进行游戏的相关初始化工作
		if (GameInitialize())
		{
			// 定义消息结构变量
			MSG msg;

			// 将变量msg分配到的内存块先清除一下
			ZeroMemory(&msg, sizeof(msg));

			// 把光标移到屏幕的“0,0”位置
			SetCursorPos(0, 0);

			// 只要不是退出消息，则一直循环
			while (msg.message != WM_QUIT)
			{
				// 检查消息队列，并将该消息（如果存在）放于msg结构变量中
				// 如果该消息可得到，则返回非零值
				if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
				{
					TranslateMessage(&msg);  // 对相关消息进行转换
					DispatchMessage(&msg);   // 向系统派送消息，并调用窗口函数
				}
				else
				{
					// 否则执行游戏职能（将渲染好的场景绘制到显示器）
					GameLoop();
				}
			}
		}
	}

	// 释放所有内存资源
	GameShutdown();
	ShutdownEngine();

	// 取消窗口的注册
	UnregisterClass(WINDOW_CLASS, wc.hInstance);
	return 0;
}
