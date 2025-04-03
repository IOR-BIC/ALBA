/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaRWIBase
 Authors: Silvano Imboden - Paolo Quadrani - Daniele Giunchi (Save Image)
 
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


#include "albaRWIBase.h"
#include "albaDecl.h"

#include <wx/event.h>
#include <wx/utils.h>
#include <wx/dcclient.h>
#include <wx/image.h>
#include <wx/bitmap.h>
#include "wx/scrolwin.h"
#include "wx/gdicmn.h"
#include "wx/timer.h"

#ifdef __WXGTK__
  
  //#include <glib/gmacros.h>
  //#include <gdk/gdkprivate.h>
  //#include "wx/gtk/win_gtk.h"
  
  
  //#include "gdk/gdkprivate.h"
  #include <wx/gtk/win_gtk.h> // ok c'e'

#endif

#include "albaGUIApplicationSettings.h"
#include "albaDevice.h"
#include "albaDeviceButtonsPad.h"
#include "albaDeviceButtonsPadMouse.h"
#include "albaEventInteraction.h"
#include "albaEvent.h"

#include "vtkALBASmartPointer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkCamera.h"
#include "vtkInteractorObserver.h"
#include "vtkInteractorStyle.h"
#include "vtkCommand.h"
#include "vtkMatrix4x4.h"
#include "vtkRendererCollection.h"
#include "vtkLight.h"
#include "vtkWindowToImageFilter.h"
#include "vtkBMPWriter.h"
#include "vtkJPEGWriter.h"
#include "vtkTIFFWriter.h"
#include "vtkPostScriptWriter.h"
#include "vtkPNGWriter.h"
#include "vtkImageData.h"
#include "vtkImageExport.h"
#include "vtkImageAppend.h"
#include "vtkBMPReader.h"
#include "vtkMath.h"
#include "wx/filename.h"


#include <fstream>
//----------------------------------------------------------------------------
IMPLEMENT_DYNAMIC_CLASS(albaRWIBase, wxWindow)
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(albaRWIBase, wxWindow)
  EVT_PAINT(albaRWIBase::OnPaint)
  EVT_ERASE_BACKGROUND(albaRWIBase::OnEraseBackground)
  EVT_LEFT_DCLICK(albaRWIBase::OnLeftMouseDoubleClick)
  EVT_LEFT_DOWN(albaRWIBase::OnLeftMouseButtonDown)
  EVT_LEFT_UP(albaRWIBase::OnLeftMouseButtonUp)
  EVT_MIDDLE_DOWN(albaRWIBase::OnMiddleMouseButtonDown)
  EVT_MIDDLE_UP(albaRWIBase::OnMiddleMouseButtonUp)
	EVT_MOUSEWHEEL(albaRWIBase::OnMouseWheel)
  EVT_RIGHT_DOWN(albaRWIBase::OnRightMouseButtonDown)
  EVT_RIGHT_UP(albaRWIBase::OnRightMouseButtonUp)
  EVT_MOTION(albaRWIBase::OnMouseMotion)
  EVT_TIMER(ID_albaRWIBase_TIMER, albaRWIBase::OnTimer)
  EVT_KEY_DOWN(albaRWIBase::OnKeyDown)
  EVT_KEY_UP(albaRWIBase::OnKeyUp)
  EVT_CHAR(albaRWIBase::OnChar)
  EVT_SIZE(albaRWIBase::OnSize)
  EVT_IDLE(albaRWIBase::OnIdle)
	EVT_MOUSE_CAPTURE_LOST(albaRWIBase::OnMouseCaptureLost)
END_EVENT_TABLE()
//----------------------------------------------------------------------------
albaRWIBase::albaRWIBase() : wxWindow(), vtkRenderWindowInteractor(), m_Timer(this, ID_albaRWIBase_TIMER)
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
albaRWIBase::albaRWIBase(wxWindow *parent, wxWindowID id, const wxPoint &pos,
			 const wxSize &size, long style, const wxString &name)
  : wxWindow(parent, id, pos, size, style, name), vtkRenderWindowInteractor(), 
    m_Timer(this, ID_albaRWIBase_TIMER)
//----------------------------------------------------------------------------
{
  m_Hidden = true;
  this->Show(false);
	m_SaveDir = "";
  m_Width = m_Height = 10;
  
  m_Camera    = NULL;
  m_Mouse     = NULL;
  m_Listener  = NULL;
  
  m_StereoMovieDir     = "";
  m_StereoMovieFrameCounter = 0;
  m_StereoMovieLeftEye      = NULL;
  m_StereoMovieRightEye     = NULL;
  m_StereoImage             = NULL;
  m_StereoMoviewFrameWriter = NULL;
  m_StereoMovieEnable   = false;
  m_StereoFrameGenerate = false;

  m_LastX = 0;
  m_LastY = 0;

  m_CustomInteractorStyle = false;
}
//----------------------------------------------------------------------------
albaRWIBase::~albaRWIBase()
//----------------------------------------------------------------------------
{
  vtkDEL(m_StereoMovieLeftEye);
  vtkDEL(m_StereoMovieRightEye);
  vtkDEL(m_StereoImage);
  vtkDEL(m_StereoMoviewFrameWriter);

  this->SetRenderWindow(NULL);
}
//----------------------------------------------------------------------------
albaRWIBase * albaRWIBase::New()
//----------------------------------------------------------------------------
{
  return new albaRWIBase();
}
//----------------------------------------------------------------------------
void albaRWIBase::SetMouse(albaDeviceButtonsPadMouse *mouse)
//----------------------------------------------------------------------------
{
  m_Mouse = mouse;
}
//----------------------------------------------------------------------------
void albaRWIBase::Initialize()
//----------------------------------------------------------------------------
{
  // if don't have render window then stuck
  if (!RenderWindow)
  {
   // albaLogMessage("albaRWIBase::Initialize has no render window");
    return;
  }

#ifdef __WXMSW__
  if (RenderWindow->GetGenericWindowId() == 0)
    RenderWindow->SetWindowId( (HWND) this->GetHWND() );
#endif
#ifdef __WXGTK__
  // SIL ---- if (RenderWindow->GetGenericWindowId() == 0)
  // SIL ----   RenderWindow->SetParentId( (void *)(((GdkWindowPrivate *)GTK_PIZZA(m_wxwindow)->bin_window)->xwindow) );
  
  if (RenderWindow->GetGenericWindowId() == 0)
    RenderWindow->SetWindowId(  
      (void*) GDK_WINDOW_XWINDOW(GTK_PIZZA(m_wxwindow)->bin_window)     
    );    
#endif

//  #ifdef __WXMOTIF__
//     if (RenderWindow->GetGenericWindowId() == 0)
//     RenderWindow->SetWindowId( this->GetXWindow() );
//  #endif

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
void albaRWIBase::Enable()
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
void albaRWIBase::Disable()
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
void albaRWIBase::Start()
//----------------------------------------------------------------------------
{
  // the interactor cannot control the event loop
  albaLogMessage("albaRWIBase::Start() interactor cannot control event loop.");
}
//----------------------------------------------------------------------------
void albaRWIBase::UpdateSize(int x, int y)
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
int albaRWIBase::CreateTimer(int timertype)
//----------------------------------------------------------------------------
{
  // it's a one shot timer
  if (!m_Timer.Start(10, true))
    assert(false);
  return 1;
}
//----------------------------------------------------------------------------
int albaRWIBase::DestroyTimer()
//----------------------------------------------------------------------------
{
  // do nothing
  return 1;
}
//----------------------------------------------------------------------------
void albaRWIBase::TerminateApp()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaRWIBase::OnPaint(wxPaintEvent &event)
//----------------------------------------------------------------------------
{
  wxPaintDC pDC(this);

  //int w, h;
  //wxScrolledWindow::GetSize( &w,&h );
  //UpdateSize(w,h);
  //RenderWindow->SetSize(w, h);
  //vtkRenderWindowInteractor::SetSize(w, h);
  
  if(!RenderWindow) 
    return; //rare - may happen during Debug

  if(!Initialized) 
    Initialize();
  Render();
}
//----------------------------------------------------------------------------
void albaRWIBase::OnEraseBackground(wxEraseEvent &event)
//----------------------------------------------------------------------------
{
  event.Skip(false);
}
//----------------------------------------------------------------------------
void albaRWIBase::OnLeftMouseDoubleClick(wxMouseEvent &event)
//----------------------------------------------------------------------------
{
  if (!Enabled) return;

  CaptureMouse();
  NotifyClick(); // this will advise the albaGUIMDIChild of the click and it fires the VIEW_SELECT
                 // event. If it is necessary will be defined a NotifyDoubleClick with a new ID VIEW_DOUBLE_CLICKED

  if (m_CustomInteractorStyle)
  {
    // VTK does not support double click event, so manage it through the albaDeviceButtonsPadMouse
  }
  else
  {
    albaEventInteraction e(this,albaDeviceButtonsPadMouse::GetMouseDClickId());
    e.Set2DPosition(event.GetX(),m_Height - event.GetY() - 1);
    e.SetButton(ALBA_LEFT_BUTTON);
    e.SetModifier(ALBA_SHIFT_KEY,event.ShiftDown());
    e.SetModifier(ALBA_CTRL_KEY,event.ControlDown());
    e.SetModifier(ALBA_ALT_KEY,event.AltDown());
    e.SetChannel(MCH_OUTPUT);
    if(m_Mouse) 
      m_Mouse->OnEvent(&e);
    else if (m_Listener)
    {
      m_Listener->OnEvent(&e);
    }
  }
}
//----------------------------------------------------------------------------
void albaRWIBase::OnLeftMouseButtonDown(wxMouseEvent &event)
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
    albaEventInteraction e(this,albaDeviceButtonsPad::GetButtonDownId());
    e.Set2DPosition(event.GetX(),m_Height - event.GetY() - 1);
		e.SetButton(ALBA_LEFT_BUTTON);
		e.SetModifier(ALBA_SHIFT_KEY, event.ShiftDown());
    e.SetModifier(ALBA_CTRL_KEY,event.ControlDown());
    e.SetModifier(ALBA_ALT_KEY,event.AltDown());
    e.SetChannel(MCH_OUTPUT);
    if(m_Mouse) 
      m_Mouse->OnEvent(&e);
    else if (m_Listener)
    {
      m_Listener->OnEvent(&e);
    }
  }
}
//----------------------------------------------------------------------------
void albaRWIBase::OnMiddleMouseButtonDown(wxMouseEvent &event)
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
    albaEventInteraction e(this,albaDeviceButtonsPad::GetButtonDownId());
    e.Set2DPosition(event.GetX(),m_Height - event.GetY() - 1);
    e.SetButton(ALBA_MIDDLE_BUTTON);
    e.SetModifier(ALBA_SHIFT_KEY,event.ShiftDown());
    e.SetModifier(ALBA_CTRL_KEY,event.ControlDown());
    e.SetModifier(ALBA_ALT_KEY,event.AltDown());
    e.SetChannel(MCH_OUTPUT);
    if(m_Mouse) 
      m_Mouse->OnEvent(&e);
    else if (m_Listener)
    {
      m_Listener->OnEvent(&e);
    }
  }
}
//----------------------------------------------------------------------------
void albaRWIBase::OnRightMouseButtonDown(wxMouseEvent &event)
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
    albaEventInteraction e(this,albaDeviceButtonsPad::GetButtonDownId());
    e.Set2DPosition(event.GetX(),m_Height - event.GetY() - 1);
    e.SetButton(ALBA_RIGHT_BUTTON);
    e.SetModifier(ALBA_SHIFT_KEY,event.ShiftDown());
    e.SetModifier(ALBA_CTRL_KEY,event.ControlDown());
    e.SetModifier(ALBA_ALT_KEY,event.AltDown());
    e.SetChannel(MCH_OUTPUT);
    if(m_Mouse) 
      m_Mouse->OnEvent(&e);
    else if (m_Listener)
    {
      m_Listener->OnEvent(&e);
    }
  }
}
//----------------------------------------------------------------------------
void albaRWIBase::OnLeftMouseButtonUp(wxMouseEvent &event)
//----------------------------------------------------------------------------
{
  if (!Enabled) return;

  m_StereoFrameGenerate = false;

  if( GetCapture() == this )
    ReleaseMouse();
  
	if (m_CustomInteractorStyle)
  {
	  SetEventInformation(event.GetX(),m_Height - event.GetY() - 1,event.ControlDown(),event.ShiftDown());
	  InvokeEvent(vtkCommand::LeftButtonReleaseEvent,NULL);
  }
  else
  {
    albaEventInteraction e(this,albaDeviceButtonsPad::GetButtonUpId());
    e.Set2DPosition(event.GetX(),m_Height - event.GetY() - 1);
    e.SetButton(ALBA_LEFT_BUTTON);
    e.SetModifier(ALBA_SHIFT_KEY,event.ShiftDown());
    e.SetModifier(ALBA_CTRL_KEY,event.ControlDown());
    e.SetModifier(ALBA_ALT_KEY,event.AltDown());
    e.SetChannel(MCH_OUTPUT);
    if(m_Mouse) 
      m_Mouse->OnEvent(&e);
    else if (m_Listener)
    {
      m_Listener->OnEvent(&e);
    }
  }
}
//----------------------------------------------------------------------------
void albaRWIBase::OnMiddleMouseButtonUp(wxMouseEvent &event)
//----------------------------------------------------------------------------
{
  if (!Enabled) return;

  m_StereoFrameGenerate = false;

  if( GetCapture() == this )
    ReleaseMouse();

  if (m_CustomInteractorStyle)
  {
	  SetEventInformation(event.GetX(),m_Height - event.GetY() - 1,event.ControlDown(),event.ShiftDown());
	  InvokeEvent(vtkCommand::MiddleButtonReleaseEvent,NULL);
  }
  else
  {
    albaEventInteraction e(this,albaDeviceButtonsPad::GetButtonUpId());
    e.Set2DPosition(event.GetX(),m_Height - event.GetY() - 1);
    e.SetButton(ALBA_MIDDLE_BUTTON);
    e.SetModifier(ALBA_SHIFT_KEY,event.ShiftDown());
    e.SetModifier(ALBA_CTRL_KEY,event.ControlDown());
    e.SetModifier(ALBA_ALT_KEY,event.AltDown());
    e.SetChannel(MCH_OUTPUT);
    if(m_Mouse) 
      m_Mouse->OnEvent(&e);
    else if (m_Listener)
    {
      m_Listener->OnEvent(&e);
    }
  }
}
//----------------------------------------------------------------------------
void albaRWIBase::OnRightMouseButtonUp(wxMouseEvent &event)
//----------------------------------------------------------------------------
{
  if (!Enabled) return;

  m_StereoFrameGenerate = false;

  if( GetCapture() == this )
    ReleaseMouse();

  if (m_CustomInteractorStyle)
  {
    SetEventInformation(event.GetX(),m_Height - event.GetY() - 1,event.ControlDown(),event.ShiftDown());
    InvokeEvent(vtkCommand::RightButtonReleaseEvent,NULL);
  }
  else
  {
    albaEventInteraction e(this,albaDeviceButtonsPad::GetButtonUpId());
    e.Set2DPosition(event.GetX(),m_Height - event.GetY() - 1);
    e.SetButton(ALBA_RIGHT_BUTTON);
    e.SetModifier(ALBA_SHIFT_KEY,event.ShiftDown());
    e.SetModifier(ALBA_CTRL_KEY,event.ControlDown());
    e.SetModifier(ALBA_ALT_KEY,event.AltDown());
    e.SetChannel(MCH_OUTPUT);
    if(m_Mouse) 
      m_Mouse->OnEvent(&e);
    else if (m_Listener)
    {
      m_Listener->OnEvent(&e);
    }
  }
}
//----------------------------------------------------------------------------
void albaRWIBase::OnMouseMotion(wxMouseEvent &event)
//----------------------------------------------------------------------------
{
  if (!Enabled) return;

  if (m_LastX == event.GetX() && m_LastY == event.GetY())
  {
    return;
  }
  m_LastX = event.GetX();
  m_LastY = event.GetY();

  if (m_StereoMovieEnable && m_StereoFrameGenerate)
  {
    GenerateStereoFrames();
  }

  if (m_CustomInteractorStyle)
  {
    SetEventInformation(event.GetX(),m_Height - event.GetY() - 1,event.ControlDown(),event.ShiftDown());
    InvokeEvent(vtkCommand::MouseMoveEvent,NULL);
  }
  else
  {
    albaEventInteraction e(this,albaDeviceButtonsPadMouse::GetMouse2DMoveId());
    e.Set2DPosition(event.GetX(),m_Height - event.GetY() - 1);
    e.SetModifier(ALBA_SHIFT_KEY,event.ShiftDown());
    e.SetModifier(ALBA_CTRL_KEY,event.ControlDown());
    e.SetModifier(ALBA_ALT_KEY,event.AltDown());
    e.SetChannel(MCH_OUTPUT);
    if(m_Mouse) 
      m_Mouse->OnEvent(&e);
    else if (m_Listener)
    {
      m_Listener->OnEvent(&e);
    }
  }
}

//----------------------------------------------------------------------------
void albaRWIBase::OnMouseWheel(wxMouseEvent &event)
{
	if (!Enabled) return;

	bool rotateUp = event.GetWheelRotation() > 0;

	double delta;
	delta = event.GetWheelRotation();
	albaEventInteraction e(this, albaDeviceButtonsPadMouse::GetWheelId());
	e.Set2DPosition(event.GetX(), m_Height - event.GetY() - 1);
	e.SetData(&delta);

	e.SetModifier(ALBA_SHIFT_KEY, event.ShiftDown());
	e.SetModifier(ALBA_CTRL_KEY, event.ControlDown());
	e.SetModifier(ALBA_ALT_KEY, event.AltDown());
	e.SetChannel(MCH_OUTPUT);
	if (m_Mouse)
		m_Mouse->OnEvent(&e);
	else if (m_Listener)
	{
		m_Listener->OnEvent(&e);
	}
}

//----------------------------------------------------------------------------
void albaRWIBase::OnTimer(wxTimerEvent &event)
//----------------------------------------------------------------------------
{
  if (!Enabled) return;

  if (m_CustomInteractorStyle)
  {
    InvokeEvent(vtkCommand::TimerEvent,NULL);
  }
}
//----------------------------------------------------------------------------
void albaRWIBase::OnKeyDown(wxKeyEvent &event)
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
void albaRWIBase::OnKeyUp(wxKeyEvent &event)
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
void albaRWIBase::OnChar(wxKeyEvent &event)
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
      albaEvent e(this,albaDeviceButtonsPadMouse::GetMouseCharEventId(),(long) event.GetKeyCode());
      e.SetChannel(MCH_OUTPUT);
      m_Mouse->OnEvent(&e);
    }
  }

  event.Skip();
}
//----------------------------------------------------------------------------
void albaRWIBase::OnIdle(wxIdleEvent& event)
//----------------------------------------------------------------------------
{
  //if(m_Hidden)
  //{
  //   m_Hidden = false;
  //   this->Show(true);
  //}
}

//----------------------------------------------------------------------------
void albaRWIBase::OnMouseCaptureLost(wxMouseCaptureLostEvent& WXUNUSED(evt))
{
	wxMouseEvent evt = wxMouseEvent();
	evt.SetX(m_LastX);
	evt.SetY(m_LastY);
	OnLeftMouseButtonUp(evt);
}

//----------------------------------------------------------------------------
void albaRWIBase::NotifyClick()
//----------------------------------------------------------------------------
{
  m_StereoFrameGenerate = m_StereoMovieEnable;

  wxCommandEvent e(wxEVT_COMMAND_BUTTON_CLICKED, VIEW_CLICKED);
  e.SetEventObject(this);
  ProcessEvent(e);
}
//----------------------------------------------------------------------------
void albaRWIBase::OnSize(wxSizeEvent &event)
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
wxBitmap *albaRWIBase::GetImage(int magnification)
//----------------------------------------------------------------------------
{
	int dim[3];
	vtkRendererCollection * renderers = GetRenderWindow()->GetRenderers();

  GetRenderWindow()->OffScreenRenderingOn();
	  vtkALBASmartPointer<vtkWindowToImageFilter> w2i;
	  w2i->SetInput(GetRenderWindow());
    w2i->SetScale(magnification, magnification);
	  w2i->Update();
    w2i->GetOutput()->GetDimensions(dim);
  GetRenderWindow()->OffScreenRenderingOff();

	vtkRenderer * renderer;
	vtkCollectionSimpleIterator rsit;
	renderers->InitTraversal(rsit);
	renderer = renderers->GetNextRenderer(rsit);
	while (renderer)
	{
		renderer->SetRenderWindow(GetRenderWindow());
		renderer = renderers->GetNextRenderer(rsit);
	}

  assert( dim[0]>0 && dim[1]>0 );
  unsigned char *buffer = new unsigned char [dim[0]*dim[1]*3];

  //flip it - windows Bitmap are upside-down
  vtkALBASmartPointer<vtkImageExport> ie;
  ie->SetInputConnection(w2i->GetOutputPort());
  ie->ImageLowerLeftOff();
  ie->SetExportVoidPointer(buffer);
  ie->Export();

  //translate to a wxBitmap
  wxImage  *img = new wxImage(dim[0],dim[1],buffer,true);
  wxBitmap *bmp = new wxBitmap(*img,24);
  delete img;
  delete buffer;
  return bmp;
}
//----------------------------------------------------------------------------
void albaRWIBase::SaveImage(albaString filename, int magnification)
//---------------------------------------------------------------------------
{
  wxString path, name, ext;
  wxFileName::SplitPath(filename.GetCStr(),&path,&name,&ext);
  if (filename.IsEmpty() || ext.IsEmpty())
  {
    wxString wildc = "Image (*.bmp)|*.bmp|Image (*.jpg)|*.jpg|Image (*.png)|*.png|Image (*.ps)|*.ps|Image (*.tiff)|*.tiff";
    wxString file = "";

    file.Append(filename);
    file = albaGetSaveFile(file,wildc).ToAscii(); 
    if(file.IsEmpty()) 
      return;
    filename = file;
  }

  wxString temporary = filename.GetCStr();
  temporary = temporary.AfterLast('\\').AfterFirst('.');


  albaString basename = filename.BaseName();
  if (basename.IsEmpty())
  {
    filename = m_SaveDir << "\\" << filename;
  }
  
  ::wxBeginBusyCursor();

  long pixelXMeterX = 0;
  long pixelXMeterY = 0;
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
        //wxMessageBox(albaString::Format("%d", ren->GetActiveCamera()->GetParallelProjection()));
        double wp0x[4], wp1x[4];
        ren->SetDisplayPoint(0,0,0); //x
        ren->DisplayToWorld();
        ren->GetWorldPoint(wp0x);

        ren->SetDisplayPoint(10,0,0); //x
        ren->DisplayToWorld();
        ren->GetWorldPoint(wp1x);

        double pixelSpacingX = sqrt(vtkMath::Distance2BetweenPoints(wp1x,wp0x))/10;
        double meter = 1000; //millimeters
        pixelXMeterX = meter/pixelSpacingX;
        //wxMessageBox(albaString::Format("pixelXMeter = %f", meter/pixelSpacingX));

        double wp0y[4], wp1y[4];
        ren->SetDisplayPoint(0,0,0); //y
        ren->DisplayToWorld();
        ren->GetWorldPoint(wp0y);

        ren->SetDisplayPoint(0,10,0); //y
        ren->DisplayToWorld();
        ren->GetWorldPoint(wp1y);

        double pixelSpacingY = sqrt(vtkMath::Distance2BetweenPoints(wp1y,wp0y))/10;
        pixelXMeterY = meter/pixelSpacingY;
      }
    }

  }

  vtkALBASmartPointer<vtkWindowToImageFilter> w2i;
  w2i->SetInput(GetRenderWindow());
  w2i->SetScale(magnification,magnification);
  w2i->Update();
  
  wxFileName::SplitPath(filename.GetCStr(),&path,&name,&ext);
  ext.MakeLower();
  if (ext == "bmp")
  {
    vtkALBASmartPointer<vtkBMPWriter> w;
    w->SetInputConnection(w2i->GetOutputPort());
    w->SetFileName(filename.GetCStr());
    w->Write();

    // bitmap header
    /*
    offset    size    description
      0       2       signature, must be 4D42 hex
      2       4       size of BMP file in bytes (unreliable)
      6       2       reserved, must be zero
      8       2       reserved, must be zero
      10      4       offset to start of image data in bytes
      14      4       size of BITMAPINFOHEADER structure, must be 40
      18      4       image width in pixels
      22      4       image height in pixels
      26      2       number of planes in the image, must be 1
      28      2       number of bits per pixel (1, 4, 8, or 24)
      30      4       compression type (0=none, 1=RLE-8, 2=RLE-4)
      34      4       size of image data in bytes (including padding)
    * 38      4       horizontal resolution in pixels per meter (unreliable)
    * 42      4       vertical resolution in pixels per meter (unreliable)
      46      4       number of colors in image, or zero
      50      4       number of important colors, or zero

    * needed values
    */
#ifdef WIN32
    //open file
    fstream myFile (filename.GetCStr(), ios::in | ios::out | ios::binary);
    myFile.seekp (38);
    myFile.write ((char*)&pixelXMeterX, 4);
    myFile.write ((char*)&pixelXMeterY, 4);
    myFile.close();

    FILE *fp = fopen(filename.GetCStr(), "rb");
    if(!fp) return ;

    BITMAPFILEHEADER bmfh={0};
    BITMAPINFOHEADER bmih={0};
//    DWORD index;
//    BYTE ch;

    fread(&bmfh,sizeof(bmfh),1,fp);  //read bitmap file header
    fread(&bmih,sizeof(bmih),1,fp);  //read bitmap info header 
    fclose(fp);
#endif
  }
  else if (ext == "jpg")
  {
    vtkALBASmartPointer<vtkJPEGWriter> w;
    w->SetInputConnection(w2i->GetOutputPort());
    w->SetFileName(filename.GetCStr());
    w->Write();
  }
  else if (ext == "tiff")
  {
    vtkALBASmartPointer<vtkTIFFWriter> w;
    w->SetInputConnection(w2i->GetOutputPort());
    w->SetFileName(filename.GetCStr());
    w->Write();
  }
  else if (ext == "ps")
  {
    vtkALBASmartPointer<vtkPostScriptWriter> w;
    w->SetInputConnection(w2i->GetOutputPort());
    w->SetFileName(filename.GetCStr());
    w->Write();
  }
  else if (ext == "png")
  {
    vtkALBASmartPointer<vtkPNGWriter> w;
    w->SetInputConnection(w2i->GetOutputPort());
    w->SetFileName(filename.GetCStr());
    w->Write();
  }
  else
  {
    wxMessageBox(_("Image can not be saved. Not valid file!"), _("Warning"));
  }
  ::wxEndBusyCursor();
}
//----------------------------------------------------------------------------
void albaRWIBase::SaveImageRecursive(albaString filename, albaViewCompound *v,int magnification)
//----------------------------------------------------------------------------
{
  if(v == NULL) return;

  wxString path, name, ext;
  wxFileName::SplitPath(filename.GetCStr(),&path,&name,&ext);
  if (filename.IsEmpty() || ext.IsEmpty())
  {
    //wxString wildc = "Image (*.bmp)|*.bmp|Image (*.jpg)|*.jpg";
    wxString wildc = "Image (*.bmp)|*.bmp|Image (*.jpg)|*.jpg|Image (*.png)|*.png|Image (*.ps)|*.ps|Image (*.tiff)|*.tiff";
    wxString file = albaString::Format("%s\\%sSnapshot", m_SaveDir.GetCStr(),filename.GetCStr());
  
		//albaString file ;
    if(!wxDirExists(path))
    {
      file = m_SaveDir;
      file +=  "\\";
      filename = name;
    }

    file.Append(filename);
    file = albaGetSaveFile(file,wildc).ToAscii(); 
    if(file.IsEmpty()) 
      return;
    filename = file;
  }
  
  wxString temporary = filename.GetCStr();
  temporary = temporary.AfterLast('\\').AfterFirst('.');

 
  albaString basename = filename.BaseName();
  if (basename.IsEmpty())
  {
    filename = m_SaveDir << "\\" << filename;
  }


  RecursiveSaving(filename, v, magnification);
}
//----------------------------------------------------------------------------
void albaRWIBase::RecursiveSaving(albaString filename, albaViewCompound *v,int magnification)
//----------------------------------------------------------------------------
{
  for(int i=0; i< v->GetNumberOfSubView(); i++)
  {
    albaView *currentView;
    currentView = v->GetSubView(i);
    if(albaViewCompound::SafeDownCast(currentView) != NULL)
    {
      wxString subViewString;
      subViewString.Append(filename);
      wxString pathName, fileName, extension;
      wxFileName::SplitPath(subViewString,&pathName,&fileName,&extension);

      subViewString.Clear();
      subViewString.Append(pathName);
      subViewString.Append("\\");
      subViewString.Append(fileName);
      subViewString.Append("_");
      subViewString.Append(currentView->GetLabel());
      subViewString.Append(".");
      subViewString.Append(extension);

      RecursiveSaving(subViewString, albaViewCompound::SafeDownCast(currentView), magnification);
    }
    else
    {
      ///////////////////////////////////
      wxString temp, pathName, fileName, extension;
      temp.Append(filename);
      wxFileName::SplitPath(temp,&pathName,&fileName,&extension);
      fileName.Append(albaString::Format("_%d", i));
      temp.Clear();
      temp.Append(pathName);
      temp.Append("\\");
      temp.Append(fileName);
      temp.Append(".");
      temp.Append(extension);

      ::wxBeginBusyCursor();

      long pixelXMeterX = 0;
      long pixelXMeterY = 0;
      vtkRenderWindow *rw = currentView->GetRWI()->GetRenderWindow();
      if(rw)
      {
        vtkRendererCollection *rc = rw->GetRenderers();
        if(rc)
        {
          rc->InitTraversal();
          vtkRenderer *ren = rc->GetNextItem();
          if(ren)
          {
            //wxMessageBox(albaString::Format("%d", ren->GetActiveCamera()->GetParallelProjection()));
            double wp0x[4], wp1x[4];
            ren->SetDisplayPoint(0,0,0); //x
            ren->DisplayToWorld();
            ren->GetWorldPoint(wp0x);

            ren->SetDisplayPoint(10,0,0); //x
            ren->DisplayToWorld();
            ren->GetWorldPoint(wp1x);

            double pixelSpacingX = sqrt(vtkMath::Distance2BetweenPoints(wp1x,wp0x))/10;
            double meter = 1000; //millimeters
            pixelXMeterX = meter/pixelSpacingX;
            //wxMessageBox(albaString::Format("pixelXMeter = %f", meter/pixelSpacingX));

            double wp0y[4], wp1y[4];
            ren->SetDisplayPoint(0,0,0); //y
            ren->DisplayToWorld();
            ren->GetWorldPoint(wp0y);

            ren->SetDisplayPoint(0,10,0); //y
            ren->DisplayToWorld();
            ren->GetWorldPoint(wp1y);

            double pixelSpacingY = sqrt(vtkMath::Distance2BetweenPoints(wp1y,wp0y))/10;
            pixelXMeterY = meter/pixelSpacingY;
          }
        }

      }
      currentView->GetRWI()->GetRenderWindow()->OffScreenRenderingOn();
      vtkALBASmartPointer<vtkWindowToImageFilter> w2i;
      w2i->SetInput(currentView->GetRWI()->GetRenderWindow());
      w2i->SetScale(magnification,magnification);
      w2i->Update();
      currentView->GetRWI()->GetRenderWindow()->OffScreenRenderingOff();
      
      extension.MakeLower();
      if (extension == "bmp")
      {
        vtkALBASmartPointer<vtkBMPWriter> w;
        w->SetInputConnection(w2i->GetOutputPort());
        w->SetFileName(temp.ToAscii());
        w->Write();

        // bitmap header
        /*
        offset    size    description
        0       2       signature, must be 4D42 hex
        2       4       size of BMP file in bytes (unreliable)
        6       2       reserved, must be zero
        8       2       reserved, must be zero
        10      4       offset to start of image data in bytes
        14      4       size of BITMAPINFOHEADER structure, must be 40
        18      4       image width in pixels
        22      4       image height in pixels
        26      2       number of planes in the image, must be 1
        28      2       number of bits per pixel (1, 4, 8, or 24)
        30      4       compression type (0=none, 1=RLE-8, 2=RLE-4)
        34      4       size of image data in bytes (including padding)
        * 38      4       horizontal resolution in pixels per meter (unreliable)
        * 42      4       vertical resolution in pixels per meter (unreliable)
        46      4       number of colors in image, or zero
        50      4       number of important colors, or zero

        * needed values
        */
#ifdef WIN32
        //open file
        fstream myFile (temp.ToAscii(), ios::in | ios::out | ios::binary);
        myFile.seekp (38);
        myFile.write ((char*)&pixelXMeterX, 4);
        myFile.write ((char*)&pixelXMeterY, 4);
        myFile.close();

        FILE *fp = fopen(temp.ToAscii(), "rb");
        if(!fp) return ;

        BITMAPFILEHEADER bmfh={0};
        BITMAPINFOHEADER bmih={0};
        //    DWORD index;
        //    BYTE ch;

        fread(&bmfh,sizeof(bmfh),1,fp);  //read bitmap file header
        fread(&bmih,sizeof(bmih),1,fp);  //read bitmap info header 
        fclose(fp);
#endif
      }
      else if (extension == "jpg")
      {
        vtkALBASmartPointer<vtkJPEGWriter> w;
        w->SetInputConnection(w2i->GetOutputPort());
        w->SetFileName(temp.ToAscii());
        w->Write();
      }
      else if (extension == "tiff")
      {
        vtkALBASmartPointer<vtkTIFFWriter> w;
        w->SetInputConnection(w2i->GetOutputPort());
        w->SetFileName(temp.ToAscii());
        w->Write();
      }
      else if (extension == "ps")
      {
        vtkALBASmartPointer<vtkPostScriptWriter> w;
        w->SetInputConnection(w2i->GetOutputPort());
        w->SetFileName(temp.ToAscii());
        w->Write();
      }
      else if (extension == "png")
      {
        vtkALBASmartPointer<vtkPNGWriter> w;
        w->SetInputConnection(w2i->GetOutputPort());
        w->SetFileName(temp.ToAscii());
        w->Write();
      }
      else
      {
        wxMessageBox(_("Image can not be saved. Not valid file!"), _("Warning"));
      }
      ::wxEndBusyCursor();
      ///////////////////////////////////
    }
    

    
  }
}
//----------------------------------------------------------------------------
void albaRWIBase::SaveAllImages(albaString filename, albaViewCompound *v)
//---------------------------------------------------------------------------
{
  if(v == NULL) return;
  wxString path, name, ext;
  wxFileName::SplitPath(filename.GetCStr(),&path,&name,&ext);
  if (filename.IsEmpty() || ext.IsEmpty())
  {
    albaString wildc = "Image (*.jpg)|*.jpg|Image (*.bmp)|*.bmp|Image (*.png)|*.png";
    albaString file;
    if(!wxDirExists(path))
    {
      file = m_SaveDir;
      file +=  "\\";
      filename = albaString(name);
    }
    file.Append(filename);
    file = albaGetSaveFile(file,wildc); 
    if(file.IsEmpty())
      return;
    filename = file;
  }

  wxString temporary = filename.GetCStr();
  temporary = temporary.AfterLast('\\').AfterFirst('.');


  albaString basename = filename.BaseName();
  if (basename.IsEmpty())
  {
    filename = m_SaveDir << "\\" << filename;
  }

  ::wxBeginBusyCursor();

  wxBitmap imageBitmap;
  v->GetImage(imageBitmap);

  wxFileName::SplitPath(filename.GetCStr(),&path,&name,&ext);
  ext.MakeLower();
  if (ext == "bmp")
  {
    imageBitmap.SaveFile(filename.GetCStr(), wxBITMAP_TYPE_BMP);
  }
  else if (ext == "jpg")
  {
    wxJPEGHandler *jpegHandler = new wxJPEGHandler();
    jpegHandler->SetName("JPEGHANDLER");
    wxImage::AddHandler(jpegHandler);
    wxImage image = imageBitmap.ConvertToImage();
    image.SetOption(_("quality"), 100);
    image.SaveFile(filename.GetCStr(), wxBITMAP_TYPE_JPEG);
    wxImage::RemoveHandler("JPEGHANDLER");
  }
  else if (ext == "png")
  {
    /*wxPNGHandler *pngHandler = new wxPNGHandler();
    pngHandler->SetName("PNGHANDLER");
    wxImage::AddHandler(pngHandler);
    wxImage image = imageBitmap.ConvertToImage();
    image.SetOption(_("quality"), 100);
    image.SaveFile(filename.GetCStr(), wxBITMAP_TYPE_PNG);
    wxImage::RemoveHandler("PNGHANDLER");*/
    
    std::string fn = filename.GetCStr();
    fn = fn.substr(0,fn.size()-3);
    fn.append("bmp");
    //imageBitmap.SetDepth(24);
    imageBitmap.SaveFile(fn.c_str(), wxBITMAP_TYPE_BMP);
    
    vtkBMPReader *r = vtkBMPReader::New();
    r->SetFileName(fn.c_str());
    r->Update();

    vtkPNGWriter *w = vtkPNGWriter::New();
    w->SetInputConnection(r->GetOutputPort());
    w->SetFileName(filename.GetCStr());
    w->Write();

    r->Delete();
    w->Delete();

    wxRemoveFile(fn.c_str());
  }
  else
  {
    wxMessageBox("Must save with JPG, BMP of PNG extension");
  }
  ::wxEndBusyCursor();
}
//----------------------------------------------------------------------------
vtkCamera* albaRWIBase::GetCamera()
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
void albaRWIBase::SetInteractorStyle(vtkInteractorObserver *o)
//---------------------------------------------------------------------------
{
  vtkRenderWindowInteractor::SetInteractorStyle(o);
  m_CustomInteractorStyle = o != NULL;
}
//----------------------------------------------------------------------------
void albaRWIBase::SetStereoMovieDirectory(const char *dir)
//----------------------------------------------------------------------------
{
  m_StereoMovieDir = dir;
}
//----------------------------------------------------------------------------
void albaRWIBase::GenerateStereoFrames()
//----------------------------------------------------------------------------
{
  RenderWindow->SetStereoTypeToLeft();
  RenderWindow->Render();
  m_StereoMovieLeftEye->Modified();
  m_StereoMovieLeftEye->Update();
  RenderWindow->SetStereoTypeToRight();
  RenderWindow->Render();
  m_StereoMovieRightEye->Modified();
  m_StereoMovieRightEye->Update();
  m_StereoImage->Modified();
  m_StereoImage->Update();
  albaString filename;
  filename = m_StereoMovieDir;
  filename += "\\movie_";
  filename += albaString::Format("%05d",m_StereoMovieFrameCounter);
  filename += ".png";
  m_StereoMoviewFrameWriter->SetFileName(filename.GetCStr());
  m_StereoMoviewFrameWriter->Write();
  m_StereoMovieFrameCounter++;
}
//----------------------------------------------------------------------------
void albaRWIBase::EnableStereoMovie(bool enable)
//----------------------------------------------------------------------------
{
  m_StereoMovieEnable = enable;

  if (m_StereoMovieEnable && m_StereoMovieLeftEye == NULL)
  {
    // Build Stereo Movie pipeline
    vtkNEW(m_StereoMovieLeftEye);
    m_StereoMovieLeftEye->SetInput(RenderWindow);

    vtkNEW(m_StereoMovieRightEye);
    m_StereoMovieRightEye->SetInput(RenderWindow);

    vtkNEW(m_StereoImage);
    m_StereoImage->AddInputConnection(m_StereoMovieLeftEye->GetOutputPort());
    m_StereoImage->AddInputConnection(m_StereoMovieRightEye->GetOutputPort());

    vtkNEW(m_StereoMoviewFrameWriter);
    m_StereoMoviewFrameWriter->SetInputConnection(m_StereoImage->GetOutputPort());
  }
}
