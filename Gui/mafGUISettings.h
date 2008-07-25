/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafGUISettings.h,v $
Language:  C++
Date:      $Date: 2008-07-25 07:03:23 $
Version:   $Revision: 1.3 $
Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafGUISettings_H__
#define __mafGUISettings_H__

#include "mafObserver.h"
#include <wx/config.h>

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------
class mafGUI;

//----------------------------------------------------------------------------
// mafGUISettings :
//----------------------------------------------------------------------------
/**
*/
class mafGUISettings : public mafObserver
{
public:
	mafGUISettings(mafObserver *Listener, const mafString &label = _("Settings"));
	~mafGUISettings(); 

  /** Answer to the messages coming from interface. */
  virtual void OnEvent(mafEventBase *maf_event);

  void SetListener(mafObserver *Listener) {m_Listener = Listener;};

  /** Return the GUI of the setting panel.*/
  mafGUI* GetGui();

  /** Return the settings' label*/
  const char *GetLabel() {return m_Label.GetCStr();};

protected:
  /** Create the GUI for the setting panel.*/
  virtual void CreateGui();

  /** Initialize the settings.*/
  virtual void InitializeSettings();

  mafGUI *m_Gui;
  mafString m_Label;

  wxConfig *m_Config;
  
  mafObserver *m_Listener;
};
#endif
