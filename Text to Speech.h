
// Text to Speech.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CTexttoSpeechApp:
// See Text to Speech.cpp for the implementation of this class
//

class CTexttoSpeechApp : public CWinApp
{
public:
	CTexttoSpeechApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CTexttoSpeechApp theApp;
