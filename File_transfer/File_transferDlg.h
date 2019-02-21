
// File_transferDlg.h: 헤더 파일
//

#pragma once
#include "LayerManager.h"	// Added by ClassView
#include "EthernetLayer.h"	// Added by ClassView
#include "FileAppLayer.h"		// Added by ClassView
#include "NILayer.h"		// Added by ClassView
#include <Windows.h>

using namespace std;


// CFiletransferDlg 대화 상자
class CFiletransferDlg : public CDialogEx, public CBaseLayer
{
// 생성입니다.
public:
	CFiletransferDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FILE_TRANSFER_DIALOG };
#endif
	CListBox	m_ListChat;
	CString		m_stMessage;
	CString		m_unDstAddr;
	CString		m_unSrcAddr;
	CComboBox	m_ComboDeviceList;
	CEdit		m_EditDstAddr;
	CEdit		m_EditFileSelect;
	
	

	BOOL			Receive(unsigned char* ppayload);
	BOOL			Send(unsigned char* ppayload, int nlengt);
	unsigned char*	MacAddrToHexInt(CString ether);


	
	inline void		SendData();

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;
	
	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:

	CLayerManager	m_LayerMgr;

	afx_msg void OnBnClickedButtonSetting();
	afx_msg void OnBnClickedButtonfileselect();
	afx_msg void OnBnClickedButtonSend();
	afx_msg void OnBnClickedButtonPacketsend();

	
	CNILayer* m_NI;
	CEthernetLayer* m_Eth;
	CFileAppLayer* m_File;
	afx_msg void OnCbnSelchangeCombo();
	
	
};
