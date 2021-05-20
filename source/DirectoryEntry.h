#pragma once
#include<string>

class DirectoryEntry
{
	/* static members */
public:
	static const int DIRSIZ = 28;	/* Ŀ¼����·�����ֵ�����ַ������� */

	DirectoryEntry();
	DirectoryEntry(std::string name, int no);
	~DirectoryEntry();
	void padding();					/* ��m_name������DIRSIZ���� */

	char m_name[DIRSIZ];			/* Ŀ¼����·�������� */
	//std::string m_name;				/* Ŀ¼����·�������� */
	int m_ino;						/* Ŀ¼����Inode��Ų��� */
};