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

//格式化myDisk.img
int FileSystem::format()
{
	//新建myDisk.img，新建superblock并写入myDisk.img
	SuperBlock *sb = new SuperBlock();
	int rs = storeSuperBlock("myDisk.img", sb);
	delete sb;
	//showSuperBlock();
	//新建2#到6#DiskInode并写入myDisk.img，其他DiskInode清零
	int rdi = formatDiskInode();
	//写入1#到5#数据块，并将数据区的其余磁盘块串连起来
	int rdb = formatDataBlock();
	if (rs == 0 && rdi == 0 && rdb == 0)//三者都成功
	{
		return 0;
	}
	return -1;
}

//格式化DiskInode区
int FileSystem::formatDiskInode()
{
	//新建2#到6#DiskInode并写入myDisk.img，其他DiskInode清零
	ofstream outfile("myDisk.img", ios::out | ios::_Nocreate | ios::binary);
	if (!outfile.is_open())
	{
		cerr << "myDisk.img open error!" << endl;
		return -1;
	}
	int num_diskinode = INODE_NUMBER_PER_SECTOR * INODE_ZONE_SIZE;//DiskInode的个数
	for (int i = 0; i < num_diskinode; ++i)
	{
		DiskInode *di = new DiskInode();
		if (i == 2)//根目录
		{
			di->d_nlink = 1;
			di->d_mode = DiskInode::IDIR;
			di->d_size = 6 * (DirectoryEntry::DIRSIZ + 4);//6个目录项
			di->d_addr[0] = 1;//存储在1#数据块中
		}
		else if (i == 3)//bin目录
		{
			di->d_nlink = 1;
			di->d_mode = DiskInode::IDIR;
			di->d_size = 2 * (DirectoryEntry::DIRSIZ + 4);//2个目录项
			di->d_addr[0] = 2;//存储在2#数据块中
		}
		else if (i == 4)//etc目录
		{
			di->d_nlink = 1;
			di->d_mode = DiskInode::IDIR;
			di->d_size = 2 * (DirectoryEntry::DIRSIZ + 4);//2个目录项
			di->d_addr[0] = 3;//存储在3#数据块中
		}
		else if (i == 5)//home目录
		{
			di->d_nlink = 1;
			di->d_mode = DiskInode::IDIR;
			di->d_size = 2 * (DirectoryEntry::DIRSIZ + 4);//2个目录项
			di->d_addr[0] = 4;//存储在4#数据块中
		}
		else if (i == 6)//dev目录
		{
			di->d_nlink = 1;
			di->d_mode = DiskInode::IDIR;
			di->d_size = 2 * (DirectoryEntry::DIRSIZ + 4);//2个目录项
			di->d_addr[0] = 5;//存储在5#数据块中
		}
		outfile.seekp(INODE_ZONE_START_SECTOR * Inode::BLOCK_SIZE + i * 64, ios::beg);//按序存入2号扇区，每个DiskInode大小为64字节
		outfile.write((char*)di, sizeof(DiskInode));
		delete di;
	}
	outfile.close();
	return 0;
}

//格式化数据区
int FileSystem::formatDataBlock()
{
	ofstream outfile("myDisk.img", ios::out | ios::_Nocreate | ios::binary);
	if (!outfile.is_open())
	{
		cerr << "myDisk.img open error!" << endl;
		return -1;
	}
	//写入1#到5#数据块
	for (int i = 0; i < 5; ++i)
	{
		if (i == 0)//根目录
		{
			DirectoryEntry *de = new DirectoryEntry(".", 2);//自己的目录项
			outfile.seekp(DATA_ZONE_START_SECTOR*Inode::BLOCK_SIZE, ios::beg);
			//cout << sizeof(de->m_name) << "   " << de->m_name << endl;
			outfile.write((char*)de, 32);
			//outfile.write((char*)de->m_name.c_str(), 28);
			//outfile.write((char*)&(de->m_ino), 4);
			delete de;
			de = new DirectoryEntry("..", 2);//父目录项
			outfile.seekp(DATA_ZONE_START_SECTOR*Inode::BLOCK_SIZE + 32, ios::beg);
			outfile.write((char*)de, 32);
			//outfile.write((char*)de->m_name.c_str(), 28);
			//outfile.write((char*)&(de->m_ino), 4);
			delete de;
			de = new DirectoryEntry("bin", 3);//bin目录项
			outfile.seekp(DATA_ZONE_START_SECTOR*Inode::BLOCK_SIZE + 32 * 2, ios::beg);
			outfile.write((char*)de, 32);
			//outfile.write((char*)de->m_name.c_str(), 28);
			//outfile.write((char*)&(de->m_ino), 4);
			delete de;
			de = new DirectoryEntry("etc", 4);//etc目录项
			outfile.seekp(DATA_ZONE_START_SECTOR*Inode::BLOCK_SIZE + 32 * 3, ios::beg);
			outfile.write((char*)de, 32);
			//outfile.write((char*)de->m_name.c_str(), 28);
			//outfile.write((char*)&(de->m_ino), 4);
			delete de;
			de = new DirectoryEntry("home", 5);//home目录项
			outfile.seekp(DATA_ZONE_START_SECTOR*Inode::BLOCK_SIZE + 32 * 4, ios::beg);
			outfile.write((char*)de, 32);
			//outfile.write((char*)de->m_name.c_str(), 28);
			//outfile.write((char*)&(de->m_ino), 4);
			delete de;
			de = new DirectoryEntry("dev", 6);//dev目录项
			outfile.seekp(DATA_ZONE_START_SECTOR*Inode::BLOCK_SIZE + 32 * 5, ios::beg);
			outfile.write((char*)de, 32);
			//outfile.write((char*)de->m_name.c_str(), 28);
			//outfile.write((char*)&(de->m_ino), 4);
			delete de;
		}
		else if (i == 1)//bin目录
		{
			DirectoryEntry *de = new DirectoryEntry(".", 3);//自己的目录项
			outfile.seekp((DATA_ZONE_START_SECTOR + 1)*Inode::BLOCK_SIZE, ios::beg);
			outfile.write((char*)de, 32);
			//outfile.write((char*)de->m_name.c_str(), 28);
			//outfile.write((char*)&(de->m_ino), 4);
			delete de;
			de = new DirectoryEntry("..", 2);//父目录项
			outfile.seekp((DATA_ZONE_START_SECTOR + 1)*Inode::BLOCK_SIZE + 32, ios::beg);
			outfile.write((char*)de, 32);
			//outfile.write((char*)de->m_name.c_str(), 28);
			//outfile.write((char*)&(de->m_ino), 4);
			delete de;
		}
		else if (i == 2)//etc目录
		{
			DirectoryEntry *de = new DirectoryEntry(".", 4);//自己的目录项
			outfile.seekp((DATA_ZONE_START_SECTOR + 2)*Inode::BLOCK_SIZE, ios::beg);
			outfile.write((char*)de, 32);
			//outfile.write((char*)de->m_name.c_str(), 28);
			//outfile.write((char*)&(de->m_ino), 4);
			delete de;
			de = new DirectoryEntry("..", 2);//父目录项
			outfile.seekp((DATA_ZONE_START_SECTOR + 2)*Inode::BLOCK_SIZE + 32, ios::beg);
			outfile.write((char*)de, 32);
			//outfile.write((char*)de->m_name.c_str(), 28);
			//outfile.write((char*)&(de->m_ino), 4);
			delete de;
		}
		else if (i == 3)//home目录
		{
			DirectoryEntry *de = new DirectoryEntry(".", 5);//自己的目录项
			outfile.seekp((DATA_ZONE_START_SECTOR + 3)*Inode::BLOCK_SIZE, ios::beg);
			outfile.write((char*)de, 32);
			//outfile.write((char*)de->m_name.c_str(), 28);
			//outfile.write((char*)&(de->m_ino), 4);
			delete de;
			de = new DirectoryEntry("..", 2);//父目录项
			outfile.seekp((DATA_ZONE_START_SECTOR + 3)*Inode::BLOCK_SIZE + 32, ios::beg);
			outfile.write((char*)de, 32);
			//outfile.write((char*)de->m_name.c_str(), 28);
			//outfile.write((char*)&(de->m_ino), 4);
			delete de;
		}
		else//dev目录
		{
			DirectoryEntry *de = new DirectoryEntry(".", 6);//自己的目录项
			outfile.seekp((DATA_ZONE_START_SECTOR + 4)*Inode::BLOCK_SIZE, ios::beg);
			outfile.write((char*)de, 32);
			//outfile.write((char*)de->m_name.c_str(), 28);
			//outfile.write((char*)&(de->m_ino), 4);
			delete de;
			de = new DirectoryEntry("..", 2);//父目录项
			outfile.seekp((DATA_ZONE_START_SECTOR + 4)*Inode::BLOCK_SIZE + 32, ios::beg);
			outfile.write((char*)de, 32);
			//outfile.write((char*)de->m_name.c_str(), 28);
			//outfile.write((char*)&(de->m_ino), 4);
			delete de;
		}
	}
	//串连其余数据块
	int head = 105;//从105#数据块开始
	while (head != 0)
	{
		int next = 0;//下一个head的数据块号
		if (head + 100 <= DATA_ZONE_SIZE)
		{
			next = head + 100;
		}
		//开始4字节存放s_nfree
		int s_nfree = next == 0 ? DATA_ZONE_SIZE - head : 100;
		outfile.seekp((DATA_ZONE_START_SECTOR + head - 1)*Inode::BLOCK_SIZE, ios::beg);
		outfile.write((char*)&s_nfree, 4);
		//接下来4个字节存放下一个head的数据块号，即head+100或0
		outfile.seekp((DATA_ZONE_START_SECTOR + head - 1)*Inode::BLOCK_SIZE + 4, ios::beg);
		outfile.write((char*)&next, 4);
		//再用4*(s_nfree-1)个字节存放s_nfree-1个空数据块号，即head+1到head+s_nfree-1
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

//系统初始化时读入SuperBlock
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
	infile.read((char*)sb, sizeof(SuperBlock));//读入0#和1#磁盘块
	infile.close();
	return sb;
}

//更新SuperBlock到myDisk.img中
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
	outfile.write((char*)sb, sizeof(SuperBlock));//写入0#和1#磁盘块
	outfile.close();
	return 0;
}

void FileSystem::showSuperBlock()
{
	SuperBlock *sb = loadSuperBlock("myDisk.img");
	cout << sb->s_nfree << endl;
	delete sb;
}

//分配一个空闲DiskInode
int FileSystem::iAlloc()
{
	SuperBlock *sb = loadSuperBlock("myDisk.img");
	int res = sb->s_inode[--sb->s_ninode];
	if (sb->s_ninode == 0)//没有空闲DiskInode了
	{
		/* 去DiskInode区找min(100,空闲数量)个DiskInode */
		ifstream infile("myDisk.img", ios::in | ios::binary);
		if (!infile.is_open())
		{
			cerr << "myDisk.img open error!" << endl;
			storeSuperBlock("myDisk.img", sb);
			delete sb;
			return 0;
		}
		//读DiskInode区
		int offset = 2;//0#和1#DiskInode不可用
		for (; offset < INODE_NUMBER_PER_SECTOR * INODE_ZONE_SIZE && sb->s_ninode < 100;)
		{
			DiskInode *di = new DiskInode();
			infile.seekg(INODE_ZONE_START_SECTOR*Inode::BLOCK_SIZE + offset * sizeof(DiskInode), ios::beg);
			infile.read((char*)di, sizeof(DiskInode));
			if (di->d_addr[0] == 0)//空闲
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

//释放编号为number的DiskInode
void FileSystem::iFree(int number)
{
	SuperBlock *sb = loadSuperBlock("myDisk.img");
	/* 本系统只有80个DiskInode，因此s_inode[100]不可能满 */
	sb->s_inode[sb->s_ninode++] = number;
	storeSuperBlock("myDisk.img", sb);
	delete sb;
}

//分配空闲磁盘块
int FileSystem::alloc()
{
	SuperBlock *sb = loadSuperBlock("myDisk.img");
	int res = sb->s_free[--sb->s_nfree];
	if (sb->s_nfree == 0)//没有空闲数据块了
	{
		ifstream infile("myDisk.img", ios::in | ios::binary);
		if (!infile.is_open())
		{
			cerr << "myDisk.img open error!" << endl;
			storeSuperBlock("myDisk.img", sb);
			delete sb;
			return 0;
		}
		//复制res#数据块的前404字节到s_nfree和s_free[100]
		infile.seekg((DATA_ZONE_START_SECTOR + res - 1)*Inode::BLOCK_SIZE, ios::beg);
		infile.read((char*)&sb->s_nfree, 4);
		infile.read((char*)sb->s_free, 400);
		infile.close();
	}
	storeSuperBlock("myDisk.img", sb);
	delete sb;
	return res;
}

//释放编号为blkno的磁盘块
void FileSystem::free(int blkno)
{
	SuperBlock *sb = loadSuperBlock("myDisk.img");
	if (sb->s_nfree == 100)//SuperBlock中已满
	{
		ofstream outfile("myDisk.img", ios::in | ios::binary | ios::_Nocreate);
		if (!outfile.is_open())
		{
			cerr << "myDisk.img open error!" << endl;
			storeSuperBlock("myDisk.img", sb);
			delete sb;
			return;
		}
		//把s_nfree和s_free[100]复制到blkno#块中
		outfile.seekp((DATA_ZONE_START_SECTOR + blkno - 1)*Inode::BLOCK_SIZE, ios::beg);
		outfile.write((char*)&sb->s_nfree, 4);
		outfile.write((char*)sb->s_free, 400);
		outfile.close();
		//SuperBlock中存入该块
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

//获取文件的DiskInode号
int FileSystem::getDiskInode(vector<string>& path)
{
	ifstream infile("myDisk.img", ios::in | ios::binary);
	if (!infile.is_open())
	{
		cerr << "myDisk.img open error!" << endl;
		return -1;
	}
	int cur_di = 2;//从根目录开始
	int n = path.size();
	for (int i = 1; i < n; ++i)
	{
		//读出current DiskInode
		DiskInode *di = new DiskInode();
		infile.seekg(INODE_ZONE_START_SECTOR*Inode::BLOCK_SIZE + cur_di * 64, ios::beg);
		infile.read((char*)di, sizeof(DiskInode));
		if ((di->d_mode&DiskInode::IDIR) == 0)//存在同名的普通文件，则一定没有此名的目录文件
		{
			//退出循环，返回0
			cur_di = 0;
			delete di;
			break;
		}
		int len_read = 0;//已读长度
		int file_length = di->d_size;//目录文件的长度
		int num_block = file_length / Inode::BLOCK_SIZE;//需要的数据块数量
		int cur_addr = 0;//当前d_addr中的下标
		int cur_block = di->d_addr[0];//当前数据块号
		int cur_entry_offset = 0;//当前目录项的偏移量
		//在当前目录查找path[i]
		while (len_read < file_length)
		{
			DirectoryEntry *cur_de = new DirectoryEntry();
			infile.seekg((DATA_ZONE_START_SECTOR + cur_block - 1)*Inode::BLOCK_SIZE + cur_entry_offset * 32, ios::beg);
			//infile.read((char*)cur_de->m_name, 28);
			//infile.read((char*)cur_de->m_ino, 4);
			infile.read((char*)cur_de, 32);
			//cout << cur_de->m_name << "   " << cur_de->m_ino << endl;
			if (cur_de->m_name == path[i])//找到
			{
				cur_di = cur_de->m_ino;
				delete cur_de;
				break;
			}
			len_read += 32;
			if (cur_entry_offset == Inode::BLOCK_SIZE / 4)//该数据块最后一个目录项
			{
				++cur_addr;//下一个d_addr，假设目录文件都是直接索引
				cur_block = di->d_addr[cur_addr];
				cur_entry_offset = 0;
			}
			else
			{
				++cur_entry_offset;//下一个目录项
			}
			delete cur_de;
		}
		if (len_read < file_length)
		{
			if (i == n - 1)//找到文件
			{
				delete di;
				break;
			}
			else//继续进入目录查找
			{
				DirectoryEntry *cur_de = new DirectoryEntry();
				infile.seekg((DATA_ZONE_START_SECTOR + cur_block - 1)*Inode::BLOCK_SIZE + cur_entry_offset * 32, ios::beg);
				infile.read((char*)cur_de, 32);
				cur_di = cur_de->m_ino;
				delete cur_de;
			}
		}
		else//未找到
		{
			//退出循环，返回0
			i = n;
			cur_di = 0;
		}
		//delete de;
		delete di;
	}
	infile.close();
	return cur_di;
}

//读入一个数据块到内存
void FileSystem::loadBlock(int block_no, int length, ifstream& infile, char* data)
{
	infile.seekg((DATA_ZONE_START_SECTOR + block_no - 1)*Inode::BLOCK_SIZE, ios::beg);
	infile.read((char*)data, length);
}

//读入一个数据块到文件
void FileSystem::loadBlock(int block_no, int length, ifstream& infile, ofstream& outfile, bool is_dir)
{
	char *data = new char[length];
	//先读入到data中
	loadBlock(block_no, length, infile, data);
	//再写入到outfile中
	if (is_dir)//是目录文件，只显示目录项中的文件名
	{
		int num_de = length / sizeof(DirectoryEntry);//目录项个数
		for (int i = 0; i < num_de; ++i)
		{
			int offset = i * 32;
			int j = offset;
			for (; j < offset + 28; ++j)//文件名
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
	else//普通文件，内容全部写入文件
	{
		outfile.write((char*)data, length);
	}
	delete data;
}

//读入一个数据块输出到屏幕
void FileSystem::loadBlock(int block_no, int length, ifstream& infile, bool is_dir)
{
	char *data = new char[length];
	//先读入到data中
	loadBlock(block_no, length, infile, data);
	//再写入到outfile中
	if (is_dir)//是目录文件，只显示目录项中的文件名
	{
		int num_de = length / sizeof(DirectoryEntry);//目录项个数
		for (int i = 0; i < num_de; ++i)
		{
			int offset = i * 32;
			int j = offset;
			for (; j < offset + 28; ++j)//文件名
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
	else//普通文件，内容全部写入文件
	{
		for (int i = 0; i < length; ++i)
		{
			cout << data[i];
		}
	}
	delete data;
}

//缓存数据输出到屏幕
void FileSystem::displayBufferData(Buf* bp, int length, bool is_dir)
{
	if (is_dir)
	{
		int num_de = length / sizeof(DirectoryEntry);//目录项个数
		for (int i = 0; i < num_de; ++i)
		{
			int offset = i * 32;
			int j = offset;
			for (; j < offset + 28; ++j)//文件名
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

//从文件写入一个数据块
void FileSystem::storeBlock(int block_no, int length, std::fstream& inoutfile, std::ifstream& infile)
{
	char *data = new char[length];
	//先读入到data中
	infile.read((char*)data, length);
	//再写入到outfile中
	storeBlock(block_no, length, inoutfile, data);
	delete data;
}

//从内存写入一个数据块
void FileSystem::storeBlock(int block_no, int length, std::fstream& inoutfile, char* data)
{
	inoutfile.seekp((DATA_ZONE_START_SECTOR + block_no - 1)*Inode::BLOCK_SIZE, ios::beg);
	inoutfile.write((char*)data, length);
}

//从myDisk.img读入文件到主机
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
	//先获取DiskInode
	DiskInode *di = new DiskInode();
	infile.seekg(INODE_ZONE_START_SECTOR*Inode::BLOCK_SIZE + no_diskinode * sizeof(DiskInode), ios::beg);
	infile.read((char*)di, sizeof(DiskInode));
	bool is_dir = (di->d_mode&DiskInode::IDIR) == 0 ? false : true;//是否是目录文件
	//读数据块
	int len_read = 0;//已读文件长度
	int file_length = di->d_size;//文件长度
	int cur_addr = 0;//从d_addr[0]开始读
	int num_block = file_length / Inode::BLOCK_SIZE + 1;//需要多少数据块
	int num_block_read = 0;//已读数据块个数
	while (len_read < file_length)//没读完
	{
		//获取当前数据块
		int cur_block_no = di->d_addr[cur_addr];
		if (cur_addr > 7)//二级索引
		{
			//读出此索引块
			int index_blocks_no[Inode::BLOCK_SIZE / 4];
			int num_index1_to_read = min(Inode::BLOCK_SIZE / 4, (num_block - num_block_read) / (Inode::BLOCK_SIZE / 4) + 1);//此次需要读的一级索引个数
			infile.seekg((DATA_ZONE_START_SECTOR + cur_block_no - 1)*Inode::BLOCK_SIZE, ios::beg);
			infile.read((char*)index_blocks_no, num_index1_to_read * 4);
			for (int i = 0; i < num_index1_to_read; ++i)
			{
				//读出一级索引块
				int blocks_no[Inode::BLOCK_SIZE];
				int num_block_to_read = min(Inode::BLOCK_SIZE / 4, num_block - num_block_read);//此次需要读的磁盘块个数
				infile.seekg((DATA_ZONE_START_SECTOR + index_blocks_no[i] - 1)*Inode::BLOCK_SIZE, ios::beg);
				infile.read((char*)blocks_no, num_block_to_read * 4);
				//读每个数据块
				for (int j = 0; j < num_block_to_read; ++j)
				{
					int len_to_read = min(Inode::BLOCK_SIZE, file_length - len_read);
					loadBlock(blocks_no[j], len_to_read, infile, outfile, is_dir);
					len_read += len_to_read;
					++num_block_read;
				}
			}
			++cur_addr;//下一个d_addr
		}
		else if (cur_addr > 5)//一级索引
		{
			//读出此索引块
			int blocks_no[Inode::BLOCK_SIZE / 4];
			int num_block_to_read = min(Inode::BLOCK_SIZE / 4, num_block - num_block_read);
			infile.seekg((DATA_ZONE_START_SECTOR + cur_block_no - 1)*Inode::BLOCK_SIZE, ios::beg);
			infile.read((char*)blocks_no, num_block_to_read * 4);
			//读每个数据块
			for (int i = 0; i < num_block_to_read; ++i)
			{
				int len_to_read = min(Inode::BLOCK_SIZE, file_length - len_read);
				loadBlock(blocks_no[i], len_to_read, infile, outfile, is_dir);
				len_read += len_to_read;
				++num_block_read;
			}
			++cur_addr;//下一个d_addr
		}
		else//直接索引
		{
			int len_to_read = min(Inode::BLOCK_SIZE, file_length - len_read);
			loadBlock(cur_block_no, len_to_read, infile, outfile, is_dir);
			len_read += len_to_read;
			++num_block_read;
			++cur_addr;//下一个d_addr
		}
	}
	delete di;
	outfile.close();
	infile.close();
	return 0;
}

//从主机存文件到myDisk.img
int FileSystem::storeFile(ifstream& infile, int no_diskinode)
{
	/* 空文件已存在，写入内容即可 */
	/* 获取文件的长度 */
	infile.seekg(0, ios::end);
	int file_length = infile.tellg();
	infile.seekg(0, ios::beg);
	int num_block = file_length / Inode::BLOCK_SIZE + 1;//需要的盘块数
	if (num_block > Inode::HUGE_FILE_BLOCK)//文件太大存不下
	{
		infile.close();
		return 2;
	}
	/* 先获取num_block-1个盘块 */
	vector<int>blocks_no;
	for (int i = 0; i < num_block - 1; ++i)
	{
		int block_no = alloc();
		if (block_no == 0)//没有足够的磁盘块
		{
			//释放已经分配的磁盘块
			int n = blocks_no.size();
			for (int j = 0; j < n; ++j)
			{
				free(blocks_no[j]);
			}
			return 1;
		}
		blocks_no.emplace_back(block_no);
	}
	//添加额外的索引块
	if (num_block > Inode::LARGE_FILE_BLOCK)//超大文件
	{
		int num_index1 = num_block / (Inode::BLOCK_SIZE / 4) + 1;//需要的一级索引块数量
		int num_index2 = num_index1 / (Inode::BLOCK_SIZE / 4) + 1;//需要的二级索引块数量
		if (num_index2 > 1)//需要两个二级索引块
		{
			//先需要128个一级索引块
			for (int i = 0; i < Inode::BLOCK_SIZE / 4; ++i)
			{
				int block_no = alloc();
				if (block_no == 0)//没有足够的磁盘块
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
			//一个二级索引块
			int block_no = alloc();
			if (block_no == 0)//没有足够的磁盘块
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
		//再需要num_index1个一级索引块
		for (int i = 0; i < num_index1; ++i)
		{
			int block_no = alloc();
			if (block_no == 0)//没有足够的磁盘块
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
		//一个二级索引块
		int block_no = alloc();
		if (block_no == 0)//没有足够的磁盘块
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
	else if (num_block > Inode::SMALL_FILE_BLOCK)//大文件
	{
		if (num_block > 6 + 128)//需要两个一级索引块
		{
			int block_no = alloc();
			if (block_no == 0)//没有足够的磁盘块
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
		if (block_no == 0)//没有足够的磁盘块
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
	/* 先获取DiskInode */
	DiskInode *di = new DiskInode();
	inoutfile.seekg(INODE_ZONE_START_SECTOR*Inode::BLOCK_SIZE + no_diskinode * sizeof(DiskInode), ios::beg);
	inoutfile.read((char*)di, sizeof(DiskInode));
	int len_write = 0;//已写长度
	int num_block_store = 0;//已存数据块个数
	int num_index1 = num_block / (Inode::BLOCK_SIZE / 4) + 1;//需要的一级索引块数量
	int cur_addr = 0;
	while (len_write < file_length)//没写完
	{
		if (cur_addr > 7)//二级索引
		{
			//获取一个二级索引块
			int index2_block_no = blocks_no.back();
			blocks_no.pop_back();
			//登记入d_addr
			di->d_addr[cur_addr] = index2_block_no;
			int index_blocks_no[Inode::BLOCK_SIZE / 4];
			int num_index1_to_store = min(Inode::BLOCK_SIZE / 4, num_index1);//此次需要存的一级索引个数
			for (int i = 0; i < num_index1_to_store; ++i)
			{
				//获取一个一级索引块
				int index1_block_no = blocks_no.back();
				blocks_no.pop_back();
				//写入二级索引块中
				index_blocks_no[i] = index1_block_no;
				int direct_blocks_no[Inode::BLOCK_SIZE];
				int num_block_to_store = min(Inode::BLOCK_SIZE / 4, num_block - num_block_store);//此次需要写的磁盘块个数
				//写每个数据块
				for (int j = 0; j < num_block_to_store; ++j)
				{
					int block_no = blocks_no.back();
					blocks_no.pop_back();
					//写入二级索引块中
					direct_blocks_no[j] = block_no;
					int len_to_write = min(Inode::BLOCK_SIZE, file_length - len_write);
					storeBlock(block_no, len_to_write, inoutfile, infile);
					len_write += len_to_write;
					++num_block_store;

				}
				//写入一级索引块
				storeBlock(index1_block_no, num_block_to_store * 4, inoutfile, (char*)direct_blocks_no);
			}
			//写入二级索引块
			storeBlock(index2_block_no, num_index1_to_store * 4, inoutfile, (char*)index_blocks_no);
			num_index1 -= num_index1_to_store;
			++cur_addr;//下一个d_addr
		}
		else if (cur_addr > 5)//一级索引
		{
			int index_block_no = blocks_no.back();//获取一个数据块作为索引块
			blocks_no.pop_back();
			int num_block_to_store = min(num_block - num_block_store, Inode::BLOCK_SIZE / 4);//此块中所要放的索引个数
			//登记入d_addr
			di->d_addr[cur_addr] = index_block_no;
			int blocks_index_no[Inode::BLOCK_SIZE/4];//记录此索引块内的索引号
			for (int i = 0; i < num_block_to_store; ++i)
			{
				//取一个数据块
				int block_no = blocks_no.back();
				blocks_no.pop_back();
				blocks_index_no[i] = block_no;
				//写入数据
				int len_to_write = min(file_length - len_write, Inode::BLOCK_SIZE);
				storeBlock(block_no, len_to_write, inoutfile, infile);
				len_write += len_to_write;
				++num_block_store;
			}
			//写入索引块
			storeBlock(index_block_no, num_block_to_store * 4, inoutfile, (char*)blocks_index_no);
			--num_index1;
			++cur_addr;
		}
		else//直接索引
		{
			int block_no = di->d_addr[cur_addr];
			if (block_no == 0)//未分配数据块
			{
				//取一个数据块
				block_no = blocks_no.back();
				blocks_no.pop_back();
				//登记入d_addr
				di->d_addr[cur_addr] = block_no;
			}
			//写入数据
			int len_to_write = min(file_length - len_write, Inode::BLOCK_SIZE);
			storeBlock(block_no, len_to_write, inoutfile, infile);
			len_write += len_to_write;
			++num_block_store;
			++cur_addr;
		}
	}
	//更新文件长度
	di->d_size = file_length;
	//写回DiskInode
	inoutfile.seekp(INODE_ZONE_START_SECTOR*Inode::BLOCK_SIZE + no_diskinode * sizeof(DiskInode), ios::beg);
	inoutfile.write((char*)di, sizeof(DiskInode));
	delete di;
	inoutfile.close();
	return 0;
}

//新建文件
int FileSystem::createFile(int no_diskinode, std::string file_name)
{
	fstream inoutfile("myDisk.img", ios::in | ios::out | ios::_Nocreate | ios::binary);
	if (!inoutfile.is_open())
	{
		cerr << "myDisk.img open error!" << endl;
		return -1;
	}
	/* 先获取DiskInode */
	DiskInode *di = new DiskInode();
	inoutfile.seekg(INODE_ZONE_START_SECTOR*Inode::BLOCK_SIZE + no_diskinode * sizeof(DiskInode), ios::beg);
	inoutfile.read((char*)di, sizeof(DiskInode));
	/* 检查是否已存在此文件 */
	int num_de = di->d_size / sizeof(DirectoryEntry);//目录项个数
	int cur_addr = 0;
	int cur_block = di->d_addr[cur_addr];
	int offset = 0;//块内偏移量
	for (int i = 0; i < num_de; ++i)
	{
		DirectoryEntry *de = new DirectoryEntry();
		inoutfile.seekg((DATA_ZONE_START_SECTOR + cur_block - 1)*Inode::BLOCK_SIZE + offset * sizeof(DirectoryEntry), ios::beg);
		inoutfile.read((char*)de, sizeof(DirectoryEntry));
		if (de->m_name == file_name)//文件已存在
		{
			//返回1
			delete de;
			delete di;
			inoutfile.close();
			return 1;
		}
		if (offset == Inode::BLOCK_SIZE / sizeof(DirectoryEntry) - 1)//此块中的最后一个目录项
		{
			++cur_addr;//下一个d_addr，假设目录项都是直接索引
			cur_block = di->d_addr[cur_addr];
			offset = 0;
		}
		else
		{
			++offset;//下一个目录项
		}
		delete de;
	}
	/* 文件不存在，新建此文件 */
	//获取一个DiskInode
	int new_di_no = iAlloc();
	DiskInode *new_di = new DiskInode();
	new_di->d_nlink = 1;
	//获取一个磁盘块
	new_di->d_addr[0] = alloc();
	//写入新的DiskInode
	inoutfile.seekp(INODE_ZONE_START_SECTOR*Inode::BLOCK_SIZE + new_di_no * sizeof(DiskInode), ios::beg);
	inoutfile.write((char*)new_di, sizeof(DiskInode));
	delete new_di;
	//写入目录项
	offset = num_de % (Inode::BLOCK_SIZE / sizeof(DirectoryEntry));//第几个目录项
	if (offset == 0)//需要再分配一个数据块
	{
		if (di->d_addr[9] != 0)//目录已满
		{
			delete di;
			inoutfile.close();
			return 2;
		}
		int next_addr = 1;
		for (; di->d_addr[next_addr] != 0; ++next_addr);//找到第一个d_addr=0
		//获取一个磁盘块
		cur_block = alloc();
	}
	DirectoryEntry *de = new DirectoryEntry(file_name, new_di_no);
	inoutfile.seekp((DATA_ZONE_START_SECTOR + cur_block - 1)*Inode::BLOCK_SIZE + offset * sizeof(DirectoryEntry), ios::beg);
	inoutfile.write((char*)de, sizeof(DirectoryEntry));
	//更新目录文件的长度
	di->d_size += sizeof(DirectoryEntry);
	inoutfile.seekp(INODE_ZONE_START_SECTOR*Inode::BLOCK_SIZE + no_diskinode * sizeof(DiskInode), ios::beg);
	inoutfile.write((char*)di, sizeof(DiskInode));
	delete di;
	inoutfile.close();
	return 0;
}

//新建目录文件
int FileSystem::createDir(int no_diskinode, std::string dir_name)
{
	fstream inoutfile("myDisk.img", ios::in | ios::out | ios::_Nocreate | ios::binary);
	if (!inoutfile.is_open())
	{
		cerr << "myDisk.img open error!" << endl;
		return -1;
	}
	/* 先获取DiskInode */
	DiskInode *di = new DiskInode();
	inoutfile.seekg(INODE_ZONE_START_SECTOR*Inode::BLOCK_SIZE + no_diskinode * sizeof(DiskInode), ios::beg);
	inoutfile.read((char*)di, sizeof(DiskInode));
	/* 检查是否已存在此文件 */
	int num_de = di->d_size / sizeof(DirectoryEntry);//目录项个数
	int cur_addr = 0;
	int cur_block = di->d_addr[cur_addr];
	int offset = 0;//块内偏移量
	for (int i = 0; i < num_de; ++i)
	{
		DirectoryEntry *de = new DirectoryEntry();
		inoutfile.seekg((DATA_ZONE_START_SECTOR + cur_block - 1)*Inode::BLOCK_SIZE + offset * sizeof(DirectoryEntry), ios::beg);
		inoutfile.read((char*)de, sizeof(DirectoryEntry));
		if (de->m_name == dir_name)//文件已存在
		{
			//返回1
			delete de;
			delete di;
			inoutfile.close();
			return 1;
		}
		if (offset == Inode::BLOCK_SIZE / sizeof(DirectoryEntry) - 1)//此块中的最后一个目录项
		{
			++cur_addr;//下一个d_addr，假设目录项都是直接索引
			cur_block = di->d_addr[cur_addr];
			offset = 0;
		}
		else
		{
			++offset;//下一个目录项
		}
		delete de;
	}
	/* 文件不存在，新建此文件 */
	//获取一个DiskInode
	int new_di_no = iAlloc();
	DiskInode *new_di = new DiskInode();
	new_di->d_nlink = 1;
	new_di->d_mode |= DiskInode::IDIR;
	//获取一个磁盘块
	new_di->d_addr[0] = alloc();
	//写入自己和父目录的目录项
	DirectoryEntry *de = new DirectoryEntry(".", new_di_no);
	inoutfile.seekp((DATA_ZONE_START_SECTOR + new_di->d_addr[0] - 1)*Inode::BLOCK_SIZE, ios::beg);
	inoutfile.write((char*)de, sizeof(DirectoryEntry));
	delete de;
	de = new DirectoryEntry("..", no_diskinode);
	inoutfile.seekp((DATA_ZONE_START_SECTOR + new_di->d_addr[0] - 1)*Inode::BLOCK_SIZE + sizeof(DirectoryEntry), ios::beg);
	inoutfile.write((char*)de, sizeof(DirectoryEntry));
	delete de;
	//更新文件长度
	new_di->d_size = 2 * sizeof(DirectoryEntry);
	//写入新的DiskInode
	inoutfile.seekp(INODE_ZONE_START_SECTOR*Inode::BLOCK_SIZE + new_di_no * sizeof(DiskInode), ios::beg);
	inoutfile.write((char*)new_di, sizeof(DiskInode));
	delete new_di;
	//写入目录项
	offset = num_de % (Inode::BLOCK_SIZE / sizeof(DirectoryEntry));//第几个目录项
	if (offset == 0)//需要再分配一个数据块
	{
		if (di->d_addr[9] != 0)//目录已满
		{
			delete di;
			inoutfile.close();
			return 2;
		}
		int next_addr = 1;
		for (; di->d_addr[next_addr] != 0; ++next_addr);//找到第一个d_addr=0
		//获取一个磁盘块
		cur_block = alloc();
	}
	de = new DirectoryEntry(dir_name, new_di_no);
	inoutfile.seekp((DATA_ZONE_START_SECTOR + cur_block - 1)*Inode::BLOCK_SIZE + offset * sizeof(DirectoryEntry), ios::beg);
	inoutfile.write((char*)de, sizeof(DirectoryEntry));
	delete de;
	//更新目录文件的长度
	di->d_size += sizeof(DirectoryEntry);
	inoutfile.seekp(INODE_ZONE_START_SECTOR*Inode::BLOCK_SIZE + no_diskinode * sizeof(DiskInode), ios::beg);
	inoutfile.write((char*)di, sizeof(DiskInode));
	delete di;
	inoutfile.close();
	return 0;
}

//删除文件
int FileSystem::removeFile(int no_diskinode, int no_fdiskinode)
{
	fstream inoutfile("myDisk.img", ios::in | ios::out | ios::_Nocreate | ios::binary);
	if (!inoutfile.is_open())
	{
		cerr << "myDisk.img open error!" << endl;
		return -1;
	}
	/* 先获取DiskInode */
	DiskInode *di = new DiskInode();
	inoutfile.seekg(INODE_ZONE_START_SECTOR*Inode::BLOCK_SIZE + no_diskinode * sizeof(DiskInode), ios::beg);
	inoutfile.read((char*)di, sizeof(DiskInode));
	//判断是否是目录文件
	if ((di->d_mode&DiskInode::IDIR) != 0)//是目录文件
	{
		/* 删除其中的每个目录项，递归 */
		int cur_addr = 0;
		int cur_block = di->d_addr[cur_addr];
		int offset = 2;//目录项块内偏移量
		int num_de = di->d_size / sizeof(DirectoryEntry);//目录项个数
		//递归每个目录项
		for (int i = 2; i < num_de; ++i)//0和1是自己和父目录项
		{
			//获取目录项
			DirectoryEntry *de = new DirectoryEntry();
			inoutfile.seekg((DATA_ZONE_START_SECTOR + cur_block - 1)*Inode::BLOCK_SIZE + offset * sizeof(DirectoryEntry), ios::beg);
			inoutfile.read((char*)de, sizeof(DirectoryEntry));
			int rrm = removeFile(de->m_ino, no_diskinode);
			if (rrm != 0)//出错了
			{
				delete di;
				inoutfile.close();
				return 1;
			}
			if (offset == Inode::BLOCK_SIZE / sizeof(DirectoryEntry) - 1)//此块最后一个目录项
			{
				++cur_addr;//下一个d_addr，假设目录文件都是直接索引
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
	/*在父目录中删除目录项 */
	/* 获取父目录的DiskInode */
	DiskInode *fdi = new DiskInode();
	inoutfile.seekg(INODE_ZONE_START_SECTOR*Inode::BLOCK_SIZE + no_fdiskinode * sizeof(DiskInode), ios::beg);
	inoutfile.read((char*)fdi, sizeof(DiskInode));
	int num_de = fdi->d_size / sizeof(DirectoryEntry);//父目录的目录项个数
	//查找子文件的目录项
	int cur_addr = 0;
	int cur_block = fdi->d_addr[cur_addr];
	int offset = 2;//目录项块内偏移量
	for (int i = 2; i < num_de; ++i)//0和1是自己和父目录项
	{
		//获取目录项
		DirectoryEntry *de = new DirectoryEntry();
		inoutfile.seekg((DATA_ZONE_START_SECTOR + cur_block - 1)*Inode::BLOCK_SIZE + offset * sizeof(DirectoryEntry), ios::beg);
		inoutfile.read((char*)de, sizeof(DirectoryEntry));
		if (de->m_ino == no_diskinode)//找到了
		{
			/* 将最后一个目录项写到此位置，并更新父目录文件长度 */
			int last_addr = 0;
			for (; fdi->d_addr[last_addr] != 0; ++last_addr);
			--last_addr;
			int last_block = fdi->d_addr[last_addr];
			//获取最后一个目录项
			int last_offset = num_de % (Inode::BLOCK_SIZE / sizeof(DirectoryEntry)) - 1;//最后一个目录项在数据块中的偏移量
			DirectoryEntry *last_de = new DirectoryEntry();
			inoutfile.seekg((DATA_ZONE_START_SECTOR + last_block - 1)*Inode::BLOCK_SIZE + last_offset * sizeof(DirectoryEntry), ios::beg);
			inoutfile.read((char*)last_de, sizeof(DirectoryEntry));
			//写到对应位置
			inoutfile.seekp((DATA_ZONE_START_SECTOR + cur_block - 1)*Inode::BLOCK_SIZE + offset * sizeof(DirectoryEntry), ios::beg);
			inoutfile.write((char*)last_de, sizeof(DirectoryEntry));
			delete last_de;
			//更新父目录文件长度
			fdi->d_size -= sizeof(DirectoryEntry);
			delete de;
			break;
		}
		if (offset == Inode::BLOCK_SIZE / sizeof(DirectoryEntry) - 1)//此块最后一个目录项
		{
			++cur_addr;//下一个d_addr，假设目录文件都是直接索引
			cur_block = di->d_addr[cur_addr];
			offset = 0;
		}
		else
		{
			++offset;
		}
		delete de;
	}
	//写回父目录的DiskInode
	inoutfile.seekp(INODE_ZONE_START_SECTOR*Inode::BLOCK_SIZE + no_fdiskinode * sizeof(DiskInode), ios::beg);
	inoutfile.write((char*)fdi, sizeof(DiskInode));
	//回收数据块
	cur_addr = 0;
	int num_block = di->d_size / Inode::BLOCK_SIZE + 1;//数据块个数
	int num_index1 = num_block / (Inode::BLOCK_SIZE / 4) + 1;//一级索引块的个数
	int num_index1_free = 0;//已处理的一级索引块个数
	for (int i = 0; i < num_block;)
	{
		int cur_block_no = di->d_addr[cur_addr];
		if (cur_addr > 7)//二级索引
		{
			//读出此索引块
			int index_block_no[Inode::BLOCK_SIZE / 4];
			int num_index1_to_read = min(Inode::BLOCK_SIZE / 4, num_index1 - num_index1_free);
			inoutfile.seekg((DATA_ZONE_START_SECTOR + cur_block_no - 1)*Inode::BLOCK_SIZE, ios::beg);
			inoutfile.read((char*)index_block_no, num_index1_to_read * 4);
			//处理每个一级索引块
			for (int j = 0; j < num_index1_to_read; ++j)
			{
				//读出此索引块
				int blocks_no[Inode::BLOCK_SIZE / 4];
				int num_block_to_read = min(Inode::BLOCK_SIZE / 4, num_block - i);//此次需要读的数据块个数
				inoutfile.seekg((DATA_ZONE_START_SECTOR + index_block_no[j] - 1)*Inode::BLOCK_SIZE, ios::beg);
				inoutfile.read((char*)blocks_no, num_block_to_read * 4);
				//回收每个数据块
				for (int k = 0; k < num_block_to_read; ++k)
				{
					free(blocks_no[k]);
				}
				i += num_block_to_read;
				//回收此索引块
				free(index_block_no[j]);
				++num_index1_free;
			}
			//回收此索引块
			free(cur_block_no);
			++cur_addr;//下一个d_addr
		}
		else if (cur_addr > 5)//一级索引
		{
			//读出此索引块
			int blocks_no[Inode::BLOCK_SIZE / 4];
			int num_block_to_read = min(Inode::BLOCK_SIZE / 4, num_block - i);//此次需要读的数据块个数
			inoutfile.seekg((DATA_ZONE_START_SECTOR + cur_block_no - 1)*Inode::BLOCK_SIZE, ios::beg);
			inoutfile.read((char*)blocks_no, num_block_to_read * 4);
			//回收每个数据块
			for (int j = 0; j < num_block_to_read; ++j)
			{
				free(blocks_no[j]);
			}
			i += num_block_to_read;
			//回收此索引块
			free(cur_block_no);
			++cur_addr;//下一个d_addr
		}
		else//直接索引
		{
			free(cur_block_no);
			++i;
			++cur_addr;//下一个d_addr
		}
	}
	//回收DiskInode
	iFree(no_diskinode);
	delete fdi;
	delete di;
	inoutfile.close();
	return 0;
}

//从myDisk.img读入文件，或直接从缓存块中读出
int FileSystem::readFile(int no_diskinode)
{
	ifstream infile("myDisk.img", ios::in | ios::binary);
	if (!infile.is_open())
	{
		cerr << "myDisk.img open error!" << endl;
		return -1;
	}
	//先获取DiskInode
	DiskInode *di = disk_inodes[no_diskinode];
	if (disk_inodes[no_diskinode] == nullptr)
	{
		di = new DiskInode();
		infile.seekg(INODE_ZONE_START_SECTOR*Inode::BLOCK_SIZE + no_diskinode * sizeof(DiskInode), ios::beg);
		infile.read((char*)di, sizeof(DiskInode));
	}
	bool is_dir = (di->d_mode&DiskInode::IDIR) == 0 ? false : true;//是否是目录文件
	int file_length = di->d_size;//文件长度
	int len_read = 0;//已读长度
	int cur_addr = 0;//从d_addr[0]开始读
	int num_block = file_length / Inode::BLOCK_SIZE + 1;//需要多少数据块
	int num_block_read = 0;//已读数据块个数
	int num_buf = file_length / BufferManager::BUFFER_SIZE + 1;//需要的缓存块的个数
	while (len_read < file_length)//没读完
	{
		//获取当前数据块
		int cur_block_no = di->d_addr[cur_addr];
		if (cur_addr > 7)//二级索引
		{
			//读出此索引块
			int index_blocks_no[Inode::BLOCK_SIZE / 4];
			int num_index1_to_read = min(Inode::BLOCK_SIZE / 4, (num_block - num_block_read) / (Inode::BLOCK_SIZE / 4) + 1);//此次需要读的一级索引个数
			infile.seekg((DATA_ZONE_START_SECTOR + cur_block_no - 1)*Inode::BLOCK_SIZE, ios::beg);
			infile.read((char*)index_blocks_no, num_index1_to_read * 4);
			for (int i = 0; i < num_index1_to_read; ++i)
			{
				//读出一级索引块
				int blocks_no[Inode::BLOCK_SIZE];
				int num_block_to_read = min(Inode::BLOCK_SIZE / 4, num_block - num_block_read);//此次需要读的磁盘块个数
				infile.seekg((DATA_ZONE_START_SECTOR + index_blocks_no[i] - 1)*Inode::BLOCK_SIZE, ios::beg);
				infile.read((char*)blocks_no, num_block_to_read * 4);
				//读每个数据块
				for (int j = 0; j < num_block_to_read; ++j)
				{
					int len_to_read = min(Inode::BLOCK_SIZE, file_length - len_read);
					if (num_buf <= BufferManager::NBUF)//缓存区够用
					{
						//读入到缓存块
						Buf *bp = m_BufferManager->Bread(blocks_no[j], len_to_read);
						//输出到屏幕
						displayBufferData(bp, len_to_read, is_dir);
					}
					else
					{
						//直接输出到屏幕
						loadBlock(blocks_no[j], len_to_read, infile, is_dir);
					}
					len_read += len_to_read;
					++num_block_read;
				}
			}
			++cur_addr;//下一个d_addr
		}
		else if (cur_addr > 5)//一级索引
		{
			//读出此索引块
			int blocks_no[Inode::BLOCK_SIZE / 4];
			int num_block_to_read = min(Inode::BLOCK_SIZE / 4, num_block - num_block_read);
			infile.seekg((DATA_ZONE_START_SECTOR + cur_block_no - 1)*Inode::BLOCK_SIZE, ios::beg);
			infile.read((char*)blocks_no, num_block_to_read * 4);
			//读每个数据块
			for (int i = 0; i < num_block_to_read; ++i)
			{
				int len_to_read = min(Inode::BLOCK_SIZE, file_length - len_read);
				if (num_buf <= BufferManager::NBUF)//缓存区够用
				{
					//读入到缓存块
					Buf *bp = m_BufferManager->Bread(blocks_no[i], len_to_read);
					//输出到屏幕
					displayBufferData(bp, len_to_read, is_dir);
				}
				else
				{
					//直接输出到屏幕
					loadBlock(blocks_no[i], len_to_read, infile, is_dir);
				}
				len_read += len_to_read;
				++num_block_read;
			}
			++cur_addr;//下一个d_addr
		}
		else//直接索引
		{
			int len_to_read = min(Inode::BLOCK_SIZE, file_length - len_read);
			if (num_buf <= BufferManager::NBUF)//缓存区够用
			{
				//读入到缓存块
				Buf *bp = m_BufferManager->Bread(cur_block_no, len_to_read);
				//输出到屏幕
				displayBufferData(bp, len_to_read, is_dir);
			}
			else
			{
				//直接输出到屏幕
				loadBlock(cur_block_no, len_to_read, infile, is_dir);
			}
			len_read += len_to_read;
			++num_block_read;
			++cur_addr;//下一个d_addr
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

//将字符串写入缓存或文件
int FileSystem::writeFile(int no_diskinode, std::string str)
{
	/* 假设str的长度不超过512*6字节，因此不需要索引块，且缓存区足够存下，因此一定延迟写 */
	int length = str.size();
	int num_block = length / Inode::BLOCK_SIZE + 1;//需要的盘块数
	//int num_buf = length / BufferManager::BUFFER_SIZE + 1;//需要的缓存块数
	/* 先获取num_block-1个盘块 */
	vector<int>blocks_no;
	for (int i = 0; i < num_block - 1; ++i)
	{
		int block_no = alloc();
		if (block_no == 0)//没有足够的磁盘块
		{
			//释放已经分配的磁盘块
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
	/* 先获取DiskInode */
	DiskInode *di = disk_inodes[no_diskinode];
	if (disk_inodes[no_diskinode] == nullptr)
	{
		di = new DiskInode();
		infile.seekg(INODE_ZONE_START_SECTOR*Inode::BLOCK_SIZE + no_diskinode * sizeof(DiskInode), ios::beg);
		infile.read((char*)di, sizeof(DiskInode));
	}
	/* 登记磁盘块 */
	for (int i = 0; i < num_block - 1; ++i)
	{
		di->d_addr[i + 1] = blocks_no[i];
	}
	/* 更新文件长度 */
	di->d_size = length;
	/* 写回DiskInode */
	//inoutfile.seekp(INODE_ZONE_START_SECTOR*Inode::BLOCK_SIZE + no_diskinode * sizeof(DiskInode), ios::beg);
	//inoutfile.write((char*)di, sizeof(DiskInode));
	infile.close();
	/* 暂存DiskInode，shutdown时存入磁盘 */
	disk_inodes[no_diskinode] = di;
	int len_write = 0;//已写长度
	Buf *bp = this->m_BufferManager->GetBlk(di->d_addr[0]);//第一个缓存块
	/* 写入数据到缓存块 */
	int len_to_write = min(BufferManager::BUFFER_SIZE, length - len_write);//此次需要写的长度
	bp->b_wcount = len_to_write;
	strcpy((char*)bp->b_addr, str.substr(len_write, len_to_write).c_str());
	this->m_BufferManager->Bdwrite(bp);//延迟写
	len_write += len_to_write;
	for (int i = 0; i < num_block - 1; ++i)
	{
		bp = this->m_BufferManager->GetBlk(blocks_no[i]);
		/* 写入数据到缓存块 */
		len_to_write = min(BufferManager::BUFFER_SIZE, length - len_write);//此次需要写的长度
		bp->b_wcount = len_to_write;
		strcpy((char*)bp->b_addr, str.substr(len_write, len_to_write).c_str());
		this->m_BufferManager->Bdwrite(bp);//延迟写
		len_write += len_to_write;
	}
	//delete di;
	return 0;
}

//将延迟写的数据写回磁盘
void FileSystem::updateFile()
{
	this->m_BufferManager->Bflush();
	//存入DiskInode
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
