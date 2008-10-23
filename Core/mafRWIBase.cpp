/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafRWIBase.cpp,v $
  Language:  C++
  Date:      $Date: 2008-10-23 08:50:34 $
  Version:   $Revision: 1.33.2.1 $
  Authors:   Silvano Imboden - Paolo Quadrani - Daniele Giunchi (Save Image)
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
#include "mafDecl.h"

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

#include "mafGUIApplicationSettings.h"
#include "mafDevice.h"
#include "mmdButtonsPad.h"
#include "mmdMouse.h"
#include "mafEventInteraction.h"
#include "mafEvent.h"

#include "vtkMAFSmartPointer.h"
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
#include "vtkMath.h"

#include <fstream>
//----------------------------------------------------------------------------
IMPLEMENT_DYNAMIC_CLASS(mafRWIBase, wxWindow)
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(mafRWIBase, wxWindow)
  EVT_PAINT(mafRWIBase::OnPaint)
  EVT_ERASE_BACKGROUND(mafRWIBase::OnEraseBackground)
  EVT_LEFT_DCLICK(mafRWIBase::OnLeftMouseDoubleClick)
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
mafRWIBase::mafRWIBase() : wxWindow(), vtkRenderWindowInteractor(), m_Timer(this, ID_mafRWIBase_TIMER)
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafRWIBase::mafRWIBase(wxWindow *parent, wxWindowID id, const wxPoint &pos,
			 const wxSize &size, long style, const wxString &name)
  : wxWindow(parent, id, pos, size, style, name), vtkRenderWindowInteractor(), 
    m_Timer(this, ID_mafRWIBase_TIMER)
//----------------------------------------------------------------------------
{
  m_Hidden = true;
  this->Show(false);
	//m_SaveDir = ::wxGetHomeDir().c_str(); 
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
mafRWIBase::~mafRWIBase()
//----------------------------------------------------------------------------
{
  vtkDEL(m_StereoMovieLeftEye);
  vtkDEL(m_StereoMovieRightEye);
  vtkDEL(m_StereoImage);
  vtkDEL(m_StereoMoviewFrameWriter);

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
   // mafLogMessage("mafRWIBase::Initialize has no render window");
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
  mafLogMessage("mafRWIBase::Start() interactor cannot control event loop.");
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
  if (!m_Timer.Start(10, TRUE))
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
  
  if(!RenderWindow) 
    return; //rare - may happen during Debug

  if(!Initialized) 
    Initialize();
  Render();
}
//----------------------------------------------------------------------------
void mafRWIBase::OnEraseBackground(wxEraseEvent &event)
//----------------------------------------------------------------------------
{
  event.Skip(false);
}
//----------------------------------------------------------------------------
void mafRWIBase::OnLeftMouseDoubleClick(wxMouseEvent &event)
//----------------------------------------------------------------------------
{
  if (!Enabled) return;

  CaptureMouse();
  NotifyClick(); // this will advise the mafGUIMDIChild of the click and it fires the VIEW_SELECT
                 // event. If it is necessary will be defined a NotifyDoubleClick with a new ID VIEW_DOUBLE_CLICKED

  if (m_CustomInteractorStyle)
  {
    // VTK does not support double click event, so manage it through the mmdMouse
  }
  else
  {
    mafEventInteraction e(this,mmdMouse::MOUSE_DCLICK);
    e.Set2DPosition(event.GetX(),m_Height - event.GetY() - 1);
    e.SetButton(MAF_LEFT_BUTTON);
    e.SetModifier(MAF_SHIFT_KEY,event.ShiftDown());
    e.SetModifier(MAF_CTRL_KEY,event.ControlDown());
    e.SetModifier(MAF_ALT_KEY,event.AltDown());
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
    if(m_Mouse) 
      m_Mouse->OnEvent(&e);
    else if (m_Listener)
    {
      m_Listener->OnEvent(&e);
    }
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
    if(m_Mouse) 
      m_Mouse->OnEvent(&e);
    else if (m_Listener)
    {
      m_Listener->OnEvent(&e);
    }
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
    if(m_Mouse) 
      m_Mouse->OnEvent(&e);
    else if (m_Listener)
    {
      m_Listener->OnEvent(&e);
    }
  }
}
//----------------------------------------------------------------------------
void mafRWIBase::OnLeftMouseButtonUp(wxMouseEvent &event)
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
    mafEventInteraction e(this,mmdButtonsPad::BUTTON_UP);
    e.Set2DPosition(event.GetX(),m_Height - event.GetY() - 1);
    e.SetButton(MAF_LEFT_BUTTON);
    e.SetModifier(MAF_SHIFT_KEY,event.ShiftDown());
    e.SetModifier(MAF_CTRL_KEY,event.ControlDown());
    e.SetModifier(MAF_ALT_KEY,event.AltDown());
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
void mafRWIBase::OnMiddleMouseButtonUp(wxMouseEvent &event)
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
    mafEventInteraction e(this,mmdButtonsPad::BUTTON_UP);
    e.Set2DPosition(event.GetX(),m_Height - event.GetY() - 1);
    e.SetButton(MAF_MIDDLE_BUTTON);
    e.SetModifier(MAF_SHIFT_KEY,event.ShiftDown());
    e.SetModifier(MAF_CTRL_KEY,event.ControlDown());
    e.SetModifier(MAF_ALT_KEY,event.AltDown());
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
void mafRWIBase::OnRightMouseButtonUp(wxMouseEvent &event)
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
    mafEventInteraction e(this,mmdButtonsPad::BUTTON_UP);
    e.Set2DPosition(event.GetX(),m_Height - event.GetY() - 1);
    e.SetButton(MAF_RIGHT_BUTTON);
    e.SetModifier(MAF_SHIFT_KEY,event.ShiftDown());
    e.SetModifier(MAF_CTRL_KEY,event.ControlDown());
    e.SetModifier(MAF_ALT_KEY,event.AltDown());
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
void mafRWIBase::OnMouseMotion(wxMouseEvent &event)
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
    mafEventInteraction e(this,mmdMouse::MOUSE_2D_MOVE);
    e.Set2DPosition(event.GetX(),m_Height - event.GetY() - 1);
    e.SetModifier(MAF_SHIFT_KEY,event.ShiftDown());
    e.SetModifier(MAF_CTRL_KEY,event.ControlDown());
    e.SetModifier(MAF_ALT_KEY,event.AltDown());
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
  m_StereoFrameGenerate = m_StereoMovieEnable;

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
wxBitmap *mafRWIBase::GetImage(int magnification)
//----------------------------------------------------------------------------
{
	int dim[3];
  GetRenderWindow()->OffScreenRenderingOn();
	  vtkMAFSmartPointer<vtkWindowToImageFilter> w2i;
	  w2i->SetInput(GetRenderWindow());
    w2i->SetMagnification(magnification);
	  w2i->Update();
    w2i->GetOutput()->GetDimensions(dim);
  GetRenderWindow()->OffScreenRenderingOff();

  assert( dim[0]>0 && dim[1]>0 );
  unsigned char *buffer = new unsigned char [dim[0]*dim[1]*3];

  //flip it - windows Bitmap are upside-down
  vtkMAFSmartPointer<vtkImageExport> ie;
  ie->SetInput(w2i->GetOutput());
  ie->ImageLowerLeftOff();
  ie->SetExportVoidPointer(buffer);
  ie->Export();

  //translate to a wxBitmap
  wxImage  *img = new wxImage(dim[0],dim[1],buffer,TRUE);
  wxBitmap *bmp = new wxBitmap(img);
  delete img;
  delete buffer;
  return bmp;
}
//----------------------------------------------------------------------------
void mafRWIBase::SaveImage(mafString filename, int magnification , int forceExtension)
//---------------------------------------------------------------------------
{
  wxString path, name, ext;
  wxSplitPath(filename.GetCStr(),&path,&name,&ext);
  if (filename.IsEmpty() || ext.IsEmpty())
  {
    wxString wildc = "Image (*.bmp)|*.bmp|Image (*.jpg)|*.jpg|Image (*.png)|*.png|Image (*.ps)|*.ps|Image (*.tiff)|*.tiff";
//    wxString file = wxString::Format("%s\\%sSnapshot", m_SaveDir.GetCStr(),filename.GetCStr());
    wxString file = "";
    switch(forceExtension)
    {
      case mafGUIApplicationSettings::JPG :
        wildc = "Image (*.jpg)|*.jpg";
      break;
      case mafGUIApplicationSettings::BMP:
        wildc = "Image (*.bmp)|*.bmp";
      break;
    }
/*
    if(!wxDirExists(path))
    {
      file = m_SaveDir;
      file +=  "\\";
      filename = name.c_str();
    }
*/    
    file.Append(filename);
    file = mafGetSaveFile(file,wildc).c_str(); 
    if(file.IsEmpty()) 
      return;
    filename = file.c_str();
  }

  wxString temporary = filename.GetCStr();
  temporary = temporary.AfterLast('\\').AfterFirst('.');

  switch(forceExtension)
  {
    case mafGUIApplicationSettings::JPG :
    if(temporary != _("jpg"))
      filename += _(".jpg");
    break;
    case mafGUIApplicationSettings::BMP:
    if(temporary != _("bmp"))
      filename += _(".bmp");
    break;
  }

  mafString basename = filename.BaseName();
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
        //wxMessageBox(wxString::Format("%d", ren->GetActiveCamera()->GetParallelProjection()));
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
        //wxMessageBox(wxString::Format("pixelXMeter = %f", meter/pixelSpacingX));

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

  GetRenderWindow()->OffScreenRenderingOn();
  vtkMAFSmartPointer<vtkWindowToImageFilter> w2i;
  w2i->SetInput(GetRenderWindow());
  w2i->SetMagnification(magnification);
  w2i->Update();
  GetRenderWindow()->OffScreenRenderingOff();
  
  wxSplitPath(filename.GetCStr(),&path,&name,&ext);
  ext.MakeLower();
  if (ext == "bmp")
  {
    vtkMAFSmartPointer<vtkBMPWriter> w;
    w->SetInput(w2i->GetOutput());
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
    vtkMAFSmartPointer<vtkJPEGWriter> w;
    w->SetInput(w2i->GetOutput());
    w->SetFileName(filename.GetCStr());
    w->Write();
  }
  else if (ext == "tiff")
  {
    vtkMAFSmartPointer<vtkTIFFWriter> w;
    w->SetInput(w2i->GetOutput());
    w->SetFileName(filename.GetCStr());
    w->Write();
  }
  else if (ext == "ps")
  {
    vtkMAFSmartPointer<vtkPostScriptWriter> w;
    w->SetInput(w2i->GetOutput());
    w->SetFileName(filename.GetCStr());
    w->Write();
  }
  else if (ext == "png")
  {
    vtkMAFSmartPointer<vtkPNGWriter> w;
    w->SetInput(w2i->GetOutput());
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
void mafRWIBase::SaveImageRecursive(mafString filename, mafViewCompound *v,int magnification,int forceExtension)
//----------------------------------------------------------------------------
{
  if(v == NULL) return;

  wxString path, name, ext;
  wxSplitPath(filename.GetCStr(),&path,&name,&ext);
  if (filename.IsEmpty() || ext.IsEmpty())
  {
    //wxString wildc = "Image (*.bmp)|*.bmp|Image (*.jpg)|*.jpg";
    wxString wildc = "Image (*.bmp)|*.bmp|Image (*.jpg)|*.jpg|Image (*.png)|*.png|Image (*.ps)|*.ps|Image (*.tiff)|*.tiff";
    wxString file = wxString::Format("%s\\%sSnapshot", m_SaveDir.GetCStr(),filename.GetCStr());
    switch(forceExtension)
    {
    case mafGUIApplicationSettings::JPG :
      wildc = "Image (*.jpg)|*.jpg";
      break;
    case mafGUIApplicationSettings::BMP:
      wildc = "Image (*.bmp)|*.bmp";
      break;
    }
    //mafString file ;
    if(!wxDirExists(path))
    {
      file = m_SaveDir;
      file +=  "\\";
      filename = name.c_str();
    }

    file.Append(filename);
    file = mafGetSaveFile(file,wildc).c_str(); 
    if(file.IsEmpty()) 
      return;
    filename = file.c_str();
  }
  
  wxString temporary = filename.GetCStr();
  temporary = temporary.AfterLast('\\').AfterFirst('.');

  switch(forceExtension)
  {
  case mafGUIApplicationSettings::JPG :
    if(temporary != _("jpg"))
      filename += _(".jpg");
    break;
  case mafGUIApplicationSettings::BMP:
    if(temporary != _("bmp"))
      filename += _(".bmp");
    break;
  }

  mafString basename = filename.BaseName();
  if (basename.IsEmpty())
  {
    filename = m_SaveDir << "\\" << filename;
  }


  RecursiveSaving(filename, v, magnification);
}
//----------------------------------------------------------------------------
void mafRWIBase::RecursiveSaving(mafString filename, mafViewCompound *v,int magnification)
//----------------------------------------------------------------------------
{
  for(int i=0; i< v->GetNumberOfSubView(); i++)
  {
    mafView *currentView;
    currentView = v->GetSubView(i);
    if(mafViewCompound::SafeDownCast(currentView) != NULL)
    {
      wxString subViewString;
      subViewString.Append(filename);
      wxString pathName, fileName, extension;
      wxSplitPath(subViewString,&pathName,&fileName,&extension);

      subViewString.Clear();
      subViewString.Append(pathName);
      subViewString.Append("\\");
      subViewString.Append(fileName);
      subViewString.Append("_");
      subViewString.Append(currentView->GetLabel());
      subViewString.Append(".");
      subViewString.Append(extension);

      RecursiveSaving(subViewString, mafViewCompound::SafeDownCast(currentView), magnification);
    }
    else
    {
      ///////////////////////////////////
      wxString temp, pathName, fileName, extension;
      temp.Append(filename);
      wxSplitPath(temp,&pathName,&fileName,&extension);
      fileName.Append(wxString::Format("_%d", i));
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
            //wxMessageBox(wxString::Format("%d", ren->GetActiveCamera()->GetParallelProjection()));
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
            //wxMessageBox(wxString::Format("pixelXMeter = %f", meter/pixelSpacingX));

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
      vtkMAFSmartPointer<vtkWindowToImageFilter> w2i;
      w2i->SetInput(currentView->GetRWI()->GetRenderWindow());
      w2i->SetMagnification(magnification);
      w2i->Update();
      currentView->GetRWI()->GetRenderWindow()->OffScreenRenderingOff();
      
      extension.MakeLower();
      if (extension == "bmp")
      {
        vtkMAFSmartPointer<vtkBMPWriter> w;
        w->SetInput(w2i->GetOutput());
        w->SetFileName(temp.c_str());
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
        fstream myFile (temp.c_str(), ios::in | ios::out | ios::binary);
        myFile.seekp (38);
        myFile.write ((char*)&pixelXMeterX, 4);
        myFile.write ((char*)&pixelXMeterY, 4);
        myFile.close();

        FILE *fp = fopen(temp.c_str(), "rb");
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
        vtkMAFSmartPointer<vtkJPEGWriter> w;
        w->SetInput(w2i->GetOutput());
        w->SetFileName(temp.c_str());
        w->Write();
      }
      else if (extension == "tiff")
      {
        vtkMAFSmartPointer<vtkTIFFWriter> w;
        w->SetInput(w2i->GetOutput());
        w->SetFileName(temp.c_str());
        w->Write();
      }
      else if (extension == "ps")
      {
        vtkMAFSmartPointer<vtkPostScriptWriter> w;
        w->SetInput(w2i->GetOutput());
        w->SetFileName(temp.c_str());
        w->Write();
      }
      else if (extension == "png")
      {
        vtkMAFSmartPointer<vtkPNGWriter> w;
        w->SetInput(w2i->GetOutput());
        w->SetFileName(temp.c_str());
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
void mafRWIBase::SaveAllImages(mafString filename, mafViewCompound *v, int forceExtension)
//---------------------------------------------------------------------------
{
  if(v == NULL) return;
  wxString path, name, ext;
  wxSplitPath(filename,&path,&name,&ext);
  if (filename.IsEmpty() || ext.IsEmpty())
  {
    mafString wildc = "Image (*.jpg)|*.jpg|Image (*.bmp)|*.bmp";
    switch(forceExtension)
    {
    case mafGUIApplicationSettings::JPG :
      wildc = "Image (*.jpg)|*.jpg";
      break;
    case mafGUIApplicationSettings::BMP:
      wildc = "Image (*.bmp)|*.bmp";
      break;
    }
    mafString file;
    if(!wxDirExists(path))
    {
      file = m_SaveDir;
      file +=  "\\";
      filename = mafString(name);
    }
    file.Append(filename);
    file = mafGetSaveFile(file,wildc).c_str(); 
    if(file.IsEmpty())
      return;
    filename = file;
  }

  wxString temporary = filename.GetCStr();
  temporary = temporary.AfterLast('\\').AfterFirst('.');

  switch(forceExtension)
  {
  case mafGUIApplicationSettings::JPG :
    if(mafString(temporary) != _("jpg"))
      filename += _(".jpg");
    break;
  case mafGUIApplicationSettings::BMP:
    if(mafString(temporary) != _("bmp"))
      filename += _(".bmp");
    break;
  }

  mafString basename = filename.BaseName();
  if (basename.IsEmpty())
  {
    filename = m_SaveDir << "\\" << filename;
  }

  ::wxBeginBusyCursor();

  wxBitmap imageBitmap;
  v->GetImage(imageBitmap);

  wxSplitPath(filename,&path,&name,&ext);
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
  else
  {
    wxMessageBox("Must save with JPG or BMP extension");
  }
  ::wxEndBusyCursor();
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
//----------------------------------------------------------------------------
void mafRWIBase::SetStereoMovieDirectory(const char *dir)
//----------------------------------------------------------------------------
{
  m_StereoMovieDir = dir;
}
//----------------------------------------------------------------------------
void mafRWIBase::GenerateStereoFrames()
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
  mafString filename;
  filename = m_StereoMovieDir;
  filename += "\\movie_";
  filename += wxString::Format("%05d",m_StereoMovieFrameCounter);
  filename += ".png";
  m_StereoMoviewFrameWriter->SetFileName(filename.GetCStr());
  m_StereoMoviewFrameWriter->Write();
  m_StereoMovieFrameCounter++;
}
//----------------------------------------------------------------------------
void mafRWIBase::EnableStereoMovie(bool enable)
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
    m_StereoImage->AddInput(m_StereoMovieLeftEye->GetOutput());
    m_StereoImage->AddInput(m_StereoMovieRightEye->GetOutput());

    vtkNEW(m_StereoMoviewFrameWriter);
    m_StereoMoviewFrameWriter->SetInput(m_StereoImage->GetOutput());
  }
}
