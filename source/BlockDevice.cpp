#include "BlockDevice.h"

int ATABlockDevice::NSECTOR = 0x7FFFFFFF;	/* Max Possible Sector Numbers */

int ATABlockDevice::Strategy(Buf* bp)
{
	/* ���I/O�������Ƿ񳬳��˸�Ӳ�̵����������� */
	if (bp->b_blkno >= ATABlockDevice::NSECTOR)
	{
		/* ���ó����־ */
		bp->b_flags |= Buf::B_ERROR;

		//BufferManager& bm = Kernel::Instance().GetBufferManager();
		/*
		 * ��������²�����ִ��I/O�����������൱��ģ������ж�
		 * ��������е���IODone()���ѵȴ�I/O���������Ľ��̡�
		 */
		//bm.IODone(bp);
		return 0;	/* GCC likes it ! */
	}

	/* ��bp����I/O������еĶ�β����ʱI/O�����Ѿ��˻�����������ʽ����bp->av_forw��־�������β */
	bp->av_forw = nullptr;

	/* ���²����������ٽ����������ٽ���ԴΪ���豸��g_Atab��
	 * ��Ϊ���������Կ��豸��g_Atab��I/O������н��в�����
	 * �����жϴ������Ҳ���I/O�����������в����������ǲ��еġ�
	 * ʵ��������ֻ��ر�Ӳ�̵��жϾͿ����ˡ�
	 */
	//X86Assembly::CLI();
	if (this->d_tab->d_actf == nullptr)
	{
		this->d_tab->d_actf = bp;
	}
	else
	{
		this->d_tab->d_actl->av_forw = bp;
	}
	this->d_tab->d_actl = bp;

	/* ���Ӳ�̲�æ����������Ӳ�̲��������򽫵�ǰI/O��������
	 * ���豸��I/O�������֮��ֱ�ӷ��أ���ǰ���̵�I/O������ɺ�
	 * ����CPU���������жϣ�ϵͳ���ڴ����жϴ��������ִ�п��豸
	 * ��I/O��������е���һ��I/O����
	 */
	if (this->d_tab->d_active == 0)		/* ���̿��� */
	{
		this->Start();
	}
	//X86Assembly::STI();

	return 0;	/* GCC likes it ! */
}

void ATABlockDevice::Start()
{
	Buf* bp;

	if ((bp = this->d_tab->d_actf) == 0)
		return;		/* ���I/O�������Ϊ�գ����������� */

	this->d_tab->d_active++;	/* I/O������в��գ����ÿ�����æ��־ */

	/* ���ô��̼Ĵ���������I/O���� */
	//ATADriver::DevStart(bp);
}