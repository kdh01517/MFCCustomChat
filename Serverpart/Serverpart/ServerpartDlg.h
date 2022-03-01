
// ServerpartDlg.h: 헤더 파일
//

#pragma once

#define MAX_CLIENT_COUNT	10

// CServerpartDlg 대화 상자
class CServerpartDlg : public CDialogEx
{
private:
	SOCKET mh_listen_socket;

	SOCKET mh_client_list[MAX_CLIENT_COUNT];
	wchar_t m_client_ip[MAX_CLIENT_COUNT][24];	// 클라이언트 ip주소 저장
	int m_client_count;

// 생성입니다.
public:
	CServerpartDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

	void AcceptProcess(SOCKET parm_h_socket);
	void ClientCloseProcess(SOCKET parm_h_socket, char parm_force_flag);
	void ReceiveData(SOCKET parm_h_socket, char* parm_p_buffer, int parm_size);
	void AddEventString(CString parm_string);
	void SendFrameData(SOCKET parm_h_socket, unsigned char parm_id, const void* parm_p_data, int parm_size);


// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SERVERPART_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnLbnSelchangeList1();
private:


//	CListBox m_event_list;
	CListBox m_event_list;
};
