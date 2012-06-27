/*=========================================================================

 Program: MAF2
 Module: mafGUIDockSettings
 Authors: Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafGUIDockSettings_H__
#define __mafGUIDockSettings_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafGUISettings.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class wxFrameManager;

/**
  class name: mafGUIDockSettings
  class that handle the dock settings panel.
*/
class mafGUIDockSettings: public mafGUISettings
{
public:
 /** constructor */
 mafGUIDockSettings(wxFrameManager& mgr, const mafString &label = _("User Interface Preferences"));
 /** destructor */
 virtual ~mafGUIDockSettings();

  /** Answer to the messages coming from interface. */
 void OnEvent(mafEventBase *evt);

protected:
  /** Create the GUI for the setting panel.*/
  void CreateGui();

  wxFrameManager& m_Mgr;

  int      m_PaneBorderSize;
  int      m_SashSize;
  int      m_CaptionSize;
  wxColour m_BackgroundColor;
  wxColour m_SashColor;
  wxColour m_InactiveCaptionColor;
  wxColour m_InactiveCaptionGradientColor;
  wxColour m_InactiveCaptionTextColor;
  wxColour m_ActiveCaptionColor;
  wxColour m_ActiveCaptionGradientColor;
  wxColour m_ActiveCaptionTextColor;
  wxColour m_BorderColor;
  wxColour m_GripperColor;

  int m_AllowFloating;
  int m_AllowActivePane;
  int m_CaptionMode;
  wxString m_CaptionModeLabels[3];
};
#endif
