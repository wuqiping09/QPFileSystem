#define _CRT_SECURE_NO_WARNINGS
#include "DirectoryEntry.h"
#include<iostream>
using namespace std;

DirectoryEntry::DirectoryEntry()
{
	this->m_ino = 0;
	this->m_name[0] = '\0';
	//this->padding();
}

DirectoryEntry::DirectoryEntry(string name, int no)
{
	if (name.size() > 28 || name.size() < 1)
	{
		//cout << "文件名长度最多为28个字符" << endl;
		this->m_ino = 0;
		this->m_name[0] = '\0';
	}
	else
	{
		this->m_ino = no;
		strcpy(this->m_name, name.c_str());
	}
	//this->padding();
}

DirectoryEntry::~DirectoryEntry()
{
	
}

void DirectoryEntry::padding()
{
	//int n = this->m_name.size();
	//for (; n < 28; ++n)
	//{
	//	this->m_name += ' ';
	//}
}
