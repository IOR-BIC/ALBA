/*=========================================================================

 Program: MAF2
 Module: mafGUIDicomSettings
 Authors: Matteo Giacomoni, Simone Brazzale
 
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

#include "mafGUIDicomSettings.h"

#include "mafDecl.h"
#include "mafGUI.h"
#include "mafGUICheckListBox.h"

//----------------------------------------------------------------------------
mafGUIDicomSettings::mafGUIDicomSettings(mafObserver *Listener, const mafString &label):
mafGUISettings(Listener, label)
//----------------------------------------------------------------------------
{
	// m_Dictionary = "";

  m_CheckNameCompositor[ID_PATIENT_NAME] = TRUE;
  m_CheckNameCompositor[ID_BIRTHDATE] = FALSE;
  m_CheckNameCompositor[ID_NUM_SLICES] = TRUE;
  m_CheckNameCompositor[ID_DESCRIPTION] = TRUE; 
  m_CheckNameCompositor[ID_SERIES] = TRUE;

	m_CheckOnOff[0] = m_CheckOnOff[1] = m_CheckOnOff[2] = m_CheckOnOff[3] = m_CheckOnOff[4] = m_CheckOnOff[5] = m_CheckOnOff[6] = m_CheckOnOff[7] = TRUE;

  m_CheckOnOffVmeType[0] = m_CheckOnOffVmeType[2] = TRUE;
  m_CheckOnOffVmeType[1] = FALSE;

  m_EnableChangeSide = FALSE;
  m_EnableDiscardPosition = FALSE;
  m_EnableResampleVolume = FALSE;
  m_EnableRescaleTo16Bit = FALSE;
  m_VisualizePosition = FALSE;
  m_AutoVMEType = FALSE;
  m_PercentageTolerance = FALSE;
  m_ScalarTolerance = FALSE;
  m_OutputType = 0;
  m_ScalarDistanceTolerance = 0.3;
  m_PercentageDistanceTolerance = 88;
  m_LastDicomDir = "UNEDFINED_m_LastDicomDir";
  m_Step = ID_1X;
  m_OutputNameType = TRADITIONAL;
  m_ShowAdvancedOptionOfSorting = TRUE;
  
  m_DCM_ImagePositionPatientchoice = 0;

  m_Config->SetPath("Importer Dicom"); // Regiser key path Added by Losi 15.11.2009
	InitializeSettings();
}
//----------------------------------------------------------------------------
mafGUIDicomSettings::~mafGUIDicomSettings()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafGUIDicomSettings::CreateGui()
//----------------------------------------------------------------------------
{
	m_Gui = new mafGUI(this);
  
	m_Gui->Bool(ID_SIDE,_("Enable Change Side"),&m_EnableChangeSide,1);
  m_Gui->Bool(ID_DISCARD_ORIGIN,_("Enable Discard Origin"),&m_EnableDiscardPosition,1);
  m_Gui->Bool(ID_RESAMPLE_VOLUME,_("Enable Resample Volume"),&m_EnableResampleVolume,1);
  m_Gui->Bool(ID_RESCALE_TO_16_BIT,_("Enable Rescaling to 16 Bit"),&m_EnableRescaleTo16Bit,1);
  m_Gui->Bool(ID_SHOW_ADVANCED_OPTION_SORTING,_("Show adv setting of sorting"),&m_ShowAdvancedOptionOfSorting,1);
  m_Gui->Bool(ID_ENABLE_POS_INFO,_("Visualize Position and Orientation"),&m_VisualizePosition,1);

  m_Gui->Bool(ID_SCALAR_DISTANCE_TOLERANCE,_("Scalar distance tolerance"),&m_ScalarTolerance,1);
  m_Gui->Double(ID_SCALAR_TOLERANCE,_("Value"),&m_ScalarDistanceTolerance,0,MAXDOUBLE,5,"Value in millimeter");

  m_Gui->Bool(ID_PERCENTAGE_DISTANCE_TOLERANCE,_("Percentage distance tolerance"),&m_PercentageTolerance,1);
  m_Gui->Double(ID_PERCENTAGE_TOLERANCE,_("Value"),&m_PercentageDistanceTolerance,0,MAXDOUBLE,2,"Value in percentage");

  wxString DCM_IMGchoices[2]={_("Skip All"),_("Set Default position")};
  m_Gui->Label("Dicom image position patient exception handling");
  m_Gui->Combo(ID_DCM_POSITION_PATIENT_CHOICE,_("        "),&m_DCM_ImagePositionPatientchoice,2,DCM_IMGchoices);

  m_Gui->Divider();
  m_Gui->Divider(1);
  m_Gui->Divider();
  
  m_Gui->Bool(ID_AUTO_VME_TYPE,_("Auto VME Type"),&m_AutoVMEType,1);
  wxString typeArray[3] = {_("Volume"),_("Image")};
  m_Gui->Radio(ID_SETTING_VME_TYPE, "VME output", &m_OutputType, 2, typeArray, 1, ""/*, wxRA_SPECIFY_ROWS*/);

  m_Gui->Divider();

  m_DicomVmeTypeListBox=m_Gui->CheckList(ID_VME_TYPE,_("VME Type"));
	m_DicomVmeTypeListBox->AddItem(ID_VOLUME,_("Volume"),m_CheckOnOffVmeType[0] != 0);
	m_DicomVmeTypeListBox->AddItem(ID_IMAGE,_("Image"),m_CheckOnOffVmeType[2] != 0);

  m_Gui->Divider();
  m_Gui->Divider(1);
  m_Gui->Divider();

	wxString choices[4]={_("1x"),_("2x"),_("3x"),_("4x")};
	m_Gui->Combo(ID_STEP,_("Build Step"),&m_Step,4,choices);
	m_DicomModalityListBox=m_Gui->CheckList(ID_TYPE_DICOM,_("Modality"));
	m_DicomModalityListBox->AddItem(ID_CT_MODALITY,_("CT"),m_CheckOnOff[0] != 0);
	m_DicomModalityListBox->AddItem(ID_SC_MODALITY,_("SC"),m_CheckOnOff[1] != 0);
	m_DicomModalityListBox->AddItem(ID_MRI_MODALITY,_("MRI"),m_CheckOnOff[2] != 0);
	m_DicomModalityListBox->AddItem(ID_XA_MODALITY,_("XA"),m_CheckOnOff[3] != 0);
  m_DicomModalityListBox->AddItem(ID_OT_MODALITY,_("OT"),m_CheckOnOff[4] != 0);
  m_DicomModalityListBox->AddItem(ID_CR_MODALITY,_("CR"),m_CheckOnOff[5] != 0);
  m_DicomModalityListBox->AddItem(ID_DX_MODALITY,_("DX"),m_CheckOnOff[6] != 0);
	m_DicomModalityListBox->AddItem(ID_RF_MODALITY,_("RF"),m_CheckOnOff[7] != 0);
  
	m_Gui->Divider(1);

	

  wxString outputNameTypeChoices[3] = {_("Traditional format"),_("Format : 'description_numslices'"),_("Custom")};
  m_Gui->Radio(ID_OUTPUT_NAME,_("Output name"),&m_OutputNameType,3,outputNameTypeChoices);
  m_NameCompositorList = m_Gui->CheckList(ID_NAME_COMPOSITOR,"");
 // m_NameCompositorList->AddItem(ID_SERIES,_("Series"),m_CheckNameCompositor[ID_SERIES]);
  m_NameCompositorList->AddItem(ID_DESCRIPTION,_("Description"),m_CheckNameCompositor[ID_DESCRIPTION]);
  m_NameCompositorList->AddItem(ID_PATIENT_NAME,_("Patient Name"),m_CheckNameCompositor[ID_PATIENT_NAME]);
  m_NameCompositorList->AddItem(ID_BIRTHDATE,_("Birthdate"),m_CheckNameCompositor[ID_BIRTHDATE]);
  m_NameCompositorList->AddItem(ID_NUM_SLICES,_("Num. Slices"),m_CheckNameCompositor[ID_NUM_SLICES]);
  m_NameCompositorList->Enable(m_OutputNameType == CUSTOM);

  m_Gui->Divider(1);

	m_Gui->Update();
  m_Gui->Enable(ID_VME_TYPE,!(m_AutoVMEType==TRUE));
  m_Gui->Enable(ID_SETTING_VME_TYPE,(m_AutoVMEType==TRUE));
  m_Gui->Enable(ID_SCALAR_TOLERANCE,(m_ScalarTolerance==TRUE));
  m_Gui->Enable(ID_PERCENTAGE_TOLERANCE,(m_PercentageTolerance==TRUE));
}
//----------------------------------------------------------------------------
void mafGUIDicomSettings::EnableItems()
//----------------------------------------------------------------------------
{
	//m_Gui->Enable(ID_DICTONARY,true); Remove dictionary selection (Losi 25.11.2009)
  if (m_Gui)
  {
	  m_Gui->Enable(ID_VME_TYPE,!(m_AutoVMEType==TRUE));
    m_Gui->Enable(ID_SETTING_VME_TYPE,(m_AutoVMEType==TRUE));
	  m_Gui->Enable(ID_SCALAR_TOLERANCE,(m_ScalarTolerance==TRUE));
	  m_Gui->Enable(ID_PERCENTAGE_TOLERANCE,(m_PercentageTolerance==TRUE));
	  m_Gui->Update();
  }
}
//----------------------------------------------------------------------------
void mafGUIDicomSettings::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	switch(maf_event->GetId())
	{
  case ID_SHOW_ADVANCED_OPTION_SORTING:
    {
      m_Config->Write("ShowAdvancedOptionOfSorting",m_ShowAdvancedOptionOfSorting);
    }
    break;
	case ID_TYPE_DICOM:
		{
      m_CheckOnOff[ID_CT_MODALITY] = m_DicomModalityListBox->IsItemChecked(ID_CT_MODALITY);
      m_CheckOnOff[ID_SC_MODALITY] = m_DicomModalityListBox->IsItemChecked(ID_SC_MODALITY);
      m_CheckOnOff[ID_MRI_MODALITY] = m_DicomModalityListBox->IsItemChecked(ID_MRI_MODALITY);
      m_CheckOnOff[ID_XA_MODALITY] = m_DicomModalityListBox->IsItemChecked(ID_XA_MODALITY);
      m_CheckOnOff[ID_CR_MODALITY] = m_DicomModalityListBox->IsItemChecked(ID_CR_MODALITY);
      m_CheckOnOff[ID_OT_MODALITY] = m_DicomModalityListBox->IsItemChecked(ID_OT_MODALITY);
			m_CheckOnOff[ID_DX_MODALITY] = m_DicomModalityListBox->IsItemChecked(ID_DX_MODALITY);
			m_CheckOnOff[ID_RF_MODALITY] = m_DicomModalityListBox->IsItemChecked(ID_RF_MODALITY);

			m_Config->Write("EnableReadCT",m_DicomModalityListBox->IsItemChecked(ID_CT_MODALITY));
			m_Config->Write("EnableReadSC",m_DicomModalityListBox->IsItemChecked(ID_SC_MODALITY));
			m_Config->Write("EnableReadMI",m_DicomModalityListBox->IsItemChecked(ID_MRI_MODALITY));
			m_Config->Write("EnableReadXA",m_DicomModalityListBox->IsItemChecked(ID_XA_MODALITY));
      m_Config->Write("EnableReadCR",m_DicomModalityListBox->IsItemChecked(ID_CR_MODALITY));
      m_Config->Write("EnableReadOT",m_DicomModalityListBox->IsItemChecked(ID_OT_MODALITY));
      m_Config->Write("EnableReadDX",m_DicomModalityListBox->IsItemChecked(ID_DX_MODALITY));
			m_Config->Write("EnableReadRF",m_DicomModalityListBox->IsItemChecked(ID_RF_MODALITY));
		}
		break;
  case ID_VME_TYPE:
		{
      m_CheckOnOffVmeType[ID_VOLUME] = m_DicomVmeTypeListBox->IsItemChecked(ID_VOLUME);
      m_CheckOnOffVmeType[ID_IMAGE] = m_DicomVmeTypeListBox->IsItemChecked(ID_IMAGE);

      m_Config->Write("EnableTypeVolume",m_DicomVmeTypeListBox->IsItemChecked(ID_VOLUME));
			m_Config->Write("EnableTypeImage",m_DicomVmeTypeListBox->IsItemChecked(ID_IMAGE));
		}
		break;
  case ID_NAME_COMPOSITOR:
    {
      m_CheckNameCompositor[ID_PATIENT_NAME] = m_NameCompositorList->IsItemChecked(ID_PATIENT_NAME);
      m_CheckNameCompositor[ID_DESCRIPTION] = m_NameCompositorList->IsItemChecked(ID_DESCRIPTION);
      m_CheckNameCompositor[ID_BIRTHDATE] = m_NameCompositorList->IsItemChecked(ID_BIRTHDATE);
      m_CheckNameCompositor[ID_NUM_SLICES] = m_NameCompositorList->IsItemChecked(ID_NUM_SLICES);
  //    m_CheckNameCompositor[ID_SERIES] = m_NameCompositorList->IsItemChecked(ID_SERIES);

      m_Config->Write("NameCompositorPatientName",m_NameCompositorList->IsItemChecked(ID_PATIENT_NAME));
      m_Config->Write("NameCompositorDescription",m_NameCompositorList->IsItemChecked(ID_DESCRIPTION));
      m_Config->Write("NameCompositorBirthdate",m_NameCompositorList->IsItemChecked(ID_BIRTHDATE));
      m_Config->Write("NameCompositorNumSlices",m_NameCompositorList->IsItemChecked(ID_NUM_SLICES));
  //    m_Config->Write("NameCompositorSeries",m_NameCompositorList->IsItemChecked(ID_SERIES));
    }
    break;
	case ID_AUTO_VME_TYPE:
    {
      m_Config->Write("AutoVMEType",m_AutoVMEType);
    }
    break;
	case ID_STEP:
    {
      m_Config->Write("StepOfBuild",m_Step);
    }
    break;
  case ID_SIDE:
    {
      m_Config->Write("EnableSide",m_EnableChangeSide);
    }
    break;
  case ID_DISCARD_ORIGIN:
    {
      m_Config->Write("EnableDiscardPosition",m_EnableDiscardPosition);
    }
    break;
  case ID_RESAMPLE_VOLUME:
    {
      m_Config->Write("EnableResampleVolume",m_EnableResampleVolume);
    }
    break;
  case ID_RESCALE_TO_16_BIT:
    {
      m_Config->Write("EnableRescaleTo16Bit",m_EnableRescaleTo16Bit);
    }
    break;
  case ID_ENABLE_POS_INFO:
    {
      m_Config->Write("EnableVisualizationPosition",m_VisualizePosition);
    }
    break;
  case ID_SETTING_VME_TYPE:
    {
      m_Config->Write("VMEType",m_OutputType);
    }
    break;
  case ID_SCALAR_DISTANCE_TOLERANCE:
    {
      m_Config->Write("EnableScalarDistance",m_ScalarTolerance);
      m_PercentageTolerance = FALSE;
      m_Config->Write("EnablePercentageDistance",m_PercentageTolerance);
    }
    break;
  case ID_SCALAR_TOLERANCE:
    {
      m_Config->Write("ScalarDistance",m_ScalarDistanceTolerance);
    }
    break;
  case ID_PERCENTAGE_DISTANCE_TOLERANCE:
    {
      m_Config->Write("EnablePercentageDistance",m_PercentageTolerance);
      m_ScalarTolerance = FALSE;
      m_Config->Write("EnableScalarDistance",m_ScalarTolerance);
    }
    break;
  case ID_PERCENTAGE_TOLERANCE:
    {
      m_Config->Write("PercentageDistance",m_PercentageDistanceTolerance);
    }
    break;
  case ID_OUTPUT_NAME:
    {
      m_Config->Write("OutputNameFormat",m_OutputNameType);

      m_NameCompositorList->Enable(m_OutputNameType == CUSTOM);
    }
    break;
  case ID_DCM_POSITION_PATIENT_CHOICE:
	{
        m_Config->Write("DCM_ImagePositionPatientchoice",m_DCM_ImagePositionPatientchoice);
	}
	break;
	default:
		mafEventMacro(*maf_event);
		break; 
	}
	EnableItems();
	m_Config->Flush();
}
//----------------------------------------------------------------------------
void mafGUIDicomSettings::InitializeSettings()
//----------------------------------------------------------------------------
{
	wxString string_item;
	long long_item;
	double double_item;

  if(m_Config->Read("OutputNameFormat", &long_item))
    m_OutputNameType=long_item;
  else
    m_Config->Write("OutputNameFormat",m_OutputNameType);

	if(m_Config->Read("EnableSide", &long_item))
		m_EnableChangeSide=long_item;
	else
		m_Config->Write("EnableSide",m_EnableChangeSide);

	if(m_Config->Read("EnableDiscardPosition", &long_item))
		m_EnableDiscardPosition=long_item;
	else
		m_Config->Write("EnableDiscardPosition",m_EnableDiscardPosition);

	if(m_Config->Read("EnableResampleVolume", &long_item))
		m_EnableResampleVolume=long_item;
	else
		m_Config->Write("EnableResampleVolume",m_EnableResampleVolume);

	if(m_Config->Read("EnableRescaleTo16Bit", &long_item))
		m_EnableRescaleTo16Bit=long_item;
	else
		m_Config->Write("EnableRescaleTo16Bit",m_EnableRescaleTo16Bit);

	if(m_Config->Read("EnableVisualizationPosition", &long_item))
		m_VisualizePosition=long_item;
	else
		m_Config->Write("EnableVisualizationPosition",m_VisualizePosition);
		
	if(m_Config->Read("StepOfBuild", &long_item))
		m_Step=long_item;
	else
		m_Config->Write("StepOfBuild",m_Step);

	if(m_Config->Read("LastDicomDir", &string_item))
		m_LastDicomDir=string_item.c_str();
	else
		m_Config->Write("LastDicomDir",m_LastDicomDir.c_str());

	if(m_Config->Read("EnableReadCT", &long_item))
		m_CheckOnOff[0]=long_item;
	else
		m_Config->Write("EnableReadCT",m_CheckOnOff[0]);

	if(m_Config->Read("EnableReadSC", &long_item))
		m_CheckOnOff[1]=long_item;
	else
		m_Config->Write("EnableReadSC",m_CheckOnOff[1]);

	if(m_Config->Read("EnableReadMI", &long_item))
		m_CheckOnOff[2]=long_item;
	else
		m_Config->Write("EnableReadMI",m_CheckOnOff[2]);

	if(m_Config->Read("EnableReadXA", &long_item))
		m_CheckOnOff[3]=long_item;
	else
		m_Config->Write("EnableReadXA",m_CheckOnOff[3]);

	if(m_Config->Read("EnableReadOT", &long_item))
		m_CheckOnOff[4]=long_item;
	else
		m_Config->Write("EnableReadOT",m_CheckOnOff[4]);

	if(m_Config->Read("EnableReadCR", &long_item))
		m_CheckOnOff[5]=long_item;
	else
		m_Config->Write("EnableReadCR",m_CheckOnOff[5]);

  if(m_Config->Read("EnableReadDX", &long_item))
    m_CheckOnOff[6]=long_item;
  else
    m_Config->Write("EnableReadDX",m_CheckOnOff[6]);

	if(m_Config->Read("EnableReadRF", &long_item))
		m_CheckOnOff[7]=long_item;
	else
		m_Config->Write("EnableReadRF",m_CheckOnOff[7]);

  if(m_Config->Read("EnableTypeVolume", &long_item))
		m_CheckOnOffVmeType[0]=long_item;
	else
		m_Config->Write("EnableTypeVolume",m_CheckOnOffVmeType[0]);
  
  if(m_Config->Read("EnableTypeImage", &long_item))
		m_CheckOnOffVmeType[2]=long_item;
	else
		m_Config->Write("EnableTypeImage",m_CheckOnOffVmeType[2]);
		
	if(m_Config->Read("AutoVMEType", &long_item))
		m_AutoVMEType=long_item;
	else
		m_Config->Write("AutoVMEType",m_AutoVMEType);

	if(m_Config->Read("VMEType", &long_item))
		m_OutputType=long_item;
	else
		m_Config->Write("VMEType",m_OutputType);

	if(m_Config->Read("EnableScalarDistance", &long_item))
		m_ScalarTolerance=long_item;
	else
		m_Config->Write("EnableScalarDistance",m_ScalarTolerance);

	if(m_Config->Read("ScalarDistance", &double_item))
		m_ScalarDistanceTolerance=double_item;
	else
		m_Config->Write("ScalarDistance",m_ScalarDistanceTolerance);

	if(m_Config->Read("EnablePercentageDistance", &long_item))
		m_PercentageTolerance=long_item;
	else
		m_Config->Write("EnablePercentageDistance",m_PercentageTolerance);

	if(m_Config->Read("PercentageDistance", &double_item))
		m_PercentageDistanceTolerance=double_item;
	else
		m_Config->Write("PercentageDistance",m_PercentageDistanceTolerance);

  if(m_Config->Read("ShowAdvancedOptionOfSorting", &long_item))
    m_ShowAdvancedOptionOfSorting=long_item;
  else
    m_Config->Write("ShowAdvancedOptionOfSorting",m_ShowAdvancedOptionOfSorting);

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
//----------------------------------------------------------------------------
void mafGUIDicomSettings::SetEnableToRead(char* type,bool enable)
//----------------------------------------------------------------------------
{
  if (strcmp( type, "SC" ) == 0)
  {
    m_CheckOnOff[ID_SC_MODALITY] = enable;
    m_Config->Write("EnableReadSC",m_CheckOnOff[ID_SC_MODALITY]);
  }
  if (strcmp( type, "CT" ) == 0)
  {
    m_CheckOnOff[ID_CT_MODALITY] = enable;
    m_Config->Write("EnableReadCT",m_CheckOnOff[ID_CT_MODALITY]);
  }
  if (strcmp( type, "XA" ) == 0)
  {
    m_CheckOnOff[ID_XA_MODALITY] = enable;
    m_Config->Write("EnableReadXA",m_CheckOnOff[ID_XA_MODALITY]);
  }
  if (strcmp( type, "MR" ) == 0)
  {
    m_CheckOnOff[ID_MRI_MODALITY] = enable;
    m_Config->Write("EnableReadMI",m_CheckOnOff[ID_MRI_MODALITY]);
  }
  if (strcmp( type, "OT" ) == 0)
  {	
    m_CheckOnOff[ID_OT_MODALITY] = enable;
    m_Config->Write("EnableReadOT",m_CheckOnOff[ID_OT_MODALITY]);
  }
  if (strcmp( type, "CR" ) == 0)
  {	
    m_CheckOnOff[ID_CR_MODALITY] = enable;
    m_Config->Write("EnableReadCR",m_CheckOnOff[ID_CR_MODALITY]);
  }
  if (strcmp( type, "DX" ) == 0)
  {	
    m_CheckOnOff[ID_DX_MODALITY] = enable;
    m_Config->Write("EnableReadDX",m_CheckOnOff[ID_DX_MODALITY]);
  }
	if (strcmp( type, "RF" ) == 0)
	{	
		m_CheckOnOff[ID_RF_MODALITY] = enable;
		m_Config->Write("EnableReadRF",m_CheckOnOff[ID_RF_MODALITY]);
	}

  if (strcmp( type, "VOLUME" ) == 0)
  {	
    m_CheckOnOffVmeType[ID_VOLUME] = enable;
    m_Config->Write("EnableTypeVolume",m_CheckOnOffVmeType[ID_VOLUME]);
  }
  if (strcmp( type, "IMAGE" ) == 0)
  {	
    m_CheckOnOffVmeType[ID_IMAGE] = enable;
    m_Config->Write("EnableTypeImage",m_CheckOnOffVmeType[ID_IMAGE]);
  }
  
  InitializeSettings();
}
//----------------------------------------------------------------------------
bool mafGUIDicomSettings::EnableToRead(char* type)
//----------------------------------------------------------------------------
{
	if (m_Gui)
	{
		if (strcmp( type, "SC" ) == 0 && m_DicomModalityListBox->IsItemChecked(ID_SC_MODALITY))
		{
			return true;
		}
		if (strcmp( type, "CT" ) == 0 && m_DicomModalityListBox->IsItemChecked(ID_CT_MODALITY))
		{
			return true;
		}
		if (strcmp( type, "XA" ) == 0 && m_DicomModalityListBox->IsItemChecked(ID_XA_MODALITY))
		{
			return true;
		}
		if (strcmp( type, "MR" ) == 0 && (m_DicomModalityListBox->IsItemChecked(ID_MRI_MODALITY)))
		{	
			return true;
		}
	  if (strcmp( type, "OT" ) == 0 && (m_DicomModalityListBox->IsItemChecked(ID_OT_MODALITY)))
	  {	
	    return true;
	  }
	  if (strcmp( type, "CR" ) == 0 && (m_DicomModalityListBox->IsItemChecked(ID_CR_MODALITY)))
	  {	
	    return true;
	  }
		if (strcmp( type, "DX" ) == 0 && (m_DicomModalityListBox->IsItemChecked(ID_DX_MODALITY)))
		{	
			return true;
		}
		if (strcmp( type, "RF" ) == 0 && (m_DicomModalityListBox->IsItemChecked(ID_RF_MODALITY)))
		{	
			return true;
		}

    if (strcmp( type, "VOLUME" ) == 0 && (m_DicomVmeTypeListBox->IsItemChecked(ID_VOLUME)))
	  {	
	    return true;
	  }
    if (strcmp( type, "IMAGE" ) == 0 && (m_DicomVmeTypeListBox->IsItemChecked(ID_IMAGE)))
	  {	
	    return true;
	  }
	
		return false;
	}
  else//For test mode
  {
    if (strcmp( type, "SC" ) == 0 && m_CheckOnOff[ID_SC_MODALITY])
    {
      return true;
    }
    if (strcmp( type, "CT" ) == 0 && m_CheckOnOff[ID_CT_MODALITY])
    {
      return true;
    }
    if (strcmp( type, "XA" ) == 0 && m_CheckOnOff[ID_XA_MODALITY])
    {
      return true;
    }
    if (strcmp( type, "MR" ) == 0 && m_CheckOnOff[ID_MRI_MODALITY])
    {	
      return true;
    }
    if (strcmp( type, "OT" ) == 0 && m_CheckOnOff[ID_OT_MODALITY])
    {	
      return true;
    }
    if (strcmp( type, "CR" ) == 0 && m_CheckOnOff[ID_CR_MODALITY])
    {	
      return true;
    }
		if (strcmp( type, "DX" ) == 0 && m_CheckOnOff[ID_DX_MODALITY])
		{	
			return true;
		} 
		if (strcmp( type, "RF" ) == 0 && m_CheckOnOff[ID_RF_MODALITY])
		{	
			return true;
		}
    
    if (strcmp( type, "VOLUME" ) == 0 && m_CheckOnOffVmeType[ID_VOLUME])
    {	
      return true;
    }
    if (strcmp( type, "IMAGE" ) == 0 && m_CheckOnOffVmeType[ID_IMAGE])
    {	
      return true;
    }

    return false;
  }
}

void mafGUIDicomSettings::SetLastDicomDir( wxString lastDicomDir )
{
  m_LastDicomDir = lastDicomDir;
  assert(m_Config);
  m_Config->Write("LastDicomDir",m_LastDicomDir.c_str());

}

int mafGUIDicomSettings::GetEnabledCustomName( int type )
{
  if (type >= ID_DESCRIPTION && type<=ID_NUM_SLICES)
  {
    return m_CheckNameCompositor[type];
  }
}
