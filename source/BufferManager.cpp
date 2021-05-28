#include "BufferManager.h"
#include "FileSystem.h"
#include<fstream>
#include<iostream>
using namespace std;

BufferManager::BufferManager()
{
	//cout << "Calling BufferManager()" << endl;
}

BufferManager::~BufferManager()
{
	
}

void BufferManager::Initialize()
{
	this->bFreeList.b_forw = this->bFreeList.b_back = &(this->bFreeList);
	//this->bFreeList.av_forw = this->bFreeList.av_back = &(this->bFreeList);
	this->bFreeList.av_forw = this->bFreeList.av_back = nullptr;
	Buf* bp = nullptr;
	for (int i = 0; i < NBUF; ++i)
	{
		bp = &(this->m_Buf[i]);
		bp->b_dev = -1;
		bp->b_addr = this->Buffer[i];
		/* 初始化自由队列 */
		bp->b_back = &(this->bFreeList);
		bp->b_forw = this->bFreeList.b_forw;
		this->bFreeList.b_forw->b_back = bp;
		this->bFreeList.b_forw = bp;
		//bp->b_flags = Buf::B_BUSY;
		//Brelse(bp);
		bp->b_flags = 0;
	}
}

//申请一块缓存，用于读写磁盘块blkno
Buf* BufferManager::GetBlk(int blkno)
{
	Buf* bp = nullptr;
	/*
	 * 如果设备队列中已经存在相应缓存，则返回该缓存；
	 * 否则从自由队列中分配新的缓存用于字符块读写。
	 */
	
	/* 首先在该设备队列中搜索是否有相应的缓存 */
	for (bp = this->bFreeList.b_forw; bp != &(this->bFreeList); bp = bp->b_forw)
	{
		if (bp->b_blkno == blkno)//找到
		{
			/* 从自由队列中抽取出来 */
			this->NotAvail(bp);
			return bp;
		}
	}
	while (true)
	{
		/* 取自由队列第一个空闲块 */
		bp = this->bFreeList.b_forw;
		this->NotAvail(bp);
		/* 如果该字符块是延迟写，将其异步写到磁盘上 */
		if (bp->b_flags & Buf::B_DELWRI)
		{
			//bp->b_flags |= Buf::B_ASYNC;
			this->Bwrite(bp, bp->b_wcount);
		}
		else//此缓存块可用
		{
			break;
		}
	}
	/* 注意: 这里清除了所有其他位，只设了B_BUSY */
	bp->b_flags = Buf::B_BUSY;
	bp->b_blkno = blkno;
	return bp;
}

//释放缓存控制块buf
void BufferManager::Brelse(Buf* bp)
{
	/* 注意以下操作并没有清除B_DELWRI、B_WRITE、B_READ、B_DONE标志
	 * B_DELWRI表示虽然将该控制块释放到自由队列里面，但是有可能还没有些到磁盘上。
	 * B_DONE则是指该缓存的内容正确地反映了存储在或应存储在磁盘上的信息
	 */
	bp->b_flags &= ~(Buf::B_WANTED | Buf::B_BUSY | Buf::B_ASYNC);//清除这三个标志
	/* 插到自由队列最后 */
	(this->bFreeList.b_back)->b_forw = bp;
	bp->b_back = this->bFreeList.b_back;
	bp->b_forw = &(this->bFreeList);
	this->bFreeList.b_back = bp;
}

//读一个磁盘块，blkno为目标磁盘块号
Buf* BufferManager::Bread(int blkno, int length)
{
	ifstream infile("myDisk.img", ios::in | ios::binary);
	if (!infile.is_open())
	{
		cerr << "myDisk.img open error!" << endl;
		return nullptr;
	}
	Buf* bp = this->GetBlk(blkno);
	/* 如果在设备队列中找到所需缓存，即B_DONE已设置，就不需进行I/O操作 */
	if (bp->b_flags & Buf::B_DONE)
	{
		this->Brelse(bp);
		return bp;
	}
	/* 没有找到相应缓存，构成I/O读请求块 */
	//bp->b_flags |= Buf::B_READ;
	bp->b_wcount = length;
	infile.seekg((FileSystem::DATA_ZONE_START_SECTOR + blkno - 1)*Inode::BLOCK_SIZE, ios::beg);
	infile.read((char*)(bp->b_addr), length);
	infile.close();
	//置B_DONE
	bp->b_flags |= Buf::B_DONE;
	//释放缓存块
	this->Brelse(bp);
	return bp;
}

//写一个磁盘块
void BufferManager::Bwrite(Buf* bp, int length)
{
	ofstream outfile("myDisk.img", ios::out | ios::_Nocreate | ios::binary);
	if (!outfile.is_open())
	{
		cerr << "myDisk.img open error!" << endl;
		return;
	}
	unsigned int flags = bp->b_flags;
	bp->b_flags &= ~(Buf::B_READ | Buf::B_DONE | Buf::B_ERROR | Buf::B_DELWRI);//清除这些标志
	bp->b_wcount = length;
	outfile.seekp((FileSystem::DATA_ZONE_START_SECTOR + bp->b_blkno - 1)*Inode::BLOCK_SIZE, ios::beg);
	outfile.write((char*)(bp->b_addr), length);
	outfile.close();
	this->Brelse(bp);
}

//延迟写磁盘块
void BufferManager::Bdwrite(Buf* bp)
{
	/* 置上B_DONE允许其它进程使用该磁盘块内容 */
	bp->b_flags |= (Buf::B_DELWRI | Buf::B_DONE);
	this->Brelse(bp);
}

//将dev指定设备队列中延迟写的缓存全部输出到磁盘
void BufferManager::Bflush()
{
	Buf* bp = nullptr;
	/* 找出自由队列中所有延迟写的块 */
	for (bp = this->bFreeList.b_forw; bp != &(this->bFreeList); bp = bp->b_forw)
	{
		if ((bp->b_flags) & Buf::B_DELWRI)
		{
			//bp->b_flags |= Buf::B_ASYNC;
			this->NotAvail(bp);
			this->Bwrite(bp, bp->b_wcount);
		}
	}
}

//获取自由缓存队列控制块Buf对象引用
Buf& BufferManager::GetBFreeList()
{
	//cout << bFreeList.b_forw << endl;
	return this->bFreeList;
}

//从自由队列中摘下指定的缓存控制块buf
void BufferManager::NotAvail(Buf* bp)
{
	/* 从自由队列中取出 */
	bp->b_back->b_forw = bp->b_forw;
	bp->b_forw->b_back = bp->b_back;
	/* 设置B_BUSY标志 */
	bp->b_flags |= Buf::B_BUSY;
}
