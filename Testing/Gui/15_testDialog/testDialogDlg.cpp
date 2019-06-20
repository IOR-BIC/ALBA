/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: testDialogDlg
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

#include "testDialogDlg.h"
#include "wx/busyinfo.h"
#include "albaDecl.h"
#include "albaGUIValidator.h"
//----------------------------------------------------------------------------
// Event ids
//----------------------------------------------------------------------------
enum 
{
  ID_ENABLE = MINID,
  ID_DISABLE,
};
//----------------------------------------------------------------------------
testDialogDlg::testDialogDlg(const wxString& title)
: albaGUIDialog(title,albaOK|albaCANCEL/*|albaCLOSE*/|albaCLOSEWINDOW)
//----------------------------------------------------------------------------
{
   wxButton *b1 = new wxButton(this,ID_ENABLE,"enable ok");
   b1->SetValidator( albaGUIValidator(this,ID_ENABLE,b1) ); 
   Add(b1);

   wxButton *b2 = new wxButton(this,ID_DISABLE,"disable ok");
   b2->SetValidator( albaGUIValidator(this,ID_DISABLE,b2) ); 
   Add(b2);
}
//----------------------------------------------------------------------------
testDialogDlg::~testDialogDlg( ) 
//----------------------------------------------------------------------------
{
}
//--------------------------------------------------------------------------------
void testDialogDlg::OnEvent(albaEventBase *event)
//--------------------------------------------------------------------------------
{
  if(albaEvent *e = albaEvent::SafeDownCast(event))
  {
    switch(e->GetId())
    {
    case ID_ENABLE:
      m_OkButton->Enable(true);
      break;
    case ID_DISABLE:
      m_OkButton->Enable(false);
      break;
    default:
      albaGUIDialog::OnEvent(e); //pass unhandled evt. thorough the SuperClass
    }
  }
}
//----------------------------------------------------------------------------
void testDialogDlg::OnCloseWindow(wxCloseEvent& event)
//----------------------------------------------------------------------------
{
  int res = wxMessageBox("you are about to close the Window,\n this is the same as choosing CANCEL \n are you sure ?",
                          "test",wxYES|wxNO|wxICON_QUESTION);
  if (res == wxNO) 
  {
    event.Veto();     
    return;
  }
  wxDialog::OnCloseWindow(event);
}
//----------------------------------------------------------------------------
void testDialogDlg::OnOK(wxCommandEvent& event)
//----------------------------------------------------------------------------
{
  int res = wxMessageBox("you pressed OK, are you shure ?","test",wxYES|wxNO|wxICON_QUESTION);
  if (res == wxNO) return;
  albaGUIDialog::OnOK(event);
}
//----------------------------------------------------------------------------
void testDialogDlg::OnCancel(wxCommandEvent& event)
//----------------------------------------------------------------------------
{
  int res = wxMessageBox("really cancel ?","test",wxYES|wxNO|wxICON_QUESTION);
  if (res == wxNO) return;
  albaGUIDialog::OnCancel(event);
}

