#include "SuperBlock.h"

SuperBlock::SuperBlock()
{
	//��ʼ��superblock�����̹�1G������superblockռ��0#��1#�����inodeռ��2#��11#������12#��2097151#Ϊ������
	s_isize = 10;					/* ���Inode��ռ�õ��̿��� */
	s_fsize = 2097152;				/* �̿����� */
	s_nfree = 100;					/* ֱ�ӹ���Ŀ����̿����� */
	/* 105#�̿�Ϊ�������ݿ飬6#��104#���ݿ���� */
	s_free[0] = 105;
	for (int i = 1; i < 100; ++i)	
	{
		s_free[i] = i + 5;
	}
	s_ninode = 73;					/* ֱ�ӹ���Ŀ������Inode���� */
	for (int i = 0; i < 73; ++i)	/* 7#��79#���Inode */
	{
		s_inode[i] = i + 7;
	}
	s_flock = 0;					/* ���������̿��������־ */
	s_ilock = 0;					/* ��������Inode���־ */

	s_fmod = 1;						/* �ڴ���super block�������޸ı�־����ζ����Ҫ��������Ӧ��Super Block */
	s_ronly = 0;					/* ���ļ�ϵͳֻ�ܶ��� */
	s_time = 0;						/* ���һ�θ���ʱ�� */
	for (int i = 0; i < 47; ++i)	/* ���ʹSuperBlock���С����1024�ֽڣ�ռ��2������ */
	{
		padding[i] = 0;
	}
}

SuperBlock::~SuperBlock()
{

}
