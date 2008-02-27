// LEDPlayer.cpp : implementation file
//


#include "usbhidioc.h"
#include "LEDPlayer.h"

// CLEDPlayer dialog

IMPLEMENT_DYNAMIC(CLEDPlayer, CDialog)

CLEDPlayer::CLEDPlayer(CWnd* pParent /*=NULL*/)
: CDialog(CLEDPlayer::IDD, pParent), m_wiimote_image(this)
{

}

CLEDPlayer::~CLEDPlayer()
{
}

void CLEDPlayer::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_WIIMOTE_IMAGE, m_wiimote_image);
}

BOOL CLEDPlayer::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_wiimotes = Wiimote::ConnectToWiimotes();

	if (HasPlayer(0))
		SetWindowText("Player Connected");

	SetLEDs(GetPlayer(0), LED::LEDSet(1, 1, 1, 1));

	m_wiimote_image.m_wiimote_image_bitmap.LoadBitmapA(IDB_BITMAP1); 

	return TRUE;
}

void CLEDPlayer::SetLEDs(Wiimote & device, LED::LEDSet & set)
{
	device.SetLEDs(set);
}

BEGIN_MESSAGE_MAP(CLEDPlayer, CDialog)
	ON_BN_CLICKED(IDC_PLAY, &CLEDPlayer::OnBnClickedPlay)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDOK, &CLEDPlayer::OnBnClickedOk)
	ON_BN_CLICKED(IDC_STOP, &CLEDPlayer::OnBnClickedStop)
	ON_BN_CLICKED(IDC_GET_POSITION_BUTTON, &CLEDPlayer::OnBnClickedGetPositionButton)
END_MESSAGE_MAP()

void CLEDPlayer::OnBnClickedPlay()
{
	GetPlayer(0).StartListening();
}

void CLEDPlayer::OnClose()
{
	if (GetPlayer(0).IsListening())
		GetPlayer(0).StopListening();

	SetLEDs(GetPlayer(0), LED::LEDSet(0, 0, 0, 0));

	CDialog::OnClose();
}

void CLEDPlayer::OnBnClickedOk()
{
	OnClose();

	OnCancel();
}

void CLEDPlayer::OnBnClickedStop()
{
	GetPlayer(0).StopListening();
}
BEGIN_MESSAGE_MAP(CWiiStatic, CStatic)
	ON_WM_PAINT()
	ON_WM_TIMER()
END_MESSAGE_MAP()

void CWiiStatic::OnPaint()
{
	CPaintDC dc(this); 

    CDC BmpDc;
    VERIFY( BmpDc.CreateCompatibleDC(&dc) );
	BmpDc.SelectObject(&m_wiimote_image_bitmap);

	dc.BitBlt(0, 0, 122, 399, &BmpDc, 0, 0, SRCCOPY);

	if (on)
	{
		CBrush r;
		r.CreateSolidBrush(RGB(255, 0, 0));

		CBrush g;
		g.CreateSolidBrush(RGB(0, 255, 0));

		CBrush b;
		b.CreateSolidBrush(RGB(0, 0, 255));

		dc.SelectObject(r);
		dc.Rectangle(25, 128 + p.x, 35, 128 + p.x + 10);
		dc.SelectObject(g);
		dc.Rectangle(45, 128 + p.y, 55, 128 + p.y + 10);
		dc.SelectObject(b);
		dc.Rectangle(65, 128 + p.z, 75, 128 + p.z + 10);


	}
}

void CLEDPlayer::OnBnClickedGetPositionButton()
{
	GetPlayer(0).RequestMotionData();

	m_wiimote_image.SetTimer(77, 30, NULL);
}

void CWiiStatic::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 77)
	{
		p = m_parent->GetPlayer(0).GetLastMotionData();

		Invalidate();
	}

	CStatic::OnTimer(nIDEvent);
}
