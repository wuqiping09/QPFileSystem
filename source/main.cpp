#define _CRT_SECURE_NO_WARNINGS
#include"FileManager.h"
#include<fstream>
#include<string>
#include<iostream>
#include<stdio.h>
#include<windows.h>
#include<vector>
//#include <unistd.h>
//#include<sys/mman.h>

using namespace std;

//���ո�ָ��ַ������õ�operator��parameter
vector<string> stringSplit(string command)
{
	vector<string>res;
	int n = command.size();
	for (int i = 0; i < n; ++i)
	{
		for (; i < n && command[i] == ' '; ++i);//����ո�
		string s = "";
		for (; i < n && command[i] != ' '; ++i)
		{
			s += command[i];
		}
		if (s.size() > 0)
		{
			res.emplace_back(s);
		}
	}
	return res;
}

//string to int���жϺϷ���
int stringToInt(string s)
{
	int res = -1;
	if (s.size() <= 30 && s[0] >= '0'&&s[0] <= '9')
	{
		res = atoi(s.c_str());
	}
	return res;
}

//��ʾhelp
void showHelp()
{
	cout << "����˵�����£�" << endl;
	cout << "help                                               " << "�鿴����˵��" << endl;
	cout << "format                                             " << "��ʽ��myDisk.img" << endl;
	cout << "load <filename>                                    " << "��myDisk.img����Ϊfilename���ļ�����������" << endl;
	cout << "store <filename_out> <filename_in>                 " << "����������Ϊfilename_out���ļ�����myDisk.img����Ϊfilename_in" << endl;									  	         
	cout << "rm <filename>                                      " << "��myDisk.img��ɾ����Ϊfilename���ļ�" << endl;
	cout << "create <filename>                                  " << "��myDisk.img���½��ļ�����Ϊfilename" << endl;
	cout << "mkdir <dir>                                        " << "��myDisk.img���½�Ŀ¼�ļ���·��Ϊdir" << endl;
	cout << "read <filename>                                    " << "������������Ϊfilename���ļ�����" << endl;
	cout << "write <filename> <str>                             " << "���ļ���д��str" << endl;
	cout << "shutdown                                           " << "�˳�QIPING�ļ�ϵͳ" << endl;
	cout << "���棺��ǿ����ֹ������ܵ��»������ݶ�ʧ��" << endl;
}

//�����ļ�ϵͳ������������ģʽ
void startSystem()
{
	FileManager *FM = new FileManager();
	cout << "��ӭ����QIPING�ļ�ϵͳ" << endl;
	cout << "�״�ʹ�����ʽ��myDisk.img" << endl;
	cout << "ʹ��help�鿴����˵��" << endl;
	string command;
	cout << ">>>";
	getline(cin, command);
	while (command != "shutdown")
	{
		vector<string>v = stringSplit(command);
		if (v.size() > 0)
		{
			if (v[0] == "help")
			{
				if (v.size() > 1)
				{
					cout << "help����Ҫ����" << endl;
				}
				else
				{
					showHelp();
				}
			}
			else if (v[0] == "format")
			{
				if (v.size() != 1)
				{
					cout << "������������!" << endl;
				}
				else
				{
					cout << "��ʽ�������ݽ�ȫ����ʧ��ȷ��������(y����)";
					string s;
					getline(cin, s);
					if (s == "y")
					{
						cout << "��ʽ����..." << endl;
						int rf = FM->format();
						if (rf == 0)
						{
							cout << "��ʽ���ɹ�" << endl;
						}
						else
						{
							cout << "��ʽ��ʧ��" << endl;
						}
					}
					else
					{
						cout << "��ȡ����ʽ��" << endl;
					}
				}
			}
			else if (v[0] == "load")
			{
				if (v.size() != 2)
				{
					cout << "������������!" << endl;
				}
				else
				{
					cout << "���ļ���..." << endl;
					int ro = FM->load(v[1]);
					if (ro == 0)
					{
						cout << "���ļ��ɹ�" << endl;
					}
					else
					{
						cout << "���ļ�ʧ��" << endl;
					}
				}
			}
			else if (v[0] == "store")
			{
				if (v.size() != 3)
				{
					cout << "������������!" << endl;
				}
				else
				{
					cout << "���ڱ����ļ�..." << endl;
					int rs = FM->store(v[1], v[2]);
					if (rs == 0)
					{
						cout << "�����ļ��ɹ�" << endl;
					}
					else
					{
						cout << "�����ļ�ʧ��" << endl;
					}
				}
			}
			else if (v[0] == "rm")
			{
				if (v.size() != 2)
				{
					cout << "������������!" << endl;
				}
				else
				{
					cout << "����ɾ���ļ�..." << endl;
					int rrm = FM->rm(v[1]);
					if (rrm == 0)
					{
						cout << "ɾ���ļ��ɹ�" << endl;
					}
					else
					{
						cout << "ɾ���ļ�ʧ��" << endl;
					}
				}
			}
			else if (v[0] == "create")
			{
				if (v.size() != 2)
				{
					cout << "������������!" << endl;
				}
				else
				{
					cout << "�����½��ļ�..." << endl;
					int rc = FM->create(v[1]);
					if (rc == 0)
					{
						cout << "�½��ļ��ɹ�" << endl;
					}
					else
					{
						cout << "�½��ļ�ʧ��" << endl;
					}
				}
			}
			else if (v[0] == "mkdir")
			{
				if (v.size() != 2)
				{
					cout << "������������!" << endl;
				}
				else
				{
					cout << "�����½�Ŀ¼�ļ�..." << endl;
					int rmd = FM->mkdir(v[1]);
					if (rmd == 0)
					{
						cout << "�½�Ŀ¼�ļ��ɹ�" << endl;
					}
					else
					{
						cout << "�½�Ŀ¼�ļ�ʧ��" << endl;
					}
				}
			}
			else if (v[0] == "read")
			{
				if (v.size() != 2)
				{
					cout << "������������!" << endl;
				}
				else
				{
					cout << "���ڶ����ļ�..." << endl;
					int rr = FM->read(v[1]);
					if (rr == 0)
					{
						cout << "�����ļ��ɹ�" << endl;
					}
					else
					{
						cout << "�����ļ�ʧ��" << endl;
					}
				}
			}
			else if (v[0] == "write")
			{
				if (v.size() != 3)
				{
					cout << "������������!" << endl;
				}
				else
				{
					cout << "����д���ļ�..." << endl;
					int rw = FM->write(v[1], v[2]);
					if (rw == 0)
					{
						cout << "д���ļ��ɹ�" << endl;
					}
					else
					{
						cout << "д���ļ�ʧ��" << endl;
					}
				}
			}
			else
			{
				cout << "ָ�����!" << endl;
			}
		}
		cout << ">>>";
		getline(cin, command);
	}
	FM->update();//���ӳ�д����ˢ�����
	delete FM;
	cout << "�ɹ��˳�QIPING�ļ�ϵͳ" << endl;
}

int main()
{
	startSystem();
	return 0;
}