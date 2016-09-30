#include "GUI.h"

// 在m_controls对象链表尾部增加一个GUI控件对象
int CGUISystem::IncreaseControls()
{
	// 首次创建控件对象时，创建一个含有1个元素的控件数组
	if (!m_controls)
	{
		// m_fonts初始化为动态创建的控件对象数组
		m_controls = new stGUIControl[1];
		if (!m_controls)
		{
			return NE_FAIL;
		}

		// 为新申请的内存做初始化工作
		// 将m_controls首元素的内存块的内容清0
		memset(&m_controls[0], 0, sizeof(stGUIControl));
	}
	else
	{
		// 临时的控件对象指针，用于保存当前控件数组的值
		stGUIControl *temp;

		// 指向动态开辟的控件对象数组，该控件对象数组元素比当前多1位
		temp = new stGUIControl[m_totalControls + 1];
		if (!temp)
		{
			return NE_FAIL;
		}

		// 将temp所指向的内存块的内容清0
		memset(temp, 0, sizeof(stGUIControl) * (m_totalControls + 1));

		// 将m_controls控件对象数组的全部数据复制到temp中
		memcpy(temp, m_controls, sizeof(stGUIControl) * m_totalControls);

		delete[] m_controls;   // 销毁m_controls对象数组

		m_controls = temp;     // m_controls对象数组获得当前的temp对象数组的全部数据
	}

	return NE_OK;
}


// 添加背景图控件
bool CGUISystem::AddBackdrop(int texID, int sID, bool visible)
{
	if (texID < 0 || sID < 0)
	{
		return false;
	}

	if (m_backDropID < 0)
	{
		// 在m_controls对象链表尾部增加一个GUI控件对象
		if (!IncreaseControls())
		{
			return false;
		}

		// 填充该控件对象的成员变量
		m_controls[m_totalControls].m_type = NE_GUI_BACKDROP;  // 背景图控件类型
		m_controls[m_totalControls].m_upTex = texID;            // 纹理ID
		m_controls[m_totalControls].m_listID = sID;             // 静态缓存ID
		m_controls[m_totalControls].m_visible = visible;        // 是否渲染


		m_backDropID = m_totalControls;   // 用当前控件对象总数给当前控件ID赋值

		m_totalControls++;   // 控件对象总数递增1
	}
	else
	{
		m_controls[m_backDropID].m_upTex = texID;   // 获取纹理ID
		m_controls[m_backDropID].m_listID = sID;    // 获取静态缓存ID
	}

	return true;
}

// 添加静态文本控件
bool CGUISystem::AddStaticText(
	int id,                 // 控件ID
	char *text,             // 要显示的文本
	int x, int y,           // 文本起始X、Y坐标
	unsigned long color,    // 文本颜色
	int fontID,             // 字体类型ID
	bool visible			// 是否渲染
)
{
	if (!text || fontID < 0)
	{
		return false;
	}

	// 在m_controls对象链表尾部增加一个GUI控件对象
	if (!IncreaseControls())
	{
		return false;
	}

	// 填充该控件对象的成员变量
	m_controls[m_totalControls].m_type = NE_GUI_STATICTEXT;   // 静态文本控件类型
	m_controls[m_totalControls].m_id = id;            // 控件ID
	m_controls[m_totalControls].m_color = color;      // 文本颜色
	m_controls[m_totalControls].m_xPos = x;           // 控件X坐标
	m_controls[m_totalControls].m_yPos = y;           // 控件Y坐标
	m_controls[m_totalControls].m_listID = fontID;    // 字体类型ID
	m_controls[m_totalControls].m_visible = visible;	  // 是否可见

													  // 获取文本字节长度
	int len = strlen(text);

	// 开辟存放文本字符的内存块
	m_controls[m_totalControls].m_text = new char[len + 1];

	if (!m_controls[m_totalControls].m_text)
	{
		return false;
	}

	// 将文本text复制到该控件对应的成员变量中
	memcpy(m_controls[m_totalControls].m_text, text, len);

	// 在该字符数组尾元素添加一结束符
	m_controls[m_totalControls].m_text[len] = '\0';

	// 控件对象数量递增
	m_totalControls++;

	return true;
}

// 添加按钮控件
bool CGUISystem::AddButton(int id,                 // 控件ID
	int x, int y,           // 控件的起始X、Y坐标
	int width, int height,  // 控件的宽度和高度
	int upID, int overID, int downID,  // 控件的3种状态的纹理ID
	unsigned int staticID,   // 显示按钮几何形状的静态缓存ID
	bool visible
)
{
	// 在m_controls对象链表尾部增加一个GUI控件对象
	if (!IncreaseControls())
	{
		return false;
	}

	// 填充该控件对象的成员变量
	m_controls[m_totalControls].m_type = NE_GUI_BUTTON;  // 按钮控件类型
	m_controls[m_totalControls].m_id = id;                // 控件ID
	m_controls[m_totalControls].m_xPos = x;               // 控件的X坐标
	m_controls[m_totalControls].m_yPos = y;               // 控件的Y坐标
	m_controls[m_totalControls].m_width = width;          // 控件宽度
	m_controls[m_totalControls].m_height = height;        // 控件高度
	m_controls[m_totalControls].m_upTex = upID;           // 按钮弹起状态的纹理ID
	m_controls[m_totalControls].m_overTex = overID;       // 鼠标停靠在按钮上的纹理ID
	m_controls[m_totalControls].m_downTex = downID;       // 按钮被按下的纹理ID
	m_controls[m_totalControls].m_listID = staticID;      // 显示按钮几何形状的静态缓存ID
	m_controls[m_totalControls].m_visible = visible;		  // 是否可见

														  // 控件对象数量递增1
	m_totalControls++;

	return true;
}

void CGUISystem::SetVisisble(int id, bool visible)
{
	for (int i = 0; i < m_totalControls; i++) 
	{
		if(m_controls[i].m_id == id)
			m_controls[i].m_visible = visible;
	}
}

// 内存清理
void CGUISystem::Shutdown()
{
	// 释放控件对象数组的全部内存块
	for (int s = 0; s < m_totalControls; s++)
	{
		if (m_controls[s].m_text)
		{
			delete[] m_controls[s].m_text;   // 删除控件对象的文本字符内存
			m_controls[s].m_text = NULL;
		}
	}

	// 控件对象数量清0
	m_totalControls = 0;

	if (m_controls)
	{
		delete[] m_controls;
	}

	// 该指针指向为空
	m_controls = NULL;
}
