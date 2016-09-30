#include "main.h"
#include "game.h"
#include "gameEngine.h"

#pragma comment(lib, "winmm.lib")

HWND g_hwnd;
HINSTANCE g_hInstance;   // ����ĵ�ǰʵ���ľ��

// ���ں���
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

// ��Ϸ��ں���
int WINAPI WinMain(HINSTANCE h, HINSTANCE p, LPSTR cmd, int show)
{
	g_hInstance = h;	//Ϊȫ�־��������ֵ

	// Register the window class
	// ���ô�������(����)
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc,
		0L, 0L, GetModuleHandle(NULL), NULL, NULL,
		NULL, NULL, WINDOW_CLASS, NULL };

	// ע��ô���
	RegisterClassEx(&wc);

	// Create the application's window
	if (FULLSCREEN)
	{
		// ����FULLSCREEN��Ϊ��0ֵ����ȫ����ʾ
		g_hwnd = CreateWindowEx(NULL, WINDOW_CLASS, WINDOW_NAME,
			WS_POPUP | WS_SYSMENU | WS_VISIBLE, 0, 0,
			WIN_WIDTH, WIN_HEIGHT,
			NULL, NULL, h, NULL);
	}
	else
	{
		// ����FULLSCREEN����ֵΪ0���򴰿���ʾ
		g_hwnd = CreateWindowEx(NULL, WINDOW_CLASS, WINDOW_NAME,
			WS_OVERLAPPEDWINDOW | WS_VISIBLE, 0,
			0, WIN_WIDTH, WIN_HEIGHT,
			NULL, NULL, h, NULL);
	}

	if (g_hwnd)
	{
		// Show the window
		ShowWindow(g_hwnd, SW_SHOWDEFAULT);   // ��ʾwc����
		UpdateWindow(g_hwnd);                 // ����wc����
	}

	// ������Ⱦϵͳ���󣬳�ʼ��Direct3D�����ز���ֵ
	if (InitializeEngine())
	{
		// �Ƚ�����Ϸ����س�ʼ������
		if (GameInitialize())
		{
			// ������Ϣ�ṹ����
			MSG msg;

			// ������msg���䵽���ڴ�������һ��
			ZeroMemory(&msg, sizeof(msg));

			// �ѹ���Ƶ���Ļ�ġ�0,0��λ��
			SetCursorPos(0, 0);

			// ֻҪ�����˳���Ϣ����һֱѭ��
			while (msg.message != WM_QUIT)
			{
				// �����Ϣ���У���������Ϣ��������ڣ�����msg�ṹ������
				// �������Ϣ�ɵõ����򷵻ط���ֵ
				if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
				{
					TranslateMessage(&msg);  // �������Ϣ����ת��
					DispatchMessage(&msg);   // ��ϵͳ������Ϣ�������ô��ں���
				}
				else
				{
					// ����ִ����Ϸְ�ܣ�����Ⱦ�õĳ������Ƶ���ʾ����
					GameLoop();
				}
			}
		}
	}

	// �ͷ������ڴ���Դ
	GameShutdown();
	ShutdownEngine();

	// ȡ�����ڵ�ע��
	UnregisterClass(WINDOW_CLASS, wc.hInstance);
	return 0;
}
