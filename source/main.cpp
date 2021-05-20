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

void readFromDisk(string filename)
{
	ofstream outfile(filename);
	if (!outfile.is_open())
	{
		cerr << filename << " open error!" << endl;
		return;
	}
	ifstream myDisk("myDisk.img");
	if (!myDisk.is_open())
	{
		cerr << "myDisk open error!" << endl;
		outfile.close();
		return;
	}
	int block_num = 40;//read from 40# block
	myDisk.seekg(block_num*512, ios::beg);
	char ch = myDisk.get();
	while (ch != EOF)
	{
		//cout << ch;
		outfile.put(ch);
		ch = myDisk.get();
	}
	myDisk.close();
	outfile.close();
}

void writeToDisk(string filename)
{
	ifstream infile(filename);
	if (!infile.is_open())
	{
		cerr << filename << " open error!" << endl;
		return;
	}
	ofstream myDisk("myDisk.img");
	if (!myDisk.is_open())
	{
		cerr << "myDisk open error!" << endl;
		infile.close();
		return;
	}
	int block_num = 40;//write to 40# block
	myDisk.seekp(block_num*512, ios::beg);
	char ch = infile.get();
	while (ch != EOF)
	{
		//cout << ch;
		myDisk.put(ch);
		ch = infile.get();
	}
	myDisk.close();
	infile.close();
}

void tryExample()
{
	writeToDisk("example.txt");
	readFromDisk("example1.txt");
}

class A
{
public:
	int a;
	int b;
	A() { a = -2; b = -1; }
};

void tryMmapWrite()
{
	int FILE_SIZE = 1024 * 1024 * 1024;
	HANDLE hf = fopen("test_mmap.img","r+");
	cout << GetLastError() << endl;//��ͼ�ƶ��ļ�ָ�뵽�ļ���ͷ֮ǰ
	HANDLE hMapFile = CreateFileMapping(hf, NULL, PAGE_READWRITE, 0, FILE_SIZE, "test_mmap");
	cout << GetLastError() << endl;//�����Ч
	LPVOID lpBase = MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, FILE_SIZE);
	//int error = GetLastError();
	A start;
	memcpy((A*)lpBase + 512 * 40, &start, sizeof(A));//write into the 40# block
	UnmapViewOfFile(lpBase);
	CloseHandle(hMapFile);
}

void tryMmapRead()
{
	int FILE_SIZE = 1024 * 1024 * 1024;
	//HANDLE hf = fopen("test_mmap.img", "r+");
	HANDLE hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, FILE_SIZE, "test_mmap");
	LPVOID lpBase = MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, FILE_SIZE);
	A *start = (A *)malloc(sizeof(A));
	memcpy(start, (A*)lpBase + 512 * 40, sizeof(A));//read from the 40# block
	cout << start->a << " " << start->b << endl;
	UnmapViewOfFile(lpBase);
	CloseHandle(hMapFile);
}

void tryBinaryWrite()
{
	A a;
	ofstream outfile;
	outfile.open("test_binary.img", ios::out | ios::_Nocreate | ios::binary);
	if (!outfile.is_open())
	{
		cerr << "test_binary.img open error!" << endl;
		return;
	}
	outfile.seekp(512 * 40, ios::beg);//write into the 40# block
	outfile.write((char*)&a, sizeof(A));
	outfile.close();
}

void tryBinaryRead()
{
	A a;
	ifstream infile;
	infile.open("test_binary.img", ios::in | ios::binary);
	if (!infile.is_open())
	{
		cerr << "test_binary.img open error!" << endl;
		return;
	}
	infile.seekg(512 * 40, ios::beg);//read from the 40# block
	infile.read((char*)&a, sizeof(A));
	cout << a.a << "   " << a.b << endl;
	infile.close();
}

void tryfstream()
{
	fstream outfile("test_fstream.txt", ios::in | ios::out | ios::_Nocreate | ios::binary);
	outfile.seekp(50, ios::beg);
	outfile << "second output" << endl;
	outfile.close();
	ifstream infile("test_fstream4.txt", ios::in | ios::out | ios::_Nocreate | ios::binary);
	infile.close();
}

enum Operator
{
	help,			//��ʾ����
	format,			//��ʽ������
	load,			//�������е��ļ�����������
	store,			//�������е��ļ��������
	rm,				//�ڴ�����ɾ���ļ�
	create,			//�ڴ������½��ļ�
	read,			//���������е��ļ�����
	write,			//���ļ���д������
	lseek			//��λ���ļ��е�ĳλ��
};

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
	cout << "help									�鿴����˵��" << endl;
	cout << "format									��ʽ��myDisk.img" << endl;
	cout << "load <filename>						��myDisk.img����Ϊfilename���ļ�����������" << endl;
	cout << "store <filename_out> <filename_in>		����������Ϊfilename_out���ļ�����myDisk.img����Ϊfilename_in" << endl;
	cout << "rm <filename>							��myDisk.img��ɾ����Ϊfilename���ļ�" << endl;
	cout << "create <filename>						��myDisk.img���½��ļ�����Ϊfilename" << endl;
	cout << "mkdir <dir>							��myDisk.img���½�Ŀ¼�ļ���·��Ϊdir" << endl;
	//cout << "read <filename>						  ���������е��ļ�����" << endl;
	//cout << "write <filename> <str>				  ���ļ���д������" << endl;
	//cout << "lseek <filename> <offset>			  ��λ���ļ��е�ĳλ��" << endl;
	cout << "shutdown								�˳�QIPING�ļ�ϵͳ" << endl;
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
			/*else if (v[0] == "read")
			{
				if (v.size() != 2)
				{
					cout << "������������!" << endl;
				}
				else
				{
					FM->read(v[1]);
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
					FM->write(v[1], v[2]);
				}
			}
			else if (v[0] == "lseek")
			{
				if (v.size() != 3)
				{
					cout << "������������!" << endl;
				}
				else
				{
					int offset = stringToInt(v[2]);
					if (offset == -1)
					{
						cout << "offset����" << endl;
					}
					else
					{
						FM->lseek(v[1], offset);
					}
				}
			}*/
			else
			{
				cout << "ָ�����!" << endl;
			}
		}
		cout << ">>>";
		getline(cin, command);
	}
	delete FM;
	cout << "�ɹ��˳�QIPING�ļ�ϵͳ" << endl;
}

int main()
{
	startSystem();
	return 0;
}