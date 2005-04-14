// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

    #include "wx/laywin.h"

//#include "wxVTKWindow.h"
#include "wxVTKRenderWindowInteractor.h"
#include "vtkCamera.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkConeSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"

// the application icon
#ifndef __WXMSW__
    #include "mondrian.xpm"
#endif

class MyApp;
class MyFrame;

// Define a new application type, each program should derive a class from wxApp
class MyApp : public wxApp
{
public:
    // this one is called on application startup and is a good place for the app
    // initialization (doing it here and not in the ctor allows to have an error
    // return: if OnInit() returns false, the application terminates)
    virtual bool OnInit();
};

// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
    // ctor(s)
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
    ~MyFrame();

    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnSashDrag(wxSashEvent& event);

protected:
    void ConstructVTK();
    void ConfigureVTK();
    void DestroyVTK();

private:
    wxSashLayoutWindow *m_pSashWindowLeft;
    wxSashLayoutWindow *m_pSashWindowRight;
    wxVTKRenderWindowInteractor        *m_pVTKWindow;
    wxVTKRenderWindowInteractor        *m_pAnotherVTKWindow;

  // vtk classes
  vtkRenderer       *pRenderer;
  vtkRenderer       *pAnotherRenderer;
  vtkRenderWindow   *pRenderWindow;
  vtkRenderWindow   *pAnotherRenderWindow;
  vtkPolyDataMapper *pConeMapper;
  vtkActor          *pConeActor;
  vtkConeSource     *pConeSource;

private:
    // any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE()
};

// IDs for the controls and the menu commands
enum
{
    // menu items
    Minimal_Quit = 1,
    Minimal_About
};

#define MY_FRAME      101
#define MY_VTK_WINDOW 102
#define MY_ANOTHER_VTK_WINDOW 105
#define MY_WINDOW_LEFT  103
#define MY_WINDOW_RIGHT 104

// the event tables connect the wxWindows events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Minimal_Quit,  MyFrame::OnQuit)
    EVT_MENU(Minimal_About, MyFrame::OnAbout)
    EVT_SIZE(MyFrame::OnSize)
    EVT_SASH_DRAGGED_RANGE(MY_WINDOW_LEFT, MY_WINDOW_RIGHT, MyFrame::OnSashDrag)
END_EVENT_TABLE()

// Create a new application object: this macro will allow wxWindows to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also declares the accessor function
// wxGetApp() which will return the reference of the right type (i.e. MyApp and
// not wxApp)
IMPLEMENT_APP(MyApp)

// 'Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    // create the main application window
    MyFrame *frame = new MyFrame("wxWindows-VTK App",
                                 wxPoint(50, 50), wxSize(450, 340));

    // and show it (the frames, unlike simple controls, are not shown when
    // created initially)
    frame->Show(TRUE);

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned FALSE here, the
    // application would exit immediately.
    return TRUE;
}

// frame constructor
MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
       : wxFrame((wxFrame *)NULL, -1, title, pos, size), m_pSashWindowLeft(0),
	     m_pSashWindowRight(0)
{
#ifdef __WXMAC__
    // we need this in order to allow the about menu relocation, since ABOUT is
    // not the default id of the about menu
    wxApp::s_macAboutMenuItemId = Minimal_About;
#endif

    // set the frame icon
    SetIcon(wxICON(mondrian));

    // create a menu bar
    wxMenu *menuFile = new wxMenu("", wxMENU_TEAROFF);

    // the "About" item should be in the help menu
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(Minimal_About, "&About...\tCtrl-A", "Show about dialog");

    menuFile->Append(Minimal_Quit, "E&xit\tAlt-X", "Quit this program");

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, "&File");
    menuBar->Append(helpMenu, "&Help");

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);

#if wxUSE_STATUSBAR
    // create a status bar just for fun (by default with 1 pane only)
    // not exactly fun...as it show a bug in wxWindow 2.4.0 :(
    CreateStatusBar(2);
    SetStatusText("Welcome to wxVTK!");
#endif // wxUSE_STATUSBAR

    m_pSashWindowLeft = new wxSashLayoutWindow(this, MY_WINDOW_LEFT, wxDefaultPosition, 
					       wxSize(size.GetWidth()/2, size.GetHeight()), 
					       wxNO_BORDER|wxSW_3D| wxCLIP_CHILDREN);
    m_pSashWindowLeft->SetDefaultSize(wxSize(size.GetWidth()/2, size.GetHeight()));
    m_pSashWindowLeft->SetOrientation(wxLAYOUT_VERTICAL);
    m_pSashWindowLeft->SetAlignment(wxLAYOUT_LEFT);
    m_pSashWindowLeft->SetSashVisible(wxSASH_RIGHT, TRUE);

    m_pSashWindowRight = new wxSashLayoutWindow(this, MY_WINDOW_RIGHT, wxDefaultPosition,
						wxSize(size.GetWidth()/2, size.GetHeight()), 
						wxNO_BORDER|wxSW_3D| wxCLIP_CHILDREN);
    m_pSashWindowRight->SetDefaultSize(wxSize(size.GetWidth()/2, size.GetHeight()));
    m_pSashWindowRight->SetOrientation(wxLAYOUT_VERTICAL);
    m_pSashWindowRight->SetAlignment(wxLAYOUT_LEFT);

    m_pVTKWindow = new wxVTKRenderWindowInteractor(m_pSashWindowLeft, MY_VTK_WINDOW);

    m_pAnotherVTKWindow = new wxVTKRenderWindowInteractor(m_pSashWindowRight, MY_ANOTHER_VTK_WINDOW);

    ConstructVTK();
    ConfigureVTK();
}

MyFrame::~MyFrame()
{
  DestroyVTK();
  m_pVTKWindow->Delete();
  m_pAnotherVTKWindow->Delete();
}

void MyFrame::ConstructVTK()
{
  pRenderer     = vtkRenderer::New();
  pAnotherRenderer = vtkRenderer::New();
  pConeMapper   = vtkPolyDataMapper::New();
  pConeActor    = vtkActor::New();
  pConeSource   = vtkConeSource::New();
}

void MyFrame::ConfigureVTK()
{
  // connect  render -> render window -> wxVTK window
  pRenderWindow =  m_pVTKWindow->GetRenderWindow();
  pRenderWindow->AddRenderer(pRenderer);

  // connect  render -> render window -> wxVTK window
  pAnotherRenderWindow = m_pAnotherVTKWindow->GetRenderWindow();
  pAnotherRenderWindow->AddRenderer(pAnotherRenderer);
 
  // initialize cone
  pConeSource->SetResolution(8);

  // connect pipeline
  pConeMapper->SetInput(pConeSource->GetOutput());
  pConeActor->SetMapper(pConeMapper);
  pRenderer->AddActor(pConeActor);
  pAnotherRenderer->AddActor(pConeActor);

  // configure renderers
  pRenderer->SetBackground(0.0,0.0,0.0);
  pRenderer->GetActiveCamera()->Elevation(30.0);
  pRenderer->GetActiveCamera()->Azimuth(30.0);
  pRenderer->GetActiveCamera()->Zoom(1.0);
  pRenderer->GetActiveCamera()->SetClippingRange(1,1000);
  pAnotherRenderer->SetBackground(0.0,0.0,0.0);
  pAnotherRenderer->GetActiveCamera()->Elevation(30.0);
  pAnotherRenderer->GetActiveCamera()->Azimuth(-30.0);
  pAnotherRenderer->GetActiveCamera()->Zoom(1.0);
  pAnotherRenderer->GetActiveCamera()->SetClippingRange(1,1000);
}

void MyFrame::DestroyVTK()
{
  if (pRenderer != 0)
    pRenderer->Delete();
  if (pAnotherRenderer != 0)
    pAnotherRenderer->Delete();
  if (pConeMapper != 0)
    pConeMapper->Delete();
  if (pConeActor != 0)
    pConeActor->Delete();
  if (pConeSource != 0)
    pConeSource->Delete();
}

// event handlers

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // TRUE is to force the frame to close
    Close(TRUE);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxString msg;
    msg.Printf( _T("This is the about dialog of wx-vtk sample.\n"));

    wxMessageBox(msg, "About wx-vtk", wxOK | wxICON_INFORMATION, this);
}

void MyFrame::OnSize(wxSizeEvent& event)
{
  if (m_pSashWindowLeft != 0)
    m_pSashWindowLeft->SetDefaultSize(wxSize(event.GetSize().GetWidth()/2, event.GetSize().GetHeight()));
  if (m_pSashWindowRight != 0)
    m_pSashWindowRight->SetDefaultSize(wxSize(event.GetSize().GetWidth()/2, event.GetSize().GetHeight()));

  wxLayoutAlgorithm layout;
  layout.LayoutFrame(this);
}

void MyFrame::OnSashDrag(wxSashEvent& event)
{
    if (event.GetDragStatus() == wxSASH_STATUS_OUT_OF_RANGE)
        return;

    switch (event.GetId())
    {
        case MY_WINDOW_LEFT:
        {
	  if (m_pSashWindowLeft != 0)
            m_pSashWindowLeft->SetDefaultSize(wxSize(event.GetDragRect().width, GetSize().GetHeight()));
	  if (m_pSashWindowRight != 0)
	    m_pSashWindowRight->SetDefaultSize(wxSize(GetSize().GetWidth() - event.GetDragRect().width, GetSize().GetHeight()));
            break;
        }
        case MY_WINDOW_RIGHT:
        {
	  if (m_pSashWindowRight != 0)
  	    m_pSashWindowRight->SetDefaultSize(wxSize(event.GetDragRect().width, GetSize().GetHeight()));
	  if (m_pSashWindowLeft != 0)
	    m_pSashWindowLeft->SetDefaultSize(wxSize(GetSize().GetWidth() - event.GetDragRect().width, GetSize().GetHeight()));
            break;
        }
    }
    wxLayoutAlgorithm layout;
    layout.LayoutFrame(this);
}
