/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: Bmp2Xpm
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


#include "albaWXLog.h"
#include "albaDecl.h"
#include "albaPics.h"
#include <wx/dir.h>
#include <stdio.h>

//--------------------------------------------------------------------------------
class myApp : public wxApp
//--------------------------------------------------------------------------------
{
public:
    virtual bool OnInit();
};
//--------------------------------------------------------------------------------
class myFrame : public wxFrame
//--------------------------------------------------------------------------------
{
public:
  myFrame();
  void Convert();
private:
};
//--------------------------------------------------------------------------------
IMPLEMENT_APP(myApp)
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
bool myApp::OnInit()
//--------------------------------------------------------------------------------
{
  wxInitAllImageHandlers();
  myFrame *frame = new myFrame();

  SetTopWindow(frame);
  frame->Show(true);
  frame->Convert();
  return FALSE;
}
//--------------------------------------------------------------------------------
myFrame::myFrame()
: wxFrame(NULL, -1, "MakeIcons", wxDefaultPosition, wxSize(300,300))
//--------------------------------------------------------------------------------
  {
  //Log Area
  wxTextCtrl *log  = new wxTextCtrl( this, -1, "", wxPoint(0,0), wxSize(300,300), wxNO_BORDER | wxTE_MULTILINE );
  albaWXLog *m_logger = new albaWXLog(log);
  wxLog *old_log = wxLog::SetActiveTarget( m_logger );
  cppDEL(old_log);
  }
//--------------------------------------------------------------------------------
void myFrame::Convert()
//--------------------------------------------------------------------------------
{
  wxDir dir(wxGetCwd());
  wxLogMessage("current directory is %s", wxGetCwd());
  wxString filename;
  wxString filespec = "*.bmp";
  bool cont = dir.GetFirst(&filename, filespec);
  while ( cont )
  {
    wxImage img;
    img.LoadFile(filename/*,wxBITMAP_TYPE_BMP*/);
    if(img != wxNullImage)
    {
      wxLogMessage("converting  %s", filename);

      wxString path,name,ext;
      wxSplitPath(filename,&path,&name,&ext);
      name += ".xpm";
      img.SaveFile(name,wxBITMAP_TYPE_XPM);
    }
    cont = dir.GetNext(&filename);
  }
}
