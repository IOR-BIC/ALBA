/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafRWIBase.cpp,v $
  Language:  C++
  Date:      $Date: 2005-06-28 10:21:56 $
  Version:   $Revision: 1.2 $
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


#include "mafRWIBase.h"

#include <wx/event.h>
#include <wx/utils.h>
#include <wx/dcclient.h>
#include <wx/image.h>
#include <wx/bitmap.h>
#include "wx/scrolwin.h"
#include "wx/gdicmn.h"
#include "wx/timer.h"

#ifdef __WXGTK__
//  #include <glib/gmacros.h>
  #include <gdk/gdkprivate.h>
  #include "wx/gtk/win_gtk.h"
#endif

#include "mafDecl.h"
#include "mafDevice.h"
#include "mmdButtonsPad.h"
#include "mmdMouse.h"
#include "mafEventInteraction.h"
#include "mafEvent.h"

#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkCamera.h"
#include "vtkInteractorObserver.h"
#include "vtkInteractorStyle.h"
#include "vtkCommand.h"
#include "vtkMatrix4x4.h"
#include "vtkRendererCollection.h"
#include "vtkLight.h"
//#include "vtkWindowToImageFilter.h"
#include "vtkBMPWriter.h"
#include "vtkImageData.h"

//----------------------------------------------------------------------------
IMPLEMENT_DYNAMIC_CLASS(mafRWIBase, wxScrolledWindow)
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(mafRWIBase, wxScrolledWindow)
  EVT_PAINT(mafRWIBase::OnPaint)
  EVT_ERASE_BACKGROUND(mafRWIBase::OnEraseBackground)
  EVT_LEFT_DOWN(mafRWIBase::OnLeftMouseButtonDown)
  EVT_LEFT_UP(mafRWIBase::OnLeftMouseButtonUp)
  EVT_MIDDLE_DOWN(mafRWIBase::OnMiddleMouseButtonDown)
  EVT_MIDDLE_UP(mafRWIBase::OnMiddleMouseButtonUp)
  EVT_RIGHT_DOWN(mafRWIBase::OnRightMouseButtonDown)
  EVT_RIGHT_UP(mafRWIBase::OnRightMouseButtonUp)
  EVT_MOTION(mafRWIBase::OnMouseMotion)
  EVT_TIMER(ID_mafRWIBase_TIMER, mafRWIBase::OnTimer)
  EVT_KEY_DOWN(mafRWIBase::OnKeyDown)
  EVT_KEY_UP(mafRWIBase::OnKeyUp)
  EVT_CHAR(mafRWIBase::OnChar)
  EVT_SIZE(mafRWIBase::OnSize)
  EVT_IDLE(mafRWIBase::OnIdle)
END_EVENT_TABLE()
//----------------------------------------------------------------------------
mafRWIBase::mafRWIBase() : wxScrolledWindow(), vtkRenderWindowInteractor(), timer(this, ID_mafRWIBase_TIMER)
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafRWIBase::mafRWIBase(wxWindow *parent, wxWindowID id, const wxPoint &pos,
			 const wxSize &size, long style, const wxString &name)
  : wxScrolledWindow(parent, id, pos, size, style, name), vtkRenderWindowInteractor(), 
    timer(this, ID_mafRWIBase_TIMER)
//----------------------------------------------------------------------------
{
  m_Hidden = true;
  this->Show(false);
	m_SaveDir = ::wxGetHomeDir(); m_SaveDir += "\\desktop"; 
  m_Width = m_Height = 10;
  m_Camera = NULL;
  m_Mouse = NULL;
  m_CustomInteractorStyle = false;
}
//----------------------------------------------------------------------------
mafRWIBase::~mafRWIBase()
//----------------------------------------------------------------------------
{
	this->SetRenderWindow(NULL);
}
//----------------------------------------------------------------------------
mafRWIBase * mafRWIBase::New()
//----------------------------------------------------------------------------
{
  return new mafRWIBase();
}
//----------------------------------------------------------------------------
void mafRWIBase::SetMouse(mmdMouse *mouse)
//----------------------------------------------------------------------------
{
  m_Mouse = mouse;
}
//----------------------------------------------------------------------------
void mafRWIBase::Initialize()
//----------------------------------------------------------------------------
{
  // if don't have render window then stuck
  if (!RenderWindow)
  {
   // wxLogMessage("mafRWIBase::Initialize has no render window");
    return;
  }

  // set minimum size of window
  int *size = RenderWindow->GetSize();
  size[0] = ((size[0] > 0) ? size[0] : 300);
  size[1] = ((size[1] > 0) ? size[1] : 300);

  // enable everything and start rendering
  Enable();
  RenderWindow->Start();

  // set the size in the render window interactor
  Size[0] = size[0];
  Size[1] = size[1];

  // this is initialized
  Initialized = 1;
}
//----------------------------------------------------------------------------
void mafRWIBase::Enable()
//----------------------------------------------------------------------------
{
  // if already enabled then done
  if (Enabled)
    return;

  // that's it
  Enabled = 1;
  Modified();
}
//----------------------------------------------------------------------------
void mafRWIBase::Disable()
//----------------------------------------------------------------------------
{
  // if already disabled then done
  if (!Enabled)
    return;

  // that's it
  Enabled = 0;
  Modified();
}
//----------------------------------------------------------------------------
void mafRWIBase::Start()
//----------------------------------------------------------------------------
{
  // the interactor cannot control the event loop
  wxLogMessage("mafRWIBase::Start() interactor cannot control event loop.");
}
//----------------------------------------------------------------------------
void mafRWIBase::UpdateSize(int x, int y)
//----------------------------------------------------------------------------
{
  // if the size changed tell render window
  if (( (x != Size[0]) || (y != Size[1]) ) && (RenderWindow != 0))
  {
    Size[0] = x;
    Size[1] = y;
    RenderWindow->SetSize(x, y);
  }
}
//----------------------------------------------------------------------------
int mafRWIBase::CreateTimer(int timertype)
//----------------------------------------------------------------------------
{
  // it's a one shot timer
  if (!timer.Start(10, TRUE))
    assert(false);
  return 1;
}
//----------------------------------------------------------------------------
int mafRWIBase::DestroyTimer()
//----------------------------------------------------------------------------
{
  // do nothing
  return 1;
}
//----------------------------------------------------------------------------
void mafRWIBase::TerminateApp()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafRWIBase::OnPaint(wxPaintEvent &event)
//----------------------------------------------------------------------------
{
  wxPaintDC pDC(this);

  //int w, h;
  //wxScrolledWindow::GetSize( &w,&h );
  //UpdateSize(w,h);
  //RenderWindow->SetSize(w, h);
  //vtkRenderWindowInteractor::SetSize(w, h);
  
  if(!RenderWindow) return; //SIL. 13-11-2003: - rare - may happen during Debug

#ifdef __WXMSW__
  if (RenderWindow->GetGenericWindowId() == 0)
    RenderWindow->SetWindowId( (HWND) this->GetHWND() );
#endif
#ifdef __WXGTK__
  if (RenderWindow->GetGenericWindowId() == 0)
    RenderWindow->SetParentId( (void *)(((GdkWindowPrivate *)GTK_PIZZA(m_wxwindow)->bin_window)->xwindow) );
#endif
#ifdef __WXMOTIF__
  if (RenderWindow->GetGenericWindowId() == 0)
    RenderWindow->SetWindowId( this->GetXWindow() );
#endif

  if(!Initialized) Initialize();

  Render();
}
//----------------------------------------------------------------------------
void mafRWIBase::OnEraseBackground(wxEraseEvent &event)
//----------------------------------------------------------------------------
{
  event.Skip(false);
}
//----------------------------------------------------------------------------
void mafRWIBase::OnLeftMouseButtonDown(wxMouseEvent &event)
//----------------------------------------------------------------------------
{
  if (!Enabled) return;

  CaptureMouse();
  NotifyClick(); 

  if (m_CustomInteractorStyle)
  {
    SetEventInformation(event.GetX(),m_Height - event.GetY() - 1,event.ControlDown(),event.ShiftDown());
    InvokeEvent(vtkCommand::LeftButtonPressEvent,NULL);
  }
  else
  {
    
    mafEventInteraction e(this,mmdButtonsPad::BUTTON_DOWN);
    e.Set2DPosition(event.GetX(),m_Height - event.GetY() - 1);
    e.SetButton(MAF_LEFT_BUTTON);
    e.SetModifier(MAF_SHIFT_KEY,event.ShiftDown());
    e.SetModifier(MAF_CTRL_KEY,event.ControlDown());
    e.SetModifier(MAF_ALT_KEY,event.AltDown());
    e.SetChannel(MCH_OUTPUT);
    if(m_Mouse) m_Mouse->OnEvent(&e);
  }
}
//----------------------------------------------------------------------------
void mafRWIBase::OnMiddleMouseButtonDown(wxMouseEvent &event)
//----------------------------------------------------------------------------
{
  if (!Enabled) return;

  CaptureMouse();
  NotifyClick(); 

  if (m_CustomInteractorStyle)
  {
    SetEventInformation(event.GetX(),m_Height - event.GetY() - 1,event.ControlDown(),event.ShiftDown());
    InvokeEvent(vtkCommand::MiddleButtonPressEvent,NULL);
  }
  else
  {
    mafEventInteraction e(this,mmdButtonsPad::BUTTON_DOWN);
    e.Set2DPosition(event.GetX(),m_Height - event.GetY() - 1);
    e.SetButton(MAF_MIDDLE_BUTTON);
    e.SetModifier(MAF_SHIFT_KEY,event.ShiftDown());
    e.SetModifier(MAF_CTRL_KEY,event.ControlDown());
    e.SetModifier(MAF_ALT_KEY,event.AltDown());
    e.SetChannel(MCH_OUTPUT);
    if(m_Mouse) m_Mouse->OnEvent(&e);
  }
}
//----------------------------------------------------------------------------
void mafRWIBase::OnRightMouseButtonDown(wxMouseEvent &event)
//----------------------------------------------------------------------------
{
  if (!Enabled) return;

  CaptureMouse();
  NotifyClick(); 

  if (m_CustomInteractorStyle)
  {
    SetEventInformation(event.GetX(),m_Height - event.GetY() - 1,event.ControlDown(),event.ShiftDown());
    InvokeEvent(vtkCommand::RightButtonPressEvent,NULL);
  }
  else
  {
    mafEventInteraction e(this,mmdButtonsPad::BUTTON_DOWN);
    e.Set2DPosition(event.GetX(),m_Height - event.GetY() - 1);
    e.SetButton(MAF_RIGHT_BUTTON);
    e.SetModifier(MAF_SHIFT_KEY,event.ShiftDown());
    e.SetModifier(MAF_CTRL_KEY,event.ControlDown());
    e.SetModifier(MAF_ALT_KEY,event.AltDown());
    e.SetChannel(MCH_OUTPUT);
    if(m_Mouse) m_Mouse->OnEvent(&e);
  }
}
//----------------------------------------------------------------------------
void mafRWIBase::OnLeftMouseButtonUp(wxMouseEvent &event)
//----------------------------------------------------------------------------
{
  if (!Enabled) return;

  if (m_CustomInteractorStyle)
  {
	  SetEventInformation(event.GetX(),m_Height - event.GetY() - 1,event.ControlDown(),event.ShiftDown());
	  InvokeEvent(vtkCommand::LeftButtonReleaseEvent,NULL);
  }
  else
  {
    mafEventInteraction e(this,mmdButtonsPad::BUTTON_UP);
    e.Set2DPosition(event.GetX(),m_Height - event.GetY() - 1);
    e.SetButton(MAF_LEFT_BUTTON);
    e.SetModifier(MAF_SHIFT_KEY,event.ShiftDown());
    e.SetModifier(MAF_CTRL_KEY,event.ControlDown());
    e.SetModifier(MAF_ALT_KEY,event.AltDown());
    e.SetChannel(MCH_OUTPUT);
    if(m_Mouse) m_Mouse->OnEvent(&e);
  }
  
  ReleaseMouse();
}
//----------------------------------------------------------------------------
void mafRWIBase::OnMiddleMouseButtonUp(wxMouseEvent &event)
//----------------------------------------------------------------------------
{
  if (!Enabled) return;

  if (m_CustomInteractorStyle)
  {
	  SetEventInformation(event.GetX(),m_Height - event.GetY() - 1,event.ControlDown(),event.ShiftDown());
	  InvokeEvent(vtkCommand::MiddleButtonReleaseEvent,NULL);
  }
  else
  {
    mafEventInteraction e(this,mmdButtonsPad::BUTTON_UP);
    e.Set2DPosition(event.GetX(),m_Height - event.GetY() - 1);
    e.SetButton(MAF_MIDDLE_BUTTON);
    e.SetModifier(MAF_SHIFT_KEY,event.ShiftDown());
    e.SetModifier(MAF_CTRL_KEY,event.ControlDown());
    e.SetModifier(MAF_ALT_KEY,event.AltDown());
    e.SetChannel(MCH_OUTPUT);
    if(m_Mouse) m_Mouse->OnEvent(&e);
  }

  ReleaseMouse();
}
//----------------------------------------------------------------------------
void mafRWIBase::OnRightMouseButtonUp(wxMouseEvent &event)
//----------------------------------------------------------------------------
{
  if (!Enabled) return;

  if (m_CustomInteractorStyle)
  {
    SetEventInformation(event.GetX(),m_Height - event.GetY() - 1,event.ControlDown(),event.ShiftDown());
    InvokeEvent(vtkCommand::RightButtonReleaseEvent,NULL);
  }
  else
  {
    mafEventInteraction e(this,mmdButtonsPad::BUTTON_UP);
    e.Set2DPosition(event.GetX(),m_Height - event.GetY() - 1);
    e.SetButton(MAF_RIGHT_BUTTON);
    e.SetModifier(MAF_SHIFT_KEY,event.ShiftDown());
    e.SetModifier(MAF_CTRL_KEY,event.ControlDown());
    e.SetModifier(MAF_ALT_KEY,event.AltDown());
    e.SetChannel(MCH_OUTPUT);
    if(m_Mouse) m_Mouse->OnEvent(&e);
  }

  ReleaseMouse();
}
//----------------------------------------------------------------------------
void mafRWIBase::OnMouseMotion(wxMouseEvent &event)
//----------------------------------------------------------------------------
{
  if (!Enabled) return;

  if (m_CustomInteractorStyle)
  {
    SetEventInformation(event.GetX(),m_Height - event.GetY() - 1,event.ControlDown(),event.ShiftDown());
    InvokeEvent(vtkCommand::MouseMoveEvent,NULL);
  }
  else
  {
    mafEventInteraction e(this,mmdMouse::MOUSE_2D_MOVE);
    e.Set2DPosition(event.GetX(),m_Height - event.GetY() - 1);
    e.SetModifier(MAF_SHIFT_KEY,event.ShiftDown());
    e.SetModifier(MAF_CTRL_KEY,event.ControlDown());
    e.SetModifier(MAF_ALT_KEY,event.AltDown());
    e.SetChannel(MCH_OUTPUT);
    if(m_Mouse) m_Mouse->OnEvent(&e);
  }
}
//----------------------------------------------------------------------------
void mafRWIBase::OnTimer(wxTimerEvent &event)
//----------------------------------------------------------------------------
{
  if (!Enabled) return;

  if (m_CustomInteractorStyle)
  {
    InvokeEvent(vtkCommand::TimerEvent,NULL);
  }
}
//----------------------------------------------------------------------------
void mafRWIBase::OnKeyDown(wxKeyEvent &event)
//----------------------------------------------------------------------------
{
  if (!Enabled) return;

  if (m_CustomInteractorStyle)
  {
    SetKeyEventInformation(event.ControlDown(),event.ShiftDown(),event.GetKeyCode(), 1, "none");
    InvokeEvent(vtkCommand::KeyPressEvent,NULL);
  }
  
  event.Skip();
}
//----------------------------------------------------------------------------
void mafRWIBase::OnKeyUp(wxKeyEvent &event)
//----------------------------------------------------------------------------
{
  if (!Enabled) return;

  if (m_CustomInteractorStyle)
  {
    SetKeyEventInformation(event.ControlDown(),event.ShiftDown(),event.GetKeyCode(), 1, "none");
    InvokeEvent(vtkCommand::KeyReleaseEvent,NULL);
  }

  event.Skip();
}
//----------------------------------------------------------------------------
void mafRWIBase::OnChar(wxKeyEvent &event)
//----------------------------------------------------------------------------
{
  if (!Enabled) return;

  if (m_CustomInteractorStyle)
  {
    SetKeyEventInformation(event.ControlDown(),event.ShiftDown(),event.GetKeyCode(), 1);
    InvokeEvent(vtkCommand::CharEvent,NULL);
  }
  else
  {
    if(m_Mouse) 
    {
      mafEvent e(this,mmdMouse::MOUSE_CHAR_EVENT,(long) event.GetKeyCode());
      e.SetChannel(MCH_OUTPUT);
      m_Mouse->OnEvent(&e);
    }
  }

  event.Skip();
}
//----------------------------------------------------------------------------
void mafRWIBase::OnIdle(wxIdleEvent& event)
//----------------------------------------------------------------------------
{
  //if(m_Hidden)
  //{
  //   m_Hidden = false;
  //   this->Show(true);
  //}
}
//----------------------------------------------------------------------------
void mafRWIBase::NotifyClick()
//----------------------------------------------------------------------------
{
  wxCommandEvent e(wxEVT_COMMAND_BUTTON_CLICKED, VIEW_CLICKED);
  e.SetEventObject(this);
  ProcessEvent(e);
}
//----------------------------------------------------------------------------
void mafRWIBase::OnSize(wxSizeEvent &event)
//----------------------------------------------------------------------------
{
  //   this->Show(false); 
     //m_Hidden = true;

  m_Width = event.GetSize().GetWidth();
  m_Height = event.GetSize().GetHeight();

  #define DONT_FIX_CAMERA_RESET
	#ifdef FIX_CAMERA_RESET

	//find the current camera and set UseHorizontalViewAngle
	//depending on the win aspect ratio
  
		vtkCamera *cam = GetCamera();
		if(cam)
		{
			if(m_Width < m_Height)
				cam->UseHorizontalViewAngleOn();
			else
				cam->UseHorizontalViewAngleOff();
		}
	}
	#endif
  
	// should be so - otherwise disappear the windowing widget - TODO: understand better
	// UpdateSize(event.GetSize().GetWidth(), event.GetSize().GetHeight());
	
  if (m_CustomInteractorStyle)
  {
	  InvokeEvent(vtkCommand::ConfigureEvent,NULL); // mah! - should introduce noise     */
  }
}
//----------------------------------------------------------------------------
wxBitmap *mafRWIBase::GetImage()
//----------------------------------------------------------------------------
{
/* @@@
	int dim[3];

	vtkWindowToImageFilter *w2i = vtkWindowToImageFilter::New();
	w2i->SetInput(GetRenderWindow());
	w2i->Update();
	w2i->GetOutput()->GetDimensions(dim);
	unsigned char *data = (unsigned char *)w2i->GetOutput()->GetScalarPointer();
	wxImage  *img = new wxImage(dim[0],dim[1],data,TRUE);
	wxBitmap *bmp = new wxBitmap(img->ConvertToBitmap());
  delete img;
  w2i->Delete();
	return bmp;
	*/
return NULL;
}
//----------------------------------------------------------------------------
void mafRWIBase::SaveImage(wxString view_name, int magnification)
//---------------------------------------------------------------------------
{
/* @@@
	wxString wildc = "Bitmap Image (*.bmp)|*.bmp";
  wxString name = wxString::Format("%s\\%sSnapshot", m_SaveDir.c_str(),view_name.c_str());
	wxString file = name + ".bmp";
  int i=1;
  while(::wxFileExists(file) && i<100 ) file = wxString::Format("%s%d.bmp",name.c_str(), i++);
	
	wxString filename = mafGetSaveFile(file,wildc).c_str(); 
	if(filename == "") return;

  wxString foo1,foo2;
  wxSplitPath(filename.c_str(),&m_SaveDir,&foo1,&foo2);  // remember save folder
		
	::wxBeginBusyCursor();

	vtkWindowToImageFilter *w2i = vtkWindowToImageFilter::New();
	w2i->SetInput(GetRenderWindow());
  w2i->SetMagnification(magnification);
	w2i->Update();
  
//wxBusyInfo wait("writing file...");
  
	vtkBMPWriter *w = vtkBMPWriter::New();
//  mafProgressMacro(w,"writing image file"); 
  w->SetInput(w2i->GetOutput());
  w->SetFileName(filename);
	w->Write();
	w2i->Delete();
	w->Delete();
	::wxEndBusyCursor();
@@@ */
}
//----------------------------------------------------------------------------
vtkCamera* mafRWIBase::GetCamera()
//---------------------------------------------------------------------------
{
  if(m_Camera == NULL)
  {
    vtkRenderWindow *rw = this->GetRenderWindow();
    if(rw)
	  {
		  vtkRendererCollection *rc = rw->GetRenderers();
		  if(rc)
		  {
			  rc->InitTraversal();
			  vtkRenderer *ren = rc->GetNextItem(); 
			  if(ren)
			  {  
				  m_Camera = ren->GetActiveCamera();
			  }	
		  }			 
	  }
  }  
  return m_Camera;
}
//---------------------------------------------------------------------------
void mafRWIBase::SetInteractorStyle(vtkInteractorObserver *o)
//---------------------------------------------------------------------------
{
  vtkRenderWindowInteractor::SetInteractorStyle(o);
  m_CustomInteractorStyle = o != NULL;
}
