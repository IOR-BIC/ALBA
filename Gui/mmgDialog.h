/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgDialog.h,v $
  Language:  C++
  Date:      $Date: 2005-05-04 11:44:03 $
  Version:   $Revision: 1.9 $
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
#include "mafObserver.h"
#include "mmgGui.h"
//----------------------------------------------------------------------------
// Const:
//----------------------------------------------------------------------------
enum mmgDIALOG_STYLES
{
  mafRESIZABLE    =1,  // make the Dialog Resizable  
  mafCLOSEWINDOW  =2,  // to enable the close button on the window frame
  mafOK           =4,  // to create an ok button
  mafCANCEL       =8,  // to create a cancel button 
  mafCLOSE        =16, // to create a close button
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

The Close button have the same behaviour of the Button on the Dialog Frame,
both of them call wxWindow::Close(), wich in turn call wx::CloseWindow(),
-- that you can redefine -- the default for CloseWindow is to call OnCancel().

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

@sa mmgDialogPreview
*/
class mmgDialog : public wxDialog, public mafObserver  
{
public:
	mmgDialog (const wxString& title,	long style = mafCLOSEWINDOW | mafRESIZABLE | mafCLOSE);
	virtual ~mmgDialog (); 
  void SetListener(mafObserver *Listener) {m_Listener = Listener;};
  void OnEvent(mafEventBase *event);

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

  /** 
  Virtual functions called to terminate ShowModal - these can be redefined without providing the Event Table. 
  called when a ShowModal stage end with ok - tipically is called by an 'OK' button -- */
  virtual void OnOK(wxCommandEvent& event);
  /** 
  Virtual functions called to terminate ShowModal - these can be redefined without providing the Event Table. 
  called when a ShowModal stage end with Cancel - tipically is called by an 'Cancel' button -- */
  virtual void OnCancel(wxCommandEvent& event);
  /** 
  Virtual functions called to terminate ShowModal - these can be redefined without providing the Event Table. 
  called when a ShowModal stage end with because the user press a 'Close' button or the CloseButton on the frame.
  The default behaviur is to call OnCancel() - I recommend not to override this, but OnCancel */
  virtual void OnCloseWindow(wxCloseEvent& event);


  // to Paolo
  // m_sizer         forse sarebbe meglio chiamarlo m_UserSizer - e' dove l'user plugga i suoi widget
  // m_PreviewSizer (se lo aggiungi) lo chiamerei m_ClientSizer - di fatto riempira la ClientArea della finestra

  wxBoxSizer *m_sizer;          ///< Sizer for user widgets -- Calling Add() insert a widget in this sizer
  wxBoxSizer *m_buttons_sizer;  ///< Sizer holding the ok,cancel,close button (if any)
  wxBoxSizer *m_dialog_sizer;   ///< Dialog sizer -- manage the Dialog initial Size, and the other sizers
  wxButton   *m_ok_button;      ///< Button used to close the dialog with wxID_OK
  wxButton   *m_cancel_button;  ///< Button used to close the dialog with wxID_CANCEL
  wxButton   *m_close_button;   ///< Button used to close the dialog with wxID_CLOSE

  mafObserver *m_Listener;

private:
  /** non virtual function, called on Dialog Closing and bound trought the event table */
  void nvOnCloseWindow(wxCloseEvent& event) {OnCloseWindow(event);};
  void nvOnOK(wxCommandEvent& event)        {OnOK(event);};    
  void nvOnCancel(wxCommandEvent& event)    {OnCancel(event);};    
  void nvOnClose(wxCommandEvent& event)     {wxDialog::Close();}; //calls nvOnCloseWindow

  DECLARE_EVENT_TABLE()
};
#endif