// MfcConsole.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "MfcConsole.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif


/////////////////////////////////////////////////////////////////////////////
// Implementation of CMyCommandLineInfo

CMyCommandLineInfo::CMyCommandLineInfo(void) : CCommandLineInfo(), m_bHelp(FALSE), m_bSuccess(TRUE)
{
}

void CMyCommandLineInfo::ParseParam(LPCTSTR lpszParam, BOOL bSwitch, BOOL /*bLast*/)
{
   if (bSwitch)
   {
      if (0 == lstrcmpi(_T("help"), lpszParam))
      {
         m_bHelp = TRUE;
      }
      else
      {
         // the for loop enables 'compound' switches like "/XYZ"
         BOOL bContinue = TRUE;
         for(int i = 0; (i < lstrlen(lpszParam)) && m_bSuccess && bContinue; i++)
         {
            switch(lpszParam[i])
            {
               case _T('?'):
               case _T('h'):
               case _T('H'):
                  m_bHelp = TRUE;
                  break;
               default:
                  m_bSuccess = bContinue = FALSE;
                  break;
            }
         }
      }
   }
   else
   {
      m_strParameter = lpszParam;
   }
}

/////////////////////////////////////////////////////////////////////////////
// Instatiate the application class

DECLARE_CONSOLEAPP

CMyApp theApp;

/////////////////////////////////////////////////////////////////////////////
// Implementation of the application class

CMyApp::CMyApp(void) : CWinApp()
{
}

CMyApp::~CMyApp(void)
{
}

BOOL CMyApp::InitInstance()
{
   if (!CWinApp::InitInstance())
   {
      return FALSE;
   }
   
   _tprintf(_T("Init\n"));

   SetRegistryKey(_T("Codeguru samples"));
   
   ParseCommandLine(m_cmdInfo);


    mOci.initOciDB();

   return TRUE;
}

int CMyApp::Run()
{
   if (!m_cmdInfo.m_bSuccess)
   {
      _tprintf(_T("Bad command line\n"));
   }
   if (m_cmdInfo.m_bHelp || (!m_cmdInfo.m_bSuccess))
   {
      _tprintf(_T("usage: [ \"string\" /?]\n"));
   }
   else
   {
      m_cmdInfo.m_strParameter;
      _tprintf(_T("Run\n"));
      
	  CString csSQL = _T("connect sms/sms@XE_131");

	 
	  if(mOci.logon(("XE_131"),("sms"),("sms")))
	  {
		  _tprintf(_T("logon success\n"));

		  mOci.AllocStmt();
		  mOci.prepareSQL("select title,content from sms.sms_send");
		  mOci.ExecuteSQL(0);
		  
		  ArrayRows rows;
		  mOci.getQuery(rows);
		  printf("%s\n",rows["CONTENT"][0]);
		   mOci.FreeStmt();
	  }
	  mOci.logout();

	  

	  //csSQL.Format(_T("connect %s/%s@%s"),csDBName,csDBPassword,csDBAddr);

	  //if(SQLCommand(csSQL) == S_OK)
	  //{
		 // //csSQL = _T("exec insert into sms.sms_send(title,content) values (\'test1\',\'hahahasdfsdf2\')");
		 // //SQLCommand(csSQL);

		 // csSQL = _T("select select * from sms.sms_send");
		 // if(SQLCommand(csSQL) == S_OK)
		 // {
		 // }

		 // SQLCommand(_T("disconnect"));
	  //}

   }   
   return CWinApp::Run(); // calls ExitInstance and exits right away when m_pMainWnd=NULL
}

int CMyApp::ExitInstance()
{
   _tprintf(_T("Done\n"));

   mOci.uninitOciDB();
   return CWinApp::ExitInstance();
}

