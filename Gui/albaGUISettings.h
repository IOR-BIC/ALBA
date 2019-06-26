/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUISettings
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaGUISettings_H__
#define __albaGUISettings_H__

#include "albaObserver.h"
#include <wx/config.h>

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------
class albaGUI;

/**
 class name: albaGUISettings
  base class for more complex specified classes regarding the setting of application.
  It returns a gui and generally can access to registry keys.
*/
class ALBA_EXPORT albaGUISettings : public albaObserver
{
public:
  /** constructor */
	albaGUISettings(albaObserver *Listener, const albaString &label = _("Settings"));
  /** destructor */
	~albaGUISettings(); 

  /** Answer to the messages coming from interface. */
  virtual void OnEvent(albaEventBase *alba_event);

  /** Set the listener of the events launched*/
  void SetListener(albaObserver *Listener) {m_Listener = Listener;};

  /** Return the GUI of the setting panel.*/
  albaGUI* GetGui();

  /** Return the settings' label*/
  const char *GetLabel() {return m_Label.GetCStr();};

protected:
  /** Create the GUI for the setting panel.*/
  virtual void CreateGui();

  /** Initialize the settings.*/
  virtual void InitializeSettings();

  albaGUI *m_Gui;
  albaString m_Label;

  wxConfig *m_Config;
  
  albaObserver *m_Listener;
};
#endif
