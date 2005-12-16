/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgMDIFrame.cpp,v $
  Language:  C++
  Date:      $Date: 2005-12-16 17:19:04 $
  Version:   $Revision: 1.10 $
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


#include "mmgMDIFrame.h"
#include <wx/icon.h>
#include "mafDecl.h"
#include "mafEvent.h"
#include "mafPics.h"

#ifdef MAF_USE_VTK //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::
  #include "vtkVersion.h"
  #include "vtkProcessObject.h"
  #include "vtkViewport.h"
  #include "vtkCommand.h"

class mmgMDIFrameCallback : public vtkCommand
{
  public:
    static mmgMDIFrameCallback *New() {return new mmgMDIFrameCallback;}
    mmgMDIFrameCallback() {m_mode=0; m_frame=NULL;};
    void SetMode(int mode){m_mode=mode;};
    void SetFrame(mmgMDIFrame *frame){m_frame=frame;};
    virtual void Execute(vtkObject *caller, unsigned long, void*)
    {
      assert(m_frame);
      if(caller->IsA("vtkProcessObject"))
      {
        vtkProcessObject *po = (vtkProcessObject*)caller;

        if(m_mode==0) // ProgressEvent-Callback
        {
          m_frame->ProgressBarSetVal(po->GetProgress()*100);
          //mafLogMessage("progress = %g", po->GetProgress()*100);
        }
        else if(m_mode==1) // StartEvent-Callback
        {
          m_frame->ProgressBarShow();
          m_frame->ProgressBarSetVal(0);
          m_frame->ProgressBarSetText(&wxString(po->GetClassName()));
        }
        else if(m_mode==2) // EndEvent-Callback
        {
          m_frame->ProgressBarHide();
        }
      } 
      else if(caller->IsA("vtkViewport"))
      {
        if(m_mode==1) // StartRenderingEvent-Callback
        {
          m_frame->RenderStart();
        }
        else if(m_mode==2) // StartRenderingEvent-Callback
        {
          m_frame->RenderEnd();
        }
      }
    }
  protected:  
    int m_mode;
    mmgMDIFrame *m_frame;
};
#endif //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//----------------------------------------------------------------------------
// mmgMDIFrame
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(mmgMDIFrame, wxMDIParentFrame)
    EVT_CLOSE(mmgMDIFrame::OnCloseWindow)
    EVT_DROP_FILES(mmgMDIFrame::OnDropFile)
    EVT_MENU_RANGE(MENU_START,MENU_END,mmgMDIFrame::OnMenu)
    //EVT_MENU_RANGE(wxID_FILE1,wxID_FILE4,mmgMDIFrame::OnMenu)
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

#ifdef __WIN32__
  ((wxWindow *)this)->DragAcceptFiles(TRUE);
#endif

  wxIconBundle ib;
  ib.AddIcon( mafPics.GetIcon("FRAME_ICON16x16") );
  ib.AddIcon( mafPics.GetIcon("FRAME_ICON32x32") );
  SetIcons(ib);

#ifdef MAF_USE_VTK
  m_ProgressCallback= mmgMDIFrameCallback::New(); 
  m_ProgressCallback->SetFrame(this);
  m_ProgressCallback->SetMode(0);
  m_StartCallback = mmgMDIFrameCallback::New();; 
  m_StartCallback->SetFrame(this);
  m_StartCallback->SetMode(1);
  m_EndCallback = mmgMDIFrameCallback::New(); 
  m_EndCallback->SetFrame(this);
  m_EndCallback->SetMode(2);
#endif //MAF_USE_VTK
}

//----------------------------------------------------------------------------
mmgMDIFrame::~mmgMDIFrame( ) 
//----------------------------------------------------------------------------
{
#ifdef MAF_USE_VTK
  vtkDEL(m_ProgressCallback); 
  vtkDEL(m_StartCallback); 
  vtkDEL(m_EndCallback); 
#endif 

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
	
	//Seems that there is no way to handle sash-messages from
	//the guiSashPanel itself. So every window using Sashes
	//have to implement OnSashDrag like this.
	//You can avoid to use specific sash names here, but you have 
	//still to be aware of the existing guiSashPanel's to 
	//write the entry in the Event-Table.
 
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
  m_gauge->SetSize(r.x,r.y+2,r.width -4 ,r.height -4);
}
//-----------------------------------------------------------
void mmgMDIFrame::OnDropFile(wxDropFilesEvent &event)
//-----------------------------------------------------------
{
  wxString path, name, ext, *file_list;
  int num_files = event.GetNumberOfFiles();
  file_list = event.GetFiles();
  for (int i=0; i< num_files; i++)
  {
    wxSplitPath(file_list[i],&path, &name, &ext);
    if (ext == "msf" || ext == "zmsf")
    {
      mafString file_to_open;
      file_to_open = file_list[i].c_str();
      mafEventMacro(mafEvent(this,MENU_FILE_OPEN,&file_to_open));
      return;
    }
  }
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
	m_gauge = new wxGauge(m_frameStatusBar, -1, 100,wxDefaultPosition,wxDefaultSize,wxGA_SMOOTH);
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
  // @@@   //SIL. 29-3-2005: 
  #ifdef __WIN32__
	MEMORYSTATUS ms;
	GlobalMemoryStatus( &ms );
  wxString s;
  s << "free mem " << ms.dwAvailPhys/1000000 << " mb";   
  SetStatusText(s,5);
	#endif
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
//-----------------------------------------------------------
void mmgMDIFrame::RenderStart()
//-----------------------------------------------------------
  {
    SetStatusText( "Rendering",1);
  }
//-----------------------------------------------------------
void mmgMDIFrame::RenderEnd()
//-----------------------------------------------------------
  {
    SetStatusText( " ",1);
  }


//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#ifdef MAF_USE_VTK
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//-----------------------------------------------------------
void mmgMDIFrame::BindToProgressBar(vtkObject* vtkobj)
//-----------------------------------------------------------
{
	if(vtkobj->IsA("vtkViewport")) 
		BindToProgressBar((vtkViewport*)vtkobj);
  else if(vtkobj->IsA("vtkProcessObject")) 
		BindToProgressBar((vtkProcessObject*)vtkobj);
	else 
    mafLogMessage("wrong vtkObject passed to BindToProgressBar");
}
/* --- used for vtk v.4.2
//----------------------------------------------------------------------------
//struct mafProgressArgs
//----------------------------------------------------------------------------
struct mafProgressArgs {
vtkProcessObject* f;   //filter
wxString m;            //message 
mmgMDIFrame* t;        //this  
bool ren;              //ProgressArg of a Renderer  
};
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
  cppDEL(args);
}
*/

//-----------------------------------------------------------
void mmgMDIFrame::BindToProgressBar(vtkProcessObject* filter)
//-----------------------------------------------------------
{
  // - syntax for vtk v.4.4
  filter->AddObserver(vtkCommand::ProgressEvent,m_ProgressCallback);
  filter->AddObserver(vtkCommand::StartEvent,m_StartCallback);
  filter->AddObserver(vtkCommand::EndEvent,m_EndCallback);

  // - syntax for vtk v.4.2 or less
  //mafProgressArgs *args = new mafProgressArgs;
  //args->f = filter;
  //args->m = *msg;
  //args->t = this;
  //args->ren = false;
  //filter->SetStartMethod(	   this->ProgressStart, (void*)args);
  //filter->SetProgressMethod( this->ProgressUpdate,(void*)args);
  //filter->SetEndMethod(	     this->ProgressEnd,   (void*)args);
  // link only one ArgDelete - the object arg is only one !!
  //filter->SetEndMethodArgDelete(this->ProgressDeleteArgs);
}
//-----------------------------------------------------------
void mmgMDIFrame::BindToProgressBar(vtkViewport* ren)
//-----------------------------------------------------------
{
  // - syntax for vtk v.4.4
  ren->AddObserver(vtkCommand::StartEvent,m_StartCallback);
  ren->AddObserver(vtkCommand::EndEvent,m_EndCallback);

  // - syntax for vtk v.4.2 or less
  //ren->SetStartRenderMethod(	this->RenderStart, this);
  //ren->SetEndRenderMethod(	this->RenderEnd,   this);
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#endif //MAF_USE_VTK
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
