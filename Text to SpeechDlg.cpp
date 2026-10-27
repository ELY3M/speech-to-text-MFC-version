
// Text to SpeechDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "Text to Speech.h"
#include "Text to SpeechDlg.h"
#include "afxdialogex.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
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


// CTexttoSpeechDlg dialog



CTexttoSpeechDlg::CTexttoSpeechDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TEXT_TO_SPEECH_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTexttoSpeechDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTexttoSpeechDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_PLAY, &CTexttoSpeechDlg::OnBnClickedPlay)
	ON_BN_CLICKED(IDC_SAVE, &CTexttoSpeechDlg::OnBnClickedSave)
	ON_BN_CLICKED(IDC_LISTEN, &CTexttoSpeechDlg::OnBnClickedListen)
END_MESSAGE_MAP()


// CTexttoSpeechDlg message handlers

BOOL CTexttoSpeechDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here


	m_bIsListening = FALSE;
	// Initialize COM if not already done in the app instance
	HRESULT hr = CoCreateInstance(CLSID_SpSharedRecoContext, NULL, CLSCTX_ALL, IID_ISpRecoContext, (void**)&m_cpRecoContext);
	if (SUCCEEDED(hr))
	{
		// Set interest in recognition events
		hr = m_cpRecoContext->SetInterest(SPFEI(SPEI_RECOGNITION), SPFEI(SPEI_RECOGNITION));
	}
	HWND hWnd = this->GetSafeHwnd();
	m_cpRecoContext->SetNotifyWindowMessage(hWnd, WM_APP + 101, 0, 0);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTexttoSpeechDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTexttoSpeechDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTexttoSpeechDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


CString GetIniFilePath()
{
	TCHAR szPath[MAX_PATH];
	GetModuleFileName(NULL, szPath, MAX_PATH);

	CString iniPath = szPath;
	int nIndex = iniPath.ReverseFind(_T('\\'));
	if (nIndex != -1)
	{
		iniPath = iniPath.Left(nIndex + 1);
	}
	iniPath += _T("text.ini");
	return iniPath;
}

CString getTextFromIni()
{
	CString iniPath = GetIniFilePath();
	TCHAR szBuffer[1024];

	// GetPrivateProfileString(Section, Key, DefaultValue, OutputBuffer, BufferSize, FilePath)
	GetPrivateProfileString(
		_T("text"),       // Section name
		_T("text"),    // Key name
		_T(""),  // Default if key/file doesn't exist
		szBuffer,
		_countof(szBuffer),
		iniPath
	);

	//CString strLoadedText = szBuffer;
	//::SetDlgItemText(IDC_TEXT, strLoadedText);
	//::SetDlgItemText(m_hWnd, IDC_TEXT, strLoadedText);

	return szBuffer;
	
}

// Ensure this matches your header declaration exactly
LRESULT CTexttoSpeechDlg::OnSapiRecognition(WPARAM wParam, LPARAM lParam)
{
	SPEVENT event = { 0 };
	ULONG ulFetched = 0;

	// Pull events until the queue is empty
	while (m_cpRecoContext && m_cpRecoContext->GetEvents(1, &event, &ulFetched) == S_OK)
	{
		if (event.eEventId == SPEI_RECOGNITION)
		{
			// Note: split into modern Windows SDK variant format
			ISpRecoResult* pResult = reinterpret_cast<ISpRecoResult*>(event.lParam);

			WCHAR* pwszText = NULL;
			// Replace the previous text constant with SP_GETWHOLEPHRASE
			if (SUCCEEDED(pResult->GetText(SP_GETWHOLEPHRASE, SP_GETWHOLEPHRASE, TRUE, &pwszText, NULL)))
			{
				// Explicitly use the MFC dialog class version of GetDlgItem
				CEdit* pEdit = (CEdit*)this->GetDlgItem(IDC_SPEECH);
				if (pEdit != NULL)
				{
					CString strExisting;
					pEdit->GetWindowText(strExisting);
					strExisting += CString(pwszText) + L" ";
					pEdit->SetWindowText(strExisting);
				}

				::CoTaskMemFree(pwszText);
			}
		}
	}
	return 0;
}


void CTexttoSpeechDlg::OnBnClickedListen()
{
/**
	bool listening = false;   
	CString listenText;  

	
	if (listening == false) {
		listening = true;
		SetDlgItemText(IDC_LISTEN, _T("Listening"));
	} 
		
	if (listening == true){
		listening = false; 
		SetDlgItemText(IDC_LISTEN, _T("Listen"));
	}

	SetDlgItemText(IDC_SPEECH, listenText);
*/

	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_SPEECH);

	if (!m_bIsListening)
	{
		HRESULT hr = m_cpRecoContext->CreateGrammar(0, &m_cpRecoGrammar);
		if (SUCCEEDED(hr))
		{
			// Load a dictation grammar (free-form speech)
			hr = m_cpRecoGrammar->LoadDictation(L"DictationTopic", SPLO_DYNAMIC);
			if (SUCCEEDED(hr))
			{
				hr = m_cpRecoGrammar->SetDictationState(SPRS_ACTIVE);
				m_bIsListening = TRUE;
				GetDlgItem(IDC_LISTEN)->SetWindowTextW(L"Stop Listening");
			}
		}
	}
	else
	{
		if (m_cpRecoGrammar)
		{
			m_cpRecoGrammar->SetDictationState(SPRS_INACTIVE);
			m_cpRecoGrammar.Release();
		}
		m_bIsListening = FALSE;
		GetDlgItem(IDC_LISTEN)->SetWindowTextW(L"Listen");
	}


}


void CTexttoSpeechDlg::OnBnClickedPlay()
{

	CString strText = getTextFromIni();

	if (!strText.IsEmpty()) {
		SetDlgItemText(IDC_TEXT, strText);
	} 
	GetDlgItemText(IDC_TEXT, strText);
	if (strText.IsEmpty())
	{
		AfxMessageBox(_T("Please enter some text."));
		return;
	}

	CString iniPath = GetIniFilePath();
	WritePrivateProfileString(_T("text"), _T("text"), strText, iniPath);

	CComPtr<ISpVoice> cpVoice;
	HRESULT hr = cpVoice.CoCreateInstance(CLSID_SpVoice);

	// Diagnostic Alert 1: Did COM fail to create the object?
	if (FAILED(hr))
	{
		CString strErr;
		strErr.Format(_T("SAPI Initialization Failed! Error Code: 0x%08X. Did you call AfxOleInit()?"), hr);
		AfxMessageBox(strErr);
		return;
	}

	// Diagnostic Alert 2: Temporarily use SPF_DEFAULT (Blocks UI until finished speaking)
	// This forces SAPI to keep the pointer alive during playback.
	hr = cpVoice->Speak(strText.GetString(), SPF_DEFAULT, NULL);

	if (FAILED(hr))
	{
		CString strErr;
		strErr.Format(_T("Speak call failed! Error Code: 0x%08X"), hr);
		AfxMessageBox(strErr);
	}





}

void CTexttoSpeechDlg::OnBnClickedSave()
{


	CComPtr<ISpVoice> cpVoice;
	CComPtr<ISpStream> cpStream;
	HRESULT hr;

	hr = cpVoice.CoCreateInstance(CLSID_SpVoice);
	if (FAILED(hr)) return;

	hr = cpStream.CoCreateInstance(CLSID_SpStream);
	if (FAILED(hr)) return;

	// Set up standard WAV PCM structure manually
	WAVEFORMATEX wfx;
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = 2;                  // Stereo
	wfx.nSamplesPerSec = 44100;         // 44.1 kHz
	wfx.wBitsPerSample = 16;            // 16-bit
	wfx.nBlockAlign = (wfx.nChannels * wfx.wBitsPerSample) / 8;
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
	wfx.cbSize = 0;

	CString strFilePath = _T("output.wav");

	// Bind using the standard Win32 WAVEFORMATEX pointer and the SAPI WAV GUID
	hr = cpStream->BindToFile(
		strFilePath.GetString(),
		SPFM_CREATE_ALWAYS,
		&SPDFID_WaveFormatEx,          // SAPI GUID indicating standard WAVEFORMATEX
		&wfx,
		SPFEI_ALL_EVENTS
	);

	if (SUCCEEDED(hr))
	{
		hr = cpVoice->SetOutput(cpStream, TRUE);

		CString strText = getTextFromIni();

		if (!strText.IsEmpty()) {
			SetDlgItemText(IDC_TEXT, strText);
		}
		GetDlgItemText(IDC_TEXT, strText);

		// Optional: Check if the edit box is empty
		if (strText.IsEmpty())
		{
			AfxMessageBox(_T("Please enter some text to speak first."));
			return;
		}
		
		CString iniPath = GetIniFilePath();
		WritePrivateProfileString(_T("text"), _T("text"), strText, iniPath);

		if (SUCCEEDED(hr))
		{
			cpVoice->Speak(strText.GetString(), SPF_DEFAULT, NULL);
			AfxMessageBox(_T("Audio successfully saved to WAV file!"));
		}
	}


}


