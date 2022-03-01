
// ClientPartDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "ClientPart.h"
#include "ClientPartDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CClientPartDlg 대화 상자



CClientPartDlg::CClientPartDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CLIENTPART_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CClientPartDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_event_list);
}

BEGIN_MESSAGE_MAP(CClientPartDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_SEND_BTN, &CClientPartDlg::OnBnClickedSendBtn)
	ON_BN_CLICKED(IDOK, &CClientPartDlg::OnBnClickedOk)
END_MESSAGE_MAP()

//	문자열 출력을 위한 함수 - AddEventString
void CClientPartDlg::AddEventString(CString parm_string)
{
	int index = m_event_list.InsertString(-1, parm_string);
	m_event_list.SetCurSel(index);
}

// CClientPartDlg 메시지 처리기

BOOL CClientPartDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	mh_socket = socket(AF_INET, SOCK_STREAM, 0);

	sockaddr_in srv_addr;

	srv_addr.sin_family = AF_INET;
	srv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");	//	로컬 호스트 -> 서버 ip 주소로 변경후 실행 가능
	srv_addr.sin_port = htons(20001);

	WSAAsyncSelect(mh_socket, m_hWnd, 27001, FD_CONNECT);
	connect(mh_socket, (sockaddr*)&srv_addr, sizeof(srv_addr));

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CClientPartDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
// 이 함수를 호출합니다.
HCURSOR CClientPartDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//	접속 시도에 대한 결과 처리를 위한 함수 - ConnectProcess
//	서버에 접속을 성공하면 비동기를 설정하고 서버에 접속을
//	실패 했다면 소켓을 제거하고 해당 변수를 초기화합니다.
void CClientPartDlg::ConnectProcess(LPARAM lParam)
{
	if (WSAGETSELECTERROR(lParam) == 0) { // 접속에 성공
		WSAAsyncSelect(mh_socket, m_hWnd, 27002, FD_READ | FD_CLOSE);
		AddEventString(L"서버에 접속했습니다!");
	}
	else {	// 접속에 실패
		DestroySocket();
		AddEventString(L"서버에 접속할 수 없습니다!");
	}
}

//	소켓 제거 함수 - DestroySocket
//	생성한 소켓을 제거하고 소켓을 초기화합니다.
void CClientPartDlg::DestroySocket()
{
	LINGER temp_linger = { TRUE, 0 };
	//	서버에서 데이터를 수신하고 있는 상태라면 강제로 소켓을 제거하지 못하기 때문에
	//	링거 옵션을 설정하여 데이터를 수신하고 있더라도 소켓을 제거할 수 있도록 합니다.
	setsockopt(mh_socket, SOL_SOCKET, SO_LINGER, (char*)&temp_linger, sizeof(temp_linger));

	closesocket(mh_socket);
	mh_socket = INVALID_SOCKET;
}

//	안전한 수신을 위한 함수 - ReceiveData
//	수신되는 데이터가 크거나 네트워크 상태가 좋지 못하면 데이터가 여러번에 걸쳐 나누어져 수신되는 경우가 있습니다.
//	이런 문제점을 처리하기 위한 함수입니다.
void CClientPartDlg::ReceiveData(char* parm_p_buffer, int parm_size)
{
	int current_size, total_size = 0, retry_count = 0;

	while (total_size < parm_size) {
		current_size = recv(mh_socket, parm_p_buffer + total_size, parm_size - total_size, 0);	// 컴퓨터 마다 속도가 다르기 때문에 읽은 크기를 측정해야함 전체(body_size)에서 수신된양(total_size)을 뺀 나머지를 읽는다.

		if (current_size == SOCKET_ERROR) {	// recv를 했는데 에러인 경우
			retry_count++;
			Sleep(50);
			if (retry_count > 5) break;	// 에러가 나더라도 바로 끊지 않고 재시도를 6번 정도 하고 끊어줌
		}
		else {
			retry_count = 0;	// 정상적으로 받았다면 에러카운팅 한것을 0으로 초기화
			total_size = total_size + current_size;	// 에러가 아니면 읽은 데이터를 계속 누적함
		}
	}
}

//	새로운 데이터가 수신되었을 때 사용하는 함수 - ReadFrameData
void CClientPartDlg::ReadFrameData()
{
	char key, message_id;
	recv(mh_socket, &key, 1, 0);
	if (key == 27) {
		unsigned short int body_size;
		recv(mh_socket, (char*)&body_size, 2, 0);
		recv(mh_socket, &message_id, 1, 0);
		if (body_size > 0) {
			char* p_body_data = new char[body_size];

			ReceiveData(p_body_data, body_size);

			if (message_id == 1) {
				AddEventString((wchar_t*)p_body_data);
			}

			delete[] p_body_data;
		}
	}
	else {
		DestroySocket();
		AddEventString(L"잘못된 프로토콜입니다.");
	}
}

//	메시지 이벤트에 따른 윈도우 처리 프로시져 - WiindowProc
LRESULT CClientPartDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == 27001) {	// FD_CONNECT
		ConnectProcess(lParam);
	}
	else if (message == 27002) {	// FD_READ or FD_CLOSE
		if (WSAGETSELECTEVENT(lParam) == FD_READ) {
			WSAAsyncSelect(mh_socket, m_hWnd, 27002, FD_CLOSE);

			ReadFrameData();

			if (mh_socket != INVALID_SOCKET)	// 소켓에 문제가 발생하지 않았다면 다시 수신할 수 있도록 FD_READ 이벤트를 추가 해준다.
				WSAAsyncSelect(mh_socket, m_hWnd, 27002, FD_CLOSE | FD_READ);
		}
		else {
			DestroySocket();
			AddEventString(L"서버에 연결을 해제하였습니다.");
		}
	}

	return CDialogEx::WindowProc(message, wParam, lParam);
}

//	데이터 전송용 함수 - SendFrameData
//	서버와 클라이언트 모두 네트워크로 데이터를 전송하는 기능이 있어야 하기 때문에
//	약속한 프로토콜 형식으로 전송하는 함수입니다.
void CClientPartDlg::SendFrameData(SOCKET parm_h_socket, unsigned char parm_id, const void* parm_p_data, int parm_size)
{
	char* p_send_data = new char[parm_size + 4];

	*p_send_data = 27;	// 헤더 프로토콜
	*(unsigned short int*)(p_send_data + 1) = parm_size;	// 바디 사이즈
	*(p_send_data + 3) = parm_id;

	memcpy(p_send_data + 4, parm_p_data, parm_size);

	send(parm_h_socket, p_send_data, parm_size + 4, 0);

	delete[] p_send_data;
}

//	문자열 전송 함수 - OnBnClickedSendBtn
//	전송 버튼을 클릭하였을 때 문자열 전송 이벤트를 처리하는 함수입니다.
void CClientPartDlg::OnBnClickedSendBtn()
{
	if (mh_socket != INVALID_SOCKET) {
		CString str;
		GetDlgItemText(IDC_EDIT1, str);

		SendFrameData(mh_socket, 1, (const wchar_t*)str, (str.GetLength() + 1) * 2);
	}
	else {
		AddEventString(L"서버에 접속된 상태가 아닙니다.");
	}

}

void CClientPartDlg::OnBnClickedOk()
{
	OnBnClickedSendBtn();
	GotoDlgCtrl(GetDlgItem(IDC_EDIT1));

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// CDialogEx::OnOK();
}
