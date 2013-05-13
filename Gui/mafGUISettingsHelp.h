/*=========================================================================

 Program: MAF2
 Module: mafGUISettingsHelp
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafGUISettingsHelp_H__
#define __mafGUISettingsHelp_H__

#include "mafGUISettings.h"

//----------------------------------------------------------------------------
// mafGUISettingsHelp :
//----------------------------------------------------------------------------
/**
  Help settings
*/

class MAF_EXPORT mafGUISettingsHelp : public mafGUISettings
{
public:
	mafGUISettingsHelp(mafObserver *Listener, const mafString &label = _("Help Settings"));
	~mafGUISettingsHelp(); 

  enum HELP_SETTINGS_WIDGET_ID
  {
    ID_BUILD_HELP_GUI = MINID,
  };

  /** Answer to the messages coming from interface. */
  void OnEvent(mafEventBase *maf_event);

  /** Set python.exe interpreter used at runtime*/
  void SetBuildHelpGui(bool buildHelpGui){m_BuildHelpGui = buildHelpGui;};
  
  /** Get python.exe interpreter used at runtime*/
  bool GetBuildHelpGui(){return m_BuildHelpGui;};

protected:
  /** Create the GUI for the setting panel.*/
  void CreateGui();

  /** Initialize the application settings.*/
  void InitializeSettings();

  /** Used to enable/disable gui items*/
  void EnableItems(bool enable);

  int m_BuildHelpGui;
  
};
#endif
