// OciApiPack.cpp: implementation of the COciApiPack class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "OCIConnTest.h"
#include "OciApiPack.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#pragma comment(lib,"oci")


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COciApiPack::COciApiPack()
{
	m_pOciHandle = NULL;
//	m_pOciHandle->m_bInit = FALSE;
//	m_pOciHandle->m_bLog = FALSE;
//	m_pOciHandle->m_nCount = 0;
	m_csError = "";
	m_nError = 0;
	rcState = 0;

}

COciApiPack::~COciApiPack()
{
	detach();
}

//////////////////////////////// connection operations for oracle //////////////////////////////////////////
void COciApiPack::detach()
{
	ASSERT(m_pOciHandle);//较验句柄

		if(m_pOciHandle->m_bLog)
		{
			logout();
			m_pOciHandle->m_bLog=FALSE;
		}
		if(m_pOciHandle->m_bInit)
		{
			uninitOciDB();
			m_pOciHandle->m_bInit=FALSE;
		}

		delete m_pOciHandle;
	//	m_pOciHandle=NULL;
	//}

}

bool COciApiPack::initOciDB()
{
	if(m_pOciHandle!=NULL)
	{
		m_csError = "已经初始化过Oci连接句柄!";
		return false;
	}
	//new address of handles
	m_pOciHandle = new OciDBHandle[1];
	m_pOciHandle->m_nCount = 0;
	m_pOciHandle->m_bInit = FALSE;
	m_pOciHandle->m_bLog = FALSE;
/*
	//初始化一个环境
	if (OCIInitialize((ub4) OCI_DEFAULT, (dvoid *)0,
		(dvoid * (*)(dvoid *, size_t)) 0,
		(dvoid * (*)(dvoid *, dvoid *, size_t))0,
		(void (*)(dvoid *, dvoid *)) 0 ))
	{
		GetOraError();
		return false;
	}
	
	if (OCIEnvInit((OCIEnv **) &m_pOciHandle->m_pEnvHD, (ub4) OCI_DEFAULT,
		(size_t) 0, (dvoid **) 0 ))
	{
		GetOraError();
		return false;
	}
*/
	//if success retrieve OCI_SUCCESS 0,创建一个环境
	/* initialize the mode to be the threaded and object environment */
	if (OCIEnvCreate((OCIEnv **)&m_pOciHandle->m_pEnvHD, OCI_THREADED|OCI_OBJECT, (dvoid *)0, 
                               0, 0, 0, (size_t) 0, (dvoid **)0))
	{
		GetOraError();
		return false;
	}
	

	if (OCIHandleAlloc((dvoid *) m_pOciHandle->m_pEnvHD, (dvoid **) &m_pOciHandle->m_pSrvHD,
		(ub4) OCI_HTYPE_SERVER, (size_t) 0, (dvoid **) 0))
	{
		GetOraError();
		return false;
	}
	
	if (OCIHandleAlloc((dvoid *) m_pOciHandle->m_pEnvHD, (dvoid **) &m_pOciHandle->m_pSvcHD,
		(ub4) OCI_HTYPE_SVCCTX, (size_t) 0, (dvoid **) 0))
	{
		GetOraError();
		return false;
	}
	
	if (OCIHandleAlloc((dvoid *) m_pOciHandle->m_pEnvHD, (dvoid **) &m_pOciHandle->m_pErrHD,
		(ub4) OCI_HTYPE_ERROR, (size_t) 0, (dvoid **) 0))
	{
		GetOraError();
		return false;
	}
	
	if (OCIHandleAlloc((dvoid *) m_pOciHandle->m_pEnvHD, (dvoid **) &m_pOciHandle->m_pSesHD,
		(ub4) OCI_HTYPE_SESSION, (size_t) 0, (dvoid **) 0))
	{
		GetOraError();
		return false;
	}
/*	

	if (OCIDescriptorAlloc((dvoid *) m_pOciHandle->m_pEnvHD, (dvoid **) m_pLobLocator+i,
		(ub4)OCI_DTYPE_LOB, (size_t) 0, (dvoid **) 0))
	{
		GetOraError();
		return false;
	}
*/
	m_pOciHandle->m_bInit=TRUE;
	return TRUE;
}

bool COciApiPack::logon(string strOra,string strUser,string strPass)
{
	//may be can use OCILogon2(),use pool,may have some capability improve
	rcState = OCILogon(m_pOciHandle->m_pEnvHD,m_pOciHandle->m_pErrHD,&m_pOciHandle->m_pSvcHD,
		(text*)strUser.c_str(),(ub4)strUser.length(),(text*)strPass.c_str(),(ub4)strPass.length(),
        (text*)strOra.c_str(),(ub4)strOra.length());
	if (rcState == OCI_SUCCESS || rcState == OCI_SUCCESS_WITH_INFO)
	{
		m_pOciHandle->m_bLog=TRUE;
		m_pOciHandle->m_nCount++;
		return TRUE;
	}
	else
		CheckBackMess();
	return FALSE;
/*
//老版本的登录操作，但其是属性设置可用,支持多用户模式
	if (OCIServerAttach(m_pOciHandle->m_pSrvHD, m_pOciHandle->m_pErrHD, (text *) strOra.c_str(),
		(sb4) _tcslen(szOra), (ub4) OCI_DEFAULT))
	{
		GetOraError();
		return false;
	}
	
	if (OCIAttrSet((dvoid *) m_pOciHandle->m_pSesHD, (ub4) OCI_HTYPE_SESSION,
		(dvoid *) strUser.c_str(), (ub4) strlen(strUser.c_str()),
		(ub4) OCI_ATTR_USERNAME, m_pOciHandle->m_pErrHD))
	{
		GetOraError();
		return false;
	}
	
	if (OCIAttrSet((dvoid *) m_pOciHandle->m_pSesHD, (ub4) OCI_HTYPE_SESSION,
		(dvoid *) strPass.c_str(), (ub4) strlen(strPass.c_str()),
		(ub4) OCI_ATTR_PASSWORD, m_pOciHandle->m_pErrHD))
	{
		GetOraError();
		return false;
	}
	
	if (OCIAttrSet((dvoid *) m_pOciHandle->m_pSvcHD, (ub4) OCI_HTYPE_SVCCTX,
		(dvoid *) m_pOciHandle->m_pSrvHD, (ub4) 0, (ub4) OCI_ATTR_SERVER, m_pOciHandle->m_pErrHD))
	{
		GetOraError();
		return false;
	}
	
	if (OCISessionBegin(m_pOciHandle->m_pSvcHD, m_pOciHandle->m_pErrHD, m_pOciHandle->m_pSesHD, (ub4) OCI_CRED_RDBMS,
		(ub4) OCI_DEFAULT))
	{
		GetOraError();
		return false;
	}
	
	if (OCIAttrSet((dvoid *) m_pOciHandle->m_pSvcHD, (ub4) OCI_HTYPE_SVCCTX, (dvoid *) m_pOciHandle->m_pSesHD,
		(ub4) 0, (ub4) OCI_ATTR_SESSION, m_pOciHandle->m_pErrHD))
	{
		GetOraError();
		return false;
	}
*/

//	return TRUE;
}

void COciApiPack::logout()
{
	ASSERT(m_pOciHandle);
//	OCISessionEnd(m_pOciHandle->m_pSvcHD, m_pOciHandle->m_pErrHD, m_pOciHandle->m_pSesHD, (ub4) 0);
//	OCIServerDetach(m_pOciHandle->m_pSrvHD, m_pOciHandle->m_pErrHD, (ub4) OCI_DEFAULT);
	OCILogoff(m_pOciHandle->m_pSvcHD, m_pOciHandle->m_pErrHD);
}

void COciApiPack::uninitOciDB()
{
	ASSERT(m_pOciHandle);
	OCIHandleFree((dvoid *) m_pOciHandle->m_pSrvHD, (ub4) OCI_HTYPE_SERVER);
	OCIHandleFree((dvoid *) m_pOciHandle->m_pSvcHD, (ub4) OCI_HTYPE_SVCCTX);
	OCIHandleFree((dvoid *) m_pOciHandle->m_pErrHD, (ub4) OCI_HTYPE_ERROR);
	OCIHandleFree((dvoid *) m_pOciHandle->m_pSesHD, (ub4) OCI_HTYPE_SESSION);
//	delete m_pOciHandle;
}

string COciApiPack::CheckBackMess()
{
	ASSERT(m_pOciHandle);
	text  msgbuf[ERROR_LEN];
	msgbuf[0] = '\0';
	m_nError = 0;

	string m_Mesg;
	switch (rcState)
	{ 
	case OCI_SUCCESS:
		m_Mesg = "OCI_SUCCESS";
		break;
	case OCI_SUCCESS_WITH_INFO:
		m_Mesg = "OCI_SUCCESS_WITH_INFO";
		break;
	case OCI_NEED_DATA:
		m_Mesg = "OCI_NEED_DATA";
		break;
	case OCI_NO_DATA:
		m_Mesg = "OCI_NO_DATA";
		break;
	case OCI_ERROR:
		OCIErrorGet((dvoid *) m_pOciHandle->m_pErrHD, (ub4) 1, (text *) NULL, &m_nError,
			msgbuf, (ub4) sizeof(msgbuf), (ub4) OCI_HTYPE_ERROR);
		m_Mesg = (char*)msgbuf;
		break;
	case OCI_INVALID_HANDLE:
		m_Mesg = "OCI_INVALID_HANDLE";
		break;
	case OCI_STILL_EXECUTING:
		m_Mesg = "OCI_STILL_EXECUTING";
		break;
	case OCI_CONTINUE:
		m_Mesg = "OCI_CONTINUE";
		break;
	default:
		break;
	}
	//trans erroe message and error code
	m_csError = m_Mesg;
	return m_Mesg;
}

string COciApiPack::GetOraError()
{
	ASSERT(m_pOciHandle);
	text  msgbuf[ERROR_LEN];
	m_nError = 0;
	
	OCIErrorGet((dvoid *) m_pOciHandle->m_pErrHD, (ub4) 1, (text *) NULL, &m_nError,
								msgbuf, (ub4) sizeof(msgbuf), (ub4) OCI_HTYPE_ERROR);
	m_csError = (char*)msgbuf;
	return m_csError;
}

bool COciApiPack::transStart()
{
	ASSERT(m_pOciHandle);

	rcState = OCITransStart(m_pOciHandle->m_pSvcHD,m_pOciHandle->m_pErrHD,30,OCI_TRANS_NEW);
	if (rcState == OCI_SUCCESS || rcState == OCI_SUCCESS_WITH_INFO)
		return TRUE;
	else
		CheckBackMess();
	return FALSE;
}

bool COciApiPack::transCommit()
{
	ASSERT(m_pOciHandle);
	
	rcState = OCITransCommit(m_pOciHandle->m_pSvcHD, m_pOciHandle->m_pErrHD, OCI_DEFAULT);
	if (rcState == OCI_SUCCESS || rcState == OCI_SUCCESS_WITH_INFO)
		return TRUE;
	else
		CheckBackMess();
	return FALSE;
}


bool COciApiPack::transRollback() 
{
	ASSERT(m_pOciHandle);
	rcState = OCITransRollback(m_pOciHandle->m_pSvcHD, m_pOciHandle->m_pErrHD, OCI_DEFAULT);
	if (rcState == OCI_SUCCESS || rcState == OCI_SUCCESS_WITH_INFO)
		return TRUE;
	else
		CheckBackMess();
	return FALSE;
}


//////////////////////////////// recordset operations for oracle //////////////////////////////////////////
//release the SQL Statement handle
void COciApiPack::FreeStmt()
{
	if(m_pStmtHD != NULL)
	{
		OCIHandleFree((dvoid *) m_pStmtHD, (ub4) OCI_HTYPE_STMT);
		m_pStmtHD = NULL;
	}
}
//alloc the SQL Statement handle
void COciApiPack::AllocStmt()
{
	rcState = OCIHandleAlloc((dvoid *) m_pOciHandle->m_pEnvHD, 
		(dvoid **) &m_pStmtHD,	(ub4) OCI_HTYPE_STMT, (size_t) 0, (dvoid **) 0);
	CheckBackMess();
}

bool COciApiPack::prepareSQL(string preSQL)
{
	ASSERT(m_pOciHandle);
	rcState = OCIStmtPrepare(m_pStmtHD,m_pOciHandle->m_pErrHD,(text*)preSQL.c_str(),(ub4)preSQL.length(),OCI_NTV_SYNTAX,OCI_DEFAULT);
	if (rcState == OCI_SUCCESS || rcState == OCI_SUCCESS_WITH_INFO)
		return TRUE;
	else
		CheckBackMess();
	return FALSE;

}

//0 ->-1 means a DQL,1->(returns >=0)
int COciApiPack::ExecuteSQL(int sql_mode)
{
	ASSERT(m_pStmtHD != NULL);//this is very important ,if not 
	ASSERT(m_pOciHandle);
	//对于DML，DGL，DDL执行一次足够！,executing times
	if (sql_mode)
	{
		rcState = OCIStmtExecute(m_pOciHandle->m_pSvcHD,m_pStmtHD,m_pOciHandle->m_pErrHD,(ub4)sql_mode,0,NULL,NULL,OCI_DEFAULT);
		if (rcState == OCI_SUCCESS || rcState == OCI_SUCCESS_WITH_INFO)
		{
			return TRUE;
		}
		else
			CheckBackMess();
	}
	else
	{
		//executing a DQL,for select,缺省情况下，游标只往下走
		rcState = OCIStmtExecute(m_pOciHandle->m_pSvcHD,m_pStmtHD,m_pOciHandle->m_pErrHD,(ub4)0,0,NULL,NULL,OCI_DEFAULT|OCI_STMT_SCROLLABLE_READONLY );
		if (rcState == OCI_SUCCESS || rcState == OCI_SUCCESS_WITH_INFO)
		{
			return -1;
		}
		else
			CheckBackMess();
	}
	return -2;//hanpens some error
	
}

int COciApiPack::getColNums()
{
	ub4 col_num = 0;
	rcState = OCIAttrGet(m_pStmtHD,OCI_HTYPE_STMT,&col_num,0,OCI_ATTR_PARAM_COUNT,m_pOciHandle->m_pErrHD);
	if (rcState == OCI_SUCCESS || rcState == OCI_SUCCESS_WITH_INFO)
	{
		return (int)col_num;
	}
	else
		CheckBackMess();
	return 0;
}

string COciApiPack::getColName(int col_num,ub4 &value_len)
{
	text col_name[FIELD_LEN];
	text *pCol = NULL;
//	ub4 col_lens;
	col_name[0] = '\0';


	//alloc a OCIPara handle
	OCIParamGet(m_pStmtHD,OCI_HTYPE_STMT,m_pOciHandle->m_pErrHD,(dvoid**)&m_pColHD,col_num);
	//同样是下面这个函数，我们同样能够获取相应字段的定义长度，数据类型，精度，小数位数，是否为空，
	//get definition width
	OCIAttrGet(m_pColHD,OCI_DTYPE_PARAM,&collen[col_num-1],0,OCI_ATTR_DATA_SIZE,m_pOciHandle->m_pErrHD);
	//get the type code
	OCIAttrGet(m_pColHD,OCI_DTYPE_PARAM,&coltype[col_num-1],0,OCI_ATTR_DATA_TYPE,m_pOciHandle->m_pErrHD);
	rcState = OCIAttrGet(m_pColHD,OCI_DTYPE_PARAM,(dvoid**)&pCol,(ub4*)&value_len,OCI_ATTR_NAME,m_pOciHandle->m_pErrHD);
	
	if (rcState == OCI_SUCCESS || rcState == OCI_SUCCESS_WITH_INFO)
	{
		col_name[value_len] = '\0';//要及时截断，否则会带出后面一长串
		strncpy((char*)col_name,(char*)pCol,size_t(value_len));//复制前value_len个字符
		string str = (string)(char*)col_name;
		return str;
	}
	else
		CheckBackMess();
	return "";
}
//暂时就提供这一种绑定,注意得是必需从第一项开始
bool COciApiPack::DefindbyPos(int col_num,char *defType,int lengths)
{
//	text m_pszBuf[DATA_LEN];
	sb2 ind;
	//rcState = OCIDefineByPos(m_pStmtHD, &m_pDefHD[col_num-1], m_pOciHandle->m_pErrHD, (ub4)col_num,
	//	(dvoid *)defType, (sb4)lengths,SQLT_STR, (dvoid *)0, (ub2 *)0, (ub2 *)0, (ub4)OCI_DEFAULT);
	/*char temp[40];
	rcState = OCIDefineByPos(m_pStmtHD,&m_pDefHD[col_num-1],m_pOciHandle->m_pErrHD,1,
		(dvoid*)temp,(sb4)lengths,SQLT_STR,(dvoid*)&ind,0,0,OCI_DEFAULT);*/

	rcState = OCIDefineByPos(m_pStmtHD,&m_pDefHD[col_num-1],m_pOciHandle->m_pErrHD,col_num,
		(dvoid*)defType,(sb4)lengths,SQLT_STR,(dvoid*)&ind,0,0,OCI_DEFAULT);
	if (rcState == OCI_SUCCESS || rcState == OCI_SUCCESS_WITH_INFO)
		return TRUE;
	else
		CheckBackMess();
	return FALSE;
}

//after a select sentence ,then this function can retrieve the whole query
void COciApiPack::getQuery(ArrayRows &m_rowSets)
{
	ASSERT(m_pStmtHD != NULL);
	ASSERT(m_pOciHandle);

	int cols = getColNums();
	sb2 ind;

	ColBuff *tempbuff = new ColBuff[cols];

	//for (int i=0;i<cols;i++)
	//	m_pDefHD[i] = (OCIDefine*)0;

	for(int i=0;i<cols;i++)
	{
		tempbuff[i].data[0] = '\0';

		//alloc a OCIPara handle
		/*OCIParamGet(m_pStmtHD,OCI_HTYPE_STMT,m_pOciHandle->m_pErrHD,(dvoid**)&m_pColHD,(ub4)(i+1));

		OCIAttrGet(m_pColHD,OCI_DTYPE_PARAM,&collen[i],0,OCI_ATTR_DATA_SIZE,m_pOciHandle->m_pErrHD);
		OCIAttrGet(m_pColHD,OCI_DTYPE_PARAM,&coltype[i],0,OCI_ATTR_DATA_TYPE,m_pOciHandle->m_pErrHD);*/

		//rcState = OCIDefineByPos(m_pStmtHD,&m_pDefHD[i],m_pOciHandle->m_pErrHD,i+1,(dvoid *)colbuff,collen[i]+1,SQLT_STR,&ind[i],0,0,OCI_DEFAULT);
		rcState = OCIDefineByPos(m_pStmtHD,&m_pDefHD[i],m_pOciHandle->m_pErrHD,i+1,
			(dvoid*)tempbuff[i].data,(sb4)DATA_LEN,SQLT_STR,(dvoid*)&ind,0,0,OCI_DEFAULT);
		
		if (OCI_ERROR == rcState)
			CheckBackMess();
	}

//	int i = 0;
	ArrayRecords RecordArray;
	string temp = "";
	int col_num = 1;
	while (moveNext())
	{
		RecordArray.clear();
		for (int i=0;i<cols;i++)
		{
			temp = (char*)tempbuff[i].data;
			RecordArray.push_back(temp);
		}
		ub4 len;
		m_rowSets.insert(pair<string, ArrayRecords>(this->getColName(col_num,len),RecordArray));
		col_num ++;
	}
	//释放空间
	delete []tempbuff;

}


bool COciApiPack::moveNext()
{
//	int nStatus;
	rcState = OCIStmtFetch2(m_pStmtHD,m_pOciHandle->m_pErrHD ,(ub4)1,OCI_FETCH_NEXT,(sb4)1,OCI_DEFAULT);
	if (rcState == OCI_SUCCESS || rcState == OCI_SUCCESS_WITH_INFO)
		return TRUE;
	else
		CheckBackMess();
	return FALSE;
}

bool COciApiPack::movePrior()
{
	rcState = OCIStmtFetch2(m_pStmtHD,m_pOciHandle->m_pErrHD ,(ub4)1,OCI_FETCH_PRIOR,(sb4)-1,OCI_DEFAULT);
	if (rcState == OCI_SUCCESS || rcState == OCI_SUCCESS_WITH_INFO)
		return TRUE;
	else
		CheckBackMess();
	return FALSE;
}

void COciApiPack::moveFirst()
{
	rcState = OCIStmtFetch2(m_pStmtHD,m_pOciHandle->m_pErrHD ,(ub4)1,OCI_FETCH_FIRST,(sb4)0,OCI_DEFAULT);
	if (!(rcState == OCI_SUCCESS || rcState == OCI_SUCCESS_WITH_INFO))
		CheckBackMess();
}

void COciApiPack::moveLast()
{
	rcState = OCIStmtFetch2(m_pStmtHD,m_pOciHandle->m_pErrHD ,(ub4)1,OCI_FETCH_LAST,(sb4)0,OCI_DEFAULT);
	if (!(rcState == OCI_SUCCESS || rcState == OCI_SUCCESS_WITH_INFO))
		CheckBackMess();
}

void COciApiPack::moveAbsolute(ub4 rowNum)
{
	rcState = OCIStmtFetch2(m_pStmtHD,m_pOciHandle->m_pErrHD ,(ub4)rowNum,OCI_FETCH_LAST,(sb4)1,OCI_DEFAULT);
	if (!(rcState == OCI_SUCCESS || rcState == OCI_SUCCESS_WITH_INFO))
		CheckBackMess();
}

