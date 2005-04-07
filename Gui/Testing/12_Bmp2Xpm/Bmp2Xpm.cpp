/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: Bmp2Xpm.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-07 08:39:39 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDefines.h"
#include "mafWXLog.h"
#include "mafDecl.h"
#include "mmgBitmaps.h"
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
  mafWXLog *m_logger = new mafWXLog(log);
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
    img.LoadFile(filename,wxBITMAP_TYPE_BMP);
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
