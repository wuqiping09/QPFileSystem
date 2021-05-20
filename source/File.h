#pragma once
#include "Inode.h"

/*
 * ���ļ����ƿ�File�ࡣ
 * �ýṹ��¼�˽��̴��ļ�
 * �Ķ���д�������ͣ��ļ���дλ�õȵȡ�
 */
class File
{
public:
	enum FileFlags
	{
		FREAD = 0x1,			/* ���������� */
		FWRITE = 0x2,			/* д�������� */
		FPIPE = 0x4				/* �ܵ����� */
	};

	File();
	~File();

	unsigned int f_flag;		/* �Դ��ļ��Ķ���д����Ҫ�� */
	int		f_count;			/* ��ǰ���ø��ļ����ƿ�Ľ������� */
	Inode*	f_inode;			/* ָ����ļ����ڴ�Inodeָ�� */
	int		f_offset;			/* �ļ���дλ��ָ�� */
};