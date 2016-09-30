#pragma once

// GUI���˵��������ض�����Ϣ
#define GUI_MAIN_SCREEN       1   // ���˵�����
#define GUI_START_SCREEN      2   // ��ʼ�˵�����
#define GUI_CREDITS_SCREEN    3   // ��Ļ�˵�����
#define GUI_LOADING_SCREEN    4   // ����ҳ��

// �õ���GUI�ؼ�ID
#define STATIC_TEXT_ID     1    // ��̬�ı��ؼ�
#define BUTTON_START_ID    2    // ����ʼ����ť�ؼ�
#define BUTTON_CREDITS_ID  3    // ����Ļ����ť�ؼ�
#define BUTTON_QUIT_ID     4    // ���˳�����ť�ؼ�
#define BUTTON_BACK_ID     5    // �����ء���ť�ؼ�
#define BUTTON_LEVEL_1_ID  6    // ������1����ť�ؼ�
#define BUTTON_MUSIC_ON_ID   7    // �����ֿ�������ť�ؼ�
#define BUTTON_MUSIC_OFF_ID  8    // �����ֹرա���ť�ؼ�

// Menu functions.
bool InitializeMainMenu();
void MainMenuCallback(int id, int state);
void MainMenuRender();

extern int g_mainGui;
extern int g_startGui;
extern int g_creditsGui;
extern int g_loadingGui;
extern int g_currentGUI;

extern int g_menuSound;
extern int g_buttonSound;