
// ClientPartDlg.h: 헤더 파일
//

#pragma once


// CClientPartDlg 대화 상자
class CClientPartDlg : public CDialogEx
{
private:
	SOCKET mh_socket;

// 생성입니다.
public:
	CClientPartDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

	void AddEventString(CString parm_string);
	void ConnectProcess(LPARAM lParam);
	void DestroySocket();
	void ReadFrameData();
	void ReceiveData(char* parm_p_buffer, int parm_size);
	void SendFrameData(SOCKET parm_h_socket, unsigned char parm_id, const void *parm_p_data, int parm_size);



// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CLIENTPART_DIALOG };
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
private:
	CListBox m_event_list;
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnBnClickedSendBtn();
	afx_msg void OnBnClickedOk();
};
