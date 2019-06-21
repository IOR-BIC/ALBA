/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUISettingsAdvanced
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "albaGUISettingsAdvanced.h"
#include "albaDecl.h"
#include "albaGUI.h"

//----------------------------------------------------------------------------
albaGUISettingsAdvanced::albaGUISettingsAdvanced(albaObserver *Listener, const albaString &label):
albaGUISettings(Listener, label)
//----------------------------------------------------------------------------
{
  m_ConversionUnits = NONE;

  InitializeSettings();
}
//----------------------------------------------------------------------------
void albaGUISettingsAdvanced::CreateGui()
//----------------------------------------------------------------------------
{
  m_Gui = new albaGUI(this);

  wxString choices_conversion[2] = {"NONE","mm2m"};
  m_Gui->Label(_("Convert Data in SI units"));
  m_Gui->Combo(ID_CONVERSION_UNITS,"",&m_ConversionUnits,2,choices_conversion);
  
  m_Gui->Label("");
}
//----------------------------------------------------------------------------
albaGUISettingsAdvanced::~albaGUISettingsAdvanced() 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaGUISettingsAdvanced::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  switch(alba_event->GetId())
  {
  case ID_CONVERSION_UNITS:
    {
      m_Config->Write("ConversionUnits",m_ConversionUnits);
    }
    break;
  default:
    albaEventMacro(*alba_event);
    break; 
  }

  m_Config->Flush();
}
//----------------------------------------------------------------------------
void albaGUISettingsAdvanced::SetConversionType(int conversion)
//----------------------------------------------------------------------------
{
  m_ConversionUnits = conversion;
  if (m_Config)
  {
  	m_Config->Write("ConversionUnits",m_ConversionUnits);
  }
  if (m_Gui)
  {
  	m_Gui->Update();
  }
}
//----------------------------------------------------------------------------
void albaGUISettingsAdvanced::InitializeSettings()
//----------------------------------------------------------------------------
{
  long long_item;

  if(m_Config->Read("ConversionUnits", &long_item))
  {
    m_ConversionUnits=long_item;
  }
  else
  {
    m_Config->Write("ConversionUnits",m_ConversionUnits);
  }

  m_Config->Flush();
}
