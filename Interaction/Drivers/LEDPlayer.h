#pragma once


#include "stdafx.h"

#include "usbhidioc.h"

#include <wtypes.h>
#include <initguid.h>
#include <iostream>

#define MAX_LOADSTRING 256

extern "C" {

// This file is in the Windows DDK available from Microsoft.
#include "hidsdi.h"

#include <setupapi.h>
#include <dbt.h>
}

#include "Wiimote.h"
#include "afxcmn.h"
#include "afxwin.h"

using namespace std;

class CLEDPlayer;

// CLEDPlayer dialog

class CWiiStatic : public CStatic
{
public:
	CLEDPlayer * m_parent;

	CWiiStatic(CLEDPlayer * parent) : on(true), m_parent(parent) {}

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnPaint();

	CBitmap m_wiimote_image_bitmap;
	Position p;
	int timer;
	bool on;
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};

class CLEDPlayer : public CDialog
{
	DECLARE_DYNAMIC(CLEDPlayer)

	vector<Wiimote> m_wiimotes;

	bool HasPlayer(int index) { return index < m_wiimotes.size(); }
	Wiimote & GetPlayer(int index) { return m_wiimotes[index]; }

	void SetLEDs(Wiimote & device, LED::LEDSet & set);

public:
	CLEDPlayer(CWnd* pParent = NULL);   // standard constructor
	virtual ~CLEDPlayer();

	virtual BOOL OnInitDialog();

// Dialog Data
	enum { IDD = IDD_LEDPLAYER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	CRichEditCtrl m_led_1;
	CRichEditCtrl m_led_2;
	CRichEditCtrl m_led_3;
	CRichEditCtrl m_led_4;
public:
	afx_msg void OnBnClickedPlay();
public:
	afx_msg void OnClose();
public:
	afx_msg void OnBnClickedOk();
public:
	afx_msg void OnBnClickedStop();
public:
	CWiiStatic m_wiimote_image;
public:
	afx_msg void OnBnClickedGetPositionButton();
};
