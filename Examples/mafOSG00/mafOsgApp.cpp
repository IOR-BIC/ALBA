/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOsgApp.cpp,v $
  Language:  C++
  Date:      $Date: 2006-02-13 15:49:13 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mVueGlCanvas.h"

//--------------------------------------------------------------------------------
class mafOsgApp : public wxApp
//--------------------------------------------------------------------------------
{
public:

  bool OnInit()
  {
    wxFrame *frame = new wxFrame(NULL,-1,"mafOsg");
    SetTopWindow(frame);  

    //mVueGlCanvas *canvas  = new mVueGlCanvas(frame,1,wxPoint(0,0),wxSize(100,100));
    //mVueGlCanvas *canvas2 = new mVueGlCanvas(frame,2,wxPoint(101,0),wxSize(100,100));
    //wxBoxSizer *sz = new wxBoxSizer(wxHORIZONTAL);
    //sz->Add(canvas, 1, wxEXPAND | wxALL ,2);
    //sz->Add(canvas2, 1, wxEXPAND | wxALL ,2);
    //frame->SetSizer( sz );
    //sz->SetSizeHints( frame );   

    mVueGlCanvas *canvas  = new mVueGlCanvas(frame,-1);
    frame->Show();
    canvas->LoadModel("D:\\__OSG_DATA\\cow.osg");

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

