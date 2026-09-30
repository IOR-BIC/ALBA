/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIMDIFrame
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


#include "albaGUIMDIFrame.h"
#include <wx/icon.h>
#include "albaDecl.h"
#include "albaEvent.h"
#include "albaPics.h"
#include "albaAbsLogicManager.h"

#ifdef __WIN32__
#include <malloc.h>
#endif
#include "wx/filename.h"

#ifdef ALBA_USE_VTK //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::
	#include "vtkAlgorithm.h"
  #include "vtkVersion.h"
  #include "vtkViewport.h"
  #include "vtkCommand.h"

class albaGUIMDIFrameCallback : public vtkCommand
{
  public:
    vtkTypeMacro(albaGUIMDIFrameCallback,vtkCommand);

    static albaGUIMDIFrameCallback *New() {return new albaGUIMDIFrameCallback;}
    albaGUIMDIFrameCallback() {m_mode=0; m_Frame=NULL;};
    void SetMode(int mode){m_mode=mode;};
    void SetFrame(albaGUIMDIFrame *frame){m_Frame=frame;};
    virtual void Execute(vtkObject *caller, unsigned long, void*)
    {
      assert(m_Frame);
      if(caller->IsA("vtkProcessObject"))
      {
        vtkAlgorithm *po = (vtkAlgorithm*)caller;

        if(m_mode==0) // ProgressEvent-Callback
        {
          ::wxSafeYield(NULL,true); //fix on bug #2082 
          m_Frame->ProgressBarSetVal(po->GetProgress()*100);
          //albaLogMessage("progress = %g", po->GetProgress()*100);
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
    albaGUIMDIFrame *m_Frame;
};
#endif 

//----------------------------------------------------------------------------
// albaGUIMDIFrame
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(albaGUIMDIFrame, wxMDIParentFrame)
    EVT_CLOSE(albaGUIMDIFrame::OnCloseWindow)
    EVT_DROP_FILES(albaGUIMDIFrame::OnDropFile)
	EVT_MENU_RANGE(WIZARD_START,WIZARD_END,albaGUIMDIFrame::OnMenuWizard)
    EVT_MENU_RANGE(MENU_START,MENU_END,albaGUIMDIFrame::OnMenu)
		EVT_MENU_RANGE(OP_START,OP_END,albaGUIMDIFrame::OnMenuOp)
    EVT_MENU_RANGE(VIEW_START,VIEW_END,albaGUIMDIFrame::OnMenuView)
		EVT_MENU_RANGE(wxID_FILE1, wxID_FILE9, albaGUIMDIFrame::OnMenu)
    EVT_UPDATE_UI_RANGE(MENU_START,MENU_END,albaGUIMDIFrame::OnUpdateUI)
    EVT_UPDATE_UI_RANGE(OP_START,OP_END,albaGUIMDIFrame::OnUpdateUI)
    EVT_SIZE(albaGUIMDIFrame::OnSize) //SIL. 23-may-2006 : 

    EVT_IDLE(albaGUIMDIFrame::OnIdle) 
END_EVENT_TABLE()



//----------------------------------------------------------------------------
albaGUIMDIFrame::albaGUIMDIFrame(const wxString& title, const wxPoint& pos, const wxSize& size, long style)
//----------------------------------------------------------------------------
: wxMDIParentFrame((wxFrame *)NULL, -1, title, pos, size, wxDEFAULT_FRAME_STYLE|wxHSCROLL|wxVSCROLL|style)
{
  albaSetFrame( this );

  this->SetMinSize(wxSize(600,500)); // m_DockManager cant handle correctly the frame MinSize (yet)
  m_DockManager.SetFrame(this);
	wxDockArt* artProvider = m_DockManager.GetArtProvider();
  artProvider->SetMetric(wxAUI_ART_PANE_BORDER_SIZE,0 );
	
  artProvider->SetColor(wxAUI_ART_INACTIVE_CAPTION_COLOUR, artProvider->GetColor(wxAUI_ART_ACTIVE_CAPTION_GRADIENT_COLOUR));
  artProvider->SetColor(wxAUI_ART_INACTIVE_CAPTION_GRADIENT_COLOUR, artProvider->GetColor(wxAUI_ART_ACTIVE_CAPTION_COLOUR));
  artProvider->SetColor(wxAUI_ART_INACTIVE_CAPTION_TEXT_COLOUR, artProvider->GetColor(wxAUI_ART_ACTIVE_CAPTION_TEXT_COLOUR));
	artProvider->SetMetric(wxAUI_ART_GRADIENT_TYPE, wxAUI_GRADIENT_HORIZONTAL);
  m_DockManager.Update();
  
  m_ID_PBCall = 0;
  m_PBCalls.clear();
  m_MemoryLimitAlert = 50; // 50 MB is the default low limit to alert the user.
  m_UserAlerted = false;

  m_Listener = NULL;
  CreateStatusbar();
  Centre();

#ifdef __WIN32__
  ((wxWindow *)this)->DragAcceptFiles(true);
#endif

  wxIconBundle ib;
  ib.AddIcon( albaPictureFactory::GetPictureFactory()->GetIcon("FRAME_ICON16x16") );
  ib.AddIcon( albaPictureFactory::GetPictureFactory()->GetIcon("FRAME_ICON32x32") );
	ib.AddIcon(albaPictureFactory::GetPictureFactory()->GetIcon("FRAME_ICON64x64"));
	ib.AddIcon(albaPictureFactory::GetPictureFactory()->GetIcon("FRAME_ICON128x128"));
	ib.AddIcon(albaPictureFactory::GetPictureFactory()->GetIcon("FRAME_ICON256x256"));
  SetIcons(ib);

#ifdef ALBA_USE_VTK
  m_ProgressCallback= albaGUIMDIFrameCallback::New(); 
  m_ProgressCallback->SetFrame(this);
  m_ProgressCallback->SetMode(0);
  m_StartCallback = albaGUIMDIFrameCallback::New();; 
  m_StartCallback->SetFrame(this);
  m_StartCallback->SetMode(1);
  m_EndCallback = albaGUIMDIFrameCallback::New(); 
  m_EndCallback->SetFrame(this);
  m_EndCallback->SetMode(2);
#endif //ALBA_USE_VTK
}

//----------------------------------------------------------------------------
albaGUIMDIFrame::~albaGUIMDIFrame()
//----------------------------------------------------------------------------
{
#ifdef ALBA_USE_VTK
  vtkDEL(m_ProgressCallback); 
  vtkDEL(m_StartCallback); 
  vtkDEL(m_EndCallback); 
#endif 

  m_PBCalls.clear();

  albaSetFrame(NULL);
}
//----------------------------------------------------------------------------
void albaGUIMDIFrame::OnMenuWizard(wxCommandEvent& e)
	//----------------------------------------------------------------------------
{ 
	//Tell logic about click on wizard menu
	albaEventMacro(albaEvent(this,MENU_WIZARD,(long)e.GetId()));
}
//----------------------------------------------------------------------------
void albaGUIMDIFrame::OnMenu(wxCommandEvent& e)
//----------------------------------------------------------------------------
{ 
	//if(e.GetId() > SASH_START && e.GetId() < SASH_END )
	//  ShowDockPane(e.GetId(), !DockPaneIsShown( (wxWindow*)(e.GetEventObject())) );
	// else
  albaEventMacro(albaEvent(this,e.GetId()));
}
//----------------------------------------------------------------------------
void albaGUIMDIFrame::OnMenuOp(wxCommandEvent& e)
//----------------------------------------------------------------------------
{ 
  albaEventMacro(albaEvent(this,MENU_OP,(long)e.GetId()));
}
//----------------------------------------------------------------------------
void albaGUIMDIFrame::OnMenuView(wxCommandEvent& e)
//----------------------------------------------------------------------------
{ 
  albaEventMacro(albaEvent(this,VIEW_CREATE,(long)e.GetId()));
}
//----------------------------------------------------------------------------
void albaGUIMDIFrame::OnUpdateUI(wxUpdateUIEvent& e)
//----------------------------------------------------------------------------
{ 
	albaEventMacro(albaEvent(this,UPDATE_UI,&e));
}
//----------------------------------------------------------------------------
void albaGUIMDIFrame::OnCloseWindow(wxCloseEvent& event)
//----------------------------------------------------------------------------
{ 
  albaEventMacro(albaEvent(this,MENU_FILE_QUIT));
}
//----------------------------------------------------------------------------
void albaGUIMDIFrame::OnSize(wxSizeEvent& event)
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
void albaGUIMDIFrame::OnDropFile(wxDropFilesEvent &event)
//-----------------------------------------------------------
{
  wxString path, name, ext, *file_list;
  albaString file_to_open;

  int num_files = event.GetNumberOfFiles();
  file_list = event.GetFiles();
  for (int i=0; i< num_files; i++)
  {
    file_to_open = file_list[i];

    wxFileName::SplitPath(file_list[i],&path, &name, &ext);

		albaString zippedExt;
		const char *archExt = GetLogicManager()->GetMsfFileExtension();
		zippedExt.Printf("z%s", archExt);

		//Accept msf, zmsf and specific app extensions 
    if (ext == "msf" || ext == "zmsf" || ext == archExt || ext == zippedExt.GetCStr())
    {
      albaEventMacro(albaEvent(this,MENU_FILE_OPEN,&file_to_open));
      return;
    }
    else
    {
      albaEventMacro(albaEvent(this,IMPORT_FILE,&file_to_open));
    }
  }
}
//----------------------------------------------------------------------------
void albaGUIMDIFrame::CreateStatusbar()
//----------------------------------------------------------------------------
{
  static const int widths[]={-1,60,60,60,210,100}; // enlarged the last tab for the "free mem watch" information
	CreateStatusBar(6); 
  m_frameStatusBar->SetStatusWidths(6,widths);
	SetStatusText( _("Welcome"),0);
	SetStatusText( " ",1);
	SetStatusText( " ",2);
	SetStatusText( " ",3);

  // Paolo 10 Jul 2006: due to position correctly the progress bar.
	wxRect pr;
  m_frameStatusBar->GetFieldRect(4,pr);
  m_Busy=false;
	m_Gauge = new wxGauge(m_frameStatusBar, -1, 100,pr.GetPosition(),pr.GetSize(),wxGA_SMOOTH);
	m_Gauge->SetForegroundColour( *wxRED );
  m_Gauge->Show(false);
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
void albaGUIMDIFrame::OnIdle(wxIdleEvent& event)
//----------------------------------------------------------------------------
{ 
#ifdef __WIN32__
  MEMORYSTATUS ms;
  GlobalMemoryStatus( &ms );
  wxString s;
  int current_free_memory = ms.dwAvailPhys/1000000;
  s << "free mem " << current_free_memory << " mb";   
  SetStatusText(s,5);
  if (current_free_memory < m_MemoryLimitAlert && !m_UserAlerted)
  {
    m_UserAlerted = true;
    int answere = wxMessageBox(_("Program is running with few free memory!! \nFree memory used by UnDo stack?."), _("Warning"), wxYES_NO);
    if (answere == wxYES)
    {
      // Clear UnDo stack to gain memory.
      albaEventMacro(albaEvent(this, CLEAR_UNDO_STACK));
    }
  }
#endif
}
//----------------------------------------------------------------------------
void albaGUIMDIFrame::FreeMemorySizeOnIdle(wxIdleEvent& event)
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
      albaEventMacro(albaEvent(this, CLEAR_UNDO_STACK));
    }
  }
#endif
}
//-----------------------------------------------------------
void albaGUIMDIFrame::Busy()
//-----------------------------------------------------------
{
  SetStatusText("Busy",2);
  SetStatusText("",3);
  m_Gauge->Show(true);
  m_Gauge->SetValue(0);
  Refresh(false);
}
//-----------------------------------------------------------
void albaGUIMDIFrame::Ready()
//-----------------------------------------------------------
{
  SetStatusText("",2);
  SetStatusText("",3);
  m_Gauge->Show(false);
  Refresh(false);
}
//-----------------------------------------------------------
void albaGUIMDIFrame::ProgressBarShow()
//-----------------------------------------------------------
{
  if (m_PBCalls.size()==0)
  {
    SetStatusText("",0);
    Busy();
    m_PBCalls.push_back(m_ID_PBCall);
  }
  else
  {
    m_ID_PBCall++;
  }
}
//-----------------------------------------------------------
void albaGUIMDIFrame::ProgressBarHide()
//-----------------------------------------------------------
{
  if (m_PBCalls.size()>0 && (m_PBCalls.at((m_PBCalls.size()-1)) == m_ID_PBCall) )
  {
    SetStatusText("",0);
    Ready();
    m_PBCalls.pop_back();
  }
  else
  {
    m_ID_PBCall--;
  }
}
//-----------------------------------------------------------
int albaGUIMDIFrame::ProgressBarGetStatus()
//-----------------------------------------------------------
{
  return m_Gauge->GetValue();
}
//-----------------------------------------------------------
void albaGUIMDIFrame::ProgressBarSetVal(int progress)
//-----------------------------------------------------------
{
  if (m_PBCalls.size()>0 && (m_PBCalls.at((m_PBCalls.size()-1)) == m_ID_PBCall) )
  {
    m_Gauge->SetValue(progress);
    SetStatusText(albaString::Format(" %d%% ",progress),3);
    ::wxSafeYield(NULL,true); //fix on bug #2082
  }
}
//-----------------------------------------------------------
void albaGUIMDIFrame::ProgressBarSetText(wxString *msg)
//-----------------------------------------------------------
{
  if(msg) SetStatusText(*msg,0);
}
//-----------------------------------------------------------
void albaGUIMDIFrame::RenderStart()
//-----------------------------------------------------------
{
  SetStatusText( "Rendering",1);
}
//-----------------------------------------------------------
void albaGUIMDIFrame::RenderEnd()
//-----------------------------------------------------------
{
  SetStatusText( " ",1);
}
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#ifdef ALBA_USE_VTK
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//-----------------------------------------------------------
void albaGUIMDIFrame::BindToProgressBar(vtkObject* vtkobj)
//-----------------------------------------------------------
{
	if(vtkobj->IsA("vtkViewport")) 
		BindToProgressBar((vtkViewport*)vtkobj);
  else if(vtkobj->IsA("vtkAlgorithm")) 
		BindToProgressBar((vtkAlgorithm*)vtkobj);
	else 
    albaLogMessage("wrong vtkObject passed to BindToProgressBar");
}
//-----------------------------------------------------------
void albaGUIMDIFrame::BindToProgressBar(vtkAlgorithm* filter)
//-----------------------------------------------------------
{
  filter->AddObserver(vtkCommand::ProgressEvent,m_ProgressCallback);
  filter->AddObserver(vtkCommand::StartEvent,m_StartCallback);
  filter->AddObserver(vtkCommand::EndEvent,m_EndCallback);
}
//-----------------------------------------------------------
void albaGUIMDIFrame::BindToProgressBar(vtkViewport* ren)
//-----------------------------------------------------------
{
  ren->AddObserver(vtkCommand::StartEvent,m_StartCallback);
  ren->AddObserver(vtkCommand::EndEvent,m_EndCallback);
}
//-----------------------------------------------------------
void albaGUIMDIFrame::OnQuit()
//-----------------------------------------------------------
{
  m_DockManager.UnInit();
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#endif //ALBA_USE_VTK
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

/*
//-----------------------------------------------------------
void albaGUIMDIFrame::ShowDockSettings()
//-----------------------------------------------------------
{
  m_DockSettings->ShowModal();
}
*/
//-----------------------------------------------------------
void albaGUIMDIFrame::AddDockPane(wxWindow *window, wxPaneInfo& pane_info, const wxString &menu, const wxString &subMenu)
//-----------------------------------------------------------
{
  m_DockManager.AddPane(window,pane_info, menu, subMenu);
  m_DockManager.Update();
}
//-----------------------------------------------------------
void albaGUIMDIFrame::RemoveDockPane(wxString pane_name)
//-----------------------------------------------------------
{
  wxPaneInfo& pi = m_DockManager.GetPane(pane_name);
  if(pi.IsOk())
  {
    m_DockManager.DetachPane(pi.m_Window);
    m_DockManager.Update();
  }
}
//-----------------------------------------------------------
void albaGUIMDIFrame::ShowDockPane(wxString pane_name, bool show)
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
void albaGUIMDIFrame::ShowDockPane(wxWindow *window, bool show)
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
bool albaGUIMDIFrame::DockPaneIsShown(wxString pane_name)
//-----------------------------------------------------------
{
  wxPaneInfo& pi = m_DockManager.GetPane(pane_name);
  if(pi.IsOk())
    return pi.IsShown();
  return false;
}
//-----------------------------------------------------------
bool albaGUIMDIFrame::DockPaneIsShown(wxWindow *window)
//-----------------------------------------------------------
{
  wxPaneInfo& pi = m_DockManager.GetPane(window);
  if(pi.IsOk())
    return pi.IsShown();
  return false;
}


