/*=========================================================================

 Program: MAF2Medical
 Module: medOpComputeWrapping
 Authors: Gianluigi Crimi
 
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

#include "medWizardSettings.h"
#include <wx/intl.h>
#include "mafGUI.h"
#include "mafGUIDialog.h"

//----------------------------------------------------------------------------
medWizardSettings::medWizardSettings(mafObserver *Listener, const mafString &label):mafGUISettings(Listener, label)
//----------------------------------------------------------------------------
{
  //default constructor
  m_ShowInformationBoxes = true; 
  InitializeSettings();
}

//----------------------------------------------------------------------------
medWizardSettings::~medWizardSettings() 
//----------------------------------------------------------------------------
{
  //default destructor
}

//----------------------------------------------------------------------------
void medWizardSettings::CreateGui()
//----------------------------------------------------------------------------
{

  //Creating Wizard Setting Gui
  m_Gui = new mafGUI(this);   
  m_Gui->Label(_("Wizard Settings"));
  m_Gui->Label("");
  m_Gui->Label("");
  m_Gui->Label("");
  m_Gui->Bool(WIZARD_SETTINGS_ID,"Show information boxes",&m_ShowInformationBoxes,true);
}

//----------------------------------------------------------------------------
void medWizardSettings::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  switch(maf_event->GetId())
  {
    case WIZARD_SETTINGS_ID:
    {
      //Save options on user changes
      m_Config->Write("Wizard_info_boxes",m_ShowInformationBoxes);
      m_Config->Flush();
    }
    break;
    default:
      mafEventMacro(*maf_event);
    break; 
  }
}

//----------------------------------------------------------------------------
void medWizardSettings::InitializeSettings()
//----------------------------------------------------------------------------
{
  //On first run i cannot read configuration
  if(!m_Config->Read("Wizard_info_boxes", &m_ShowInformationBoxes))
    //So i will save default value
    m_Config->Write("Wizard_info_boxes",m_ShowInformationBoxes);
  m_Config->Flush();
}

//----------------------------------------------------------------------------
void medWizardSettings::SetShowInformationBoxes( int value )
//----------------------------------------------------------------------------
{
  //Update value and store it to persistent memory
  m_ShowInformationBoxes=value;
  m_Config->Write("Wizard_info_boxes",m_ShowInformationBoxes);
  m_Config->Flush();
}

//----------------------------------------------------------------------------
int medWizardSettings::GetShowInformationBoxes()
//----------------------------------------------------------------------------
{
  //return current settings
  return m_ShowInformationBoxes;
}
