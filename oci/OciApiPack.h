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


//用存放一条记录
typedef std::vector<string> ArrayRecords;
//用于存放一个记录集
typedef std::map<string,ArrayRecords> ArrayRows;

//字段数
#define MAX_FIELD 30
//字段名长度
#define FIELD_LEN 64
//错误信息缓存区长度
#define ERROR_LEN 512
//数据缓存区长度,一定得足够大，要不会发生数据写入溢出，max=4000
#define DATA_LEN 512
//备用
#define BUF_SIZE 2048//*8

typedef text* pText;

typedef struct
{
	OCIEnv        *m_pEnvHD;//the environment handle	
    OCIServer     *m_pSrvHD;//the server handle
    OCISvcCtx     *m_pSvcHD;//the service handle
	OCIError      *m_pErrHD; //the error handle
    OCISession    *m_pSesHD;//the session handle
	
	BOOL           m_bLog;//是否登录
	BOOL           m_bInit;//是否初始化环境
	
	int            m_nCount;//记录登录次数
}OciDBHandle;

typedef struct
{
	char data[DATA_LEN];
}ColBuff;

//将OCI封装成一个综合类
class COciApiPack  
{
public:
	COciApiPack();
	virtual ~COciApiPack();
private:
	//定义句柄体，以方便调用
	OciDBHandle* m_pOciHandle;

//for connection operation
public:
	//初始化一系列句柄
	bool initOciDB();
	void uninitOciDB();
	// 登录
	bool logon(string szOra,string szUser,string szPass);
	void logout();
	//release environment and log off
	void detach();
	// 事务管理
	bool transStart();
	bool transCommit();
	bool transRollback();

	//retrieve the erroe message
	string getError() {return m_csError;};
	int getErrorCode() {return (int)m_nError;};//retrieve error code
private:
	//Error catch
	string GetOraError();
	//返回值检查,此函数主要是用来调试
	string CheckBackMess();

	string		m_csError;
	sb4         m_nError;
	bool m_bIsEOF;
private:
	OCIStmt *m_pStmtHD;
	OCIParam *m_pColHD;

	OCIDefine *m_pDefHD[MAX_FIELD];//自右住左，且[]的优先级高于*,30个指针变量
	OCIBind *m_pBindHD[MAX_FIELD];

	ub2   collen[MAX_FIELD];//存储字段长度
	ub2   coltype[MAX_FIELD];//存储字段类型号
	

	char colbuff[MAX_FIELD];//just one column,用来绑定某个字段
//	sb2   ind[MAX_FIELD];

	sword rcState;//保存函数执行返回值

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
	//注：Define必须从第一个开始绑定，不能跳越，即每个都得绑，而且最好按顺序来
	bool DefindbyPos(int col_num,char* defType,int lengths);
	//Bind Parameter flexible,the you just need to change the values
	bool BindbyPos(int num_id);
};

#endif // !defined(AFX_OCIAPIPACK_H__0BA374BC_1B6E_4DDA_9363_147A6D2A3BE2__INCLUDED_)
