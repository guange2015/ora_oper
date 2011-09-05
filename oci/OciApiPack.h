// OciApiPack.h: interface for the COciApiPack class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OCIAPIPACK_H__0BA374BC_1B6E_4DDA_9363_147A6D2A3BE2__INCLUDED_)
#define AFX_OCIAPIPACK_H__0BA374BC_1B6E_4DDA_9363_147A6D2A3BE2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "oci.h"
#include <vector>
#include <map>
#include <iostream>
#include "string"
using namespace std;


//�ô��һ����¼
typedef std::vector<string> ArrayRecords;
//���ڴ��һ����¼��
typedef std::map<string,ArrayRecords> ArrayRows;

//�ֶ���
#define MAX_FIELD 30
//�ֶ�������
#define FIELD_LEN 64
//������Ϣ����������
#define ERROR_LEN 512
//���ݻ���������,һ�����㹻��Ҫ���ᷢ������д�������max=4000
#define DATA_LEN 512
//����
#define BUF_SIZE 2048//*8

typedef text* pText;

typedef struct
{
	OCIEnv        *m_pEnvHD;//the environment handle	
    OCIServer     *m_pSrvHD;//the server handle
    OCISvcCtx     *m_pSvcHD;//the service handle
	OCIError      *m_pErrHD; //the error handle
    OCISession    *m_pSesHD;//the session handle
	
	BOOL           m_bLog;//�Ƿ��¼
	BOOL           m_bInit;//�Ƿ��ʼ������
	
	int            m_nCount;//��¼��¼����
}OciDBHandle;

typedef struct
{
	char data[DATA_LEN];
}ColBuff;

//��OCI��װ��һ���ۺ���
class COciApiPack  
{
public:
	COciApiPack();
	virtual ~COciApiPack();
private:
	//�������壬�Է������
	OciDBHandle* m_pOciHandle;

//for connection operation
public:
	//��ʼ��һϵ�о��
	bool initOciDB();
	void uninitOciDB();
	// ��¼
	bool logon(string szOra,string szUser,string szPass);
	void logout();
	//release environment and log off
	void detach();
	// �������
	bool transStart();
	bool transCommit();
	bool transRollback();

	//retrieve the erroe message
	string getError() {return m_csError;};
	int getErrorCode() {return (int)m_nError;};//retrieve error code
private:
	//Error catch
	string GetOraError();
	//����ֵ���,�˺�����Ҫ����������
	string CheckBackMess();

	string		m_csError;
	sb4         m_nError;
	bool m_bIsEOF;
private:
	OCIStmt *m_pStmtHD;
	OCIParam *m_pColHD;

	OCIDefine *m_pDefHD[MAX_FIELD];//����ס����[]�����ȼ�����*,30��ָ�����
	OCIBind *m_pBindHD[MAX_FIELD];

	ub2   collen[MAX_FIELD];//�洢�ֶγ���
	ub2   coltype[MAX_FIELD];//�洢�ֶ����ͺ�
	

	char colbuff[MAX_FIELD];//just one column,������ĳ���ֶ�
//	sb2   ind[MAX_FIELD];

	sword rcState;//���溯��ִ�з���ֵ

//	sb2 ind[MAX_FIELD];
//for resultset operation
public:
	//for SQL Statement
	void AllocStmt();
	void FreeStmt();
//	bool close();
	//for DML,DDL,DGL,DQL
	bool prepareSQL(string preSQL);//before executing

	//execute DML,DDL,DGL ,return the affected rows
	//0 for DQL,1 for others,if you want to execute many times in server,sql_mode=execute times
	int ExecuteSQL(int sql_mode);
	//execute DQL,return the resultSets
	void getQuery(ArrayRows &m_rowSets);
	int getColNums();
	//start from 1 to getColNums()
	string getColName(int col_num,ub4 &value_len);

//rows moving
	bool moveNext();
	bool movePrior();
	void moveFirst();
	void moveLast();
	void moveAbsolute(ub4 rowNum);

//for some operation inside the class
public:
	//Define data flexible,then just need to scroll the cursor
	//ע��Define����ӵ�һ����ʼ�󶨣�������Խ����ÿ�����ð󣬶�����ð�˳����
	bool DefindbyPos(int col_num,char* defType,int lengths);
	//Bind Parameter flexible,the you just need to change the values
	bool BindbyPos(int num_id);
};

#endif // !defined(AFX_OCIAPIPACK_H__0BA374BC_1B6E_4DDA_9363_147A6D2A3BE2__INCLUDED_)
