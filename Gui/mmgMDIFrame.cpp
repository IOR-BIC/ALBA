/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgMDIFrame.cpp,v $
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
#include "mmgMDIFrame.h"


#include "mafDecl.h"
#include "mafEvent.h"

//#include "vtkVersion.h"

//----------------------------------------------------------------------------
// mmgMDIFrame
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(mmgMDIFrame, wxMDIParentFrame)
    EVT_CLOSE(mmgMDIFrame::OnCloseWindow)
    EVT_MENU_RANGE(MENU_START,MENU_END,mmgMDIFrame::OnMenu)
    EVT_MENU_RANGE(wxID_FILE1,wxID_FILE4,mmgMDIFrame::OnMenu)
		EVT_MENU_RANGE(OP_START,OP_END,mmgMDIFrame::OnMenuOp)
    EVT_MENU_RANGE(VIEW_START,VIEW_END,mmgMDIFrame::OnMenuView)
		EVT_MENU_RANGE(wxID_FILE1, wxID_FILE9, mmgMDIFrame::OnMenu)
    EVT_UPDATE_UI_RANGE(MENU_START,MENU_END,mmgMDIFrame::OnUpdateUI)
    EVT_UPDATE_UI_RANGE(OP_START,OP_END,mmgMDIFrame::OnUpdateUI)
    EVT_SASH_DRAGGED_RANGE(SASH_START, SASH_END, mmgMDIFrame::OnSashDrag)
    EVT_BUTTON (ID_LAYOUT, mmgMDIFrame::OnLayout)
    EVT_SIZE(mmgMDIFrame::OnSize)
		EVT_IDLE(mmgMDIFrame::OnIdle)  
END_EVENT_TABLE()

/*
//----------------------------------------------------------------------------
//struct mafProgressArgs
//----------------------------------------------------------------------------
struct mafProgressArgs {
 vtkProcessObject* f;   //filter
 wxString m;            //message 
 mmgMDIFrame* t;        //this  
 bool ren;              //ProgressArg of a Renderer  
};
*/
//----------------------------------------------------------------------------
mmgMDIFrame::mmgMDIFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
//----------------------------------------------------------------------------
: wxMDIParentFrame((wxFrame *)NULL, -1, title, pos, size, wxDEFAULT_FRAME_STYLE|wxHSCROLL|wxVSCROLL)
{
  mafSetFrame( this );
  m_Listener = NULL;
  m_clientwin= NULL;
  CreateStatusbar();
  Centre();
}
//----------------------------------------------------------------------------
mmgMDIFrame::~mmgMDIFrame( ) 
//----------------------------------------------------------------------------
{
  mafSetFrame( NULL);
}
//----------------------------------------------------------------------------
void mmgMDIFrame::OnMenu(wxCommandEvent& e)
//----------------------------------------------------------------------------
{ 
	mafEventMacro(mafEvent(this,e.GetId()));
}
//----------------------------------------------------------------------------
void mmgMDIFrame::OnMenuOp(wxCommandEvent& e)
//----------------------------------------------------------------------------
{ 
  mafEventMacro(mafEvent(this,MENU_OP,(long)e.GetId()));
}
//----------------------------------------------------------------------------
void mmgMDIFrame::OnMenuView(wxCommandEvent& e)
//----------------------------------------------------------------------------
{ 
  mafEventMacro(mafEvent(this,VIEW_CREATE,(long)e.GetId()));
}
//----------------------------------------------------------------------------
void mmgMDIFrame::OnUpdateUI(wxUpdateUIEvent& e)
//----------------------------------------------------------------------------
{ 
	mafEventMacro(mafEvent(this,UPDATE_UI,&e));
}
//----------------------------------------------------------------------------
void mmgMDIFrame::OnCloseWindow(wxCloseEvent& event)
//----------------------------------------------------------------------------
{ 
  mafEventMacro(mafEvent(this,MENU_FILE_QUIT));
}

//----------------------------------------------------------------------------
void mmgMDIFrame::OnSashDrag(wxSashEvent& event)
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
 
	if(event.GetDragStatus() == wxSASH_STATUS_OUT_OF_RANGE)
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
void mmgMDIFrame::OnSize(wxSizeEvent& event)
//----------------------------------------------------------------------------
{ 
	LayoutWindow();	
	Refresh(false);

  if(m_frameStatusBar==NULL) 
		return;
  wxRect r;
	m_frameStatusBar->GetFieldRect(4,r);
  m_gauge->SetSize(r.x,r.y,r.width,r.height);
}
//----------------------------------------------------------------------------
void mmgMDIFrame::LayoutWindow()
//----------------------------------------------------------------------------
{
	Refresh();
	if (GetClientWindow())
	{
		wxLayoutAlgorithm layout;
		layout.LayoutWindow(this,GetClientWindow());
		Refresh(false);
	}
}
//----------------------------------------------------------------------------
void mmgMDIFrame::Put( wxWindow* w)
//----------------------------------------------------------------------------
{
  if(m_clientwin) m_clientwin->Show(false);
  m_clientwin = w;
  m_clientwin->Reparent(this);
  m_clientwin->Show(true);
  LayoutWindow();
}
//----------------------------------------------------------------------------
void mmgMDIFrame::CreateStatusbar ()
//----------------------------------------------------------------------------
{
  static const int widths[]={-1,60,60,60,210,100}; // enlarged the last tab for the "free mem watch" information
	CreateStatusBar(6); 
  m_frameStatusBar->SetStatusWidths(6,widths);
	SetStatusText( "Welcome",0);
	SetStatusText( " ",1);
	SetStatusText( " ",2);
	SetStatusText( " ",3);

	m_busy=FALSE;
	m_gauge = new wxGauge(m_frameStatusBar, -1, 100);
	m_gauge->SetForegroundColour( *wxRED );
  m_gauge->Show(FALSE);
}
//----------------------------------------------------------------------------
void mmgMDIFrame::OnLayout(wxCommandEvent& event)
//----------------------------------------------------------------------------
{ 
  LayoutWindow();
}
//----------------------------------------------------------------------------
void mmgMDIFrame::OnIdle(wxIdleEvent& event)
//----------------------------------------------------------------------------
{ 
  /* @@@   //SIL. 29-3-2005: 
  #ifdef __WIN32__
	MEMORYSTATUS ms;
	GlobalMemoryStatus( &ms );
  wxString s;
  s << "free mem " << ms.dwAvailPhys/1000000 << " mb";   
  SetStatusText(s,5);
	#endif
  */
}
/*
//-----------------------------------------------------------
void mmgMDIFrame::BindToProgressBar(vtkObject* vtkobj, wxString *msg)
//-----------------------------------------------------------
{
  // New methods to handle the ProgressBar
  vtkViewport *v = NULL;
  vtkProcessObject *p = NULL;

	if(vtkobj->IsA("vtkViewport")) 
	{
		v = (vtkViewport*)vtkobj;
		BindToProgressBar(v,msg);
  } 

  if(vtkobj->IsA("vtkProcessObject")) 
	{
		p = (vtkProcessObject*)vtkobj;
		BindToProgressBar(p,msg);
  } 
	
	if(v == NULL && p == NULL)
	{
    wxLogMessage("wrong vtkObject passed to BindToProgressBar");
	}
}
//-----------------------------------------------------------
void mmgMDIFrame::BindToProgressBar(vtkProcessObject* filter, wxString  *msg)
//-----------------------------------------------------------
{
  mafProgressArgs *args = new mafProgressArgs;
  args->f = filter;
	args->m = *msg;
	args->t = this;
	args->ren = false;
	
  filter->SetStartMethod(	   this->ProgressStart, (void*)args);
  filter->SetProgressMethod( this->ProgressUpdate,(void*)args);
  filter->SetEndMethod(	     this->ProgressEnd,   (void*)args);

  // link only one ArgDelete - the object arg is only one !!
  filter->SetEndMethodArgDelete(this->ProgressDeleteArgs);
}
//-----------------------------------------------------------
void mmgMDIFrame::BindToProgressBar(vtkViewport* ren, wxString  *msg)
//-----------------------------------------------------------
{
  ren->SetStartRenderMethod(	this->RenderStart, this);
  ren->SetEndRenderMethod(	this->RenderEnd,   this);
}
//-----------------------------------------------------------
void mmgMDIFrame::ProgressStart(void* a)
//-----------------------------------------------------------
{
  mafProgressArgs *args = (mafProgressArgs *)a;
  args->t->Busy();
  args->t->SetStatusText(args->m,0);
}
//-----------------------------------------------------------
void mmgMDIFrame::ProgressUpdate(void* a)
//-----------------------------------------------------------
{
  mafProgressArgs *args = (mafProgressArgs *)a;
  int prg = (int)(args->f->GetProgress()*100);
  args->t->m_gauge->SetValue(prg);
  args->t->SetStatusText(wxString::Format(" %d%% ",prg),3);
}
//-----------------------------------------------------------
void mmgMDIFrame::ProgressEnd(void* a)
//-----------------------------------------------------------
{
  mafProgressArgs *args = (mafProgressArgs *)a;
  args->t->SetStatusText( " ",0);
  args->t->SetStatusText( " ",2);
  args->t->SetStatusText( " ",3);
  args->t->Ready();
}
//-----------------------------------------------------------
void mmgMDIFrame::ProgressDeleteArgs(void* a)
//-----------------------------------------------------------
{
  mafProgressArgs *args = (mafProgressArgs *)a;
  wxDEL(args);
}
*/
//-----------------------------------------------------------
void mmgMDIFrame::RenderStart(void* a)
//-----------------------------------------------------------
{
  ((mmgMDIFrame*)a)->SetStatusText( "rendering",1);
}
//-----------------------------------------------------------
void mmgMDIFrame::RenderEnd(void* a)
//-----------------------------------------------------------
{
  ((mmgMDIFrame*)a)->SetStatusText( " ",1);
}
//-----------------------------------------------------------
void mmgMDIFrame::Busy()
//-----------------------------------------------------------
{
  SetStatusText("Busy",2);
  SetStatusText("",3);
  m_gauge->Show(TRUE);
  m_gauge->SetValue(0);
  Refresh(FALSE);
}
//-----------------------------------------------------------
void mmgMDIFrame::Ready()
//-----------------------------------------------------------
{
  SetStatusText("",2);
  SetStatusText("",3);
  m_gauge->Show(FALSE);
  Refresh(FALSE);
}
//-----------------------------------------------------------
void mmgMDIFrame::ProgressBarShow()
//-----------------------------------------------------------
{
  SetStatusText("",0);
  Busy();
}
//-----------------------------------------------------------
void mmgMDIFrame::ProgressBarHide()
//-----------------------------------------------------------
{
  SetStatusText("",0);
  Ready();
}
//-----------------------------------------------------------
void mmgMDIFrame::ProgressBarSetVal(int progress)
//-----------------------------------------------------------
{
  m_gauge->SetValue(progress);
  SetStatusText(wxString::Format(" %d%% ",progress),3);
}
//-----------------------------------------------------------
void mmgMDIFrame::ProgressBarSetText(wxString *msg)
//-----------------------------------------------------------
{
  if(msg) SetStatusText(*msg,0);
}
