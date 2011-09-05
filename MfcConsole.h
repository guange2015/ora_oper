#pragma once

#include "resource.h"
#include "OciApiPack.h"

#if !defined(_CONSOLE)
   #error Make it a console application project
#endif

/////////////////////////////////////////////////////////////////////////////
// DECLARE_CONSOLEAPP - enables MFC-like console app

#ifdef _CONSOLE
   #define DECLARE_CONSOLEAPP \
      extern int AFXAPI AfxWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,LPTSTR lpCmdLine, int nCmdShow);\
      extern "C" int _tmain( int /*argc*/, TCHAR** /*argv*/, TCHAR** /*envp*/)\
      {\
         return AfxWinMain(GetModuleHandle(NULL), NULL, GetCommandLine(), SW_SHOW);\
      }
      // remember to instantiate app class
#endif // _CONSOLE



/////////////////////////////////////////////////////////////////////////////
// Interface of CMyCommandLineInfo

class CMyCommandLineInfo : public CCommandLineInfo 
{
// Attributes
public:
   BOOL    m_bHelp;        // /H /? /HELP
   CString m_strParameter;
   BOOL    m_bSuccess;     // all switches ok

// Construction
public:
   CMyCommandLineInfo(void);

// Overrides
public:
   virtual void ParseParam( LPCTSTR lpszParam, BOOL bFlag, BOOL bLast );
};


/////////////////////////////////////////////////////////////////////////////
// Interface of some application class

class CMyApp : public CWinApp
{
// Construction
public:
   CMyApp(void);

// Attributes
public:
   CMyCommandLineInfo m_cmdInfo;

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CMyApp)
   public:
   virtual BOOL InitInstance();
   virtual int Run();
   virtual int ExitInstance();
   //}}AFX_VIRTUAL

// Implementation
public:
   virtual ~CMyApp(void);


private:
	//CCLog *g_Log;				//»’÷æ

	CArray<CString, CString > g_CArray_SelectItem;

	COciApiPack mOci;
};

#define INFO  _tprintf
#define ERRORING _tprintf