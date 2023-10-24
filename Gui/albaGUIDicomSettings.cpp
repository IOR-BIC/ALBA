/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIDicomSettings
 Authors: Matteo Giacomoni, Simone Brazzale
 
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

#include "albaGUIDicomSettings.h"

#include "albaDecl.h"
#include "albaGUI.h"
#include "albaGUICheckListBox.h"

//----------------------------------------------------------------------------
albaGUIDicomSettings::albaGUIDicomSettings(albaObserver *Listener, const albaString &label):
albaGUISettings(Listener, label)
//----------------------------------------------------------------------------
{
	// m_Dictionary = "";

  m_CheckNameCompositor[ID_PATIENT_NAME] = TRUE;
  m_CheckNameCompositor[ID_BIRTHDATE] = FALSE;
  m_CheckNameCompositor[ID_NUM_SLICES] = TRUE;
  m_CheckNameCompositor[ID_DESCRIPTION] = TRUE; 
  m_CheckNameCompositor[ID_SERIES] = TRUE;
	  
  m_AutoResample = m_AutoVMEType = m_SkipCrop = FALSE;
  m_OutputType = 0;
  m_LastDicomDir = "UNEDFINED_m_LastDicomDir";
  m_Step = ID_1X;
  
  m_DCM_ImagePositionPatientchoice = 0;

  m_Config->SetPath("Importer Dicom"); // Regiser key path Added by Losi 15.11.2009
	InitializeSettings();
}
//----------------------------------------------------------------------------
albaGUIDicomSettings::~albaGUIDicomSettings()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaGUIDicomSettings::CreateGui()
//----------------------------------------------------------------------------
{
	m_Gui = new albaGUI(this);

	wxString DCM_IMGchoices[2]={_("Skip All"),_("Set Default position")};
	wxString typeArray[2] = { _("Volume"),_("Images") };
	wxString SkipChoices[4] = { _("Load All"),_("load one in two"),_("load one in trhee"),_("load one in four") };
	
	m_Gui->Label("Dicom Settings", true);
	m_Gui->Label("");
  m_Gui->Label("Image position patient exception handling");
  m_Gui->Combo(ID_DCM_POSITION_PATIENT_CHOICE,_("        "),&m_DCM_ImagePositionPatientchoice,2,DCM_IMGchoices);
  m_Gui->Divider(1);
  m_Gui->Bool(ID_AUTO_VME_TYPE,_("Auto VME Type"),&m_AutoVMEType,1);
  m_Gui->Radio(ID_SETTING_VME_TYPE, "VME output", &m_OutputType, 2, typeArray, 1, "");
	m_Gui->Bool(ID_AUTORESAMPLE_OUTPUT, _("Auto Resample Volume"), &m_AutoResample, 1,"When the slicing space is not regular a resample filter is applied.");
  m_Gui->Divider(1);
	m_Gui->Combo(ID_STEP,_("Skip Slices:"),&m_Step,4,SkipChoices);
	m_Gui->Divider(1);
	m_Gui->Label("Wizard phases:");
	m_Gui->Bool(ID_SKIP_CROP, _("Skip Crop"), &m_SkipCrop, 1);

	m_Gui->Divider(1);
  
	EnableItems();
}
//----------------------------------------------------------------------------
void albaGUIDicomSettings::EnableItems()
//----------------------------------------------------------------------------
{
  if (m_Gui)
  {
    m_Gui->Enable(ID_SETTING_VME_TYPE,(m_AutoVMEType || m_SkipCrop ));
		m_Gui->Enable(ID_AUTORESAMPLE_OUTPUT, (!m_AutoVMEType || m_OutputType == 0));
	  m_Gui->Update();
  }
}
//----------------------------------------------------------------------------
void albaGUIDicomSettings::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
	switch (alba_event->GetId())
	{
		case ID_AUTO_VME_TYPE:
		{
			m_Config->Write("AutoVMEType", m_AutoVMEType);
		}
		break;
		case ID_SKIP_CROP:
		{
			m_Config->Write("SkipCrop", m_SkipCrop);
		}
		break;
		case ID_STEP:
		{
			m_Config->Write("StepOfBuild", m_Step);
		}
		break;
		case ID_SETTING_VME_TYPE:
		{
			m_Config->Write("VMEType", m_OutputType);
		}
		break;
		case ID_AUTORESAMPLE_OUTPUT:
		{
			m_Config->Write("AutoResample", m_AutoResample);
		}
		break;
		case ID_DCM_POSITION_PATIENT_CHOICE:
		{
			m_Config->Write("DCM_ImagePositionPatientchoice", m_DCM_ImagePositionPatientchoice);
		}
		break;
		default:
			albaEventMacro(*alba_event);
			break;
	}
	EnableItems();
	m_Config->Flush();
}
//----------------------------------------------------------------------------
void albaGUIDicomSettings::InitializeSettings()
//----------------------------------------------------------------------------
{
	wxString string_item;
	long long_item;
	double double_item;

	if(m_Config->Read("StepOfBuild", &long_item))
		m_Step=long_item;
	else
		m_Config->Write("StepOfBuild",m_Step);

	if(m_Config->Read("LastDicomDir", &string_item))
		m_LastDicomDir=string_item.c_str();
	else
		m_Config->Write("LastDicomDir",m_LastDicomDir);
	
	if(m_Config->Read("AutoVMEType", &long_item))
		m_AutoVMEType=long_item;
	else
		m_Config->Write("AutoVMEType",m_AutoVMEType);

	if (m_Config->Read("SkipCrop", &long_item))
		m_SkipCrop = long_item;
	else
		m_Config->Write("SkipCrop", m_SkipCrop);

	if(m_Config->Read("VMEType", &long_item))
		m_OutputType=long_item;
	else
		m_Config->Write("VMEType",m_OutputType);

	if (m_Config->Read("AutoResample", &long_item))
		m_AutoResample = long_item;
	else
		m_Config->Write("AutoResample", m_AutoResample);
		
  if(m_Config->Read("DCM_ImagePositionPatientchoice", &long_item))
	  m_DCM_ImagePositionPatientchoice=long_item;
  else
	  m_Config->Write("DCM_ImagePositionPatientchoice", m_DCM_ImagePositionPatientchoice);

  if(m_Config->Read("NameCompositorPatientName", &long_item))
    m_CheckNameCompositor[ID_PATIENT_NAME] = long_item;
  else
    m_Config->Write("NameCompositorPatientName",m_CheckNameCompositor[ID_PATIENT_NAME]);

  if(m_Config->Read("NameCompositorDescription", &long_item))
    m_CheckNameCompositor[ID_DESCRIPTION] = long_item;
  else
    m_Config->Write("NameCompositorDescription",m_CheckNameCompositor[ID_DESCRIPTION]);

  if(m_Config->Read("NameCompositorBirthdate", &long_item))
    m_CheckNameCompositor[ID_BIRTHDATE] = long_item;
  else
    m_Config->Write("NameCompositorBirthdate",m_CheckNameCompositor[ID_BIRTHDATE]);

  if(m_Config->Read("NameCompositorNumSlices", &long_item))
    m_CheckNameCompositor[ID_NUM_SLICES] = long_item;
  else
    m_Config->Write("NameCompositorNumSlices",m_CheckNameCompositor[ID_NUM_SLICES]);

	m_Config->Flush();
}

void albaGUIDicomSettings::SetLastDicomDir( wxString lastDicomDir )
{
  m_LastDicomDir = lastDicomDir;
  assert(m_Config);
  m_Config->Write("LastDicomDir",m_LastDicomDir);

}

int albaGUIDicomSettings::GetEnabledCustomName(enum NAME_COMPOSITOR type)
{
  if (type >= ID_DESCRIPTION && type<=ID_NUM_SLICES)
  {
    return m_CheckNameCompositor[type];
  }
}
