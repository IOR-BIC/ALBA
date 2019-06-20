/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIFrame
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "albaGUIFrame.h"
#include "albaDecl.h"
#include "albaEvent.h"

static wxWindow* TheFrame = NULL;
//----------------------------------------------------------------------------
wxWindow* albaGetFrame()
//----------------------------------------------------------------------------
{
  return TheFrame;
}
//----------------------------------------------------------------------------
void albaSetFrame(wxWindow* frame)
//----------------------------------------------------------------------------
{
  TheFrame = frame;
}

//----------------------------------------------------------------------------
// albaGUIFrame
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(albaGUIFrame, wxFrame)
    EVT_CLOSE(albaGUIFrame::OnCloseWindow)
    EVT_MENU_RANGE(MENU_START,MENU_END,albaGUIFrame::OnMenu)
    EVT_MENU_RANGE(wxID_FILE1,wxID_FILE9,albaGUIFrame::OnMenu)
    EVT_MENU_RANGE(OP_START,OP_END,albaGUIFrame::OnMenuOp)
    EVT_UPDATE_UI_RANGE(MENU_START,MENU_END,albaGUIFrame::OnUpdateUI)
    EVT_UPDATE_UI_RANGE(OP_START,OP_END,albaGUIFrame::OnUpdateUI)
    EVT_SASH_DRAGGED_RANGE(SASH_START, SASH_END, albaGUIFrame::OnSashDrag)
    EVT_BUTTON (ID_LAYOUT, albaGUIFrame::OnLayout)
    EVT_SIZE(albaGUIFrame::OnSize)
END_EVENT_TABLE()

//----------------------------------------------------------------------------
albaGUIFrame::albaGUIFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
//----------------------------------------------------------------------------
: wxFrame((wxFrame *)NULL, -1, title, pos, size, wxDEFAULT_FRAME_STYLE|wxCLIP_CHILDREN)
{
  albaSetFrame( this );
  m_Listener = NULL;
  m_ClientWin= NULL;
  CreateStatusbar();
  Centre();
}
//----------------------------------------------------------------------------
albaGUIFrame::~albaGUIFrame( ) 
//----------------------------------------------------------------------------
{
  albaSetFrame( NULL );
}
//----------------------------------------------------------------------------
void albaGUIFrame::OnMenu(wxCommandEvent& e)
//----------------------------------------------------------------------------
{ 
  albaEventMacro(albaEvent(this,e.GetId()));
}
//----------------------------------------------------------------------------
void albaGUIFrame::OnMenuOp(wxCommandEvent& e)
//----------------------------------------------------------------------------
{ 
  albaEventMacro(albaEvent(this,MENU_OP,(long)e.GetId()));
}
//----------------------------------------------------------------------------
void albaGUIFrame::OnUpdateUI(wxUpdateUIEvent& e)
//----------------------------------------------------------------------------
{ 
  albaEventMacro(albaEvent(this,UPDATE_UI,&e));
}
//----------------------------------------------------------------------------
void albaGUIFrame::OnCloseWindow(wxCloseEvent& event)
//----------------------------------------------------------------------------
{ 
  albaEventMacro(albaEvent(this,MENU_FILE_QUIT));
}
//----------------------------------------------------------------------------
void albaGUIFrame::OnSashDrag(wxSashEvent& event)
//----------------------------------------------------------------------------
{
	/*
	Seems that there is no way to handle sash-messages from
	the guiSashPanel itself. So every window using Sashes
	have to implement OnSashDrag like this.
	You can avoid to use specific sash names here, but you have 
	still to be aware of the existing guiSashPanel's to 
	write the entry in the Event-Table.
	*/
	
	if (event.GetDragStatus() == wxSASH_STATUS_OUT_OF_RANGE)
		return;

	wxSashLayoutWindow *w = (wxSashLayoutWindow*) event.GetEventObject();

	switch( event.GetEdge() )
  {
    case wxSASH_TOP:
    case wxSASH_BOTTOM:
			w->SetDefaultSize(wxSize(1000,event.GetDragRect().height));
    break;
    case wxSASH_RIGHT:
    case wxSASH_LEFT:
			w->SetDefaultSize(wxSize(event.GetDragRect().width, 1000));
    break;
  }        
  LayoutWindow();
}
//----------------------------------------------------------------------------
void albaGUIFrame::OnSize(wxSizeEvent& event)
//----------------------------------------------------------------------------
{ 
  LayoutWindow();
 	Refresh(false);

}
//----------------------------------------------------------------------------
void albaGUIFrame::LayoutWindow()
//----------------------------------------------------------------------------
{
	Refresh();
	if(m_ClientWin)
	{
		wxLayoutAlgorithm layout;
		layout.LayoutWindow(this,m_ClientWin);
		Refresh(false);
	}
}
//----------------------------------------------------------------------------
void albaGUIFrame::Put( wxWindow* w)
//----------------------------------------------------------------------------
{
  if(m_ClientWin) m_ClientWin->Show(false);
  m_ClientWin = w;
  m_ClientWin->Reparent(this);
  m_ClientWin->Show(true);
  LayoutWindow();
}
//----------------------------------------------------------------------------
void albaGUIFrame::CreateStatusbar ()
//----------------------------------------------------------------------------
{
  static const int widths[]={-1,60,60,60,210,60};
	CreateStatusBar(6); 
        m_frameStatusBar->SetStatusWidths(6,widths);
	SetStatusText( "Welcome",0);
	SetStatusText( " ",1);
	SetStatusText( " ",2);
	SetStatusText( " ",3);
}
//----------------------------------------------------------------------------
void albaGUIFrame::OnLayout(wxCommandEvent& event)
//----------------------------------------------------------------------------
{ 
  LayoutWindow();
}
