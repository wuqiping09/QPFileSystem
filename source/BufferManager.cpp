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
		/* ��ʼ�����ɶ��� */
		bp->b_back = &(this->bFreeList);
		bp->b_forw = this->bFreeList.b_forw;
		this->bFreeList.b_forw->b_back = bp;
		this->bFreeList.b_forw = bp;
		//bp->b_flags = Buf::B_BUSY;
		//Brelse(bp);
		bp->b_flags = 0;
	}
}

//����һ�黺�棬���ڶ�д���̿�blkno
Buf* BufferManager::GetBlk(int blkno)
{
	Buf* bp = nullptr;
	/*
	 * ����豸�������Ѿ�������Ӧ���棬�򷵻ظû��棻
	 * ��������ɶ����з����µĻ��������ַ����д��
	 */
	
	/* �����ڸ��豸�����������Ƿ�����Ӧ�Ļ��� */
	for (bp = this->bFreeList.b_forw; bp != &(this->bFreeList); bp = bp->b_forw)
	{
		if (bp->b_blkno == blkno)//�ҵ�
		{
			/* �����ɶ����г�ȡ���� */
			this->NotAvail(bp);
			return bp;
		}
	}
	while (true)
	{
		/* ȡ���ɶ��е�һ�����п� */
		bp = this->bFreeList.b_forw;
		this->NotAvail(bp);
		/* ������ַ������ӳ�д�������첽д�������� */
		if (bp->b_flags & Buf::B_DELWRI)
		{
			//bp->b_flags |= Buf::B_ASYNC;
			this->Bwrite(bp, bp->b_wcount);
		}
		else//�˻�������
		{
			break;
		}
	}
	/* ע��: �����������������λ��ֻ����B_BUSY */
	bp->b_flags = Buf::B_BUSY;
	bp->b_blkno = blkno;
	return bp;
}

//�ͷŻ�����ƿ�buf
void BufferManager::Brelse(Buf* bp)
{
	/* ע�����²�����û�����B_DELWRI��B_WRITE��B_READ��B_DONE��־
	 * B_DELWRI��ʾ��Ȼ���ÿ��ƿ��ͷŵ����ɶ������棬�����п��ܻ�û��Щ�������ϡ�
	 * B_DONE����ָ�û����������ȷ�ط�ӳ�˴洢�ڻ�Ӧ�洢�ڴ����ϵ���Ϣ
	 */
	bp->b_flags &= ~(Buf::B_WANTED | Buf::B_BUSY | Buf::B_ASYNC);//�����������־
	/* �嵽���ɶ������ */
	(this->bFreeList.b_back)->b_forw = bp;
	bp->b_back = this->bFreeList.b_back;
	bp->b_forw = &(this->bFreeList);
	this->bFreeList.b_back = bp;
}

//��һ�����̿飬blknoΪĿ����̿��
Buf* BufferManager::Bread(int blkno, int length)
{
	ifstream infile("myDisk.img", ios::in | ios::binary);
	if (!infile.is_open())
	{
		cerr << "myDisk.img open error!" << endl;
		return nullptr;
	}
	Buf* bp = this->GetBlk(blkno);
	/* ������豸�������ҵ����軺�棬��B_DONE�����ã��Ͳ������I/O���� */
	if (bp->b_flags & Buf::B_DONE)
	{
		this->Brelse(bp);
		return bp;
	}
	/* û���ҵ���Ӧ���棬����I/O������� */
	//bp->b_flags |= Buf::B_READ;
	bp->b_wcount = length;
	infile.seekg((FileSystem::DATA_ZONE_START_SECTOR + blkno - 1)*Inode::BLOCK_SIZE, ios::beg);
	infile.read((char*)(bp->b_addr), length);
	infile.close();
	//��B_DONE
	bp->b_flags |= Buf::B_DONE;
	//�ͷŻ����
	this->Brelse(bp);
	return bp;
}

//дһ�����̿�
void BufferManager::Bwrite(Buf* bp, int length)
{
	ofstream outfile("myDisk.img", ios::out | ios::_Nocreate | ios::binary);
	if (!outfile.is_open())
	{
		cerr << "myDisk.img open error!" << endl;
		return;
	}
	unsigned int flags = bp->b_flags;
	bp->b_flags &= ~(Buf::B_READ | Buf::B_DONE | Buf::B_ERROR | Buf::B_DELWRI);//�����Щ��־
	bp->b_wcount = length;
	outfile.seekp((FileSystem::DATA_ZONE_START_SECTOR + bp->b_blkno - 1)*Inode::BLOCK_SIZE, ios::beg);
	outfile.write((char*)(bp->b_addr), length);
	outfile.close();
	this->Brelse(bp);
}

//�ӳ�д���̿�
void BufferManager::Bdwrite(Buf* bp)
{
	/* ����B_DONE������������ʹ�øô��̿����� */
	bp->b_flags |= (Buf::B_DELWRI | Buf::B_DONE);
	this->Brelse(bp);
}

//��devָ���豸�������ӳ�д�Ļ���ȫ�����������
void BufferManager::Bflush()
{
	Buf* bp = nullptr;
	/* �ҳ����ɶ����������ӳ�д�Ŀ� */
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

//��ȡ���ɻ�����п��ƿ�Buf��������
Buf& BufferManager::GetBFreeList()
{
	//cout << bFreeList.b_forw << endl;
	return this->bFreeList;
}

//�����ɶ�����ժ��ָ���Ļ�����ƿ�buf
void BufferManager::NotAvail(Buf* bp)
{
	/* �����ɶ�����ȡ�� */
	bp->b_back->b_forw = bp->b_forw;
	bp->b_forw->b_back = bp->b_back;
	/* ����B_BUSY��־ */
	bp->b_flags |= Buf::B_BUSY;
}
