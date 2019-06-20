/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: testDialogLogic
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


#include "testDialogLogic.h"
#include "albaGUIDialog.h"
#include "albaGUIDialogPreview.h"
#include "albaGUI.h"
#include "albaGUIValidator.h"
#include "testDialogDlg.h"
//--------------------------------------------------------------------------------
//const:
//--------------------------------------------------------------------------------
enum 
{
  ID_D1 = MINID,
  ID_D2,
  ID_D3,
  ID_D4,
  ID_D5,
  ID_D6,
  ID_D7,
  ID_D8,
  ID_D9,
  ID_10,
  ID_11,
  ID_12,
  ID_13,
  ID_14,
  ID_15,
  ID_SLIDER,
  ID_TEST,
};
//--------------------------------------------------------------------------------
testDialogLogic::testDialogLogic()
//--------------------------------------------------------------------------------
{
  m_test_variable =1;

  m_win = new wxFrame(NULL,-1,"TestDialog",wxDefaultPosition,wxDefaultSize,
    wxMINIMIZE_BOX | wxMAXIMIZE_BOX | /*wxRESIZE_BORDER |*/ wxSYSTEM_MENU | wxCAPTION );
  albaSetFrame(m_win);

  albaGUI *gui = new albaGUI(this);
  gui->Divider();
  gui->Label("Examples of albaGUIDialog",true); 
  gui->Label("albaGUIDialog styles"); 
  gui->Button(ID_D1,"using albaRESIZABLE style");
  gui->Button(ID_D2,"without");
  gui->Button(ID_D3,"using albaCLOSEWINDOW");
  gui->Button(ID_D4,"without");
  gui->Button(ID_D5,"using albaOK style");
  gui->Button(ID_D6,"using albaOK|albaCANCEL");
  gui->Button(ID_D7,"using albaCLOSE");

  gui->Label("Dialogs with a albaGUI",true); 
  gui->Button(ID_D8,"non sizable");
  gui->Button(ID_D9,"sizable");
  gui->Button(ID_10,"GUI and resizable elements");

  gui->Label("Receiving Events form a Dialog",true); 
  gui->Button(ID_11,"using albaGUI");
  gui->Button(ID_12,"using albaGUIValidators");

  gui->Label("Deriving from albaGUIDialog",true); 
  gui->Label("allow more control like:"); 
  gui->Label("dynamically enabling ok/cancel,"); 
  gui->Label("or perform actions after OK evt."); 
  gui->Label("but before the dialog closes."); 
  gui->Button(ID_13,"test custom dialog");

  gui->Label("dialog with preview renderwindow.");
  gui->Button(ID_14,"test preview dialog");
  gui->Button(ID_15,"test preview dialog again");

  gui->Reparent(m_win);
  m_win->Fit(); // resize m_win to fit it's content
}
//--------------------------------------------------------------------------------
testDialogLogic::~testDialogLogic()
//--------------------------------------------------------------------------------
{
  //m_win->Destroy(); //is implicit
}
//--------------------------------------------------------------------------------
void testDialogLogic::OnEvent(albaEventBase *alba_event)
//--------------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event)) 
  {
    switch(e->GetId())
    {
    case ID_D1: { albaGUIDialog dlg("foo",albaRESIZABLE|albaCLOSEWINDOW);         dlg.ShowModal(); }break;
    case ID_D2: { albaGUIDialog dlg("foo",albaCLOSEWINDOW);                      dlg.ShowModal(); }break;
    case ID_D3: { albaGUIDialog dlg("foo",albaRESIZABLE|albaCLOSEWINDOW);         dlg.ShowModal(); }break;
    case ID_D4: { albaGUIDialog dlg("foo",albaRESIZABLE|albaCLOSE);               dlg.ShowModal(); }break;
    case ID_D5: { albaGUIDialog dlg("foo",albaRESIZABLE|albaOK);                  dlg.ShowModal(); }break;
    case ID_D6: { albaGUIDialog dlg("foo",albaRESIZABLE|albaOK|albaCANCEL);        dlg.ShowModal(); }break;
    case ID_D7: { albaGUIDialog dlg("foo",albaRESIZABLE|albaCLOSE);               dlg.ShowModal(); }break;
    case ID_D8: 
      { 
        //very easy example (without using Sizers): 
        //create a GUI 
        //place it on top a a Dialog 
        //make the Dialog non sizable

        //if you make the Dialog the Listener of the GUI
        //also the GUI Button can close the Dialog

        albaGUI *gui = new albaGUI(NULL);
        gui->Divider();
        gui->Label("note that also albaGUI buttons"); 
        gui->Label("can close the dialog.");
        gui->Label("");
        gui->Button(wxID_OK,"ok"); //send a wxID_OK
        gui->Button(wxID_CANCEL,"cancel"); //send a wxID_CANCEL
        gui->OkCancel(); // send a wxOK / wxCANCEL
        gui->Divider();
        gui->Label("this change a variable in logic");
        gui->Slider(ID_SLIDER,"",&m_test_variable,0,10); 

        // place the GUI on the Dialog calling Add.
        // note that the initial (and minimum if sizable) size of the Dialog are correct //SIL. 14-4-2005: :-) finally
        albaGUIDialog dlg("pippo",albaCLOSEWINDOW);
        gui->SetListener(&dlg);  
        dlg.Add(gui,1,wxEXPAND);
        dlg.ShowModal();

        // also note that you dont need to destroy the GUI.
        // The Dialog take possess of it and destroy it with itself
      } 
      break;
    case ID_D9: 
      { 
        //example2:
        // placing a albaGUI on a Sizable Dialog: 
        // Since Gui don't stretch (if you want this feature, shout a lot to the management)
        // this is a poor solution unless you have something else that stretch on the Dialog
        albaGUI *gui = new albaGUI(NULL);
        gui->Label("placing a albaGUI alone");
        gui->Label("on a resizable Dialog is poor");
        gui->Slider(ID_SLIDER,"",&m_test_variable,0,10); 
        gui->Slider(ID_SLIDER,"",&m_test_variable,0,10); 
        gui->Slider(ID_SLIDER,"",&m_test_variable,0,10); 

        albaGUIDialog dlg("pippo");
        dlg.Add(gui,1,wxEXPAND);
        dlg.ShowModal();
      }
      break;
    case ID_10: 
      { 
        //example3:
        // Mixing a gui togheter with other resizable Elements
        albaGUIDialog dlg("pippo");

        wxPanel *p = new wxPanel(&dlg,-1,wxDefaultPosition,wxSize(200,200),wxSUNKEN_BORDER);

        albaGUI *gui = new albaGUI(NULL);
        gui->OkCancel();
        gui->Label("now is better");
        gui->Button(0,"button");
        gui->Button(0,"button");
        gui->Button(0,"button");
        gui->Button(0,"button");
        gui->Slider(ID_SLIDER,"",&m_test_variable,0,10); 
        gui->Slider(ID_SLIDER,"",&m_test_variable,0,10); 
        gui->Slider(ID_SLIDER,"",&m_test_variable,0,10); 
        gui->SetListener(&dlg);
        // Reparent the GUI under the Dialog is normally a side effect of dlg.Add;
        // Since here we not Add the GUI, but a Sizer containing the Gui,
        // the Gui must be reparented explicitly.
        gui->Reparent(&dlg);

        wxSizer *sz = new wxBoxSizer(wxHORIZONTAL);
        sz->Add(p,1,wxEXPAND);
        sz->Add(gui,0);
        dlg.Add(sz,1);
        dlg.ShowModal();
      }
      break;
    case ID_11: 
      {
        //example 4: Receiving Events form a Dialog - using albaGUI
        albaGUI *gui = new albaGUI(NULL);
        gui->Label("This call a function in Logic");
        gui->Divider();
        gui->Button(ID_TEST,"test");

        albaGUIDialog dlg("pippo");
        gui->SetListener(&dlg); // event from Gui goes to the Dialog
        dlg.SetListener(this);  // event from the Dialog come here
        dlg.Add(gui,1,wxEXPAND);
        dlg.ShowModal();
      }
      break;
    case ID_12: 
      {
        //example 4: Receiving Events form a Dialog - using albaGUIValidators
        albaGUIDialog dlg("pippo");
        wxStaticText *lab = new wxStaticText(&dlg,-1,"This call a function in Logic") ;
        dlg.Add(lab);
        wxButton *b = new wxButton(&dlg,ID_TEST,"test");
        b->SetValidator( albaGUIValidator(this,ID_TEST,b) ); // this way send the event to me
        //b->SetValidator( albaGUIValidator(&dlg,ID_TEST,b) ); // this way send the event to dlg
        dlg.Add(b);
        dlg.SetListener(this);  // event from the Dialog come here
        dlg.ShowModal();
      }
      break;
    case ID_13:
      {
        testDialogDlg dlg("Customized Dialog");
        dlg.ShowModal();
      }
      break;
    case ID_14:
      {
        albaGUIDialogPreview dlg("preview",albaCLOSEWINDOW | albaRESIZABLE | albaUSEGUI | albaUSERWI);
        dlg.SetListener(this);  // event from the Dialog come here
        albaGUI *gui = dlg.GetGui();
        gui->Label("This call a function in Logic");
        gui->Divider();
        gui->Button(ID_TEST,"test");
        dlg.ShowModal();
      }
      break;
    case ID_15:
      {
        albaGUIDialogPreview dlg("preview",albaCLOSEWINDOW | albaRESIZABLE | albaCLOSE | albaCANCEL | albaOK |albaUSERWI);
        dlg.SetListener(this);  // event from the Dialog come here
        wxSlider *sli = new wxSlider(&dlg, ID_SLIDER, 0,100,50, wxDefaultPosition);
        dlg.Add(sli);
        dlg.ShowModal();
      }
      break;
    case ID_TEST: 
      wxMessageBox("ID_TEST event received");
      break;
    }
  }
}
//--------------------------------------------------------------------------------
void testDialogLogic::Show()
//--------------------------------------------------------------------------------
{
  m_win->Show(true);
}
