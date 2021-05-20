#include "BufferManager.h"

BufferManager::BufferManager()
{
	
}

BufferManager::~BufferManager()
{
	
}

void BufferManager::Brelse(Buf* bp)
{
	//ProcessManager& procMgr = Kernel::Instance().GetProcessManager();

	if (bp->b_flags & Buf::B_WANTED)
	{
		//procMgr.WakeUpAll((unsigned long)bp);
	}

	/* 如果有进程正在等待分配自由队列中的缓存，则唤醒相应进程 */
	if (this->bFreeList.b_flags & Buf::B_WANTED)
	{
		/* 清楚B_WANTED标志，表示已有空闲缓存 */
		this->bFreeList.b_flags &= (~Buf::B_WANTED);
		//procMgr.WakeUpAll((unsigned long)&this->bFreeList);
	}

	/* 如果有错误发生，则将次设备号改掉，
	 * 避免后续进程误用缓冲区中的错误数据。
	 */
	if (bp->b_flags & Buf::B_ERROR)
	{
		//Utility::SetMinor(bp->b_dev, -1);
	}

	/* 临界资源，比如：在同步读末期会调用这个函数，
	 * 此时很有可能会产生磁盘中断，同样会调用这个函数。
	 */
	//X86Assembly::CLI();		/* spl6();  UNIX V6的做法 */

	/* 注意以下操作并没有清除B_DELWRI、B_WRITE、B_READ、B_DONE标志
	 * B_DELWRI表示虽然将该控制块释放到自由队列里面，但是有可能还没有些到磁盘上。
	 * B_DONE则是指该缓存的内容正确地反映了存储在或应存储在磁盘上的信息
	 */
	bp->b_flags &= ~(Buf::B_WANTED | Buf::B_BUSY | Buf::B_ASYNC);
	(this->bFreeList.av_back)->av_forw = bp;
	bp->av_back = this->bFreeList.av_back;
	bp->av_forw = &(this->bFreeList);
	this->bFreeList.av_back = bp;

	//X86Assembly::STI();
	return;
}

void BufferManager::Initialize()
{
	this->bFreeList.b_forw = this->bFreeList.b_back = &(this->bFreeList);
	this->bFreeList.av_forw = this->bFreeList.av_back = &(this->bFreeList);
	Buf* bp;
	for (int i = 0; i < NBUF; ++i)
	{
		bp = &(this->m_Buf[i]);
		bp->b_dev = -1;
		bp->b_addr = this->Buffer[i];
		/* 初始化NODEV队列 */
		bp->b_back = &(this->bFreeList);
		bp->b_forw = this->bFreeList.b_forw;
		this->bFreeList.b_forw->b_back = bp;
		this->bFreeList.b_forw = bp;
		/* 初始化自由队列 */
		bp->b_flags = Buf::B_BUSY;
		Brelse(bp);
	}
	//this->m_DeviceManager = &Kernel::Instance().GetDeviceManager();
	return;
}