#pragma once

// GUI主菜单界面的相关定义信息
#define GUI_MAIN_SCREEN       1   // 主菜单界面
#define GUI_START_SCREEN      2   // 开始菜单界面
#define GUI_CREDITS_SCREEN    3   // 字幕菜单界面
#define GUI_LOADING_SCREEN    4   // 加载页面

// 用到的GUI控件ID
#define STATIC_TEXT_ID     1    // 静态文本控件
#define BUTTON_START_ID    2    // “开始”按钮控件
#define BUTTON_CREDITS_ID  3    // “字幕”按钮控件
#define BUTTON_QUIT_ID     4    // “退出”按钮控件
#define BUTTON_BACK_ID     5    // “返回”按钮控件
#define BUTTON_LEVEL_1_ID  6    // “级别1”按钮控件
#define BUTTON_MUSIC_ON_ID   7    // “音乐开启”按钮控件
#define BUTTON_MUSIC_OFF_ID  8    // “音乐关闭”按钮控件

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