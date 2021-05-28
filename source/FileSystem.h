#pragma once
#include "SuperBlock.h"
#include "Buf.h"
#include "BufferManager.h"
#include "DiskInode.h"
#include "Inode.h"
#include<string>
#include<vector>

/*
 * 文件系统类(FileSystem)管理文件存储设备中
 * 的各类存储资源，磁盘块、外存INode的分配、
 * 释放。
 */
class FileSystem
{
public:
	//static const int NMOUNT = 5;										/* 系统中用于挂载子文件系统的装配块数量 */

	static const int SUPER_BLOCK_SECTOR_NUMBER = 0;						/* 定义SuperBlock位于磁盘上的扇区号，占据0，1两个扇区。 */

	static const int ROOTINO = 2;										/* 文件系统根目录外存Inode编号 */

	static const int INODE_NUMBER_PER_SECTOR = 8;						/* 每个磁盘块可以存放8个外存Inode */
	static const int INODE_ZONE_START_SECTOR = 2;						/* 外存Inode区位于磁盘上的起始扇区号 */
	static const int INODE_ZONE_SIZE = 10;								/* 磁盘上外存Inode区占据的扇区数 */

	static const int DATA_ZONE_START_SECTOR = 12;						/* 数据区的起始扇区号 */
	static const int DATA_ZONE_END_SECTOR = 2097140 - 1;				/* 数据区的结束扇区号 */
	static const int DATA_ZONE_SIZE = 2097140 - DATA_ZONE_START_SECTOR;	/* 数据区占据的扇区数量 */

	FileSystem();
	~FileSystem();

	/*
	 * @comment 初始化成员变量
	 */
	void Initialize();

	//磁盘格式化
	int format();

	int formatDiskInode();

	int formatDataBlock();

	//将myDisk.img的superblock读入内存
	SuperBlock* loadSuperBlock(std::string disk_name);

	//将内存中的superblock存入myDisk.img
	int storeSuperBlock(std::string disk_name, SuperBlock* sb);

	void showSuperBlock();

	//分配一个空闲DiskInode
	int iAlloc();

	//释放编号为number的DiskInode
	void iFree(int number);

	//分配空闲磁盘块
	int alloc();

	//释放编号为blkno的磁盘块
	void free(int blkno);

	//获取文件的DiskInode号
	int getDiskInode(std::vector<std::string>& path);

	//读入一个数据块到内存
	void loadBlock(int block_no, int length, std::ifstream& infile, char* data);

	//读入一个数据块到文件
	void loadBlock(int block_no, int length, std::ifstream& infile, std::ofstream& outfile, bool isDir);

	//读入一个数据块输出到屏幕
	void loadBlock(int block_no, int length, std::ifstream& infile, bool is_dir);

	//缓存数据输出到屏幕
	void displayBufferData(Buf* bp, int length, bool is_dir);

	//从文件写入一个数据块
	void storeBlock(int block_no, int length, std::fstream& inoutfile, std::ifstream& infile);

	//从内存写入一个数据块
	void storeBlock(int block_no, int length, std::fstream& inoutfile, char* data);

	//从myDisk.img读入文件到主机
	int loadFile(int no_diskinode, std::string file_name);

	//从主机存文件到myDisk.img
	int storeFile(std::ifstream& infile, int no_diskinode);

	//新建文件
	int createFile(int no_diskinode, std::string file_name);

	//新建目录文件
	int createDir(int no_diskinode, std::string dir_name);

	//删除文件
	int removeFile(int no_diskinode, int no_fdiskinode);

	//从myDisk.img读入文件，或直接从缓存块中读出
	int readFile(int no_diskinode);

	//将字符串写入缓存或文件
	int writeFile(int no_diskinode, std::string str);

	//将延迟写的数据写回磁盘
	void updateFile();

	/*
	 * @comment 根据文件存储设备的设备号dev获取
	 * 该文件系统的SuperBlock
	 */
	//SuperBlock* GetFS(short dev);

	/*
	 * @comment 将SuperBlock对象的内存副本更新到
	 * 存储设备的SuperBlock中去
	 */
	//void Update();

	/*
	 * @comment  在存储设备dev上分配一个空闲
	 * 外存INode，一般用于创建新的文件。
	 */
	//Inode* IAlloc(short dev);

	/*
	 * @comment  释放存储设备dev上编号为number
	 * 的外存INode，一般用于删除文件。
	 */
	//void IFree(short dev, int number);

	/*
	 * @comment 在存储设备dev上分配空闲磁盘块
	 */
	//Buf* Alloc(short dev);

	/*
	 * @comment 释放存储设备dev上编号为blkno的磁盘块
	 */
	//void Free(short dev, int blkno);

	/*
	 * @comment 查找文件系统装配表，搜索指定Inode对应的Mount装配块
	 */
	//Mount* GetMount(Inode* pInode);

private:
	/*
	 * @comment 检查设备dev上编号blkno的磁盘块是否属于
	 * 数据盘块区
	 */
	//bool BadBlock(SuperBlock* spb, short dev, int blkno);

	/* Members */
public:
	//Mount m_Mount[NMOUNT];		/* 文件系统装配块表，Mount[0]用于根文件系统 */

private:
	BufferManager* m_BufferManager;		/* FileSystem类需要缓存管理模块(BufferManager)提供的接口 */
	int updlock;				/* Update()函数的锁，该函数用于同步内存各个SuperBlock副本以及，
								被修改过的内存Inode。任一时刻只允许一个进程调用该函数 */
	DiskInode* disk_inodes[80];
};