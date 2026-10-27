
// Text to SpeechDlg.h : header file
//

#pragma once
#include <sapi.h>
#pragma warning(push)
#pragma warning(disable : 4996)
#include <sphelper.h>
#pragma warning(pop)
#include <atlbase.h>   // For CComPtr smart pointers


// CTexttoSpeechDlg dialog
class CTexttoSpeechDlg : public CDialogEx
{
// Construction
public:
	CTexttoSpeechDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TEXT_TO_SPEECH_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnSapiRecognition(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
private: 
	CComPtr<ISpRecoContext> m_cpRecoContext;
	CComPtr<ISpRecoGrammar> m_cpRecoGrammar;
	BOOL m_bIsListening;
public:
	afx_msg void OnBnClickedPlay();
	afx_msg void OnBnClickedSave();
	afx_msg void OnBnClickedListen();
	static LRESULT CALLBACK SapiNotificationProc(WPARAM wParam, LPARAM lParam, LPVOID pAppData);

};
