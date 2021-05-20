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

	/* ����н������ڵȴ��������ɶ����еĻ��棬������Ӧ���� */
	if (this->bFreeList.b_flags & Buf::B_WANTED)
	{
		/* ���B_WANTED��־����ʾ���п��л��� */
		this->bFreeList.b_flags &= (~Buf::B_WANTED);
		//procMgr.WakeUpAll((unsigned long)&this->bFreeList);
	}

	/* ����д��������򽫴��豸�Ÿĵ���
	 * ��������������û������еĴ������ݡ�
	 */
	if (bp->b_flags & Buf::B_ERROR)
	{
		//Utility::SetMinor(bp->b_dev, -1);
	}

	/* �ٽ���Դ�����磺��ͬ����ĩ�ڻ�������������
	 * ��ʱ���п��ܻ���������жϣ�ͬ����������������
	 */
	//X86Assembly::CLI();		/* spl6();  UNIX V6������ */

	/* ע�����²�����û�����B_DELWRI��B_WRITE��B_READ��B_DONE��־
	 * B_DELWRI��ʾ��Ȼ���ÿ��ƿ��ͷŵ����ɶ������棬�����п��ܻ�û��Щ�������ϡ�
	 * B_DONE����ָ�û����������ȷ�ط�ӳ�˴洢�ڻ�Ӧ�洢�ڴ����ϵ���Ϣ
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
		/* ��ʼ��NODEV���� */
		bp->b_back = &(this->bFreeList);
		bp->b_forw = this->bFreeList.b_forw;
		this->bFreeList.b_forw->b_back = bp;
		this->bFreeList.b_forw = bp;
		/* ��ʼ�����ɶ��� */
		bp->b_flags = Buf::B_BUSY;
		Brelse(bp);
	}
	//this->m_DeviceManager = &Kernel::Instance().GetDeviceManager();
	return;
}