/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgMDIFrame.cpp,v $
  Language:  C++
  Date:      $Date: 2006-06-14 14:46:33 $
  Version:   $Revision: 1.15 $
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
#include "mmgDockSettings.h"

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
		EVT_MENU_RANGE(OP_START,OP_END,mmgMDIFrame::OnMenuOp)
    EVT_MENU_RANGE(VIEW_START,VIEW_END,mmgMDIFrame::OnMenuView)
		EVT_MENU_RANGE(wxID_FILE1, wxID_FILE9, mmgMDIFrame::OnMenu)
    EVT_UPDATE_UI_RANGE(MENU_START,MENU_END,mmgMDIFrame::OnUpdateUI)
    EVT_UPDATE_UI_RANGE(OP_START,OP_END,mmgMDIFrame::OnUpdateUI)
    EVT_SIZE(mmgMDIFrame::OnSize) //SIL. 23-may-2006 : 

    EVT_IDLE(mmgMDIFrame::OnIdle)  
END_EVENT_TABLE()

//----------------------------------------------------------------------------
mmgMDIFrame::mmgMDIFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
//----------------------------------------------------------------------------
: wxMDIParentFrame((wxFrame *)NULL, -1, title, pos, size, wxDEFAULT_FRAME_STYLE|wxHSCROLL|wxVSCROLL)
{
  mafSetFrame( this );

  this->SetMinSize( wxSize(600,500)); // m_DockManager cant handle correctly the frame MinSize (yet)
  m_DockManager.SetFrame(this);
  m_DockManager.GetArtProvider()->SetMetric(wxAUI_ART_PANE_BORDER_SIZE,0 );
  m_DockManager.GetArtProvider()->SetColor(wxAUI_ART_INACTIVE_CAPTION_COLOUR, m_DockManager.GetArtProvider()->GetColor(wxAUI_ART_ACTIVE_CAPTION_COLOUR));
  m_DockManager.GetArtProvider()->SetColor(wxAUI_ART_INACTIVE_CAPTION_GRADIENT_COLOUR, m_DockManager.GetArtProvider()->GetColor(wxAUI_ART_ACTIVE_CAPTION_GRADIENT_COLOUR));
  m_DockManager.GetArtProvider()->SetColor(wxAUI_ART_INACTIVE_CAPTION_TEXT_COLOUR, m_DockManager.GetArtProvider()->GetColor(wxAUI_ART_ACTIVE_CAPTION_TEXT_COLOUR));
  m_DockManager.Update();
  m_DockSettings = new mmgDockSettings(m_DockManager);

  m_Listener = NULL;
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

  //m_DockManager.UnInit(); - must be done in OnQuit
  cppDEL(m_DockSettings);

  mafSetFrame(NULL);
}
//----------------------------------------------------------------------------
void mmgMDIFrame::OnMenu(wxCommandEvent& e)
//----------------------------------------------------------------------------
{ 
	 //SIL. 30-may-2006 : to be cleaned
	 //cosi non va -- ho l'ID del Dock ma mi serve il puntatore 
	
	//if(e.GetId() > SASH_START && e.GetId() < SASH_END )
	//  ShowDockPane(e.GetId(), !DockPaneIsShown( (wxWindow*)(e.GetEventObject())) );
	// else
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
void mmgMDIFrame::OnSize(wxSizeEvent& event)
//----------------------------------------------------------------------------
{ 
  if(m_frameStatusBar) 
  {
    wxRect r;
    m_frameStatusBar->GetFieldRect(4,r);
    m_Gauge->SetSize(r.x,r.y+2,r.width -4 ,r.height -4);
  }
  event.Skip();
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
void mmgMDIFrame::CreateStatusbar ()
//----------------------------------------------------------------------------
{
  static const int widths[]={-1,60,60,60,210,100}; // enlarged the last tab for the "free mem watch" information
	CreateStatusBar(6); 
  m_frameStatusBar->SetStatusWidths(6,widths);
	SetStatusText( _("welcome"),0);
	SetStatusText( " ",1);
	SetStatusText( " ",2);
	SetStatusText( " ",3);

	m_Busy=FALSE;
	m_Gauge = new wxGauge(m_frameStatusBar, -1, 100,wxDefaultPosition,wxDefaultSize,wxGA_SMOOTH);
	m_Gauge->SetForegroundColour( *wxRED );
  m_Gauge->Show(FALSE);
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
  m_Gauge->Show(TRUE);
  m_Gauge->SetValue(0);
  Refresh(FALSE);
  }
//-----------------------------------------------------------
void mmgMDIFrame::Ready()
//-----------------------------------------------------------
  {
  SetStatusText("",2);
  SetStatusText("",3);
  m_Gauge->Show(FALSE);
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
  m_Gauge->SetValue(progress);
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
//-----------------------------------------------------------
void mmgMDIFrame::BindToProgressBar(vtkProcessObject* filter)
//-----------------------------------------------------------
{
  filter->AddObserver(vtkCommand::ProgressEvent,m_ProgressCallback);
  filter->AddObserver(vtkCommand::StartEvent,m_StartCallback);
  filter->AddObserver(vtkCommand::EndEvent,m_EndCallback);
}
//-----------------------------------------------------------
void mmgMDIFrame::BindToProgressBar(vtkViewport* ren)
//-----------------------------------------------------------
{
  ren->AddObserver(vtkCommand::StartEvent,m_StartCallback);
  ren->AddObserver(vtkCommand::EndEvent,m_EndCallback);
}
//-----------------------------------------------------------
void mmgMDIFrame::OnQuit()
//-----------------------------------------------------------
{
  m_DockManager.UnInit();
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#endif //MAF_USE_VTK
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

/*
//-----------------------------------------------------------
void mmgMDIFrame::ShowDockSettings()
//-----------------------------------------------------------
{
  m_DockSettings->ShowModal();
}
*/
//-----------------------------------------------------------
void mmgMDIFrame::AddDockPane(wxWindow *window, wxPaneInfo& pane_info)
//-----------------------------------------------------------
{
  m_DockManager.AddPane(window,pane_info);
  // devo creare la voce di menu ?  --- non adesso, e' gia nel DockManager
  // magari si - ma poi mi devo anche rispondere
  m_DockManager.Update();
}
//-----------------------------------------------------------
void mmgMDIFrame::RemoveDockPane(wxString pane_name)
//-----------------------------------------------------------
{
  wxPaneInfo& pi = m_DockManager.GetPane(pane_name);
  if(pi.IsOk())
  {
    m_DockManager.DetachPane(pi.window);
    m_DockManager.Update();
  }
}
//-----------------------------------------------------------
void mmgMDIFrame::ShowDockPane(wxString pane_name, bool show)
//-----------------------------------------------------------
{
  wxPaneInfo& pi = m_DockManager.GetPane(pane_name);
  if(pi.IsOk())
  {
    pi.Show( show );
    m_DockManager.Update();
  }
}
//-----------------------------------------------------------
void mmgMDIFrame::ShowDockPane(wxWindow *window, bool show)
//-----------------------------------------------------------
{
  wxPaneInfo& pi = m_DockManager.GetPane(window);
  if(pi.IsOk())
  {
    pi.Show( show );
    m_DockManager.Update(); // sync the MenuItems check 
  }
}
//-----------------------------------------------------------
bool mmgMDIFrame::DockPaneIsShown(wxString pane_name)
//-----------------------------------------------------------
{
  wxPaneInfo& pi = m_DockManager.GetPane(pane_name);
  if(pi.IsOk())
    return pi.IsShown();
  return false;
}
//-----------------------------------------------------------
bool mmgMDIFrame::DockPaneIsShown(wxWindow *window)
//-----------------------------------------------------------
{
  wxPaneInfo& pi = m_DockManager.GetPane(window);
  if(pi.IsOk())
    return pi.IsShown();
  return false;
}


