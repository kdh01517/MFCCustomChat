
// Serverpart.cpp: 애플리케이션에 대한 클래스 동작을 정의합니다.
//

#include "pch.h"
#include "framework.h"
#include "Serverpart.h"
#include "ServerpartDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CServerpartApp

BEGIN_MESSAGE_MAP(CServerpartApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CServerpartApp 생성

CServerpartApp::CServerpartApp()
{
	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}


// 유일한 CServerpartApp 개체입니다.

CServerpartApp theApp;


// CServerpartApp 초기화

BOOL CServerpartApp::InitInstance()
{
	CWinApp::InitInstance();

	// 내가 지금부터 네트워크를 사용하겠다!!
	// 지금부터 소켓을 사용하겠다.
	// 2.2버전을 사용
	WSADATA temp;
	WSAStartup(0x0202, &temp);

	CServerpartDlg dlg;
	m_pMainWnd = &dlg;
	dlg.DoModal();
	
	WSACleanup();

	return FALSE;
}

