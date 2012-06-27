/*=========================================================================

 Program: MAF2
 Module: mafOsgApp
 Authors: Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafOsgCanvas.h"

//--------------------------------------------------------------------------------
class mafOsgApp : public wxApp
//--------------------------------------------------------------------------------
{
public:

  bool OnInit()
  {
    wxFrame *frame = new wxFrame(NULL,-1,"mafOsg");
    SetTopWindow(frame);  

    //mafOsgCanvas *canvas  = new mafOsgCanvas(frame,1,wxPoint(0,0),wxSize(100,100));
    //mafOsgCanvas *canvas2 = new mafOsgCanvas(frame,2,wxPoint(101,0),wxSize(100,100));
    //wxBoxSizer *sz = new wxBoxSizer(wxHORIZONTAL);
    //sz->Add(canvas, 1, wxEXPAND | wxALL ,2);
    //sz->Add(canvas2, 1, wxEXPAND | wxALL ,2);
    //frame->SetSizer( sz );
    //sz->SetSizeHints( frame );   

    mafOsgCanvas *canvas  = new mafOsgCanvas(frame,-1);
    frame->Show();
    //canvas->LoadModel("D:\\__OSG_DATA\\cow.osg");

    return true;
  };

  int  OnExit() 
  {
    return 0;
  };

};
//--------------------------------------------------------------------------------
// Create the Application
//--------------------------------------------------------------------------------
DECLARE_APP(mafOsgApp)
IMPLEMENT_APP(mafOsgApp)

