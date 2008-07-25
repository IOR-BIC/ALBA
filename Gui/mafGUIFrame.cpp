/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUIFrame.cpp,v $
  Language:  C++
  Date:      $Date: 2008-07-25 06:53:38 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "mafGUIFrame.h"
#include "mafDecl.h"
#include "mafEvent.h"

static wxWindow* TheFrame = NULL;
//----------------------------------------------------------------------------
wxWindow* mafGetFrame()
//----------------------------------------------------------------------------
{
  return TheFrame;
}
//----------------------------------------------------------------------------
void mafSetFrame(wxWindow* frame)
//----------------------------------------------------------------------------
{
  TheFrame = frame;
}

//----------------------------------------------------------------------------
// mafGUIFrame
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(mafGUIFrame, wxFrame)
    EVT_CLOSE(mafGUIFrame::OnCloseWindow)
    EVT_MENU_RANGE(MENU_START,MENU_END,mafGUIFrame::OnMenu)
    EVT_MENU_RANGE(wxID_FILE1,wxID_FILE9,mafGUIFrame::OnMenu)
    EVT_MENU_RANGE(OP_START,OP_END,mafGUIFrame::OnMenuOp)
    EVT_UPDATE_UI_RANGE(MENU_START,MENU_END,mafGUIFrame::OnUpdateUI)
    EVT_UPDATE_UI_RANGE(OP_START,OP_END,mafGUIFrame::OnUpdateUI)
    EVT_SASH_DRAGGED_RANGE(SASH_START, SASH_END, mafGUIFrame::OnSashDrag)
    EVT_BUTTON (ID_LAYOUT, mafGUIFrame::OnLayout)
    EVT_SIZE(mafGUIFrame::OnSize)
END_EVENT_TABLE()

//----------------------------------------------------------------------------
mafGUIFrame::mafGUIFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
//----------------------------------------------------------------------------
: wxFrame((wxFrame *)NULL, -1, title, pos, size, wxDEFAULT_FRAME_STYLE|wxCLIP_CHILDREN)
{
  mafSetFrame( this );
  m_Listener = NULL;
  m_ClientWin= NULL;
  CreateStatusbar();
  Centre();
}
//----------------------------------------------------------------------------
mafGUIFrame::~mafGUIFrame( ) 
//----------------------------------------------------------------------------
{
  mafSetFrame( NULL );
}
//----------------------------------------------------------------------------
void mafGUIFrame::OnMenu(wxCommandEvent& e)
//----------------------------------------------------------------------------
{ 
  mafEventMacro(mafEvent(this,e.GetId()));
}
//----------------------------------------------------------------------------
void mafGUIFrame::OnMenuOp(wxCommandEvent& e)
//----------------------------------------------------------------------------
{ 
  mafEventMacro(mafEvent(this,MENU_OP,(long)e.GetId()));
}
//----------------------------------------------------------------------------
void mafGUIFrame::OnUpdateUI(wxUpdateUIEvent& e)
//----------------------------------------------------------------------------
{ 
  mafEventMacro(mafEvent(this,UPDATE_UI,&e));
}
//----------------------------------------------------------------------------
void mafGUIFrame::OnCloseWindow(wxCloseEvent& event)
//----------------------------------------------------------------------------
{ 
  mafEventMacro(mafEvent(this,MENU_FILE_QUIT));
}
//----------------------------------------------------------------------------
void mafGUIFrame::OnSashDrag(wxSashEvent& event)
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
void mafGUIFrame::OnSize(wxSizeEvent& event)
//----------------------------------------------------------------------------
{ 
  LayoutWindow();
 	Refresh(false);

}
//----------------------------------------------------------------------------
void mafGUIFrame::LayoutWindow()
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
void mafGUIFrame::Put( wxWindow* w)
//----------------------------------------------------------------------------
{
  if(m_ClientWin) m_ClientWin->Show(false);
  m_ClientWin = w;
  m_ClientWin->Reparent(this);
  m_ClientWin->Show(true);
  LayoutWindow();
}
//----------------------------------------------------------------------------
void mafGUIFrame::CreateStatusbar ()
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
void mafGUIFrame::OnLayout(wxCommandEvent& event)
//----------------------------------------------------------------------------
{ 
  LayoutWindow();
}
