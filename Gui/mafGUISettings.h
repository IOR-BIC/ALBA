/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafGUISettings.h,v $
Language:  C++
Date:      $Date: 2009-12-01 14:36:33 $
Version:   $Revision: 1.3.2.1 $
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

/**
 class name: mafGUISettings
  base class for more complex specified classes regarding the setting of application.
  It returns a gui and generally can access to registry keys.
*/
class mafGUISettings : public mafObserver
{
public:
  /** constructor */
	mafGUISettings(mafObserver *Listener, const mafString &label = _("Settings"));
  /** destructor */
	~mafGUISettings(); 

  /** Answer to the messages coming from interface. */
  virtual void OnEvent(mafEventBase *maf_event);

  /** Set the listener of the events launched*/
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
