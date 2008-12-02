/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUIMDIFrame.cpp,v $
  Language:  C++
  Date:      $Date: 2008-12-02 15:06:44 $
  Version:   $Revision: 1.1.2.1 $
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


#include "mafGUIMDIFrame.h"
#include <wx/icon.h>
#include "mafDecl.h"
#include "mafEvent.h"
#include "mafPics.h"
#include "mafGUIDockSettings.h"

#ifdef __WIN32__
#include <malloc.h>
#endif


#ifdef MAF_USE_VTK //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::
  #include "vtkVersion.h"
  #include "vtkProcessObject.h"
  #include "vtkViewport.h"
  #include "vtkCommand.h"

class mafGUIMDIFrameCallback : public vtkCommand
{
  public:
    static mafGUIMDIFrameCallback *New() {return new mafGUIMDIFrameCallback;}
    mafGUIMDIFrameCallback() {m_mode=0; m_Frame=NULL;};
    void SetMode(int mode){m_mode=mode;};
    void SetFrame(mafGUIMDIFrame *frame){m_Frame=frame;};
    virtual void Execute(vtkObject *caller, unsigned long, void*)
    {
      assert(m_Frame);
      if(caller->IsA("vtkProcessObject"))
      {
        vtkProcessObject *po = (vtkProcessObject*)caller;

        if(m_mode==0) // ProgressEvent-Callback
        {
          m_Frame->ProgressBarSetVal(po->GetProgress()*100);
          //mafLogMessage("progress = %g", po->GetProgress()*100);
        }
        else if(m_mode==1) // StartEvent-Callback
        {
          m_Frame->ProgressBarShow();
          m_Frame->ProgressBarSetVal(0);
          //m_Frame->ProgressBarSetText(&wxString(po->GetClassName()));
          m_Frame->ProgressBarSetText(&wxString(po->GetProgressText()));
        }
        else if(m_mode==2) // EndEvent-Callback
        {
          m_Frame->ProgressBarHide();
        }
      } 
      else if(caller->IsA("vtkViewport"))
      {
        if(m_mode==1) // StartRenderingEvent-Callback
        {
          m_Frame->RenderStart();
        }
        else if(m_mode==2) // StartRenderingEvent-Callback
        {
          m_Frame->RenderEnd();
        }
      }
    }
  protected:  
    int m_mode;
    mafGUIMDIFrame *m_Frame;
};
#endif //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//----------------------------------------------------------------------------
// mafGUIMDIFrame
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(mafGUIMDIFrame, wxMDIParentFrame)
    EVT_CLOSE(mafGUIMDIFrame::OnCloseWindow)
    EVT_DROP_FILES(mafGUIMDIFrame::OnDropFile)
    EVT_MENU_RANGE(MENU_START,MENU_END,mafGUIMDIFrame::OnMenu)
		EVT_MENU_RANGE(OP_START,OP_END,mafGUIMDIFrame::OnMenuOp)
    EVT_MENU_RANGE(VIEW_START,VIEW_END,mafGUIMDIFrame::OnMenuView)
		EVT_MENU_RANGE(wxID_FILE1, wxID_FILE9, mafGUIMDIFrame::OnMenu)
    EVT_UPDATE_UI_RANGE(MENU_START,MENU_END,mafGUIMDIFrame::OnUpdateUI)
    EVT_UPDATE_UI_RANGE(OP_START,OP_END,mafGUIMDIFrame::OnUpdateUI)
    EVT_SIZE(mafGUIMDIFrame::OnSize) //SIL. 23-may-2006 : 

    EVT_IDLE(mafGUIMDIFrame::OnIdle)  
END_EVENT_TABLE()

//----------------------------------------------------------------------------
mafGUIMDIFrame::mafGUIMDIFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
//----------------------------------------------------------------------------
: wxMDIParentFrame((wxFrame *)NULL, -1, title, pos, size, wxDEFAULT_FRAME_STYLE|wxHSCROLL|wxVSCROLL)
{
  mafSetFrame( this );

  this->SetMinSize(wxSize(600,500)); // m_DockManager cant handle correctly the frame MinSize (yet)
  m_DockManager.SetFrame(this);
  m_DockManager.GetArtProvider()->SetMetric(wxAUI_ART_PANE_BORDER_SIZE,0 );
  m_DockManager.GetArtProvider()->SetColor(wxAUI_ART_INACTIVE_CAPTION_COLOUR, m_DockManager.GetArtProvider()->GetColor(wxAUI_ART_ACTIVE_CAPTION_COLOUR));
  m_DockManager.GetArtProvider()->SetColor(wxAUI_ART_INACTIVE_CAPTION_GRADIENT_COLOUR, m_DockManager.GetArtProvider()->GetColor(wxAUI_ART_ACTIVE_CAPTION_GRADIENT_COLOUR));
  m_DockManager.GetArtProvider()->SetColor(wxAUI_ART_INACTIVE_CAPTION_TEXT_COLOUR, m_DockManager.GetArtProvider()->GetColor(wxAUI_ART_ACTIVE_CAPTION_TEXT_COLOUR));
  m_DockManager.Update();
  m_DockSettings = new mafGUIDockSettings(m_DockManager);

  m_MemoryLimitAlert = 50; // 50 MB is the default low limit to alert the user.
  m_UserAlerted = false;

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
  m_ProgressCallback= mafGUIMDIFrameCallback::New(); 
  m_ProgressCallback->SetFrame(this);
  m_ProgressCallback->SetMode(0);
  m_StartCallback = mafGUIMDIFrameCallback::New();; 
  m_StartCallback->SetFrame(this);
  m_StartCallback->SetMode(1);
  m_EndCallback = mafGUIMDIFrameCallback::New(); 
  m_EndCallback->SetFrame(this);
  m_EndCallback->SetMode(2);
#endif //MAF_USE_VTK
}

//----------------------------------------------------------------------------
mafGUIMDIFrame::~mafGUIMDIFrame()
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
void mafGUIMDIFrame::OnMenu(wxCommandEvent& e)
//----------------------------------------------------------------------------
{ 
	//if(e.GetId() > SASH_START && e.GetId() < SASH_END )
	//  ShowDockPane(e.GetId(), !DockPaneIsShown( (wxWindow*)(e.GetEventObject())) );
	// else
  mafEventMacro(mafEvent(this,e.GetId()));
}
//----------------------------------------------------------------------------
void mafGUIMDIFrame::OnMenuOp(wxCommandEvent& e)
//----------------------------------------------------------------------------
{ 
  mafEventMacro(mafEvent(this,MENU_OP,(long)e.GetId()));
}
//----------------------------------------------------------------------------
void mafGUIMDIFrame::OnMenuView(wxCommandEvent& e)
//----------------------------------------------------------------------------
{ 
  mafEventMacro(mafEvent(this,VIEW_CREATE,(long)e.GetId()));
}
//----------------------------------------------------------------------------
void mafGUIMDIFrame::OnUpdateUI(wxUpdateUIEvent& e)
//----------------------------------------------------------------------------
{ 
	mafEventMacro(mafEvent(this,UPDATE_UI,&e));
}
//----------------------------------------------------------------------------
void mafGUIMDIFrame::OnCloseWindow(wxCloseEvent& event)
//----------------------------------------------------------------------------
{ 
  mafEventMacro(mafEvent(this,MENU_FILE_QUIT));
}
//----------------------------------------------------------------------------
void mafGUIMDIFrame::OnSize(wxSizeEvent& event)
//----------------------------------------------------------------------------
{ 
  if(m_frameStatusBar) 
  {
    wxRect r;
    m_frameStatusBar->GetFieldRect(4,r);
    m_Gauge->SetPosition(r.GetPosition());
  }
  //event.Skip();
}
//-----------------------------------------------------------
void mafGUIMDIFrame::OnDropFile(wxDropFilesEvent &event)
//-----------------------------------------------------------
{
  wxString path, name, ext, *file_list;
  mafString file_to_open;

  int num_files = event.GetNumberOfFiles();
  file_list = event.GetFiles();
  for (int i=0; i< num_files; i++)
  {
    file_to_open = file_list[i].c_str();

    wxSplitPath(file_list[i],&path, &name, &ext);
    if (ext == "msf" || ext == "zmsf")
    {
      mafEventMacro(mafEvent(this,MENU_FILE_OPEN,&file_to_open));
      return;
    }
    else
    {
      mafEventMacro(mafEvent(this,IMPORT_FILE,&file_to_open));
    }
  }
}
//----------------------------------------------------------------------------
void mafGUIMDIFrame::CreateStatusbar()
//----------------------------------------------------------------------------
{
  static const int widths[]={-1,60,60,60,210,100}; // enlarged the last tab for the "free mem watch" information
	CreateStatusBar(6); 
  m_frameStatusBar->SetStatusWidths(6,widths);
	SetStatusText( _("welcome"),0);
	SetStatusText( " ",1);
	SetStatusText( " ",2);
	SetStatusText( " ",3);

  // Paolo 10 Jul 2006: due to position correctly the progress bar.
	wxRect pr;
  m_frameStatusBar->GetFieldRect(4,pr);
  m_Busy=FALSE;
	m_Gauge = new wxGauge(m_frameStatusBar, -1, 100,pr.GetPosition(),pr.GetSize(),wxGA_SMOOTH);
	m_Gauge->SetForegroundColour( *wxRED );
  m_Gauge->Show(FALSE);
}

#ifdef __WIN32__
/** 
Determines the total amount of free memory available for the current
application and the size of the largest free block (in bytes) */
size_t GetFreeMemorySize(size_t* pLargest)
{
  MEMORY_BASIC_INFORMATION ssMemInfo;
  memset(&ssMemInfo, 0, sizeof(ssMemInfo));
  ssMemInfo.BaseAddress = (LPVOID)0x10000;    //First 64 KB is reserved

  size_t nTotalMem = 0;
  size_t nLargestFree = 0;
  while (VirtualQuery(ssMemInfo.BaseAddress, &ssMemInfo, sizeof(ssMemInfo)) != 0)
  {    
    if (ssMemInfo.State == MEM_FREE)
    {
      nTotalMem += ssMemInfo.RegionSize;
      if (ssMemInfo.RegionSize > nLargestFree)
        nLargestFree = ssMemInfo.RegionSize;
    }

    ssMemInfo.BaseAddress = ((BYTE*)ssMemInfo.BaseAddress) + ssMemInfo.RegionSize;
    //VirtualQuery returns 0 if the passed address goes into the kernel region
    //which is on top addresses
  }

  //we may have also some free memory in the CRT heap
  _HEAPINFO hinfo;
  int heapstatus;  
  hinfo._pentry = NULL;

  while((heapstatus = _heapwalk(&hinfo)) == _HEAPOK)
  {
    if (hinfo._useflag != _USEDENTRY)
    {
      nTotalMem += hinfo._size;    
      if (hinfo._size > nLargestFree)
        nLargestFree = hinfo._size;      
    }
  }

  if (pLargest != NULL)
    *pLargest = nLargestFree;
  return nTotalMem;
}
#endif

//----------------------------------------------------------------------------
void mafGUIMDIFrame::OnIdle(wxIdleEvent& event)
//----------------------------------------------------------------------------
{ 
#ifdef __WIN32__

  //BES: 30.5.2008 - do not refresh the memory status all the time
  static DWORD dwLastTime = 0;
  DWORD dwCurTime = GetTickCount();
  if (dwCurTime - dwLastTime < 1000)
    return; //1 second is the minimal time

//BES: 30.5.2008 - GlobalMemoryStatus actually measures available free physical 
//memory in the whole system, it only tells you that your next memory operations
//may be slow as some data will have to be swapped on disk and vice versa.
//it is NOT related to the memory consumption of the running application
//	MEMORYSTATUS ms;
//	GlobalMemoryStatus( &ms );         

  size_t nLargest;
  int current_free_memory = (int)(GetFreeMemorySize(&nLargest) / (1024*1024));
    //ms.dwAvailPhys/1000000;

  static bool bShowTotal = false;
  if (bShowTotal)
  {
    //6 seconds for total free
    if ((dwCurTime - dwLastTime) >= 6000)
    {
      dwLastTime = dwCurTime;
      bShowTotal = false;
    }
  }
  else
  {
    //3 seconds for total free
    if ((dwCurTime - dwLastTime) >= 3000)
    {
      dwLastTime = dwCurTime;
      bShowTotal = true;
    }
  }

  wxString s;
  if (bShowTotal)
    s << "free mem: " << current_free_memory << " mb";
  else
    s << "largest: " << (int)(nLargest / (1024*1024)) << " mb";
  SetStatusText(s,5);  
  //GetStatusBar()->SetToolTip(s);
  if (current_free_memory < m_MemoryLimitAlert && !m_UserAlerted)
  {
    m_UserAlerted = true;
    int answere = wxMessageBox(_("Program is running with few free memory!! \nFree memory used by UnDo stack?."), _("Warning"), wxYES_NO);
    if (answere == wxYES)
    {
      // Clear UnDo stack to gain memory.
      mafEventMacro(mafEvent(this, CLEAR_UNDO_STACK));
    }
  }
	#endif
}
//-----------------------------------------------------------
void mafGUIMDIFrame::Busy()
//-----------------------------------------------------------
{
  SetStatusText("Busy",2);
  SetStatusText("",3);
  m_Gauge->Show(TRUE);
  m_Gauge->SetValue(0);
  Refresh(FALSE);
}
//-----------------------------------------------------------
void mafGUIMDIFrame::Ready()
//-----------------------------------------------------------
{
  SetStatusText("",2);
  SetStatusText("",3);
  m_Gauge->Show(FALSE);
  Refresh(FALSE);
}
//-----------------------------------------------------------
void mafGUIMDIFrame::ProgressBarShow()
//-----------------------------------------------------------
{
  SetStatusText("",0);
  Busy();
}
//-----------------------------------------------------------
void mafGUIMDIFrame::ProgressBarHide()
//-----------------------------------------------------------
{
  SetStatusText("",0);
  Ready();
}
//-----------------------------------------------------------
void mafGUIMDIFrame::ProgressBarSetVal(int progress)
//-----------------------------------------------------------
{
  m_Gauge->SetValue(progress);
  SetStatusText(wxString::Format(" %d%% ",progress),3);
}
//-----------------------------------------------------------
void mafGUIMDIFrame::ProgressBarSetText(wxString *msg)
//-----------------------------------------------------------
{
  if(msg) SetStatusText(*msg,0);
}
//-----------------------------------------------------------
void mafGUIMDIFrame::RenderStart()
//-----------------------------------------------------------
{
  SetStatusText( "Rendering",1);
}
//-----------------------------------------------------------
void mafGUIMDIFrame::RenderEnd()
//-----------------------------------------------------------
{
  SetStatusText( " ",1);
}
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#ifdef MAF_USE_VTK
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//-----------------------------------------------------------
void mafGUIMDIFrame::BindToProgressBar(vtkObject* vtkobj)
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
void mafGUIMDIFrame::BindToProgressBar(vtkProcessObject* filter)
//-----------------------------------------------------------
{
  filter->AddObserver(vtkCommand::ProgressEvent,m_ProgressCallback);
  filter->AddObserver(vtkCommand::StartEvent,m_StartCallback);
  filter->AddObserver(vtkCommand::EndEvent,m_EndCallback);
}
//-----------------------------------------------------------
void mafGUIMDIFrame::BindToProgressBar(vtkViewport* ren)
//-----------------------------------------------------------
{
  ren->AddObserver(vtkCommand::StartEvent,m_StartCallback);
  ren->AddObserver(vtkCommand::EndEvent,m_EndCallback);
}
//-----------------------------------------------------------
void mafGUIMDIFrame::OnQuit()
//-----------------------------------------------------------
{
  m_DockManager.UnInit();
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#endif //MAF_USE_VTK
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

/*
//-----------------------------------------------------------
void mafGUIMDIFrame::ShowDockSettings()
//-----------------------------------------------------------
{
  m_DockSettings->ShowModal();
}
*/
//-----------------------------------------------------------
void mafGUIMDIFrame::AddDockPane(wxWindow *window, wxPaneInfo& pane_info, const wxString &menu, const wxString &subMenu)
//-----------------------------------------------------------
{
  m_DockManager.AddPane(window,pane_info, menu, subMenu);
  m_DockManager.Update();
}
//-----------------------------------------------------------
void mafGUIMDIFrame::RemoveDockPane(wxString pane_name)
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
void mafGUIMDIFrame::ShowDockPane(wxString pane_name, bool show)
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
void mafGUIMDIFrame::ShowDockPane(wxWindow *window, bool show)
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
bool mafGUIMDIFrame::DockPaneIsShown(wxString pane_name)
//-----------------------------------------------------------
{
  wxPaneInfo& pi = m_DockManager.GetPane(pane_name);
  if(pi.IsOk())
    return pi.IsShown();
  return false;
}
//-----------------------------------------------------------
bool mafGUIMDIFrame::DockPaneIsShown(wxWindow *window)
//-----------------------------------------------------------
{
  wxPaneInfo& pi = m_DockManager.GetPane(window);
  if(pi.IsOk())
    return pi.IsShown();
  return false;
}


