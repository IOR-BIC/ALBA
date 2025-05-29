/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIDialog
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaGUIDialog_H__
#define __albaGUIDialog_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "wx/dialog.h"
#include "albaEvent.h"
#include "albaObserver.h"
#include "albaGUI.h"
#include "albaServiceClient.h"
#include "albaAbsLogicManager.h"

//----------------------------------------------------------------------------
// Const:
//----------------------------------------------------------------------------
enum DIALOG_STYLES
{
  albaRESIZABLE    =  1,  // make the Dialog Resizable  
  albaCLOSEWINDOW  =  2,  // to enable the close button on the window frame
  albaOK           =  4,  // to create an ok button
  albaCANCEL       =  8,  // to create a cancel button 
  albaCLOSE        = 16, // to create a close button
};
//----------------------------------------------------------------------------
// albaGUIDialog :
//----------------------------------------------------------------------------
/**
albaGUIDialog is the base class for albaDialogs.
albaGUIDialog can be constructed on the Stack as well as on the Heap
ex1: {albaGUIDialog dlg("foo"); dlg.ShowModal();}
ex2: {albaGUIDialog* dlg = new albaGUIDialog("foo"); dlg->ShowModal(); cppDEL(dlg);}

depending on the flag passed as the "Style" parameter
albaGUIDialog can be resizable or not, 
have a close button on the frame or nor,
have an ok and/or cancel and/or close button.

The Close button have the same behaviour of the Button on the Dialog Frame,
both of them call wxWindow::Close(), wich in turn call wx::CloseWindow(),
-- that you can redefine -- the default for CloseWindow is to call OnCancel().

albaGUIDialog is meant do be "Modal", to create non-modal Dialogs use a wxFrame instead.

The Layout of the Dialog is managed using "Sizers",
user widget or sizer can be added calling the "Add" member function.

It is possible to place a albaGUI on an albaGUIDialog (call Add).
Normally a albaGUI send event to it's creator,
but if you place an OkCancel button on the Gui, to be able to close the Dialog,
then events from the Gui must go first to the Dialog.
Example: 
-create GUI
-Create Dialog
-Dialog Add GUI
-Gui SetListener Dialog
-Dialog SetListener Me

@sa albaGUIDialogPreview
*/
class ALBA_EXPORT albaGUIDialog : public wxDialog, public albaObserver, public albaServiceClient  
{
public:
	albaGUIDialog (const wxString& title,	long style = albaCLOSEWINDOW | albaRESIZABLE | albaCLOSE, albaObserver *listener=NULL);
	virtual ~albaGUIDialog (); 
  void SetListener(albaObserver *listener) {m_Listener = listener;};
  void OnEvent(albaEventBase *alba_event);

  /** Add a widget to the dialog. */
  void Add(wxWindow* window,int option = 0, int flag = wxEXPAND, int border = 0)  {window->Reparent(this); m_GuiSizer->Add(window,option,flag,border);};

  /** Add a albaGUI to the dialog. TECH: this is to force the calling of the correct reparent, is not virtual in wxWindow */
  void Add(albaGUI* window,int option = 0, int flag = wxEXPAND, int border = 0)  {window->Reparent(this); window->FitGui(); m_GuiSizer->Add(window,option,flag,border);};

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


	virtual void WXSetInitialFittingClientSize(int flags, wxSizer* sizer = NULL);


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

  /** Handle on size event */
  virtual void OnSize(wxSizeEvent &event){DoLayout();};

  wxBoxSizer *m_GuiSizer;          ///< Sizer for user widgets -- Calling Add() insert a widget in this sizer
  wxBoxSizer *m_ButtonsSizer;  ///< Sizer holding the ok,cancel,close button (if any)
  wxBoxSizer *m_DialogSizer;   ///< Dialog sizer -- manage the Dialog initial Size, and the other sizers
  wxButton   *m_OkButton;      ///< Button used to close the dialog with wxID_OK
  wxButton   *m_CancelButton;  ///< Button used to close the dialog with wxID_CANCEL
  wxButton   *m_CloseButton;   ///< Button used to close the dialog with wxID_CLOSE

  albaObserver *m_Listener;

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
