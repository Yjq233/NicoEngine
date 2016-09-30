#pragma once

#include"defines.h"


// GUI控件结构
struct stGUIControl
{
	int m_type;    // 控件类型
	int m_id;      // 控件ID
	unsigned long m_color;   // 控件颜色

	int m_listID;            // 链表索引，字体对象ID或顶点缓存ID

	int m_xPos, m_yPos;      // 控件的XY坐标
	int m_width, m_height;   // 控件的宽和高

	char *m_text;            // 指向静态控件文本字符的指针

	int m_upTex, m_downTex, m_overTex;  // 按钮的弹起、按下和停靠

	bool m_visible;
};



// GUI控件系统类
class CGUISystem
{
public:
	// 构造函数
	CGUISystem() : m_controls(0), m_totalControls(0), m_backDropID(-1)
	{
	}
	// 析构函数
	~CGUISystem()
	{
		Shutdown();
	}

	int IncreaseControls();

	// 添加背景图控件
	bool AddBackdrop(int texID, int staticID, bool visible = true);

	// 添加静态文本控件
	bool AddStaticText(int id, char *text, int x, int y, unsigned long color, int fontID, bool visible = true);

	// 添加按钮控件
	bool AddButton(int id, int x, int y, int width, int height,
		int upID, int overID, int downID, unsigned int staticID, bool visible = true);

	void SetVisisble(int id, bool visible);

	// 内存清理
	void Shutdown();

	// 获取指定ID的GUI控件对象
	stGUIControl *GetGUIControl(int id)
	{
		if (id < 0 || id >= m_totalControls)
		{
			return NULL;
		}
		return &m_controls[id];
	}

	// 返回控件对象总数 m_totalControls
	int GetTotalControls()
	{
		return m_totalControls;
	}

	// 返回指定背景图控件对象
	stGUIControl *GetBackDrop()
	{
		if (m_backDropID >= 0 && m_totalControls)
		{
			return &m_controls[m_backDropID];
		}

		// 失败则返回0值
		return NULL;
	}


private:
	stGUIControl *m_controls;    // GUI控件对象指针
	int m_totalControls;         // 控件对象的数量
	int m_backDropID;            // 背景图控件的ID
};
