/*=========================================================================

 Program: MAF2
 Module: mafGUISettingsAdvanced
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafGUISettingsAdvanced.h"
#include "mafDecl.h"
#include "mafGUI.h"

//----------------------------------------------------------------------------
mafGUISettingsAdvanced::mafGUISettingsAdvanced(mafObserver *Listener, const mafString &label):
mafGUISettings(Listener, label)
//----------------------------------------------------------------------------
{
  m_ConversionUnits = NONE;

  InitializeSettings();
}
//----------------------------------------------------------------------------
void mafGUISettingsAdvanced::CreateGui()
//----------------------------------------------------------------------------
{
  m_Gui = new mafGUI(this);

  wxString choices_conversion[2] = {"NONE","mm2m"};
  m_Gui->Label(_("Convert Data in SI units"));
  m_Gui->Combo(ID_CONVERSION_UNITS,"",&m_ConversionUnits,2,choices_conversion);
  
  m_Gui->Label("");
}
//----------------------------------------------------------------------------
mafGUISettingsAdvanced::~mafGUISettingsAdvanced() 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafGUISettingsAdvanced::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  switch(maf_event->GetId())
  {
  case ID_CONVERSION_UNITS:
    {
      m_Config->Write("ConversionUnits",m_ConversionUnits);
    }
    break;
  default:
    mafEventMacro(*maf_event);
    break; 
  }

  m_Config->Flush();
}
//----------------------------------------------------------------------------
void mafGUISettingsAdvanced::SetConversionType(int conversion)
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
void mafGUISettingsAdvanced::InitializeSettings()
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
