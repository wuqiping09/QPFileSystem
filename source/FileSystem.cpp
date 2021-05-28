#define _CRT_SECURE_NO_WARNINGS
#include "FileSystem.h"
#include "DirectoryEntry.h"
#include "BufferManager.h"
#include<fstream>
#include<iostream>
#include<algorithm>
using namespace std;

FileSystem::FileSystem()
{
	this->m_BufferManager = new BufferManager();
	this->updlock = 0;
	for (int i = 0; i < 80; ++i)
	{
		disk_inodes[i] = nullptr;
	}
	this->Initialize();
}

FileSystem::~FileSystem()
{
	delete this->m_BufferManager;
}

void FileSystem::Initialize()
{
	this->m_BufferManager->Initialize();
}

//��ʽ��myDisk.img
int FileSystem::format()
{
	//�½�myDisk.img���½�superblock��д��myDisk.img
	SuperBlock *sb = new SuperBlock();
	int rs = storeSuperBlock("myDisk.img", sb);
	delete sb;
	//showSuperBlock();
	//�½�2#��6#DiskInode��д��myDisk.img������DiskInode����
	int rdi = formatDiskInode();
	//д��1#��5#���ݿ飬������������������̿鴮������
	int rdb = formatDataBlock();
	if (rs == 0 && rdi == 0 && rdb == 0)//���߶��ɹ�
	{
		return 0;
	}
	return -1;
}

//��ʽ��DiskInode��
int FileSystem::formatDiskInode()
{
	//�½�2#��6#DiskInode��д��myDisk.img������DiskInode����
	ofstream outfile("myDisk.img", ios::out | ios::_Nocreate | ios::binary);
	if (!outfile.is_open())
	{
		cerr << "myDisk.img open error!" << endl;
		return -1;
	}
	int num_diskinode = INODE_NUMBER_PER_SECTOR * INODE_ZONE_SIZE;//DiskInode�ĸ���
	for (int i = 0; i < num_diskinode; ++i)
	{
		DiskInode *di = new DiskInode();
		if (i == 2)//��Ŀ¼
		{
			di->d_nlink = 1;
			di->d_mode = DiskInode::IDIR;
			di->d_size = 6 * (DirectoryEntry::DIRSIZ + 4);//6��Ŀ¼��
			di->d_addr[0] = 1;//�洢��1#���ݿ���
		}
		else if (i == 3)//binĿ¼
		{
			di->d_nlink = 1;
			di->d_mode = DiskInode::IDIR;
			di->d_size = 2 * (DirectoryEntry::DIRSIZ + 4);//2��Ŀ¼��
			di->d_addr[0] = 2;//�洢��2#���ݿ���
		}
		else if (i == 4)//etcĿ¼
		{
			di->d_nlink = 1;
			di->d_mode = DiskInode::IDIR;
			di->d_size = 2 * (DirectoryEntry::DIRSIZ + 4);//2��Ŀ¼��
			di->d_addr[0] = 3;//�洢��3#���ݿ���
		}
		else if (i == 5)//homeĿ¼
		{
			di->d_nlink = 1;
			di->d_mode = DiskInode::IDIR;
			di->d_size = 2 * (DirectoryEntry::DIRSIZ + 4);//2��Ŀ¼��
			di->d_addr[0] = 4;//�洢��4#���ݿ���
		}
		else if (i == 6)//devĿ¼
		{
			di->d_nlink = 1;
			di->d_mode = DiskInode::IDIR;
			di->d_size = 2 * (DirectoryEntry::DIRSIZ + 4);//2��Ŀ¼��
			di->d_addr[0] = 5;//�洢��5#���ݿ���
		}
		outfile.seekp(INODE_ZONE_START_SECTOR * Inode::BLOCK_SIZE + i * 64, ios::beg);//�������2��������ÿ��DiskInode��СΪ64�ֽ�
		outfile.write((char*)di, sizeof(DiskInode));
		delete di;
	}
	outfile.close();
	return 0;
}

//��ʽ��������
int FileSystem::formatDataBlock()
{
	ofstream outfile("myDisk.img", ios::out | ios::_Nocreate | ios::binary);
	if (!outfile.is_open())
	{
		cerr << "myDisk.img open error!" << endl;
		return -1;
	}
	//д��1#��5#���ݿ�
	for (int i = 0; i < 5; ++i)
	{
		if (i == 0)//��Ŀ¼
		{
			DirectoryEntry *de = new DirectoryEntry(".", 2);//�Լ���Ŀ¼��
			outfile.seekp(DATA_ZONE_START_SECTOR*Inode::BLOCK_SIZE, ios::beg);
			//cout << sizeof(de->m_name) << "   " << de->m_name << endl;
			outfile.write((char*)de, 32);
			//outfile.write((char*)de->m_name.c_str(), 28);
			//outfile.write((char*)&(de->m_ino), 4);
			delete de;
			de = new DirectoryEntry("..", 2);//��Ŀ¼��
			outfile.seekp(DATA_ZONE_START_SECTOR*Inode::BLOCK_SIZE + 32, ios::beg);
			outfile.write((char*)de, 32);
			//outfile.write((char*)de->m_name.c_str(), 28);
			//outfile.write((char*)&(de->m_ino), 4);
			delete de;
			de = new DirectoryEntry("bin", 3);//binĿ¼��
			outfile.seekp(DATA_ZONE_START_SECTOR*Inode::BLOCK_SIZE + 32 * 2, ios::beg);
			outfile.write((char*)de, 32);
			//outfile.write((char*)de->m_name.c_str(), 28);
			//outfile.write((char*)&(de->m_ino), 4);
			delete de;
			de = new DirectoryEntry("etc", 4);//etcĿ¼��
			outfile.seekp(DATA_ZONE_START_SECTOR*Inode::BLOCK_SIZE + 32 * 3, ios::beg);
			outfile.write((char*)de, 32);
			//outfile.write((char*)de->m_name.c_str(), 28);
			//outfile.write((char*)&(de->m_ino), 4);
			delete de;
			de = new DirectoryEntry("home", 5);//homeĿ¼��
			outfile.seekp(DATA_ZONE_START_SECTOR*Inode::BLOCK_SIZE + 32 * 4, ios::beg);
			outfile.write((char*)de, 32);
			//outfile.write((char*)de->m_name.c_str(), 28);
			//outfile.write((char*)&(de->m_ino), 4);
			delete de;
			de = new DirectoryEntry("dev", 6);//devĿ¼��
			outfile.seekp(DATA_ZONE_START_SECTOR*Inode::BLOCK_SIZE + 32 * 5, ios::beg);
			outfile.write((char*)de, 32);
			//outfile.write((char*)de->m_name.c_str(), 28);
			//outfile.write((char*)&(de->m_ino), 4);
			delete de;
		}
		else if (i == 1)//binĿ¼
		{
			DirectoryEntry *de = new DirectoryEntry(".", 3);//�Լ���Ŀ¼��
			outfile.seekp((DATA_ZONE_START_SECTOR + 1)*Inode::BLOCK_SIZE, ios::beg);
			outfile.write((char*)de, 32);
			//outfile.write((char*)de->m_name.c_str(), 28);
			//outfile.write((char*)&(de->m_ino), 4);
			delete de;
			de = new DirectoryEntry("..", 2);//��Ŀ¼��
			outfile.seekp((DATA_ZONE_START_SECTOR + 1)*Inode::BLOCK_SIZE + 32, ios::beg);
			outfile.write((char*)de, 32);
			//outfile.write((char*)de->m_name.c_str(), 28);
			//outfile.write((char*)&(de->m_ino), 4);
			delete de;
		}
		else if (i == 2)//etcĿ¼
		{
			DirectoryEntry *de = new DirectoryEntry(".", 4);//�Լ���Ŀ¼��
			outfile.seekp((DATA_ZONE_START_SECTOR + 2)*Inode::BLOCK_SIZE, ios::beg);
			outfile.write((char*)de, 32);
			//outfile.write((char*)de->m_name.c_str(), 28);
			//outfile.write((char*)&(de->m_ino), 4);
			delete de;
			de = new DirectoryEntry("..", 2);//��Ŀ¼��
			outfile.seekp((DATA_ZONE_START_SECTOR + 2)*Inode::BLOCK_SIZE + 32, ios::beg);
			outfile.write((char*)de, 32);
			//outfile.write((char*)de->m_name.c_str(), 28);
			//outfile.write((char*)&(de->m_ino), 4);
			delete de;
		}
		else if (i == 3)//homeĿ¼
		{
			DirectoryEntry *de = new DirectoryEntry(".", 5);//�Լ���Ŀ¼��
			outfile.seekp((DATA_ZONE_START_SECTOR + 3)*Inode::BLOCK_SIZE, ios::beg);
			outfile.write((char*)de, 32);
			//outfile.write((char*)de->m_name.c_str(), 28);
			//outfile.write((char*)&(de->m_ino), 4);
			delete de;
			de = new DirectoryEntry("..", 2);//��Ŀ¼��
			outfile.seekp((DATA_ZONE_START_SECTOR + 3)*Inode::BLOCK_SIZE + 32, ios::beg);
			outfile.write((char*)de, 32);
			//outfile.write((char*)de->m_name.c_str(), 28);
			//outfile.write((char*)&(de->m_ino), 4);
			delete de;
		}
		else//devĿ¼
		{
			DirectoryEntry *de = new DirectoryEntry(".", 6);//�Լ���Ŀ¼��
			outfile.seekp((DATA_ZONE_START_SECTOR + 4)*Inode::BLOCK_SIZE, ios::beg);
			outfile.write((char*)de, 32);
			//outfile.write((char*)de->m_name.c_str(), 28);
			//outfile.write((char*)&(de->m_ino), 4);
			delete de;
			de = new DirectoryEntry("..", 2);//��Ŀ¼��
			outfile.seekp((DATA_ZONE_START_SECTOR + 4)*Inode::BLOCK_SIZE + 32, ios::beg);
			outfile.write((char*)de, 32);
			//outfile.write((char*)de->m_name.c_str(), 28);
			//outfile.write((char*)&(de->m_ino), 4);
			delete de;
		}
	}
	//�����������ݿ�
	int head = 105;//��105#���ݿ鿪ʼ
	while (head != 0)
	{
		int next = 0;//��һ��head�����ݿ��
		if (head + 100 <= DATA_ZONE_SIZE)
		{
			next = head + 100;
		}
		//��ʼ4�ֽڴ��s_nfree
		int s_nfree = next == 0 ? DATA_ZONE_SIZE - head : 100;
		outfile.seekp((DATA_ZONE_START_SECTOR + head - 1)*Inode::BLOCK_SIZE, ios::beg);
		outfile.write((char*)&s_nfree, 4);
		//������4���ֽڴ����һ��head�����ݿ�ţ���head+100��0
		outfile.seekp((DATA_ZONE_START_SECTOR + head - 1)*Inode::BLOCK_SIZE + 4, ios::beg);
		outfile.write((char*)&next, 4);
		//����4*(s_nfree-1)���ֽڴ��s_nfree-1�������ݿ�ţ���head+1��head+s_nfree-1
		for (int i = 1; i < s_nfree; ++i)
		{
			int num = i + head;
			outfile.seekp((DATA_ZONE_START_SECTOR + head - 1)*Inode::BLOCK_SIZE + 4 + 4 * i, ios::beg);
			outfile.write((char*)&num, 4);
		}
		head = next;
	}
	outfile.close();
	return 0;
}

//ϵͳ��ʼ��ʱ����SuperBlock
SuperBlock* FileSystem::loadSuperBlock(string disk_name)
{
	SuperBlock* sb = new SuperBlock();
	ifstream infile(disk_name, ios::in | ios::binary);
	//infile.open(disk_name, ios::in | ios::binary);
	if (!infile.is_open())
	{
		cerr << disk_name << " open error!" << endl;
		return nullptr;
	}
	infile.seekg(0, ios::beg);
	infile.read((char*)sb, sizeof(SuperBlock));//����0#��1#���̿�
	infile.close();
	return sb;
}

//����SuperBlock��myDisk.img��
int FileSystem::storeSuperBlock(string disk_name, SuperBlock* sb)
{
	ofstream outfile(disk_name, ios::out | ios::binary | ios::_Nocreate);
	//outfile.open(disk_name, ios::out | ios::binary);
	if (!outfile.is_open())
	{
		cerr << disk_name << " open error!" << endl;
		return -1;
	}
	outfile.seekp(0, ios::beg);
	outfile.write((char*)sb, sizeof(SuperBlock));//д��0#��1#���̿�
	outfile.close();
	return 0;
}

void FileSystem::showSuperBlock()
{
	SuperBlock *sb = loadSuperBlock("myDisk.img");
	cout << sb->s_nfree << endl;
	delete sb;
}

//����һ������DiskInode
int FileSystem::iAlloc()
{
	SuperBlock *sb = loadSuperBlock("myDisk.img");
	int res = sb->s_inode[--sb->s_ninode];
	if (sb->s_ninode == 0)//û�п���DiskInode��
	{
		/* ȥDiskInode����min(100,��������)��DiskInode */
		ifstream infile("myDisk.img", ios::in | ios::binary);
		if (!infile.is_open())
		{
			cerr << "myDisk.img open error!" << endl;
			storeSuperBlock("myDisk.img", sb);
			delete sb;
			return 0;
		}
		//��DiskInode��
		int offset = 2;//0#��1#DiskInode������
		for (; offset < INODE_NUMBER_PER_SECTOR * INODE_ZONE_SIZE && sb->s_ninode < 100;)
		{
			DiskInode *di = new DiskInode();
			infile.seekg(INODE_ZONE_START_SECTOR*Inode::BLOCK_SIZE + offset * sizeof(DiskInode), ios::beg);
			infile.read((char*)di, sizeof(DiskInode));
			if (di->d_addr[0] == 0)//����
			{
				sb->s_inode[sb->s_ninode++] = offset;
			}
			delete di;
		}
		infile.close();
	}
	storeSuperBlock("myDisk.img", sb);
	delete sb;
	return res;
}

//�ͷű��Ϊnumber��DiskInode
void FileSystem::iFree(int number)
{
	SuperBlock *sb = loadSuperBlock("myDisk.img");
	/* ��ϵͳֻ��80��DiskInode�����s_inode[100]�������� */
	sb->s_inode[sb->s_ninode++] = number;
	storeSuperBlock("myDisk.img", sb);
	delete sb;
}

//������д��̿�
int FileSystem::alloc()
{
	SuperBlock *sb = loadSuperBlock("myDisk.img");
	int res = sb->s_free[--sb->s_nfree];
	if (sb->s_nfree == 0)//û�п������ݿ���
	{
		ifstream infile("myDisk.img", ios::in | ios::binary);
		if (!infile.is_open())
		{
			cerr << "myDisk.img open error!" << endl;
			storeSuperBlock("myDisk.img", sb);
			delete sb;
			return 0;
		}
		//����res#���ݿ��ǰ404�ֽڵ�s_nfree��s_free[100]
		infile.seekg((DATA_ZONE_START_SECTOR + res - 1)*Inode::BLOCK_SIZE, ios::beg);
		infile.read((char*)&sb->s_nfree, 4);
		infile.read((char*)sb->s_free, 400);
		infile.close();
	}
	storeSuperBlock("myDisk.img", sb);
	delete sb;
	return res;
}

//�ͷű��Ϊblkno�Ĵ��̿�
void FileSystem::free(int blkno)
{
	SuperBlock *sb = loadSuperBlock("myDisk.img");
	if (sb->s_nfree == 100)//SuperBlock������
	{
		ofstream outfile("myDisk.img", ios::in | ios::binary | ios::_Nocreate);
		if (!outfile.is_open())
		{
			cerr << "myDisk.img open error!" << endl;
			storeSuperBlock("myDisk.img", sb);
			delete sb;
			return;
		}
		//��s_nfree��s_free[100]���Ƶ�blkno#����
		outfile.seekp((DATA_ZONE_START_SECTOR + blkno - 1)*Inode::BLOCK_SIZE, ios::beg);
		outfile.write((char*)&sb->s_nfree, 4);
		outfile.write((char*)sb->s_free, 400);
		outfile.close();
		//SuperBlock�д���ÿ�
		sb->s_nfree = 1;
		sb->s_free[0] = blkno;
	}
	else
	{
		sb->s_free[sb->s_nfree++] = blkno;
	}
	storeSuperBlock("myDisk.img", sb);
	delete sb;
}

//��ȡ�ļ���DiskInode��
int FileSystem::getDiskInode(vector<string>& path)
{
	ifstream infile("myDisk.img", ios::in | ios::binary);
	if (!infile.is_open())
	{
		cerr << "myDisk.img open error!" << endl;
		return -1;
	}
	int cur_di = 2;//�Ӹ�Ŀ¼��ʼ
	int n = path.size();
	for (int i = 1; i < n; ++i)
	{
		//����current DiskInode
		DiskInode *di = new DiskInode();
		infile.seekg(INODE_ZONE_START_SECTOR*Inode::BLOCK_SIZE + cur_di * 64, ios::beg);
		infile.read((char*)di, sizeof(DiskInode));
		if ((di->d_mode&DiskInode::IDIR) == 0)//����ͬ������ͨ�ļ�����һ��û�д�����Ŀ¼�ļ�
		{
			//�˳�ѭ��������0
			cur_di = 0;
			delete di;
			break;
		}
		int len_read = 0;//�Ѷ�����
		int file_length = di->d_size;//Ŀ¼�ļ��ĳ���
		int num_block = file_length / Inode::BLOCK_SIZE;//��Ҫ�����ݿ�����
		int cur_addr = 0;//��ǰd_addr�е��±�
		int cur_block = di->d_addr[0];//��ǰ���ݿ��
		int cur_entry_offset = 0;//��ǰĿ¼���ƫ����
		//�ڵ�ǰĿ¼����path[i]
		while (len_read < file_length)
		{
			DirectoryEntry *cur_de = new DirectoryEntry();
			infile.seekg((DATA_ZONE_START_SECTOR + cur_block - 1)*Inode::BLOCK_SIZE + cur_entry_offset * 32, ios::beg);
			//infile.read((char*)cur_de->m_name, 28);
			//infile.read((char*)cur_de->m_ino, 4);
			infile.read((char*)cur_de, 32);
			//cout << cur_de->m_name << "   " << cur_de->m_ino << endl;
			if (cur_de->m_name == path[i])//�ҵ�
			{
				cur_di = cur_de->m_ino;
				delete cur_de;
				break;
			}
			len_read += 32;
			if (cur_entry_offset == Inode::BLOCK_SIZE / 4)//�����ݿ����һ��Ŀ¼��
			{
				++cur_addr;//��һ��d_addr������Ŀ¼�ļ�����ֱ������
				cur_block = di->d_addr[cur_addr];
				cur_entry_offset = 0;
			}
			else
			{
				++cur_entry_offset;//��һ��Ŀ¼��
			}
			delete cur_de;
		}
		if (len_read < file_length)
		{
			if (i == n - 1)//�ҵ��ļ�
			{
				delete di;
				break;
			}
			else//��������Ŀ¼����
			{
				DirectoryEntry *cur_de = new DirectoryEntry();
				infile.seekg((DATA_ZONE_START_SECTOR + cur_block - 1)*Inode::BLOCK_SIZE + cur_entry_offset * 32, ios::beg);
				infile.read((char*)cur_de, 32);
				cur_di = cur_de->m_ino;
				delete cur_de;
			}
		}
		else//δ�ҵ�
		{
			//�˳�ѭ��������0
			i = n;
			cur_di = 0;
		}
		//delete de;
		delete di;
	}
	infile.close();
	return cur_di;
}

//����һ�����ݿ鵽�ڴ�
void FileSystem::loadBlock(int block_no, int length, ifstream& infile, char* data)
{
	infile.seekg((DATA_ZONE_START_SECTOR + block_no - 1)*Inode::BLOCK_SIZE, ios::beg);
	infile.read((char*)data, length);
}

//����һ�����ݿ鵽�ļ�
void FileSystem::loadBlock(int block_no, int length, ifstream& infile, ofstream& outfile, bool is_dir)
{
	char *data = new char[length];
	//�ȶ��뵽data��
	loadBlock(block_no, length, infile, data);
	//��д�뵽outfile��
	if (is_dir)//��Ŀ¼�ļ���ֻ��ʾĿ¼���е��ļ���
	{
		int num_de = length / sizeof(DirectoryEntry);//Ŀ¼�����
		for (int i = 0; i < num_de; ++i)
		{
			int offset = i * 32;
			int j = offset;
			for (; j < offset + 28; ++j)//�ļ���
			{
				if (data[j] != '\0')
				{
					outfile.put(data[j]);
				}
				else
				{
					break;
				}
			}
			outfile << endl;
		}
	}
	else//��ͨ�ļ�������ȫ��д���ļ�
	{
		outfile.write((char*)data, length);
	}
	delete data;
}

//����һ�����ݿ��������Ļ
void FileSystem::loadBlock(int block_no, int length, ifstream& infile, bool is_dir)
{
	char *data = new char[length];
	//�ȶ��뵽data��
	loadBlock(block_no, length, infile, data);
	//��д�뵽outfile��
	if (is_dir)//��Ŀ¼�ļ���ֻ��ʾĿ¼���е��ļ���
	{
		int num_de = length / sizeof(DirectoryEntry);//Ŀ¼�����
		for (int i = 0; i < num_de; ++i)
		{
			int offset = i * 32;
			int j = offset;
			for (; j < offset + 28; ++j)//�ļ���
			{
				if (data[j] != '\0')
				{
					cout << data[j];
				}
				else
				{
					break;
				}
			}
			cout << endl;
		}
	}
	else//��ͨ�ļ�������ȫ��д���ļ�
	{
		for (int i = 0; i < length; ++i)
		{
			cout << data[i];
		}
	}
	delete data;
}

//���������������Ļ
void FileSystem::displayBufferData(Buf* bp, int length, bool is_dir)
{
	if (is_dir)
	{
		int num_de = length / sizeof(DirectoryEntry);//Ŀ¼�����
		for (int i = 0; i < num_de; ++i)
		{
			int offset = i * 32;
			int j = offset;
			for (; j < offset + 28; ++j)//�ļ���
			{
				if (*(bp->b_addr+j) != '\0')
				{
					cout << *(bp->b_addr + j);
				}
				else
				{
					break;
				}
			}
			cout << endl;
		}
	}
	else
	{
		for (int i = 0; i < length; ++i)
		{
			cout << *(bp->b_addr + i);
		}
	}
}

//���ļ�д��һ�����ݿ�
void FileSystem::storeBlock(int block_no, int length, std::fstream& inoutfile, std::ifstream& infile)
{
	char *data = new char[length];
	//�ȶ��뵽data��
	infile.read((char*)data, length);
	//��д�뵽outfile��
	storeBlock(block_no, length, inoutfile, data);
	delete data;
}

//���ڴ�д��һ�����ݿ�
void FileSystem::storeBlock(int block_no, int length, std::fstream& inoutfile, char* data)
{
	inoutfile.seekp((DATA_ZONE_START_SECTOR + block_no - 1)*Inode::BLOCK_SIZE, ios::beg);
	inoutfile.write((char*)data, length);
}

//��myDisk.img�����ļ�������
int FileSystem::loadFile(int no_diskinode, string file_name)
{
	ifstream infile("myDisk.img", ios::in | ios::binary);
	if (!infile.is_open())
	{
		cerr << "myDisk.img open error!" << endl;
		return -1;
	}
	ofstream outfile(file_name, ios::out | ios::binary);
	if (!outfile.is_open())
	{
		cerr << file_name << " open error!" << endl;
		infile.close();
		return -1;
	}
	//�Ȼ�ȡDiskInode
	DiskInode *di = new DiskInode();
	infile.seekg(INODE_ZONE_START_SECTOR*Inode::BLOCK_SIZE + no_diskinode * sizeof(DiskInode), ios::beg);
	infile.read((char*)di, sizeof(DiskInode));
	bool is_dir = (di->d_mode&DiskInode::IDIR) == 0 ? false : true;//�Ƿ���Ŀ¼�ļ�
	//�����ݿ�
	int len_read = 0;//�Ѷ��ļ�����
	int file_length = di->d_size;//�ļ�����
	int cur_addr = 0;//��d_addr[0]��ʼ��
	int num_block = file_length / Inode::BLOCK_SIZE + 1;//��Ҫ�������ݿ�
	int num_block_read = 0;//�Ѷ����ݿ����
	while (len_read < file_length)//û����
	{
		//��ȡ��ǰ���ݿ�
		int cur_block_no = di->d_addr[cur_addr];
		if (cur_addr > 7)//��������
		{
			//������������
			int index_blocks_no[Inode::BLOCK_SIZE / 4];
			int num_index1_to_read = min(Inode::BLOCK_SIZE / 4, (num_block - num_block_read) / (Inode::BLOCK_SIZE / 4) + 1);//�˴���Ҫ����һ����������
			infile.seekg((DATA_ZONE_START_SECTOR + cur_block_no - 1)*Inode::BLOCK_SIZE, ios::beg);
			infile.read((char*)index_blocks_no, num_index1_to_read * 4);
			for (int i = 0; i < num_index1_to_read; ++i)
			{
				//����һ��������
				int blocks_no[Inode::BLOCK_SIZE];
				int num_block_to_read = min(Inode::BLOCK_SIZE / 4, num_block - num_block_read);//�˴���Ҫ���Ĵ��̿����
				infile.seekg((DATA_ZONE_START_SECTOR + index_blocks_no[i] - 1)*Inode::BLOCK_SIZE, ios::beg);
				infile.read((char*)blocks_no, num_block_to_read * 4);
				//��ÿ�����ݿ�
				for (int j = 0; j < num_block_to_read; ++j)
				{
					int len_to_read = min(Inode::BLOCK_SIZE, file_length - len_read);
					loadBlock(blocks_no[j], len_to_read, infile, outfile, is_dir);
					len_read += len_to_read;
					++num_block_read;
				}
			}
			++cur_addr;//��һ��d_addr
		}
		else if (cur_addr > 5)//һ������
		{
			//������������
			int blocks_no[Inode::BLOCK_SIZE / 4];
			int num_block_to_read = min(Inode::BLOCK_SIZE / 4, num_block - num_block_read);
			infile.seekg((DATA_ZONE_START_SECTOR + cur_block_no - 1)*Inode::BLOCK_SIZE, ios::beg);
			infile.read((char*)blocks_no, num_block_to_read * 4);
			//��ÿ�����ݿ�
			for (int i = 0; i < num_block_to_read; ++i)
			{
				int len_to_read = min(Inode::BLOCK_SIZE, file_length - len_read);
				loadBlock(blocks_no[i], len_to_read, infile, outfile, is_dir);
				len_read += len_to_read;
				++num_block_read;
			}
			++cur_addr;//��һ��d_addr
		}
		else//ֱ������
		{
			int len_to_read = min(Inode::BLOCK_SIZE, file_length - len_read);
			loadBlock(cur_block_no, len_to_read, infile, outfile, is_dir);
			len_read += len_to_read;
			++num_block_read;
			++cur_addr;//��һ��d_addr
		}
	}
	delete di;
	outfile.close();
	infile.close();
	return 0;
}

//���������ļ���myDisk.img
int FileSystem::storeFile(ifstream& infile, int no_diskinode)
{
	/* ���ļ��Ѵ��ڣ�д�����ݼ��� */
	/* ��ȡ�ļ��ĳ��� */
	infile.seekg(0, ios::end);
	int file_length = infile.tellg();
	infile.seekg(0, ios::beg);
	int num_block = file_length / Inode::BLOCK_SIZE + 1;//��Ҫ���̿���
	if (num_block > Inode::HUGE_FILE_BLOCK)//�ļ�̫��治��
	{
		infile.close();
		return 2;
	}
	/* �Ȼ�ȡnum_block-1���̿� */
	vector<int>blocks_no;
	for (int i = 0; i < num_block - 1; ++i)
	{
		int block_no = alloc();
		if (block_no == 0)//û���㹻�Ĵ��̿�
		{
			//�ͷ��Ѿ�����Ĵ��̿�
			int n = blocks_no.size();
			for (int j = 0; j < n; ++j)
			{
				free(blocks_no[j]);
			}
			return 1;
		}
		blocks_no.emplace_back(block_no);
	}
	//��Ӷ����������
	if (num_block > Inode::LARGE_FILE_BLOCK)//�����ļ�
	{
		int num_index1 = num_block / (Inode::BLOCK_SIZE / 4) + 1;//��Ҫ��һ������������
		int num_index2 = num_index1 / (Inode::BLOCK_SIZE / 4) + 1;//��Ҫ�Ķ�������������
		if (num_index2 > 1)//��Ҫ��������������
		{
			//����Ҫ128��һ��������
			for (int i = 0; i < Inode::BLOCK_SIZE / 4; ++i)
			{
				int block_no = alloc();
				if (block_no == 0)//û���㹻�Ĵ��̿�
				{
					int n = blocks_no.size();
					for (int j = 0; j < n; ++j)
					{
						free(blocks_no[j]);
					}
					return 1;
				}
				blocks_no.emplace_back(block_no);
			}
			num_index1 -= Inode::BLOCK_SIZE / 4;
			//һ������������
			int block_no = alloc();
			if (block_no == 0)//û���㹻�Ĵ��̿�
			{
				int n = blocks_no.size();
				for (int j = 0; j < n; ++j)
				{
					free(blocks_no[j]);
				}
				return 1;
			}
			blocks_no.emplace_back(block_no);
		}
		//����Ҫnum_index1��һ��������
		for (int i = 0; i < num_index1; ++i)
		{
			int block_no = alloc();
			if (block_no == 0)//û���㹻�Ĵ��̿�
			{
				int n = blocks_no.size();
				for (int j = 0; j < n; ++j)
				{
					free(blocks_no[j]);
				}
				return 1;
			}
			blocks_no.emplace_back(block_no);
		}
		//һ������������
		int block_no = alloc();
		if (block_no == 0)//û���㹻�Ĵ��̿�
		{
			int n = blocks_no.size();
			for (int j = 0; j < n; ++j)
			{
				free(blocks_no[j]);
			}
			return 1;
		}
		blocks_no.emplace_back(block_no);
	}
	else if (num_block > Inode::SMALL_FILE_BLOCK)//���ļ�
	{
		if (num_block > 6 + 128)//��Ҫ����һ��������
		{
			int block_no = alloc();
			if (block_no == 0)//û���㹻�Ĵ��̿�
			{
				int n = blocks_no.size();
				for (int j = 0; j < n; ++j)
				{
					free(blocks_no[j]);
				}
				return 1;
			}
			blocks_no.emplace_back(block_no);
		}
		int block_no = alloc();
		if (block_no == 0)//û���㹻�Ĵ��̿�
		{
			int n = blocks_no.size();
			for (int j = 0; j < n; ++j)
			{
				free(blocks_no[j]);
			}
			return 1;
		}
		blocks_no.emplace_back(block_no);
	}
	fstream inoutfile("myDisk.img", ios::in | ios::out | ios::_Nocreate | ios::binary);
	if (!inoutfile.is_open())
	{
		cerr << "myDisk.img open error!" << endl;
		return -1;
	}
	/* �Ȼ�ȡDiskInode */
	DiskInode *di = new DiskInode();
	inoutfile.seekg(INODE_ZONE_START_SECTOR*Inode::BLOCK_SIZE + no_diskinode * sizeof(DiskInode), ios::beg);
	inoutfile.read((char*)di, sizeof(DiskInode));
	int len_write = 0;//��д����
	int num_block_store = 0;//�Ѵ����ݿ����
	int num_index1 = num_block / (Inode::BLOCK_SIZE / 4) + 1;//��Ҫ��һ������������
	int cur_addr = 0;
	while (len_write < file_length)//ûд��
	{
		if (cur_addr > 7)//��������
		{
			//��ȡһ������������
			int index2_block_no = blocks_no.back();
			blocks_no.pop_back();
			//�Ǽ���d_addr
			di->d_addr[cur_addr] = index2_block_no;
			int index_blocks_no[Inode::BLOCK_SIZE / 4];
			int num_index1_to_store = min(Inode::BLOCK_SIZE / 4, num_index1);//�˴���Ҫ���һ����������
			for (int i = 0; i < num_index1_to_store; ++i)
			{
				//��ȡһ��һ��������
				int index1_block_no = blocks_no.back();
				blocks_no.pop_back();
				//д�������������
				index_blocks_no[i] = index1_block_no;
				int direct_blocks_no[Inode::BLOCK_SIZE];
				int num_block_to_store = min(Inode::BLOCK_SIZE / 4, num_block - num_block_store);//�˴���Ҫд�Ĵ��̿����
				//дÿ�����ݿ�
				for (int j = 0; j < num_block_to_store; ++j)
				{
					int block_no = blocks_no.back();
					blocks_no.pop_back();
					//д�������������
					direct_blocks_no[j] = block_no;
					int len_to_write = min(Inode::BLOCK_SIZE, file_length - len_write);
					storeBlock(block_no, len_to_write, inoutfile, infile);
					len_write += len_to_write;
					++num_block_store;

				}
				//д��һ��������
				storeBlock(index1_block_no, num_block_to_store * 4, inoutfile, (char*)direct_blocks_no);
			}
			//д�����������
			storeBlock(index2_block_no, num_index1_to_store * 4, inoutfile, (char*)index_blocks_no);
			num_index1 -= num_index1_to_store;
			++cur_addr;//��һ��d_addr
		}
		else if (cur_addr > 5)//һ������
		{
			int index_block_no = blocks_no.back();//��ȡһ�����ݿ���Ϊ������
			blocks_no.pop_back();
			int num_block_to_store = min(num_block - num_block_store, Inode::BLOCK_SIZE / 4);//�˿�����Ҫ�ŵ���������
			//�Ǽ���d_addr
			di->d_addr[cur_addr] = index_block_no;
			int blocks_index_no[Inode::BLOCK_SIZE/4];//��¼���������ڵ�������
			for (int i = 0; i < num_block_to_store; ++i)
			{
				//ȡһ�����ݿ�
				int block_no = blocks_no.back();
				blocks_no.pop_back();
				blocks_index_no[i] = block_no;
				//д������
				int len_to_write = min(file_length - len_write, Inode::BLOCK_SIZE);
				storeBlock(block_no, len_to_write, inoutfile, infile);
				len_write += len_to_write;
				++num_block_store;
			}
			//д��������
			storeBlock(index_block_no, num_block_to_store * 4, inoutfile, (char*)blocks_index_no);
			--num_index1;
			++cur_addr;
		}
		else//ֱ������
		{
			int block_no = di->d_addr[cur_addr];
			if (block_no == 0)//δ�������ݿ�
			{
				//ȡһ�����ݿ�
				block_no = blocks_no.back();
				blocks_no.pop_back();
				//�Ǽ���d_addr
				di->d_addr[cur_addr] = block_no;
			}
			//д������
			int len_to_write = min(file_length - len_write, Inode::BLOCK_SIZE);
			storeBlock(block_no, len_to_write, inoutfile, infile);
			len_write += len_to_write;
			++num_block_store;
			++cur_addr;
		}
	}
	//�����ļ�����
	di->d_size = file_length;
	//д��DiskInode
	inoutfile.seekp(INODE_ZONE_START_SECTOR*Inode::BLOCK_SIZE + no_diskinode * sizeof(DiskInode), ios::beg);
	inoutfile.write((char*)di, sizeof(DiskInode));
	delete di;
	inoutfile.close();
	return 0;
}

//�½��ļ�
int FileSystem::createFile(int no_diskinode, std::string file_name)
{
	fstream inoutfile("myDisk.img", ios::in | ios::out | ios::_Nocreate | ios::binary);
	if (!inoutfile.is_open())
	{
		cerr << "myDisk.img open error!" << endl;
		return -1;
	}
	/* �Ȼ�ȡDiskInode */
	DiskInode *di = new DiskInode();
	inoutfile.seekg(INODE_ZONE_START_SECTOR*Inode::BLOCK_SIZE + no_diskinode * sizeof(DiskInode), ios::beg);
	inoutfile.read((char*)di, sizeof(DiskInode));
	/* ����Ƿ��Ѵ��ڴ��ļ� */
	int num_de = di->d_size / sizeof(DirectoryEntry);//Ŀ¼�����
	int cur_addr = 0;
	int cur_block = di->d_addr[cur_addr];
	int offset = 0;//����ƫ����
	for (int i = 0; i < num_de; ++i)
	{
		DirectoryEntry *de = new DirectoryEntry();
		inoutfile.seekg((DATA_ZONE_START_SECTOR + cur_block - 1)*Inode::BLOCK_SIZE + offset * sizeof(DirectoryEntry), ios::beg);
		inoutfile.read((char*)de, sizeof(DirectoryEntry));
		if (de->m_name == file_name)//�ļ��Ѵ���
		{
			//����1
			delete de;
			delete di;
			inoutfile.close();
			return 1;
		}
		if (offset == Inode::BLOCK_SIZE / sizeof(DirectoryEntry) - 1)//�˿��е����һ��Ŀ¼��
		{
			++cur_addr;//��һ��d_addr������Ŀ¼���ֱ������
			cur_block = di->d_addr[cur_addr];
			offset = 0;
		}
		else
		{
			++offset;//��һ��Ŀ¼��
		}
		delete de;
	}
	/* �ļ������ڣ��½����ļ� */
	//��ȡһ��DiskInode
	int new_di_no = iAlloc();
	DiskInode *new_di = new DiskInode();
	new_di->d_nlink = 1;
	//��ȡһ�����̿�
	new_di->d_addr[0] = alloc();
	//д���µ�DiskInode
	inoutfile.seekp(INODE_ZONE_START_SECTOR*Inode::BLOCK_SIZE + new_di_no * sizeof(DiskInode), ios::beg);
	inoutfile.write((char*)new_di, sizeof(DiskInode));
	delete new_di;
	//д��Ŀ¼��
	offset = num_de % (Inode::BLOCK_SIZE / sizeof(DirectoryEntry));//�ڼ���Ŀ¼��
	if (offset == 0)//��Ҫ�ٷ���һ�����ݿ�
	{
		if (di->d_addr[9] != 0)//Ŀ¼����
		{
			delete di;
			inoutfile.close();
			return 2;
		}
		int next_addr = 1;
		for (; di->d_addr[next_addr] != 0; ++next_addr);//�ҵ���һ��d_addr=0
		//��ȡһ�����̿�
		cur_block = alloc();
	}
	DirectoryEntry *de = new DirectoryEntry(file_name, new_di_no);
	inoutfile.seekp((DATA_ZONE_START_SECTOR + cur_block - 1)*Inode::BLOCK_SIZE + offset * sizeof(DirectoryEntry), ios::beg);
	inoutfile.write((char*)de, sizeof(DirectoryEntry));
	//����Ŀ¼�ļ��ĳ���
	di->d_size += sizeof(DirectoryEntry);
	inoutfile.seekp(INODE_ZONE_START_SECTOR*Inode::BLOCK_SIZE + no_diskinode * sizeof(DiskInode), ios::beg);
	inoutfile.write((char*)di, sizeof(DiskInode));
	delete di;
	inoutfile.close();
	return 0;
}

//�½�Ŀ¼�ļ�
int FileSystem::createDir(int no_diskinode, std::string dir_name)
{
	fstream inoutfile("myDisk.img", ios::in | ios::out | ios::_Nocreate | ios::binary);
	if (!inoutfile.is_open())
	{
		cerr << "myDisk.img open error!" << endl;
		return -1;
	}
	/* �Ȼ�ȡDiskInode */
	DiskInode *di = new DiskInode();
	inoutfile.seekg(INODE_ZONE_START_SECTOR*Inode::BLOCK_SIZE + no_diskinode * sizeof(DiskInode), ios::beg);
	inoutfile.read((char*)di, sizeof(DiskInode));
	/* ����Ƿ��Ѵ��ڴ��ļ� */
	int num_de = di->d_size / sizeof(DirectoryEntry);//Ŀ¼�����
	int cur_addr = 0;
	int cur_block = di->d_addr[cur_addr];
	int offset = 0;//����ƫ����
	for (int i = 0; i < num_de; ++i)
	{
		DirectoryEntry *de = new DirectoryEntry();
		inoutfile.seekg((DATA_ZONE_START_SECTOR + cur_block - 1)*Inode::BLOCK_SIZE + offset * sizeof(DirectoryEntry), ios::beg);
		inoutfile.read((char*)de, sizeof(DirectoryEntry));
		if (de->m_name == dir_name)//�ļ��Ѵ���
		{
			//����1
			delete de;
			delete di;
			inoutfile.close();
			return 1;
		}
		if (offset == Inode::BLOCK_SIZE / sizeof(DirectoryEntry) - 1)//�˿��е����һ��Ŀ¼��
		{
			++cur_addr;//��һ��d_addr������Ŀ¼���ֱ������
			cur_block = di->d_addr[cur_addr];
			offset = 0;
		}
		else
		{
			++offset;//��һ��Ŀ¼��
		}
		delete de;
	}
	/* �ļ������ڣ��½����ļ� */
	//��ȡһ��DiskInode
	int new_di_no = iAlloc();
	DiskInode *new_di = new DiskInode();
	new_di->d_nlink = 1;
	new_di->d_mode |= DiskInode::IDIR;
	//��ȡһ�����̿�
	new_di->d_addr[0] = alloc();
	//д���Լ��͸�Ŀ¼��Ŀ¼��
	DirectoryEntry *de = new DirectoryEntry(".", new_di_no);
	inoutfile.seekp((DATA_ZONE_START_SECTOR + new_di->d_addr[0] - 1)*Inode::BLOCK_SIZE, ios::beg);
	inoutfile.write((char*)de, sizeof(DirectoryEntry));
	delete de;
	de = new DirectoryEntry("..", no_diskinode);
	inoutfile.seekp((DATA_ZONE_START_SECTOR + new_di->d_addr[0] - 1)*Inode::BLOCK_SIZE + sizeof(DirectoryEntry), ios::beg);
	inoutfile.write((char*)de, sizeof(DirectoryEntry));
	delete de;
	//�����ļ�����
	new_di->d_size = 2 * sizeof(DirectoryEntry);
	//д���µ�DiskInode
	inoutfile.seekp(INODE_ZONE_START_SECTOR*Inode::BLOCK_SIZE + new_di_no * sizeof(DiskInode), ios::beg);
	inoutfile.write((char*)new_di, sizeof(DiskInode));
	delete new_di;
	//д��Ŀ¼��
	offset = num_de % (Inode::BLOCK_SIZE / sizeof(DirectoryEntry));//�ڼ���Ŀ¼��
	if (offset == 0)//��Ҫ�ٷ���һ�����ݿ�
	{
		if (di->d_addr[9] != 0)//Ŀ¼����
		{
			delete di;
			inoutfile.close();
			return 2;
		}
		int next_addr = 1;
		for (; di->d_addr[next_addr] != 0; ++next_addr);//�ҵ���һ��d_addr=0
		//��ȡһ�����̿�
		cur_block = alloc();
	}
	de = new DirectoryEntry(dir_name, new_di_no);
	inoutfile.seekp((DATA_ZONE_START_SECTOR + cur_block - 1)*Inode::BLOCK_SIZE + offset * sizeof(DirectoryEntry), ios::beg);
	inoutfile.write((char*)de, sizeof(DirectoryEntry));
	delete de;
	//����Ŀ¼�ļ��ĳ���
	di->d_size += sizeof(DirectoryEntry);
	inoutfile.seekp(INODE_ZONE_START_SECTOR*Inode::BLOCK_SIZE + no_diskinode * sizeof(DiskInode), ios::beg);
	inoutfile.write((char*)di, sizeof(DiskInode));
	delete di;
	inoutfile.close();
	return 0;
}

//ɾ���ļ�
int FileSystem::removeFile(int no_diskinode, int no_fdiskinode)
{
	fstream inoutfile("myDisk.img", ios::in | ios::out | ios::_Nocreate | ios::binary);
	if (!inoutfile.is_open())
	{
		cerr << "myDisk.img open error!" << endl;
		return -1;
	}
	/* �Ȼ�ȡDiskInode */
	DiskInode *di = new DiskInode();
	inoutfile.seekg(INODE_ZONE_START_SECTOR*Inode::BLOCK_SIZE + no_diskinode * sizeof(DiskInode), ios::beg);
	inoutfile.read((char*)di, sizeof(DiskInode));
	//�ж��Ƿ���Ŀ¼�ļ�
	if ((di->d_mode&DiskInode::IDIR) != 0)//��Ŀ¼�ļ�
	{
		/* ɾ�����е�ÿ��Ŀ¼��ݹ� */
		int cur_addr = 0;
		int cur_block = di->d_addr[cur_addr];
		int offset = 2;//Ŀ¼�����ƫ����
		int num_de = di->d_size / sizeof(DirectoryEntry);//Ŀ¼�����
		//�ݹ�ÿ��Ŀ¼��
		for (int i = 2; i < num_de; ++i)//0��1���Լ��͸�Ŀ¼��
		{
			//��ȡĿ¼��
			DirectoryEntry *de = new DirectoryEntry();
			inoutfile.seekg((DATA_ZONE_START_SECTOR + cur_block - 1)*Inode::BLOCK_SIZE + offset * sizeof(DirectoryEntry), ios::beg);
			inoutfile.read((char*)de, sizeof(DirectoryEntry));
			int rrm = removeFile(de->m_ino, no_diskinode);
			if (rrm != 0)//������
			{
				delete di;
				inoutfile.close();
				return 1;
			}
			if (offset == Inode::BLOCK_SIZE / sizeof(DirectoryEntry) - 1)//�˿����һ��Ŀ¼��
			{
				++cur_addr;//��һ��d_addr������Ŀ¼�ļ�����ֱ������
				cur_block = di->d_addr[cur_addr];
				offset = 0;
			}
			else
			{
				++offset;
			}
			delete de;
		}
	}
	/*�ڸ�Ŀ¼��ɾ��Ŀ¼�� */
	/* ��ȡ��Ŀ¼��DiskInode */
	DiskInode *fdi = new DiskInode();
	inoutfile.seekg(INODE_ZONE_START_SECTOR*Inode::BLOCK_SIZE + no_fdiskinode * sizeof(DiskInode), ios::beg);
	inoutfile.read((char*)fdi, sizeof(DiskInode));
	int num_de = fdi->d_size / sizeof(DirectoryEntry);//��Ŀ¼��Ŀ¼�����
	//�������ļ���Ŀ¼��
	int cur_addr = 0;
	int cur_block = fdi->d_addr[cur_addr];
	int offset = 2;//Ŀ¼�����ƫ����
	for (int i = 2; i < num_de; ++i)//0��1���Լ��͸�Ŀ¼��
	{
		//��ȡĿ¼��
		DirectoryEntry *de = new DirectoryEntry();
		inoutfile.seekg((DATA_ZONE_START_SECTOR + cur_block - 1)*Inode::BLOCK_SIZE + offset * sizeof(DirectoryEntry), ios::beg);
		inoutfile.read((char*)de, sizeof(DirectoryEntry));
		if (de->m_ino == no_diskinode)//�ҵ���
		{
			/* �����һ��Ŀ¼��д����λ�ã������¸�Ŀ¼�ļ����� */
			int last_addr = 0;
			for (; fdi->d_addr[last_addr] != 0; ++last_addr);
			--last_addr;
			int last_block = fdi->d_addr[last_addr];
			//��ȡ���һ��Ŀ¼��
			int last_offset = num_de % (Inode::BLOCK_SIZE / sizeof(DirectoryEntry)) - 1;//���һ��Ŀ¼�������ݿ��е�ƫ����
			DirectoryEntry *last_de = new DirectoryEntry();
			inoutfile.seekg((DATA_ZONE_START_SECTOR + last_block - 1)*Inode::BLOCK_SIZE + last_offset * sizeof(DirectoryEntry), ios::beg);
			inoutfile.read((char*)last_de, sizeof(DirectoryEntry));
			//д����Ӧλ��
			inoutfile.seekp((DATA_ZONE_START_SECTOR + cur_block - 1)*Inode::BLOCK_SIZE + offset * sizeof(DirectoryEntry), ios::beg);
			inoutfile.write((char*)last_de, sizeof(DirectoryEntry));
			delete last_de;
			//���¸�Ŀ¼�ļ�����
			fdi->d_size -= sizeof(DirectoryEntry);
			delete de;
			break;
		}
		if (offset == Inode::BLOCK_SIZE / sizeof(DirectoryEntry) - 1)//�˿����һ��Ŀ¼��
		{
			++cur_addr;//��һ��d_addr������Ŀ¼�ļ�����ֱ������
			cur_block = di->d_addr[cur_addr];
			offset = 0;
		}
		else
		{
			++offset;
		}
		delete de;
	}
	//д�ظ�Ŀ¼��DiskInode
	inoutfile.seekp(INODE_ZONE_START_SECTOR*Inode::BLOCK_SIZE + no_fdiskinode * sizeof(DiskInode), ios::beg);
	inoutfile.write((char*)fdi, sizeof(DiskInode));
	//�������ݿ�
	cur_addr = 0;
	int num_block = di->d_size / Inode::BLOCK_SIZE + 1;//���ݿ����
	int num_index1 = num_block / (Inode::BLOCK_SIZE / 4) + 1;//һ��������ĸ���
	int num_index1_free = 0;//�Ѵ����һ�����������
	for (int i = 0; i < num_block;)
	{
		int cur_block_no = di->d_addr[cur_addr];
		if (cur_addr > 7)//��������
		{
			//������������
			int index_block_no[Inode::BLOCK_SIZE / 4];
			int num_index1_to_read = min(Inode::BLOCK_SIZE / 4, num_index1 - num_index1_free);
			inoutfile.seekg((DATA_ZONE_START_SECTOR + cur_block_no - 1)*Inode::BLOCK_SIZE, ios::beg);
			inoutfile.read((char*)index_block_no, num_index1_to_read * 4);
			//����ÿ��һ��������
			for (int j = 0; j < num_index1_to_read; ++j)
			{
				//������������
				int blocks_no[Inode::BLOCK_SIZE / 4];
				int num_block_to_read = min(Inode::BLOCK_SIZE / 4, num_block - i);//�˴���Ҫ�������ݿ����
				inoutfile.seekg((DATA_ZONE_START_SECTOR + index_block_no[j] - 1)*Inode::BLOCK_SIZE, ios::beg);
				inoutfile.read((char*)blocks_no, num_block_to_read * 4);
				//����ÿ�����ݿ�
				for (int k = 0; k < num_block_to_read; ++k)
				{
					free(blocks_no[k]);
				}
				i += num_block_to_read;
				//���մ�������
				free(index_block_no[j]);
				++num_index1_free;
			}
			//���մ�������
			free(cur_block_no);
			++cur_addr;//��һ��d_addr
		}
		else if (cur_addr > 5)//һ������
		{
			//������������
			int blocks_no[Inode::BLOCK_SIZE / 4];
			int num_block_to_read = min(Inode::BLOCK_SIZE / 4, num_block - i);//�˴���Ҫ�������ݿ����
			inoutfile.seekg((DATA_ZONE_START_SECTOR + cur_block_no - 1)*Inode::BLOCK_SIZE, ios::beg);
			inoutfile.read((char*)blocks_no, num_block_to_read * 4);
			//����ÿ�����ݿ�
			for (int j = 0; j < num_block_to_read; ++j)
			{
				free(blocks_no[j]);
			}
			i += num_block_to_read;
			//���մ�������
			free(cur_block_no);
			++cur_addr;//��һ��d_addr
		}
		else//ֱ������
		{
			free(cur_block_no);
			++i;
			++cur_addr;//��һ��d_addr
		}
	}
	//����DiskInode
	iFree(no_diskinode);
	delete fdi;
	delete di;
	inoutfile.close();
	return 0;
}

//��myDisk.img�����ļ�����ֱ�Ӵӻ�����ж���
int FileSystem::readFile(int no_diskinode)
{
	ifstream infile("myDisk.img", ios::in | ios::binary);
	if (!infile.is_open())
	{
		cerr << "myDisk.img open error!" << endl;
		return -1;
	}
	//�Ȼ�ȡDiskInode
	DiskInode *di = disk_inodes[no_diskinode];
	if (disk_inodes[no_diskinode] == nullptr)
	{
		di = new DiskInode();
		infile.seekg(INODE_ZONE_START_SECTOR*Inode::BLOCK_SIZE + no_diskinode * sizeof(DiskInode), ios::beg);
		infile.read((char*)di, sizeof(DiskInode));
	}
	bool is_dir = (di->d_mode&DiskInode::IDIR) == 0 ? false : true;//�Ƿ���Ŀ¼�ļ�
	int file_length = di->d_size;//�ļ�����
	int len_read = 0;//�Ѷ�����
	int cur_addr = 0;//��d_addr[0]��ʼ��
	int num_block = file_length / Inode::BLOCK_SIZE + 1;//��Ҫ�������ݿ�
	int num_block_read = 0;//�Ѷ����ݿ����
	int num_buf = file_length / BufferManager::BUFFER_SIZE + 1;//��Ҫ�Ļ����ĸ���
	while (len_read < file_length)//û����
	{
		//��ȡ��ǰ���ݿ�
		int cur_block_no = di->d_addr[cur_addr];
		if (cur_addr > 7)//��������
		{
			//������������
			int index_blocks_no[Inode::BLOCK_SIZE / 4];
			int num_index1_to_read = min(Inode::BLOCK_SIZE / 4, (num_block - num_block_read) / (Inode::BLOCK_SIZE / 4) + 1);//�˴���Ҫ����һ����������
			infile.seekg((DATA_ZONE_START_SECTOR + cur_block_no - 1)*Inode::BLOCK_SIZE, ios::beg);
			infile.read((char*)index_blocks_no, num_index1_to_read * 4);
			for (int i = 0; i < num_index1_to_read; ++i)
			{
				//����һ��������
				int blocks_no[Inode::BLOCK_SIZE];
				int num_block_to_read = min(Inode::BLOCK_SIZE / 4, num_block - num_block_read);//�˴���Ҫ���Ĵ��̿����
				infile.seekg((DATA_ZONE_START_SECTOR + index_blocks_no[i] - 1)*Inode::BLOCK_SIZE, ios::beg);
				infile.read((char*)blocks_no, num_block_to_read * 4);
				//��ÿ�����ݿ�
				for (int j = 0; j < num_block_to_read; ++j)
				{
					int len_to_read = min(Inode::BLOCK_SIZE, file_length - len_read);
					if (num_buf <= BufferManager::NBUF)//����������
					{
						//���뵽�����
						Buf *bp = m_BufferManager->Bread(blocks_no[j], len_to_read);
						//�������Ļ
						displayBufferData(bp, len_to_read, is_dir);
					}
					else
					{
						//ֱ���������Ļ
						loadBlock(blocks_no[j], len_to_read, infile, is_dir);
					}
					len_read += len_to_read;
					++num_block_read;
				}
			}
			++cur_addr;//��һ��d_addr
		}
		else if (cur_addr > 5)//һ������
		{
			//������������
			int blocks_no[Inode::BLOCK_SIZE / 4];
			int num_block_to_read = min(Inode::BLOCK_SIZE / 4, num_block - num_block_read);
			infile.seekg((DATA_ZONE_START_SECTOR + cur_block_no - 1)*Inode::BLOCK_SIZE, ios::beg);
			infile.read((char*)blocks_no, num_block_to_read * 4);
			//��ÿ�����ݿ�
			for (int i = 0; i < num_block_to_read; ++i)
			{
				int len_to_read = min(Inode::BLOCK_SIZE, file_length - len_read);
				if (num_buf <= BufferManager::NBUF)//����������
				{
					//���뵽�����
					Buf *bp = m_BufferManager->Bread(blocks_no[i], len_to_read);
					//�������Ļ
					displayBufferData(bp, len_to_read, is_dir);
				}
				else
				{
					//ֱ���������Ļ
					loadBlock(blocks_no[i], len_to_read, infile, is_dir);
				}
				len_read += len_to_read;
				++num_block_read;
			}
			++cur_addr;//��һ��d_addr
		}
		else//ֱ������
		{
			int len_to_read = min(Inode::BLOCK_SIZE, file_length - len_read);
			if (num_buf <= BufferManager::NBUF)//����������
			{
				//���뵽�����
				Buf *bp = m_BufferManager->Bread(cur_block_no, len_to_read);
				//�������Ļ
				displayBufferData(bp, len_to_read, is_dir);
			}
			else
			{
				//ֱ���������Ļ
				loadBlock(cur_block_no, len_to_read, infile, is_dir);
			}
			len_read += len_to_read;
			++num_block_read;
			++cur_addr;//��һ��d_addr
		}
	}
	if (disk_inodes[no_diskinode] == nullptr)
	{
		delete di;
	}
	infile.close();
	cout << endl;
	return 0;
}

//���ַ���д�뻺����ļ�
int FileSystem::writeFile(int no_diskinode, std::string str)
{
	/* ����str�ĳ��Ȳ�����512*6�ֽڣ���˲���Ҫ�����飬�һ������㹻���£����һ���ӳ�д */
	int length = str.size();
	int num_block = length / Inode::BLOCK_SIZE + 1;//��Ҫ���̿���
	//int num_buf = length / BufferManager::BUFFER_SIZE + 1;//��Ҫ�Ļ������
	/* �Ȼ�ȡnum_block-1���̿� */
	vector<int>blocks_no;
	for (int i = 0; i < num_block - 1; ++i)
	{
		int block_no = alloc();
		if (block_no == 0)//û���㹻�Ĵ��̿�
		{
			//�ͷ��Ѿ�����Ĵ��̿�
			int n = blocks_no.size();
			for (int j = 0; j < n; ++j)
			{
				free(blocks_no[j]);
			}
			return 1;
		}
		blocks_no.emplace_back(block_no);
	}
	ifstream infile("myDisk.img", ios::in | ios::binary);
	if (!infile.is_open())
	{
		cerr << "myDisk.img open error!" << endl;
		return -1;
	}
	/* �Ȼ�ȡDiskInode */
	DiskInode *di = disk_inodes[no_diskinode];
	if (disk_inodes[no_diskinode] == nullptr)
	{
		di = new DiskInode();
		infile.seekg(INODE_ZONE_START_SECTOR*Inode::BLOCK_SIZE + no_diskinode * sizeof(DiskInode), ios::beg);
		infile.read((char*)di, sizeof(DiskInode));
	}
	/* �ǼǴ��̿� */
	for (int i = 0; i < num_block - 1; ++i)
	{
		di->d_addr[i + 1] = blocks_no[i];
	}
	/* �����ļ����� */
	di->d_size = length;
	/* д��DiskInode */
	//inoutfile.seekp(INODE_ZONE_START_SECTOR*Inode::BLOCK_SIZE + no_diskinode * sizeof(DiskInode), ios::beg);
	//inoutfile.write((char*)di, sizeof(DiskInode));
	infile.close();
	/* �ݴ�DiskInode��shutdownʱ������� */
	disk_inodes[no_diskinode] = di;
	int len_write = 0;//��д����
	Buf *bp = this->m_BufferManager->GetBlk(di->d_addr[0]);//��һ�������
	/* д�����ݵ������ */
	int len_to_write = min(BufferManager::BUFFER_SIZE, length - len_write);//�˴���Ҫд�ĳ���
	bp->b_wcount = len_to_write;
	strcpy((char*)bp->b_addr, str.substr(len_write, len_to_write).c_str());
	this->m_BufferManager->Bdwrite(bp);//�ӳ�д
	len_write += len_to_write;
	for (int i = 0; i < num_block - 1; ++i)
	{
		bp = this->m_BufferManager->GetBlk(blocks_no[i]);
		/* д�����ݵ������ */
		len_to_write = min(BufferManager::BUFFER_SIZE, length - len_write);//�˴���Ҫд�ĳ���
		bp->b_wcount = len_to_write;
		strcpy((char*)bp->b_addr, str.substr(len_write, len_to_write).c_str());
		this->m_BufferManager->Bdwrite(bp);//�ӳ�д
		len_write += len_to_write;
	}
	//delete di;
	return 0;
}

//���ӳ�д������д�ش���
void FileSystem::updateFile()
{
	this->m_BufferManager->Bflush();
	//����DiskInode
	ofstream outfile("myDisk.img", ios::out | ios::_Nocreate | ios::binary);
	if (!outfile.is_open())
	{
		cerr << "myDisk.img open error!" << endl;
		return;
	}
	for (int i = 0; i < 80; ++i)
	{
		if (disk_inodes[i] != nullptr)
		{
			outfile.seekp(INODE_ZONE_START_SECTOR*Inode::BLOCK_SIZE + i * sizeof(DiskInode), ios::beg);
			outfile.write((char*)disk_inodes[i], sizeof(DiskInode));
			delete disk_inodes[i];
			disk_inodes[i] = nullptr;
		}
	}
	outfile.close();
}
