/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUIDialog.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 06:53:38 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __mafGUIDialog_H__
#define __mafGUIDialog_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "wx/dialog.h"
#include "mafEvent.h"
#include "mafObserver.h"
#include "mafGUI.h"
//----------------------------------------------------------------------------
// Const:
//----------------------------------------------------------------------------
enum DIALOG_STYLES
{
  mafRESIZABLE    =  1,  // make the Dialog Resizable  
  mafCLOSEWINDOW  =  2,  // to enable the close button on the window frame
  mafOK           =  4,  // to create an ok button
  mafCANCEL       =  8,  // to create a cancel button 
  mafCLOSE        = 16, // to create a close button
};
//----------------------------------------------------------------------------
// mafGUIDialog :
//----------------------------------------------------------------------------
/**
mafGUIDialog is the base class for mafDialogs.
mafGUIDialog can be constructed on the Stack as well as on the Heap
ex1: {mafGUIDialog dlg("foo"); dlg.ShowModal();}
ex2: {mafGUIDialog* dlg = new mafGUIDialog("foo"); dlg->ShowModal(); cppDEL(dlg);}

depending on the flag passed as the "Style" parameter
mafGUIDialog can be resizable or not, 
have a close button on the frame or nor,
have an ok and/or cancel and/or close button.

The Close button have the same behaviour of the Button on the Dialog Frame,
both of them call wxWindow::Close(), wich in turn call wx::CloseWindow(),
-- that you can redefine -- the default for CloseWindow is to call OnCancel().

mafGUIDialog is meant do be "Modal", to create non-modal Dialogs use a wxFrame instead.

The Layout of the Dialog is managed using "Sizers",
user widget or sizer can be added calling the "Add" member function.

It is possible to place a mafGUI on an mafGUIDialog (call Add).
Normally a mafGUI send event to it's creator,
but if you place an OkCancel button on the Gui, to be able to close the Dialog,
then events from the Gui must go first to the Dialog.
Example: 
-create GUI
-Create Dialog
-Dialog Add GUI
-Gui SetListener Dialog
-Dialog SetListener Me

@sa mafGUIDialogPreview
*/
class mafGUIDialog : public wxDialog, public mafObserver  
{
public:
	mafGUIDialog (const wxString& title,	long style = mafCLOSEWINDOW | mafRESIZABLE | mafCLOSE);
	virtual ~mafGUIDialog (); 
  void SetListener(mafObserver *Listener) {m_Listener = Listener;};
  void OnEvent(mafEventBase *maf_event);

  /** Add a widget to the dialog. */
  void Add(wxWindow* window,int option = 0, int flag = wxEXPAND, int border = 0)  {window->Reparent(this); m_GuiSizer->Add(window,option,flag,border);};

  /** Add a mafGUI to the dialog. TECH: this is to force the calling of the correct reparent, is not virtual in wxWindow */
  void Add(mafGUI* window,int option = 0, int flag = wxEXPAND, int border = 0)  {window->Reparent(this); window->FitGui(); m_GuiSizer->Add(window,option,flag,border);};

  /** Add a sizer to the dialog. */
  void Add(wxSizer*  sizer, int option = 0, int flag = wxEXPAND, int border = 0)  {m_GuiSizer->Add(sizer, option,flag,border);};

  /** Remove a widget from the dialog. */
  bool Remove(wxWindow* window) {return m_GuiSizer->Detach(window);};

  /** Remove a sizer from the dialog. */
  bool Remove(wxSizer*  sizer ) {return m_GuiSizer->Detach(sizer);};

  int ShowModal();

  /** Enable/Disable the default buttons -- doesn't apply to user created buttons */
  void EnableOk(bool enable)     {if(m_OkButton)     m_OkButton->Enable(enable);};
  void EnableCancel(bool enable) {if(m_CancelButton) m_CancelButton->Enable(enable);};
  void EnableClose(bool enable)  {if(m_CloseButton)  m_CloseButton->Enable(enable);};

  /** 
  Virtual functions called to terminate ShowModal - these can be redefined without providing the Event Table. 
  called when a ShowModal stage end with ok - in general is called by an 'OK' button -- */
  virtual void OnOK(wxCommandEvent &event);
  /** 
  Virtual functions called to terminate ShowModal - these can be redefined without providing the Event Table. 
  called when a ShowModal stage end with Cancel - in general is called by an 'Cancel' button -- */
  virtual void OnCancel(wxCommandEvent &event);
  /** 
  Virtual functions called to terminate ShowModal - these can be redefined without providing the Event Table. 
  called when a ShowModal stage end with because the user press a 'Close' button or the CloseButton on the frame.
  The default behavior is to call OnCancel() - I recommend not to override this, but OnCancel */
  virtual void OnCloseWindow(wxCloseEvent &event);


  wxBoxSizer *m_GuiSizer;          ///< Sizer for user widgets -- Calling Add() insert a widget in this sizer
  wxBoxSizer *m_ButtonsSizer;  ///< Sizer holding the ok,cancel,close button (if any)
  wxBoxSizer *m_DialogSizer;   ///< Dialog sizer -- manage the Dialog initial Size, and the other sizers
  wxButton   *m_OkButton;      ///< Button used to close the dialog with wxID_OK
  wxButton   *m_CancelButton;  ///< Button used to close the dialog with wxID_CANCEL
  wxButton   *m_CloseButton;   ///< Button used to close the dialog with wxID_CLOSE

  mafObserver *m_Listener;

private:
  /** non virtual function, called on Dialog Closing and bound through the event table */
  void nvOnCloseWindow(wxCloseEvent &event) {OnCloseWindow(event);};
  void nvOnOK(wxCommandEvent &event)        {OnOK(event);};    
  void nvOnCancel(wxCommandEvent &event)    {OnCancel(event);};    
  void nvOnClose(wxCommandEvent &event)     {wxDialog::Close();}; //calls nvOnCloseWindow

  bool m_DialogInitialized;
  DECLARE_EVENT_TABLE()
};
#endif
