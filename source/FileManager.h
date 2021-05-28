#pragma once
#include "Inode.h"
#include "File.h"
#include "FileSystem.h"
#include<string>
#include<vector>

/*
 * �ļ�������(FileManager)
 * ��װ���ļ�ϵͳ�ĸ���ϵͳ�����ں���̬�´�����̣�
 * ����ļ���Open()��Close()��Read()��Write()�ȵ�
 * ��װ�˶��ļ�ϵͳ���ʵľ���ϸ�ڡ�
 */
class FileManager
{
public:
	/* Ŀ¼����ģʽ������NameI()���� */
	enum DirectorySearchMode
	{
		OPEN = 0,		/* �Դ��ļ���ʽ����Ŀ¼ */
		CREATE = 1,		/* ���½��ļ���ʽ����Ŀ¼ */
		DELETE = 2		/* ��ɾ���ļ���ʽ����Ŀ¼ */
	};

	FileManager();
	~FileManager();

	/*
	 * @comment ��ʼ����ȫ�ֶ��������
	 */
	void Initialize();

	//���̸�ʽ��
	int format();

	int load(std::string file_name);

	int store(std::string file_name_out, std::string file_name_in);

	int rm(std::string file_name);

	int create(std::string file_name);

	int mkdir(std::string dir);

	int read(std::string file_name);

	int write(std::string file_name, std::string s);

	//�����������ӳ�д����д�����
	void update();

	//�з�·��
	std::vector<std::string> pathSplit(std::string file_name);

	//���������ϵ��ļ���
	std::string pathGenerate(std::vector<std::string>& path);

	/*
	 * @comment Open()ϵͳ���ô������
	 */
	//void Open();

	/*
	 * @comment Creat()ϵͳ���ô������
	 */
	//void Creat();

	/*
	 * @comment Open()��Creat()ϵͳ���õĹ�������
	 */
	//void Open1(Inode* pInode, int mode, int trf);

	/*
	 * @comment Close()ϵͳ���ô������
	 */
	//void Close();

	/*
	 * @comment Seek()ϵͳ���ô������
	 */
	//void Seek();

	/*
	 * @comment Dup()���ƽ��̴��ļ�������
	 */
	//void Dup();

	/*
	 * @comment FStat()��ȡ�ļ���Ϣ
	 */
	//void FStat();

	/*
	 * @comment FStat()��ȡ�ļ���Ϣ
	 */
	//void Stat();

	/* FStat()��Stat()ϵͳ���õĹ������� */
	//void Stat1(Inode* pInode, unsigned long statBuf);

	/*
	 * @comment Read()ϵͳ���ô������
	 */
	//void Read();

	/*
	 * @comment Write()ϵͳ���ô������
	 */
	//void Write();

	/*
	 * @comment ��дϵͳ���ù������ִ���
	 */
	//void Rdwr(enum File::FileFlags mode);

	/*
	 * @comment Pipe()�ܵ�����ϵͳ���ô������
	 */
	//void Pipe();

	/*
	 * @comment �ܵ�������
	 */
	//void ReadP(File* pFile);

	/*
	 * @comment �ܵ�д����
	 */
	//void WriteP(File* pFile);

	/*
	 * @comment Ŀ¼��������·��ת��Ϊ��Ӧ��Inode��
	 * �����������Inode
	 */
	//Inode* NameI(char(*func)(), enum DirectorySearchMode mode);

	/*
	 * @comment ��ȡ·���е���һ���ַ�
	 */
	//static char NextChar();

	/*
	 * @comment ��Creat()ϵͳ����ʹ�ã�����Ϊ�������ļ������ں���Դ
	 */
	//Inode* MakNode(unsigned int mode);

	/*
	 * @comment ��Ŀ¼��Ŀ¼�ļ�д��һ��Ŀ¼��
	 */
	//void WriteDir(Inode* pInode);

	/*
	 * @comment ���õ�ǰ����·��
	 */
	//void SetCurDir(char* pathname);

	/*
	 * @comment �����ļ���Ŀ¼������������Ȩ�ޣ���Ϊϵͳ���õĸ�������
	 */
	//int Access(Inode* pInode, unsigned int mode);

	/*
	 * @comment ����ļ��Ƿ����ڵ�ǰ�û�
	 */
	//Inode* Owner();

	/* �ı��ļ�����ģʽ */
	//void ChMod();

	/* �ı��ļ��ļ�������user ID����group ID */
	//void ChOwn();

	/* �ı䵱ǰ����Ŀ¼ */
	//void ChDir();

	/* �����ļ����������� */
	//void Link();

	/* ȡ���ļ� */
	//void UnLink();

	/* ���ڽ��������豸�ļ���ϵͳ���� */
	//void MkNod();

public:
	/* ��Ŀ¼�ڴ�Inode */
	Inode* rootDirInode;

	/* ��ȫ�ֶ���g_FileSystem�����ã��ö���������ļ�ϵͳ�洢��Դ */
	FileSystem* m_FileSystem;

	/* ��ȫ�ֶ���g_InodeTable�����ã��ö������ڴ�Inode��Ĺ��� */
	//InodeTable* m_InodeTable;

	/* ��ȫ�ֶ���g_OpenFileTable�����ã��ö�������ļ�����Ĺ��� */
	//OpenFileTable* m_OpenFileTable;
};