/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testDialogDlg.cpp,v $
  Language:  C++
  Date:      $Date: 2008-07-25 07:04:41 $
  Version:   $Revision: 1.3 $
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

#include "testDialogDlg.h"
#include "wx/busyinfo.h"
#include "mafDecl.h"
#include "mafGUIValidator.h"
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
: mafGUIDialog(title,mafOK|mafCANCEL/*|mafCLOSE*/|mafCLOSEWINDOW)
//----------------------------------------------------------------------------
{
   wxButton *b1 = new wxButton(this,ID_ENABLE,"enable ok");
   b1->SetValidator( mafGUIValidator(this,ID_ENABLE,b1) ); 
   Add(b1);

   wxButton *b2 = new wxButton(this,ID_DISABLE,"disable ok");
   b2->SetValidator( mafGUIValidator(this,ID_DISABLE,b2) ); 
   Add(b2);
}
//----------------------------------------------------------------------------
testDialogDlg::~testDialogDlg( ) 
//----------------------------------------------------------------------------
{
}
//--------------------------------------------------------------------------------
void testDialogDlg::OnEvent(mafEventBase *event)
//--------------------------------------------------------------------------------
{
  if(mafEvent *e = mafEvent::SafeDownCast(event))
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
      mafGUIDialog::OnEvent(e); //pass unhandled evt. thorough the SuperClass
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
  mafGUIDialog::OnOK(event);
}
//----------------------------------------------------------------------------
void testDialogDlg::OnCancel(wxCommandEvent& event)
//----------------------------------------------------------------------------
{
  int res = wxMessageBox("really cancel ?","test",wxYES|wxNO|wxICON_QUESTION);
  if (res == wxNO) return;
  mafGUIDialog::OnCancel(event);
}

