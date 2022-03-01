
// ServerpartDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "Serverpart.h"
#include "ServerpartDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CServerpartDlg 대화 상자



CServerpartDlg::CServerpartDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SERVERPART_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_client_count = 0;
}

void CServerpartDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_event_list);
}

BEGIN_MESSAGE_MAP(CServerpartDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_LBN_SELCHANGE(IDC_LIST1, &CServerpartDlg::OnLbnSelchangeList1)
END_MESSAGE_MAP()


// CServerpartDlg 메시지 처리기

BOOL CServerpartDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	// 프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	mh_listen_socket = socket(AF_INET, SOCK_STREAM, 0); // TCP, 3번째 인자 0으로 하면 앞에 타입에 맞는 IP프로토콜을 맞춰준다.

	struct sockaddr_in srv_addr;
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");	//	로컬 호스트 -> 자기 ip 주소로 변경후 실행 가능
	srv_addr.sin_port = htons(20001);

	bind(mh_listen_socket, (SOCKADDR*)&srv_addr, sizeof(srv_addr));

	listen(mh_listen_socket, 1);
	AddEventString(L"클라이언트의 접속을 허락합니다...");
	WSAAsyncSelect(mh_listen_socket, m_hWnd, 27001, FD_ACCEPT);
	// 실제로 클라이언트가 접속하지 않으면 내 프로그램이 동기방식은 스레드 프로그래밍을 해야함, 비동기 방식은 운영체제가
	// 제공하는 소켓매니저한테 어떤일이 발생하면 나한테 알려줘~ 라는 방식으로 함수 호출이 일어나면
	// 응답없음 상태로 빠지지 않고 어떤 상태이던지 소켓값을 나한테 전달 받을 수 있도록 하는 방식입니다.
	// FD_ACCEPT는 클라이언트가 어디로 접속을 한다라는 이벤트
	// 27001은 내가정한 포트번호

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CServerpartDlg::OnPaint()
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
//  이 함수를 호출합니다.
HCURSOR CServerpartDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//	클라이언트 접속 함수 - AcceptProcess
//	클라이언트의 연결을 받아들이는 함수입니다.
void CServerpartDlg::AcceptProcess(SOCKET parm_h_socket)
{
	if (MAX_CLIENT_COUNT > m_client_count) {
		struct sockaddr_in client_addr;
		int sockaddr_in_size = sizeof(client_addr);
		//-> 기존 소켓은 다음 사용자와 통신하도록 구현

		mh_client_list[m_client_count] = accept(parm_h_socket, (SOCKADDR*)&client_addr, &sockaddr_in_size);	// 현재 listen 소켓을 복제해서 기존 사용자와 통신하도록 함(listen socket, 클라이언트 구조체의 주소, 구조체 크기의 주소) 

		WSAAsyncSelect(mh_client_list[m_client_count], m_hWnd, 27002, FD_READ | FD_CLOSE);

		CString ip_address;
		ip_address = inet_ntoa(client_addr.sin_addr);
		wcscpy(m_client_ip[m_client_count], ip_address);

		AddEventString(L"새로운 클라이언트가 접속했습니다 : " + ip_address);

		m_client_count++;
	}
	else {

	}
}

//	문자열 출력을 위한 함수 - AddEventString
void CServerpartDlg::AddEventString(CString parm_string)
{
	int index = m_event_list.InsertString(-1, parm_string);	// -1은 문자열의 끝에 추가함을 의미한다.
	m_event_list.SetCurSel(index);
}

//	클라이언트 접속 종료 함수 - ClientCloseProcess
//	클라이언트의 연결이 끊어지거나 다른 곳에서 에러가 발생한 경우 클라이언트의 접속을 종료하는 함수입니다.
void CServerpartDlg::ClientCloseProcess(SOCKET parm_h_socket, char parm_force_flag)
{
	if (parm_force_flag == 1) {
		LINGER temp_linger = { TRUE, 0 };	// LINGER는 소켓을 끊을 때 어떤 값을 전달할 것인지를 처리하는 역할을 한다. 여기서 { TRUE, 0 }이라고 넣으면 데이터가 전송 및 수신 상황에 상관없이 즉각적으로 소켓을 닫겠다 라는 의미입니다.
		setsockopt(parm_h_socket, SOL_SOCKET, SO_LINGER, (char*)&temp_linger, sizeof(temp_linger));
	}

	closesocket(parm_h_socket);

	for (int i = 0; i < m_client_count; i++) {
		if (mh_client_list[i] == parm_h_socket) {	// 소켓이 순차적으로 나가는 것이 아니라 중간에 나가버리는 경우 맨뒤에 친구를 복사해서 중간에 넣고 그만큼 카운트를 줄여준다.
			m_client_count--;
			if (i != m_client_count) {
				mh_client_list[i] = mh_client_list[m_client_count];
				wcscpy(m_client_ip[i], m_client_ip[m_client_count]);	// 소켓을 이동하더라도 ip를 같이 이동시켜줘서 문제가 없도록 유지함
			}
		}
	}
}

//	안전한 수신을 위한 함수 - ReceiveData
//	수신되는 데이터가 크거나 네트워크 상태가 좋지 못하면 데이터가 여러번에 걸쳐 나누어져 수신되는 경우가 있습니다.
//	이런 문제점을 처리하기 위한 함수입니다.
void CServerpartDlg::ReceiveData(SOCKET parm_h_socket, char* parm_p_buffer, int parm_size)
{
	int current_size, total_size = 0, retry_count = 0;

	while (total_size < parm_size) {
		// 컴퓨터 마다 속도가 다르기 때문에 읽은 크기를 측정해야함
		// 전체(body_size)에서 수신된양(total_size)을 뺀 나머지를 읽는다.
		current_size = recv(parm_h_socket, parm_p_buffer + total_size, parm_size - total_size, 0);

		if (current_size == SOCKET_ERROR) {			// recv를 했는데 에러인 경우
			retry_count++;
			Sleep(50);
			if (retry_count > 5) break;				// 에러가 나더라도 바로 끊지 않고 재시도를 6번 정도 하고 끊어줌
		}
		else {
			retry_count = 0;						// 정상적으로 받았다면 에러카운팅 한것을 0으로 초기화
			total_size = total_size + current_size;	// 에러가 아니면 읽은 데이터를 계속 누적함
		}
	}
}

//	데이터 전송용 함수 - SendFrameData
//	서버와 클라이언트 모두 네트워크로 데이터를 전송하는 기능이 있어야 하기 때문에
//	약속한 프로토콜 형식으로 전송하는 함수입니다.
void CServerpartDlg::SendFrameData(SOCKET parm_h_socket, unsigned char parm_id, const void* parm_p_data, int parm_size)
{
	char* p_send_data = new char[parm_size + 4];

	*p_send_data = 27;	// 헤더 프로토콜
	*(unsigned short int*)(p_send_data + 1) = parm_size;	// 바디 사이즈
	*(p_send_data + 3) = parm_id;

	memcpy(p_send_data + 4, parm_p_data, parm_size);

	send(parm_h_socket, p_send_data, parm_size + 4, 0);

	delete[] p_send_data;
}

//	메시지 이벤트에 따른 윈도우 처리 프로시져 - WiindowProc
LRESULT CServerpartDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (27001 == message) {
		AcceptProcess((SOCKET)wParam);
	}
	else if (27002 == message)	// FD_READ, FD_CLOSE
	{
		SOCKET h_socket = (SOCKET)wParam;	// mh_listen_socket 핸들값과 동일
		if (WSAGETSELECTEVENT(lParam) == FD_READ) { // WSAGETSELECTEVENT는 32bit인 lParam 들어있는 정보중에서 어떤이벤트가 발생하였는지에 대한 16bit에 대한 값을 빼내서 반환해줍니다.
			WSAAsyncSelect(h_socket, m_hWnd, 27002, FD_CLOSE);	// 이렇게 하면 다시 읽든 재시도 하든 비동기를 끊어줫기때문에 FD_READ가 여러번 발생하는 문제를 해결할 수 있다.

			char key, network_message_id;
			recv(h_socket, &key, 1, 0);	// h_socket에 연결된 수신 데이터에서 내 클라이언트인지 확인하는 1바이트 만큼 데이터를 가져옴
			if (key == 27) {
				int current_size, total_size = 0, retry_count = 0;
				unsigned short int body_size;
				recv(h_socket, (char*)&body_size, sizeof(body_size), 0);	// 바디 크기가 얼마인지를 알려주는 바디 사이즈 2바이트
				recv(h_socket, &network_message_id, 1, 0);	// 네트워크 메시지 아이디 1바이트

				if (body_size > 0) {
					char* p_body_data = new char[body_size];

					ReceiveData(h_socket, p_body_data, body_size);

					if (network_message_id == 1) {
						// 실제로 클라이언트가 보내준 데이터를 처리 (p_body_data)

						CString str;

						for (int i = 0; i < m_client_count; i++) {
							if (h_socket == mh_client_list[i]) {
								str.Format(L"%s : %s", m_client_ip[i], p_body_data);
								break;
							}
						}

						AddEventString(str);
						for (int i = 0; i < m_client_count; i++) {
							SendFrameData(mh_client_list[i], 1, (const wchar_t*)str, (str.GetLength() + 1) * 2);
						}
					}
					delete[] p_body_data;
				}

				WSAAsyncSelect(h_socket, m_hWnd, 27002, FD_CLOSE | FD_READ);
			}
			else // key가 27이 아니면 다른 곳에서 온 소켓일 수 있으니 소켓을 파괴시켜준다.
				ClientCloseProcess(h_socket, 1);
		}
		else { // FD_CLOSE -> 상대편 연결이 끊어짐
			ClientCloseProcess(h_socket, 0);
			AddEventString(L"클라이언트가 접속을 해제했습니다.");
		}
	}
	return CDialogEx::WindowProc(message, wParam, lParam);
}


void CServerpartDlg::OnLbnSelchangeList1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}
