/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpComputeWrapping
 Authors: Gianluigi Crimi
 
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

#include "albaWizardSettings.h"
#include <wx/intl.h>
#include "albaGUI.h"
#include "albaGUIDialog.h"

//----------------------------------------------------------------------------
albaWizardSettings::albaWizardSettings(albaObserver *Listener, const albaString &label):albaGUISettings(Listener, label)
//----------------------------------------------------------------------------
{
  //default constructor
  m_ShowInformationBoxes = true;
  m_Gui=NULL;
  InitializeSettings();
}

//----------------------------------------------------------------------------
albaWizardSettings::~albaWizardSettings() 
//----------------------------------------------------------------------------
{
  //default destructor
}

//----------------------------------------------------------------------------
void albaWizardSettings::CreateGui()
//----------------------------------------------------------------------------
{

  //Creating Wizard Setting Gui
  m_Gui = new albaGUI(this);   
  m_Gui->Label(_("Wizard Settings"));
  m_Gui->Label("");
  m_Gui->Label("");
  m_Gui->Label("");
  m_Gui->Bool(WIZARD_SETTINGS_ID,"Show information boxes",&m_ShowInformationBoxes,true);
}

//----------------------------------------------------------------------------
void albaWizardSettings::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  switch(alba_event->GetId())
  {
    case WIZARD_SETTINGS_ID:
    {
      //Save options on user changes
      m_Config->Write("Wizard_info_boxes",m_ShowInformationBoxes);
      m_Config->Flush();
    }
    break;
    default:
      albaEventMacro(*alba_event);
    break; 
  }
}

//----------------------------------------------------------------------------
void albaWizardSettings::InitializeSettings()
//----------------------------------------------------------------------------
{
  //On first run i cannot read configuration
  if(!m_Config->Read("Wizard_info_boxes", &m_ShowInformationBoxes))
    //So i will save default value
    m_Config->Write("Wizard_info_boxes",m_ShowInformationBoxes);
  m_Config->Flush();
}

//----------------------------------------------------------------------------
void albaWizardSettings::SetShowInformationBoxes( int value )
//----------------------------------------------------------------------------
{
  //Update value and store it to persistent memory
  m_ShowInformationBoxes=value;
  m_Config->Write("Wizard_info_boxes",m_ShowInformationBoxes);
  m_Config->Flush();
  if (m_Gui)
    m_Gui->Update();
}

//----------------------------------------------------------------------------
int albaWizardSettings::GetShowInformationBoxes()
//----------------------------------------------------------------------------
{
  //return current settings
  return m_ShowInformationBoxes;
}
