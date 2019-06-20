/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaWizardSettings
 Authors: Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaGUILocaleSettings_H__
#define __albaGUILocaleSettings_H__

#include "albaGUISettings.h"
#include "albaDefines.h"

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------

/**
  class name :albaWizardSettings 
  class that manage wizard specific settings
*/
class ALBA_EXPORT albaWizardSettings : public albaGUISettings
{

  enum WIZARD_SETTINGS
  {
    WIZARD_SETTINGS_ID = MINID,
  };

public:
  /** constructor */
	albaWizardSettings(albaObserver *Listener, const albaString &label = _("Wizard Settings"));
  /** destructor */
	~albaWizardSettings(); 

  /** Answer to the messages coming from interface. */
  void OnEvent(albaEventBase *alba_event);

  /** Enable/disable show of information boxes */
  void SetShowInformationBoxes(int value);

  /** return the state of show of information boxes */
  int GetShowInformationBoxes();

protected:
  /** Create the GUI for the setting panel.*/
  void CreateGui();

  /** Initialize language used into the application.*/
  void InitializeSettings();

  int  m_ShowInformationBoxes;
};
#endif
