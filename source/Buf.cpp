#include "Buf.h"

Buf::Buf()
{
	b_flags = 0;			/* ������ƿ��־λ */
	padding = 0;			/* 4�ֽ���䣬ʹ��b_forw��b_back��Buf������Devtab���е��ֶ�˳���ܹ�һ�£�����ǿ��ת������� */
	/* ������ƿ���й���ָ�� */
	b_forw = nullptr;
	b_back = nullptr;
	av_forw = nullptr;
	av_back = nullptr;
	b_dev = -1;				/* �������豸�ţ����и�8λ�����豸�ţ���8λ�Ǵ��豸�� */
	b_wcount = 0;			/* �贫�͵��ֽ��� */
	b_addr = nullptr;		/* ָ��û�����ƿ�������Ļ��������׵�ַ */
	b_blkno = -1;			/* �����߼���� */
	b_error = 0;			/* I/O����ʱ��Ϣ */
	b_resid = 0;			/* I/O����ʱ��δ���͵�ʣ���ֽ��� */
}

Buf::~Buf()
{

}