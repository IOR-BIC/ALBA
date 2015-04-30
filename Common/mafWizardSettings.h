/*=========================================================================

 Program: MAF2
 Module: mafWizardSettings
 Authors: Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafGUILocaleSettings_H__
#define __mafGUILocaleSettings_H__

#include "mafGUISettings.h"
#include "mafDefines.h"

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------

/**
  class name :mafWizardSettings 
  class that manage wizard specific settings
*/
class MAF_EXPORT mafWizardSettings : public mafGUISettings
{

  enum WIZARD_SETTINGS
  {
    WIZARD_SETTINGS_ID = MINID,
  };

public:
  /** constructor */
	mafWizardSettings(mafObserver *Listener, const mafString &label = _("Wizard Settings"));
  /** destructor */
	~mafWizardSettings(); 

  /** Answer to the messages coming from interface. */
  void OnEvent(mafEventBase *maf_event);

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
