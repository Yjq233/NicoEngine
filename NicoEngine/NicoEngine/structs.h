#pragma once

// GUI�ؼ��Ķ���ṹ
struct stGUIVertex
{
	float x, y, z, rhw;
	unsigned long color;
	float tu, tv;
};

// ģ�Ͷ���ṹ
struct stModelVertex
{
	float x, y, z;         // ģ�Ͷ�������
	float nx, ny, nz;      // ���㷨��
	unsigned long color;   // ������ɫֵ
	float tu, tv;          // ��������
};
