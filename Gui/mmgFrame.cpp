/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgFrame.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-01 09:01:23 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

//----------------------------------------------------------------------------
// Include: - include the class being defined first
//----------------------------------------------------------------------------
#include "mmgFrame.h"

#include "mafDecl.h"
#include "mafEvent.h"
//#include "vtkVersion.h"

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
// mmgFrame
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(mmgFrame, wxFrame)
    EVT_CLOSE(mmgFrame::OnCloseWindow)
    EVT_MENU_RANGE(MENU_START,MENU_END,mmgFrame::OnMenu)
    EVT_MENU_RANGE(wxID_FILE1,wxID_FILE4,mmgFrame::OnMenu)
    EVT_MENU_RANGE(OP_START,OP_END,mmgFrame::OnMenuOp)
    EVT_UPDATE_UI_RANGE(MENU_START,MENU_END,mmgFrame::OnUpdateUI)
    EVT_UPDATE_UI_RANGE(OP_START,OP_END,mmgFrame::OnUpdateUI)
    EVT_SASH_DRAGGED_RANGE(SASH_START, SASH_END, mmgFrame::OnSashDrag)
    EVT_BUTTON (ID_LAYOUT, mmgFrame::OnLayout)
    EVT_SIZE(mmgFrame::OnSize)
END_EVENT_TABLE()

//----------------------------------------------------------------------------
mmgFrame::mmgFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
//----------------------------------------------------------------------------
: wxFrame((wxFrame *)NULL, -1, title, pos, size, wxDEFAULT_FRAME_STYLE|wxCLIP_CHILDREN)
{
  mafSetFrame( this );
  m_Listener = NULL;
  m_clientwin= NULL;
  CreateStatusbar();
  Centre();
}
//----------------------------------------------------------------------------
mmgFrame::~mmgFrame( ) 
//----------------------------------------------------------------------------
{
  mafSetFrame( NULL );
}
//----------------------------------------------------------------------------
void mmgFrame::OnMenu(wxCommandEvent& e)
//----------------------------------------------------------------------------
{ 
  mafEventMacro(mafEvent(this,e.GetId()));
}
//----------------------------------------------------------------------------
void mmgFrame::OnMenuOp(wxCommandEvent& e)
//----------------------------------------------------------------------------
{ 
  mafEventMacro(mafEvent(this,MENU_OP,(long)e.GetId()));
}
//----------------------------------------------------------------------------
void mmgFrame::OnUpdateUI(wxUpdateUIEvent& e)
//----------------------------------------------------------------------------
{ 
  mafEventMacro(mafEvent(this,UPDATE_UI,&e));
}
//----------------------------------------------------------------------------
void mmgFrame::OnCloseWindow(wxCloseEvent& event)
//----------------------------------------------------------------------------
{ 
  mafEventMacro(mafEvent(this,MENU_FILE_QUIT));
}
//----------------------------------------------------------------------------
void mmgFrame::OnSashDrag(wxSashEvent& event)
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
void mmgFrame::OnSize(wxSizeEvent& event)
//----------------------------------------------------------------------------
{ 
  LayoutWindow();
 	Refresh(false);

}
//----------------------------------------------------------------------------
void mmgFrame::LayoutWindow()
//----------------------------------------------------------------------------
{
	Refresh();
	if(m_clientwin)
	{
		wxLayoutAlgorithm layout;
		layout.LayoutWindow(this,m_clientwin);
		Refresh(false);
	}
}
//----------------------------------------------------------------------------
void mmgFrame::Put( wxWindow* w)
//----------------------------------------------------------------------------
{
  if(m_clientwin) m_clientwin->Show(false);
  m_clientwin = w;
  m_clientwin->Reparent(this);
  m_clientwin->Show(true);
  LayoutWindow();
}
//----------------------------------------------------------------------------
void mmgFrame::CreateStatusbar ()
//----------------------------------------------------------------------------
{
  static const int widths[]={-1,60,60,60,210,60};
	CreateStatusBar(6); 
        m_frameStatusBar->SetStatusWidths(6,widths);
	SetStatusText( "Welcome",0);
	SetStatusText( " ",1);
	SetStatusText( " ",2);
	SetStatusText( " ",3);

  //vtkVersion *v = vtkVersion::New();
  //wxString s;
  //s << "vtk" << v->GetVTKVersion();
  //SetStatusText(s,5);
  //v->Delete();
}
//----------------------------------------------------------------------------
void mmgFrame::OnLayout(wxCommandEvent& event)
//----------------------------------------------------------------------------
{ 
  LayoutWindow();
}
