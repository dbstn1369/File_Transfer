
// File_transferDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "File_transfer.h"
#include "File_transferDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CFiletransferDlg 대화 상자



CFiletransferDlg::CFiletransferDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FILE_TRANSFER_DIALOG, pParent),
	CBaseLayer("ChatDlg")
{
	m_unDstAddr = "";
	m_unSrcAddr = "";
	//m_stMessage = _T("");

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_LayerMgr.AddLayer(new CFileAppLayer("File"));//FilaApp
	m_LayerMgr.AddLayer(new CEthernetLayer("Ethernet"));
	m_LayerMgr.AddLayer(new CNILayer("NL"));
	
	m_LayerMgr.AddLayer(this);

	m_LayerMgr.ConnectLayers("NL ( *Ethernet (  *File ( *ChatDlg ) ) )");
	
	m_NI = (CNILayer *)m_LayerMgr.GetLayer("NL");
	m_Eth = (CEthernetLayer *)m_LayerMgr.GetLayer("Ethernet");
	m_File = (CFileAppLayer *)m_LayerMgr.GetLayer("File");
}

void CFiletransferDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_chat_list, m_ListChat);
	//DDX_Text(pDX, IDC_EDIT_chat, m_stMessage);
	DDX_Text(pDX, IDC_EDIT_Src, m_unSrcAddr);
	DDX_Control(pDX, IDC_COMBO, m_ComboDeviceList);
	DDX_Control(pDX, IDC_EDIT_Dst,m_EditDstAddr);
	DDX_Control(pDX, IDC_EDIT_file_select, m_EditFileSelect);
	
}

BEGIN_MESSAGE_MAP(CFiletransferDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()

	ON_BN_CLICKED(IDC_BUTTON_Setting, &CFiletransferDlg::OnBnClickedButtonSetting)
	ON_BN_CLICKED(IDC_BUTTON_file_select, &CFiletransferDlg::OnBnClickedButtonfileselect)
	ON_BN_CLICKED(IDC_BUTTON_Send, &CFiletransferDlg::OnBnClickedButtonSend)//file send
	ON_BN_CLICKED(IDC_BUTTON_PacketSend, &CFiletransferDlg::OnBnClickedButtonPacketsend)//packet test send
	ON_CBN_SELCHANGE(IDC_COMBO, &CFiletransferDlg::OnCbnSelchangeCombo)
END_MESSAGE_MAP()


// CFiletransferDlg 메시지 처리기

BOOL CFiletransferDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다. 
	UpdateData(TRUE);
	//////////////////////// fill the blank ///////////////////////////////
	CComboBox*	p_EtherComboBox = (CComboBox*)GetDlgItem(IDC_COMBO);
	int i;
	CString device_description;

	for (i = 0; i < NI_COUNT_NIC; i++) {
		if (!m_NI->GetAdapterObject(i))
			break;
		else {
			device_description = m_NI->GetAdapterObject(i)->description;
			device_description.Trim();
			p_EtherComboBox->AddString(device_description);
		}
	}
	p_EtherComboBox->SetCurSel(0);

	CString inNicName = m_NI->GetAdapterObject(0)->name;
	///////////////////////////////////////////////////////////////////////
	UpdateData(FALSE);
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CFiletransferDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CFiletransferDlg::OnPaint()
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
HCURSOR CFiletransferDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CFiletransferDlg::Receive(unsigned char* ppayload) {
	BOOL bSuccess = FALSE;

	u_char spayload[FILE_APP_DATA_SIZE];
	memcpy(spayload, ppayload, FILE_APP_DATA_SIZE);
	
	FILE* file_pointer = NULL;
	

	file_pointer=fopen("receive.txt", "w");

	if (file_pointer != NULL) {
		//file operations 

		fwrite((char*)spayload,sizeof(u_char),FILE_APP_DATA_SIZE, file_pointer);

		fclose(file_pointer);
	}
	

	AfxMessageBox("패킷 도착");
	return TRUE;
}

BOOL CFiletransferDlg::Send(unsigned char* ppayload, int nlength) {
	BOOL bSuccess = FALSE;
	bSuccess = mp_UnderLayer->Send(ppayload, nlength);

	return bSuccess;
}

inline void CFiletransferDlg::SendData() {
	
	int sz;
	CString path;
	
	m_EditFileSelect.GetWindowTextA(path);
	
		Send((u_char*)(LPCSTR)path,path.GetLength());
	}
	


void CFiletransferDlg::OnBnClickedButtonSetting()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	unsigned char *eth_temp = MacAddrToHexInt(m_unSrcAddr);
	ETHERNET_ADDR srcaddr;
	srcaddr.addr0 = eth_temp[0];
	srcaddr.addr1 = eth_temp[1];
	srcaddr.addr2 = eth_temp[2];
	srcaddr.addr3 = eth_temp[3];
	srcaddr.addr4 = eth_temp[4];
	srcaddr.addr5 = eth_temp[5];


	CString temp;
	ETHERNET_ADDR dstaddr;
	
	m_EditDstAddr.GetWindowTextA(m_unDstAddr);
	for (int i = 0; i < 6; i++)
	{
		AfxExtractSubString(temp, (m_unDstAddr), i, ':');
		dstaddr.addrs[i]= strtol(temp, NULL, 16);
	}
	

	m_Eth->SetSourceAddress(srcaddr.addrs);
	m_Eth->SetDestinAddress(dstaddr.addrs);
	m_File->StartFileThread();
	m_NI->PacketStartDriver();

}


void CFiletransferDlg::OnBnClickedButtonfileselect()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	
	
	const TCHAR szFilter[] = "Image (*.BMP, *.GIF, *.JPG) | *.BMP;*.GIF;*JPG | All Files(*.*)|*.*||";
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter);
	if (dlg.DoModal() == IDOK)
	{
		CString sFilePath = dlg.GetPathName();
		m_EditFileSelect.SetWindowTextA(sFilePath);
		
	}

}


void CFiletransferDlg::OnBnClickedButtonSend()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	SendData();

	
}


void CFiletransferDlg::OnBnClickedButtonPacketsend()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	
	SendData();

}


void CFiletransferDlg::OnCbnSelchangeCombo()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	UpdateData(TRUE);
	// NICard 이름으로 Mac address 가져오기
	CComboBox*	pEX_EtherComboBox = (CComboBox*)GetDlgItem(IDC_COMBO);

	// ComboBox에서 선택된 인덱스 번호 가져오기
	int sIndex = pEX_EtherComboBox->GetCurSel();

	// 가져온 인덱스 번호로 NIC의 Adapter 이름 가져오기
	CString nicName = m_NI->GetAdapterObject(sIndex)->name;

	// Adapter 이름으로 Mac Address를 가져오기
	m_unSrcAddr = m_NI->GetNICardAddress((char *)nicName.GetString());
	
	UpdateData(FALSE);

}
unsigned char* CFiletransferDlg::MacAddrToHexInt(CString ether)
{
	// 콜론(:)으로 구분 되어진 Ethernet 주소를
	// 콜론(:)을 토큰으로 한 바이트씩 값을 가져와서 Ethernet배열에 넣어준다.
	CString cstr;
	unsigned char *file_ether = (u_char *)malloc(sizeof(u_char) * 6);

	for (int i = 0; i < 6; i++) {
		AfxExtractSubString(cstr, ether, i, ':');
		// strtoul -> 문자열을 원하는 진수로 변환 시켜준다.
		file_ether[i] = (unsigned char)strtoul(cstr.GetString(), NULL, 16);
	}
	file_ether[6] = '\0';

	return file_ether;
}

