#include "FileManager.h"
#include<fstream>
#include<iostream>
using namespace std;

FileManager::FileManager()
{
	this->m_FileSystem = new FileSystem();
	this->rootDirInode = new Inode();
}

FileManager::~FileManager()
{
	delete this->m_FileSystem;
	delete this->rootDirInode;
}

void FileManager::Initialize()
{

}

int FileManager::format()
{
	return m_FileSystem->format();
}

int FileManager::load(string file_name)
{
	vector<string>path = pathSplit(file_name);
	if (path.size() == 0)
	{
		cout << "file_name��������" << endl;
		return -1;
	}
	int no_di = this->m_FileSystem->getDiskInode(path);
	if (no_di == 0)
	{
		cout << "δ�ҵ����ļ�" << endl;
		return 1;
	}
	string new_name = pathGenerate(path);
	return this->m_FileSystem->loadFile(no_di, new_name);
}

int FileManager::store(string file_name_out, string file_name_in)
{
	ifstream infile(file_name_out, ios::in | ios::binary);
	if (!infile.is_open())
	{
		cerr << file_name_out << " open error!" << endl;
		return -1;
	}
	vector<string>path = pathSplit(file_name_in);
	if (path.size() <= 1)
	{
		cout << "file_name��������" << endl;
		infile.close();
		return -1;
	}
	int no_di = this->m_FileSystem->getDiskInode(path);
	string new_name = path.back();
	path.pop_back();
	int no_fdi = this->m_FileSystem->getDiskInode(path);
	if (no_fdi == 0)
	{
		cout << "��·��������" << endl;
		infile.close();
		return -1;
	}
	if (no_di > 0)//�ļ��Ѵ���
	{
		//��rm
		//int rrm = this->m_FileSystem->removeFile(no_di, no_fdi);
		cout << "���ļ��Ѵ��ڣ���Ҫ��rm" << endl;
		infile.close();
		return -1;
	}
	int rc = this->m_FileSystem->createFile(no_fdi, new_name);
	path.emplace_back(new_name);
	no_di = this->m_FileSystem->getDiskInode(path);
	int rs = this->m_FileSystem->storeFile(infile, no_di);
	infile.close();
	if (rs == 1)
	{
		cout << "myDisk.img�ռ䲻��" << endl;
	}
	else if (rs == 2)
	{
		cout << "�ļ�̫���޷�����myDisk.img" << endl;
	}
	return rs;
}

int FileManager::rm(string file_name)
{
	vector<string>path = pathSplit(file_name);
	if (path.size() == 0)
	{
		cout << "file_name��������" << endl;
		return -1;
	}
	if (path.size() == 1)
	{
		cout << "��ֹɾ����Ŀ¼" << endl;
		return 1;
	}
	int no_di = this->m_FileSystem->getDiskInode(path);
	path.pop_back();
	int no_fdi = this->m_FileSystem->getDiskInode(path);
	if (no_di == 0 || no_fdi == 0)
	{
		cout << "δ�ҵ����ļ�" << endl;
		return 1;
	}
	return this->m_FileSystem->removeFile(no_di, no_fdi);
}

int FileManager::create(string file_name)
{
	vector<string>path = pathSplit(file_name);
	if (path.size() <= 1)
	{
		cout << "file_name��������" << endl;
		return -1;
	}
	string new_name = path.back();
	path.pop_back();
	int no_di = this->m_FileSystem->getDiskInode(path);//����Ŀ¼���DiskInode��
	if (no_di == 0)
	{
		cout << "δ�ҵ���·��" << endl;
		return 1;
	}
	int r = this->m_FileSystem->createFile(no_di, new_name);
	if (r == 1)
	{
		cout << "�ļ��Ѵ���" << endl;
	}
	else if (r == 2)
	{
		cout << "��Ŀ¼����" << endl;
	}
	return r;
}

int FileManager::mkdir(string dir)
{
	vector<string>path = pathSplit(dir);
	if (path.size() <= 1)
	{
		cout << "file_name��������" << endl;
		return -1;
	}
	string new_name = path.back();
	path.pop_back();
	int no_di = this->m_FileSystem->getDiskInode(path);//����Ŀ¼���DiskInode��
	if (no_di == 0)
	{
		cout << "δ�ҵ���·��" << endl;
		return 1;
	}
	int r = this->m_FileSystem->createDir(no_di, new_name);
	if (r == 1)
	{
		cout << "�ļ��Ѵ���" << endl;
	}
	else if (r == 2)
	{
		cout << "��Ŀ¼����" << endl;
	}
	return r;
}

int FileManager::read(string file_name)
{
	vector<string>path = pathSplit(file_name);
	if (path.size() == 0)
	{
		cout << "file_name��������" << endl;
		return -1;
	}
	int no_di = this->m_FileSystem->getDiskInode(path);
	if (no_di == 0)
	{
		cout << "δ�ҵ����ļ�" << endl;
		return 1;
	}
	return this->m_FileSystem->readFile(no_di);
}

int FileManager::write(string file_name, string s)
{
	vector<string>path = pathSplit(file_name);
	if (path.size() <= 1)
	{
		cout << "file_name��������" << endl;
		return -1;
	}
	int no_di = this->m_FileSystem->getDiskInode(path);
	if (no_di == 0)
	{
		cout << "���ļ�������" << endl;
		return -1;
	}
	////�Ƚ���ɾ��
	//int rrm = this->rm(file_name);
	//if (rrm != 0)
	//{
	//	cout << "ɾ���ļ�ʱ����" << endl;
	//	return -1;
	//}
	////���½�һ�����ļ�
	//int rc = this->create(file_name);
	//if (rc != 0)
	//{
	//	cout << "�½��ļ�ʱ����" << endl;
	//	return -1;
	//}
	//no_di = this->m_FileSystem->getDiskInode(path);
	//��д������
	int rw = this->m_FileSystem->writeFile(no_di, s);
	if (rw == 1)
	{
		cout << "myDisk.img�ռ䲻��" << endl;
	}
	return rw;
}

//�����������ӳ�д����д�����
void FileManager::update()
{
	this->m_FileSystem->updateFile();
}

//�з�·��
vector<string> FileManager::pathSplit(string file_name)
{
	vector<string>res;
	int n = file_name.size();
	if (n == 0 || file_name[0] != '/')
	{
		return {};
	}
	res.emplace_back("/");
	for (int i = 1; i < n; ++i)
	{
		string s = "";
		for (;i < n && file_name[i] != '/'; ++i)
		{
			s += file_name[i];
		}
		if (s.size() > 0)
		{
			if (s.size() > 28)//�ļ���̫��
			{
				return {};
			}
			//for (; s.size() < 28;)//����28���ֽ�
			//{
			//	s += ' ';
			//}
			res.emplace_back(s);
		}
		else if (i < n - 1)
		{
			return {};
		}
	}
	return res;
}

//���������ϵ��ļ���
string FileManager::pathGenerate(vector<string>& path)
{
	string res = "_";
	int n = path.size();
	for (int i = 1; i < n; ++i)
	{
		if (i > 1)
		{
			res += "_";
		}
		res += path[i];
	}
	return res;
}
