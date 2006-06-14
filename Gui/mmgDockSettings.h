/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgDockSettings.h,v $
  Language:  C++
  Date:      $Date: 2006-06-14 14:46:33 $
  Version:   $Revision: 1.2 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __mmgDockSettings_H__
#define __mmgDockSettings_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafObserver.h"
//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mmgGui;
class wxFrameManager;
//----------------------------------------------------------------------------
// mmgDockSettings :
//----------------------------------------------------------------------------
class mmgDockSettings: public mafObserver
{
public:
 mmgDockSettings(wxFrameManager& mgr);
 virtual ~mmgDockSettings();

 void OnEvent(mafEventBase *evt);
 mmgGui* GetGui() {return m_gui;};

protected:
  wxFrameManager& m_mgr;

  mmgGui*  m_gui;
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
