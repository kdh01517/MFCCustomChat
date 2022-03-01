
// ClientPart.cpp: 애플리케이션에 대한 클래스 동작을 정의합니다.
//

#include "pch.h"
#include "framework.h"
#include "ClientPart.h"
#include "ClientPartDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CClientPartApp

BEGIN_MESSAGE_MAP(CClientPartApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CClientPartApp 생성

CClientPartApp::CClientPartApp()
{
	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}


// 유일한 CClientPartApp 개체입니다.

CClientPartApp theApp;


// CClientPartApp 초기화

BOOL CClientPartApp::InitInstance()
{
	CWinApp::InitInstance();

	WSADATA temp;
	WSAStartup(0x0202, &temp);

	CClientPartDlg dlg;
	m_pMainWnd = &dlg;
	dlg.DoModal();

	WSACleanup();
	
	return FALSE;
}

