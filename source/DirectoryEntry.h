#pragma once
#include<string>

class DirectoryEntry
{
	/* static members */
public:
	static const int DIRSIZ = 28;	/* 目录项中路径部分的最大字符串长度 */

	DirectoryEntry();
	DirectoryEntry(std::string name, int no);
	~DirectoryEntry();
	void padding();					/* 将m_name补足至DIRSIZ长度 */

	char m_name[DIRSIZ];			/* 目录项中路径名部分 */
	//std::string m_name;				/* 目录项中路径名部分 */
	int m_ino;						/* 目录项中Inode编号部分 */
};