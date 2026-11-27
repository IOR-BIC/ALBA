/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIMainFrame
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


#include "albaGUIMainFrame.h"
#include "albaGUIViewArea.h"
#include <wx/icon.h>
#include "albaDecl.h"
#include "albaEvent.h"
#include "albaPics.h"
#include "albaAbsLogicManager.h"

#ifdef __WIN32__
#include <malloc.h>
#endif
#include "wx/filename.h"


//----------------------------------------------------------------------------
// albaGUIMainFrame
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(albaGUIMainFrame, wxFrame)
    EVT_CLOSE(albaGUIMainFrame::OnCloseWindow)
    EVT_DROP_FILES(albaGUIMainFrame::OnDropFile)
	EVT_MENU_RANGE(WIZARD_START,WIZARD_END,albaGUIMainFrame::OnMenuWizard)
    EVT_MENU_RANGE(MENU_START,MENU_END,albaGUIMainFrame::OnMenu)
		EVT_MENU_RANGE(OP_START,OP_END,albaGUIMainFrame::OnMenuOp)
    EVT_MENU_RANGE(VIEW_START,VIEW_END,albaGUIMainFrame::OnMenuView)
		EVT_MENU_RANGE(wxID_FILE1, wxID_FILE9, albaGUIMainFrame::OnMenu)
    EVT_SIZE(albaGUIMainFrame::OnSize) //SIL. 23-may-2006 : 

    EVT_IDLE(albaGUIMainFrame::OnIdle) 
END_EVENT_TABLE()



//----------------------------------------------------------------------------
albaGUIMainFrame::albaGUIMainFrame(const wxString& title, const wxPoint& pos, const wxSize& size, long style)
//----------------------------------------------------------------------------
: wxFrame((wxFrame *)NULL, -1, title, pos, size, wxDEFAULT_FRAME_STYLE|style)
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

  m_ViewArea = new albaGUIViewArea(this);
  m_DockManager.AddPane(m_ViewArea, wxPaneInfo().Name("views").CenterPane().PaneBorder(false));
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

}

//----------------------------------------------------------------------------
albaGUIMainFrame::~albaGUIMainFrame()
//----------------------------------------------------------------------------
{
  m_PBCalls.clear();

  albaSetFrame(NULL);
}
//----------------------------------------------------------------------------
void albaGUIMainFrame::OnMenuWizard(wxCommandEvent& e)
	//----------------------------------------------------------------------------
{ 
	//Tell logic about click on wizard menu
	albaEventMacro(albaEvent(this,MENU_WIZARD,(long)e.GetId()));
}
//----------------------------------------------------------------------------
void albaGUIMainFrame::OnMenu(wxCommandEvent& e)
//----------------------------------------------------------------------------
{ 
	//if(e.GetId() > SASH_START && e.GetId() < SASH_END )
	//  ShowDockPane(e.GetId(), !DockPaneIsShown( (wxWindow*)(e.GetEventObject())) );
	// else
  albaEventMacro(albaEvent(this,e.GetId()));
}
//----------------------------------------------------------------------------
void albaGUIMainFrame::OnMenuOp(wxCommandEvent& e)
//----------------------------------------------------------------------------
{ 
  albaEventMacro(albaEvent(this,MENU_OP,(long)e.GetId()));
}
//----------------------------------------------------------------------------
void albaGUIMainFrame::OnMenuView(wxCommandEvent& e)
//----------------------------------------------------------------------------
{ 
  albaEventMacro(albaEvent(this,VIEW_CREATE,(long)e.GetId()));
}
//----------------------------------------------------------------------------
void albaGUIMainFrame::OnCloseWindow(wxCloseEvent& event)
//----------------------------------------------------------------------------
{ 
  albaEventMacro(albaEvent(this,MENU_FILE_QUIT));
}
//----------------------------------------------------------------------------
void albaGUIMainFrame::OnSize(wxSizeEvent& event)
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
void albaGUIMainFrame::OnDropFile(wxDropFilesEvent &event)
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
      GetLogicManager()->FileOpen(file_to_open.GetCStr());
      return;
    }
    else
    {
      albaEventMacro(albaEvent(this,IMPORT_FILE,&file_to_open));
    }
  }
}
//----------------------------------------------------------------------------
void albaGUIMainFrame::CreateStatusbar()
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
void albaGUIMainFrame::OnIdle(wxIdleEvent& event)
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
      if (GetOpManager())
        GetOpManager()->ClearUndoStack();
    }
  }
#endif
}
//----------------------------------------------------------------------------
void albaGUIMainFrame::FreeMemorySizeOnIdle(wxIdleEvent& event)
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
      if (GetOpManager())
        GetOpManager()->ClearUndoStack();
    }
  }
#endif
}
//-----------------------------------------------------------
void albaGUIMainFrame::Busy()
//-----------------------------------------------------------
{
  SetStatusText("Busy",2);
  SetStatusText("",3);
  m_Gauge->Show(true);
  m_Gauge->SetValue(0);
  Refresh(false);
}
//-----------------------------------------------------------
void albaGUIMainFrame::Ready()
//-----------------------------------------------------------
{
  SetStatusText("",2);
  SetStatusText("",3);
  m_Gauge->Show(false);
  Refresh(false);
}
//-----------------------------------------------------------
void albaGUIMainFrame::ProgressBarShow()
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
void albaGUIMainFrame::ProgressBarHide()
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
int albaGUIMainFrame::ProgressBarGetStatus()
//-----------------------------------------------------------
{
  return m_Gauge->GetValue();
}
//-----------------------------------------------------------
void albaGUIMainFrame::ProgressBarSetVal(int progress)
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
void albaGUIMainFrame::ProgressBarSetText(wxString *msg)
//-----------------------------------------------------------
{
  if(msg) SetStatusText(*msg,0);
}
//-----------------------------------------------------------
void albaGUIMainFrame::RenderStart()
//-----------------------------------------------------------
{
  SetStatusText( "Rendering",1);
}
//-----------------------------------------------------------
void albaGUIMainFrame::RenderEnd()
//-----------------------------------------------------------
{
  SetStatusText( " ",1);
}
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#ifdef ALBA_USE_VTK
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//-----------------------------------------------------------
void albaGUIMainFrame::OnQuit()
//-----------------------------------------------------------
{
  m_DockManager.UnInit();
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#endif //ALBA_USE_VTK
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

/*
//-----------------------------------------------------------
void albaGUIMainFrame::ShowDockSettings()
//-----------------------------------------------------------
{
  m_DockSettings->ShowModal();
}
*/
//-----------------------------------------------------------
void albaGUIMainFrame::AddDockPane(wxWindow *window, wxPaneInfo& pane_info, const wxString &menu, const wxString &subMenu)
//-----------------------------------------------------------
{
  m_DockManager.AddPane(window,pane_info, menu, subMenu);
  m_DockManager.Update();
}
//-----------------------------------------------------------
void albaGUIMainFrame::RemoveDockPane(wxString pane_name)
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
void albaGUIMainFrame::ShowDockPane(wxString pane_name, bool show)
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
void albaGUIMainFrame::ShowDockPane(wxWindow *window, bool show)
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
bool albaGUIMainFrame::DockPaneIsShown(wxString pane_name)
//-----------------------------------------------------------
{
  wxPaneInfo& pi = m_DockManager.GetPane(pane_name);
  if(pi.IsOk())
    return pi.IsShown();
  return false;
}
//-----------------------------------------------------------
bool albaGUIMainFrame::DockPaneIsShown(wxWindow *window)
//-----------------------------------------------------------
{
  wxPaneInfo& pi = m_DockManager.GetPane(window);
  if(pi.IsOk())
    return pi.IsShown();
  return false;
}


