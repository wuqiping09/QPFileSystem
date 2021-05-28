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

//按空格分割字符串，得到operator和parameter
vector<string> stringSplit(string command)
{
	vector<string>res;
	int n = command.size();
	for (int i = 0; i < n; ++i)
	{
		for (; i < n && command[i] == ' '; ++i);//读入空格
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

//string to int并判断合法性
int stringToInt(string s)
{
	int res = -1;
	if (s.size() <= 30 && s[0] >= '0'&&s[0] <= '9')
	{
		res = atoi(s.c_str());
	}
	return res;
}

//显示help
void showHelp()
{
	cout << "命令说明如下：" << endl;
	cout << "help                                               " << "查看命令说明" << endl;
	cout << "format                                             " << "格式化myDisk.img" << endl;
	cout << "load <filename>                                    " << "将myDisk.img中名为filename的文件读到主机中" << endl;
	cout << "store <filename_out> <filename_in>                 " << "将主机中名为filename_out的文件存入myDisk.img，名为filename_in" << endl;									  	         
	cout << "rm <filename>                                      " << "在myDisk.img中删除名为filename的文件" << endl;
	cout << "create <filename>                                  " << "在myDisk.img中新建文件，名为filename" << endl;
	cout << "mkdir <dir>                                        " << "在myDisk.img中新建目录文件，路径为dir" << endl;
	cout << "read <filename>                                    " << "读出磁盘中名为filename的文件内容" << endl;
	cout << "write <filename> <str>                             " << "向文件中写入str" << endl;
	cout << "shutdown                                           " << "退出QIPING文件系统" << endl;
	cout << "警告：若强制终止程序可能导致缓存数据丢失！" << endl;
}

//启动文件系统，进入命令行模式
void startSystem()
{
	FileManager *FM = new FileManager();
	cout << "欢迎来到QIPING文件系统" << endl;
	cout << "首次使用请格式化myDisk.img" << endl;
	cout << "使用help查看命令说明" << endl;
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
					cout << "help不需要参数" << endl;
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
					cout << "参数个数错误!" << endl;
				}
				else
				{
					cout << "格式化后数据将全部丢失，确定继续？(y继续)";
					string s;
					getline(cin, s);
					if (s == "y")
					{
						cout << "格式化中..." << endl;
						int rf = FM->format();
						if (rf == 0)
						{
							cout << "格式化成功" << endl;
						}
						else
						{
							cout << "格式化失败" << endl;
						}
					}
					else
					{
						cout << "已取消格式化" << endl;
					}
				}
			}
			else if (v[0] == "load")
			{
				if (v.size() != 2)
				{
					cout << "参数个数错误!" << endl;
				}
				else
				{
					cout << "打开文件中..." << endl;
					int ro = FM->load(v[1]);
					if (ro == 0)
					{
						cout << "打开文件成功" << endl;
					}
					else
					{
						cout << "打开文件失败" << endl;
					}
				}
			}
			else if (v[0] == "store")
			{
				if (v.size() != 3)
				{
					cout << "参数个数错误!" << endl;
				}
				else
				{
					cout << "正在保存文件..." << endl;
					int rs = FM->store(v[1], v[2]);
					if (rs == 0)
					{
						cout << "保存文件成功" << endl;
					}
					else
					{
						cout << "保存文件失败" << endl;
					}
				}
			}
			else if (v[0] == "rm")
			{
				if (v.size() != 2)
				{
					cout << "参数个数错误!" << endl;
				}
				else
				{
					cout << "正在删除文件..." << endl;
					int rrm = FM->rm(v[1]);
					if (rrm == 0)
					{
						cout << "删除文件成功" << endl;
					}
					else
					{
						cout << "删除文件失败" << endl;
					}
				}
			}
			else if (v[0] == "create")
			{
				if (v.size() != 2)
				{
					cout << "参数个数错误!" << endl;
				}
				else
				{
					cout << "正在新建文件..." << endl;
					int rc = FM->create(v[1]);
					if (rc == 0)
					{
						cout << "新建文件成功" << endl;
					}
					else
					{
						cout << "新建文件失败" << endl;
					}
				}
			}
			else if (v[0] == "mkdir")
			{
				if (v.size() != 2)
				{
					cout << "参数个数错误!" << endl;
				}
				else
				{
					cout << "正在新建目录文件..." << endl;
					int rmd = FM->mkdir(v[1]);
					if (rmd == 0)
					{
						cout << "新建目录文件成功" << endl;
					}
					else
					{
						cout << "新建目录文件失败" << endl;
					}
				}
			}
			else if (v[0] == "read")
			{
				if (v.size() != 2)
				{
					cout << "参数个数错误!" << endl;
				}
				else
				{
					cout << "正在读入文件..." << endl;
					int rr = FM->read(v[1]);
					if (rr == 0)
					{
						cout << "读入文件成功" << endl;
					}
					else
					{
						cout << "读入文件失败" << endl;
					}
				}
			}
			else if (v[0] == "write")
			{
				if (v.size() != 3)
				{
					cout << "参数个数错误!" << endl;
				}
				else
				{
					cout << "正在写入文件..." << endl;
					int rw = FM->write(v[1], v[2]);
					if (rw == 0)
					{
						cout << "写入文件成功" << endl;
					}
					else
					{
						cout << "写入文件失败" << endl;
					}
				}
			}
			else
			{
				cout << "指令错误!" << endl;
			}
		}
		cout << ">>>";
		getline(cin, command);
	}
	FM->update();//将延迟写内容刷入磁盘
	delete FM;
	cout << "成功退出QIPING文件系统" << endl;
}

int main()
{
	startSystem();
	return 0;
}