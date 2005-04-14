/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgDialog.h,v $
  Language:  C++
  Date:      $Date: 2005-04-14 13:44:57 $
  Version:   $Revision: 1.3 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __mmgDialog_H__
#define __mmgDialog_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "wx/dialog.h"
#include "mafEvent.h"
#include "mmgGui.h"
//----------------------------------------------------------------------------
// Const:
//----------------------------------------------------------------------------
enum mmgDIALOG_STYLES
{
  mafRESIZABLE =1,   // make the Dialog Resizable  
  mafCLOSEWINDOW =2, // to enable the close button on the window frame
  mafOK =4,          // to create an ok button
  mafCANCEL =8,      // to create a cancel button 
  mafCLOSE =16,      // to create a close button // still closes with ok, just different caption
};
//----------------------------------------------------------------------------
// mmgDialog :
//----------------------------------------------------------------------------
/**
mmgDialog is the base class for mafDialogs.
mmgDialog can be constructed on the Stack as well as on the Heap
ex1: {mmgDialog dlg("foo"); dlg.ShowModal();}
ex2: {mmgDialog* dlg = new mmgDialog("foo"); dlg->ShowModal(); cppDEL(dlg);}

depending on the flag passed as the "Style" parameter
mmgDialog can be resizable or not, 
have a close button on the frame or nor,
have an ok and/or cancel and/or close button.

mmgDialog is meant do be "Modal", to create non-modal Dialogs use a wxFrame instead.

The Layout of the Dialog is managed using "Sizers",
user widget or sizer can be added calling the "Add" member function.

It is possible to place a mmgGui on an mmgDialog (call Add).
Normally a mmgGui send event to it's creator,
but if you an OkCancel button on the Gui to be able to close the Dialog,
then events from the Gui must go first to the Dialog.
Example: 
-create GUI
-Create Dialog
-Dialog Add GUI
-Gui SetListener Dialog
-Dialog SetListener Me

see the "TestDialog" project for more examples.
*/
class mmgDialog : public wxDialog, public mafEventListener  
{
public:
	mmgDialog (const wxString& title,	long style = mafCLOSEWINDOW | mafRESIZABLE | mafCLOSE);
	virtual ~mmgDialog (); 
  void SetListener(mafEventListener *Listener) {m_Listener = Listener;};
  void OnEvent(mafEvent& e);

  /** Add a widget to the dialog. */
  void Add(wxWindow* window,int option = 0, int flag = wxEXPAND, int border = 0)  {window->Reparent(this); m_sizer->Add(window,option,flag,border);};

  /** Add a mmgGui to the dialog. TECH: this is to force the calling of the correct reparent, is not virtual in wxWindow */
  void Add(mmgGui* window,int option = 0, int flag = wxEXPAND, int border = 0)  {window->Reparent(this); m_sizer->Add(window,option,flag,border);};

  /** Add a sizer to the dialog. */
  void Add(wxSizer*  sizer, int option = 0, int flag = wxEXPAND, int border = 0)  {m_sizer->Add(sizer, option,flag,border);};

  /** Remove a widget from the dialog. */
  bool Remove(wxWindow* window) {return m_sizer->Remove(window);};

  /** Remove a sizer from the dialog. */
  bool Remove(wxSizer*  sizer ) {return m_sizer->Remove(sizer);};

  int ShowModal();

  /** Enable/Disable the default buttons -- doesn't apply to user created buttons */
  void EnableOk(bool enable)     {if(m_ok_button)     m_ok_button->Enable(enable);};
  void EnableCancel(bool enable) {if(m_cancel_button) m_cancel_button->Enable(enable);};
  void EnableClose(bool enable)  {if(m_close_button)  m_close_button->Enable(enable);};

protected:
	/** Close the dialog */
	virtual void OnCloseWindow(wxCloseEvent& event);
  virtual void OnOK(wxCommandEvent& event);
  virtual void OnCancel(wxCommandEvent& event);
	
  /** sizer for user widgets */
  wxBoxSizer      *m_sizer; 

  wxBoxSizer      *m_buttons_sizer; 
  wxBoxSizer      *m_dialog_sizer; 
  wxButton        *m_ok_button;
  wxButton        *m_cancel_button;
  wxButton        *m_close_button;

  mafEventListener   *m_Listener;
	DECLARE_EVENT_TABLE()
};
#endif
