#pragma once
#include "Inode.h"
#include "File.h"
#include "FileSystem.h"
#include<string>
#include<vector>

/*
 * 文件管理类(FileManager)
 * 封装了文件系统的各种系统调用在核心态下处理过程，
 * 如对文件的Open()、Close()、Read()、Write()等等
 * 封装了对文件系统访问的具体细节。
 */
class FileManager
{
public:
	/* 目录搜索模式，用于NameI()函数 */
	enum DirectorySearchMode
	{
		OPEN = 0,		/* 以打开文件方式搜索目录 */
		CREATE = 1,		/* 以新建文件方式搜索目录 */
		DELETE = 2		/* 以删除文件方式搜索目录 */
	};

	FileManager();
	~FileManager();

	/*
	 * @comment 初始化对全局对象的引用
	 */
	void Initialize();

	//磁盘格式化
	int format();

	int load(std::string file_name);

	int store(std::string file_name_out, std::string file_name_in);

	int rm(std::string file_name);

	int create(std::string file_name);

	int mkdir(std::string dir);

	int read(std::string file_name);

	int write(std::string file_name, std::string s);

	//将缓存区的延迟写数据写入磁盘
	void update();

	//切分路径
	std::vector<std::string> pathSplit(std::string file_name);

	//生成主机上的文件名
	std::string pathGenerate(std::vector<std::string>& path);

	/*
	 * @comment Open()系统调用处理过程
	 */
	//void Open();

	/*
	 * @comment Creat()系统调用处理过程
	 */
	//void Creat();

	/*
	 * @comment Open()、Creat()系统调用的公共部分
	 */
	//void Open1(Inode* pInode, int mode, int trf);

	/*
	 * @comment Close()系统调用处理过程
	 */
	//void Close();

	/*
	 * @comment Seek()系统调用处理过程
	 */
	//void Seek();

	/*
	 * @comment Dup()复制进程打开文件描述符
	 */
	//void Dup();

	/*
	 * @comment FStat()获取文件信息
	 */
	//void FStat();

	/*
	 * @comment FStat()获取文件信息
	 */
	//void Stat();

	/* FStat()和Stat()系统调用的共享例程 */
	//void Stat1(Inode* pInode, unsigned long statBuf);

	/*
	 * @comment Read()系统调用处理过程
	 */
	//void Read();

	/*
	 * @comment Write()系统调用处理过程
	 */
	//void Write();

	/*
	 * @comment 读写系统调用公共部分代码
	 */
	//void Rdwr(enum File::FileFlags mode);

	/*
	 * @comment Pipe()管道建立系统调用处理过程
	 */
	//void Pipe();

	/*
	 * @comment 管道读操作
	 */
	//void ReadP(File* pFile);

	/*
	 * @comment 管道写操作
	 */
	//void WriteP(File* pFile);

	/*
	 * @comment 目录搜索，将路径转化为相应的Inode，
	 * 返回上锁后的Inode
	 */
	//Inode* NameI(char(*func)(), enum DirectorySearchMode mode);

	/*
	 * @comment 获取路径中的下一个字符
	 */
	//static char NextChar();

	/*
	 * @comment 被Creat()系统调用使用，用于为创建新文件分配内核资源
	 */
	//Inode* MakNode(unsigned int mode);

	/*
	 * @comment 向父目录的目录文件写入一个目录项
	 */
	//void WriteDir(Inode* pInode);

	/*
	 * @comment 设置当前工作路径
	 */
	//void SetCurDir(char* pathname);

	/*
	 * @comment 检查对文件或目录的搜索、访问权限，作为系统调用的辅助函数
	 */
	//int Access(Inode* pInode, unsigned int mode);

	/*
	 * @comment 检查文件是否属于当前用户
	 */
	//Inode* Owner();

	/* 改变文件访问模式 */
	//void ChMod();

	/* 改变文件文件所有者user ID及其group ID */
	//void ChOwn();

	/* 改变当前工作目录 */
	//void ChDir();

	/* 创建文件的异名引用 */
	//void Link();

	/* 取消文件 */
	//void UnLink();

	/* 用于建立特殊设备文件的系统调用 */
	//void MkNod();

public:
	/* 根目录内存Inode */
	Inode* rootDirInode;

	/* 对全局对象g_FileSystem的引用，该对象负责管理文件系统存储资源 */
	FileSystem* m_FileSystem;

	/* 对全局对象g_InodeTable的引用，该对象负责内存Inode表的管理 */
	//InodeTable* m_InodeTable;

	/* 对全局对象g_OpenFileTable的引用，该对象负责打开文件表项的管理 */
	//OpenFileTable* m_OpenFileTable;
};