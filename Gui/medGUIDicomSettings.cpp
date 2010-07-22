/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medGUIDicomSettings.cpp,v $
Language:  C++
Date:      $Date: 2010-07-22 14:55:04 $
Version:   $Revision: 1.7.2.13 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "medGUIDicomSettings.h"

#include "mafDecl.h"
#include "mafGUI.h"
#include "mafGUICheckListBox.h"

//----------------------------------------------------------------------------
medGUIDicomSettings::medGUIDicomSettings(mafObserver *Listener, const mafString &label):
mafGUISettings(Listener, label)
//----------------------------------------------------------------------------
{
	// m_Dictionary = "";

	m_CheckOnOff[0] = m_CheckOnOff[1] = m_CheckOnOff[2] = m_CheckOnOff[3] = m_CheckOnOff[4] = m_CheckOnOff[5] = true;

	m_AutoCropPos = FALSE;
	m_EnableNumberOfSlice = TRUE;
	m_EnableNumberOfTime = TRUE; 
  m_EnableChangeSide = FALSE;
  m_EnableDiscardPosition = FALSE;
  m_EnableResampleVolume = FALSE;
  m_EnableRescaleTo16Bit = FALSE;
  m_VisualizePosition = FALSE;
  m_EnableZCrop =  TRUE;
  m_AutoVMEType = FALSE;
  m_PercentageTolerance = FALSE;
  m_ScalarTolerance = FALSE;
  m_OutputType = 0;
  m_ScalarDistanceTolerance = 0.3;
  m_PercentageDistanceTolerance = 88;
  m_LastDicomDir = "UNEDFINED_m_LastDicomDir";
  m_Step = ID_1X;

  m_Config->SetPath("Importer Dicom"); // Regiser key path Added by Losi 15.11.2009
	InitializeSettings();
}
//----------------------------------------------------------------------------
medGUIDicomSettings::~medGUIDicomSettings()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void medGUIDicomSettings::CreateGui()
//----------------------------------------------------------------------------
{
	m_Gui = new mafGUI(this);
	// m_Gui->FileOpen(ID_DICTONARY,_("Dictionary"),&m_Dictionary); Remove dictionary selection (Losi 25.11.2009)
	m_Gui->Bool(ID_AUTO_POS_CROP,_("Auto Crop"),&m_AutoCropPos,1);
  m_Gui->Bool(ID_ENALBLE_TIME_BAR,_("Enable Time Bar"),&m_EnableNumberOfTime,1);
	m_Gui->Bool(ID_ENALBLE_NUMBER_OF_SLICE,_("Enable Number of Slice"),&m_EnableNumberOfSlice,1);
  m_Gui->Bool(ID_SIDE,_("Enable Change Side"),&m_EnableChangeSide,1);
  m_Gui->Bool(ID_DISCARD_ORIGIN,_("Enable Discard Origin"),&m_EnableDiscardPosition,1);
  m_Gui->Bool(ID_RESAMPLE_VOLUME,_("Enable Resample Volume"),&m_EnableResampleVolume,1);
  m_Gui->Bool(ID_RESCALE_TO_16_BIT,_("Enable Rescaling to 16 Bit"),&m_EnableRescaleTo16Bit,1);
  m_Gui->Bool(ID_Z_CROP,_("Enable Z-direction Crop"),&m_EnableZCrop,1);
  m_Gui->Bool(ID_ENABLE_POS_INFO,_("Visualize Position and Orientation"),&m_VisualizePosition,1);

  m_Gui->Bool(ID_SCALAR_DISTANCE_TOLERANCE,_("Scalar distance tolerance"),&m_ScalarTolerance,1);
  m_Gui->Double(ID_SCALAR_TOLERANCE,_("Value"),&m_ScalarDistanceTolerance,0,MAXDOUBLE,5,"Value in millimeter");

  m_Gui->Bool(ID_PERCENTAGE_DISTANCE_TOLERANCE,_("Percentage distance tolerance"),&m_PercentageTolerance,1);
  m_Gui->Double(ID_PERCENTAGE_TOLERANCE,_("Value"),&m_PercentageDistanceTolerance,0,MAXDOUBLE,2,"Value in percentage");

  m_Gui->Bool(ID_AUTO_VME_TYPE,_("Auto VME Type"),&m_AutoVMEType,1);
  wxString typeArray[3] = {_("Volume"),_("Mesh"),_("Image")};
  m_Gui->Radio(ID_SETTING_VME_TYPE, "VME output", &m_OutputType, 3, typeArray, 1, ""/*, wxRA_SPECIFY_ROWS*/);

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
	m_Gui->Divider(1);

	m_Gui->Update();
  m_Gui->Enable(ID_SETTING_VME_TYPE,m_AutoVMEType);
  m_Gui->Enable(ID_SCALAR_TOLERANCE,m_ScalarTolerance);
  m_Gui->Enable(ID_PERCENTAGE_TOLERANCE,m_PercentageTolerance);
}
//----------------------------------------------------------------------------
void medGUIDicomSettings::EnableItems()
//----------------------------------------------------------------------------
{
	//m_Gui->Enable(ID_DICTONARY,true); Remove dictionary selection (Losi 25.11.2009)
  if (m_Gui)
  {
	  m_Gui->Enable(ID_SETTING_VME_TYPE,m_AutoVMEType);
	
	  m_Gui->Enable(ID_SCALAR_TOLERANCE,m_ScalarTolerance);
	  m_Gui->Enable(ID_PERCENTAGE_TOLERANCE,m_PercentageTolerance);
	  m_Gui->Update();
  }
}
//----------------------------------------------------------------------------
void medGUIDicomSettings::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	switch(maf_event->GetId())
	{
//  Remove dictionary selection (Losi 25.11.2009)
// 	case ID_DICTONARY:
// 		{
// 			m_Config->Write("DicomDictionary",m_Dictionary.GetCStr());
// 		}
// 		break;
	case ID_TYPE_DICOM:
		{
      m_CheckOnOff[ID_CT_MODALITY] = m_DicomModalityListBox->IsItemChecked(ID_CT_MODALITY);
      m_CheckOnOff[ID_SC_MODALITY] = m_DicomModalityListBox->IsItemChecked(ID_SC_MODALITY);
      m_CheckOnOff[ID_MRI_MODALITY] = m_DicomModalityListBox->IsItemChecked(ID_MRI_MODALITY);
      m_CheckOnOff[ID_XA_MODALITY] = m_DicomModalityListBox->IsItemChecked(ID_XA_MODALITY);
      m_CheckOnOff[ID_CR_MODALITY] = m_DicomModalityListBox->IsItemChecked(ID_CR_MODALITY);
      m_CheckOnOff[ID_OT_MODALITY] = m_DicomModalityListBox->IsItemChecked(ID_OT_MODALITY);

			m_Config->Write("EnableReadCT",m_DicomModalityListBox->IsItemChecked(ID_CT_MODALITY));
			m_Config->Write("EnableReadSC",m_DicomModalityListBox->IsItemChecked(ID_SC_MODALITY));
			m_Config->Write("EnableReadMI",m_DicomModalityListBox->IsItemChecked(ID_MRI_MODALITY));
			m_Config->Write("EnableReadXA",m_DicomModalityListBox->IsItemChecked(ID_XA_MODALITY));
      m_Config->Write("EnableReadCR",m_DicomModalityListBox->IsItemChecked(ID_CR_MODALITY));
      m_Config->Write("EnableReadOT",m_DicomModalityListBox->IsItemChecked(ID_OT_MODALITY));
		}
		break;
	case ID_AUTO_POS_CROP:
		{
			m_Config->Write("AutoCropPos",m_AutoCropPos);
      EnableItems();
		}
		break;
  case ID_AUTO_VME_TYPE:
    {
      m_Config->Write("AutoVMEType",m_AutoVMEType);

    }
    break;
	case ID_ENALBLE_NUMBER_OF_SLICE:
		{
			m_Config->Write("EnableNumberOfSlice",m_EnableNumberOfSlice);
		}
		break;
	case ID_ENALBLE_TIME_BAR:
		{
			m_Config->Write("EnableTimeBar",m_EnableNumberOfTime);
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
  case ID_Z_CROP:
    {
      m_Config->Write("EnableZCrop",m_EnableZCrop);
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
	default:
		mafEventMacro(*maf_event);
		break; 
	}
	EnableItems();
	m_Config->Flush();
}
//----------------------------------------------------------------------------
void medGUIDicomSettings::InitializeSettings()
//----------------------------------------------------------------------------
{
	wxString string_item;
	long long_item;
	double double_item;

	if(m_Config->Read("EnableSide", &long_item))
	{
		m_EnableChangeSide=long_item;
	}
	else
	{
		m_Config->Write("EnableSide",m_EnableChangeSide);
	}

	if(m_Config->Read("EnableDiscardPosition", &long_item))
	{
		m_EnableDiscardPosition=long_item;
	}
	else
	{
		m_Config->Write("EnableDiscardPosition",m_EnableDiscardPosition);
	}

	if(m_Config->Read("EnableResampleVolume", &long_item))
	{
		m_EnableResampleVolume=long_item;
	}
	else
	{
		m_Config->Write("EnableResampleVolume",m_EnableResampleVolume);
	}

	if(m_Config->Read("EnableRescaleTo16Bit", &long_item))
	{
		m_EnableRescaleTo16Bit=long_item;
	}
	else
	{
		m_Config->Write("EnableRescaleTo16Bit",m_EnableRescaleTo16Bit);
	}

	if(m_Config->Read("EnableVisualizationPosition", &long_item))
	{
		m_VisualizePosition=long_item;
	}
	else
	{
		m_Config->Write("EnableVisualizationPosition",m_VisualizePosition);
	}

	if(m_Config->Read("EnableZCrop", &long_item))
	{
		m_EnableZCrop=long_item;
	}
	else
	{
		m_Config->Write("EnableZCrop",m_EnableZCrop);
	}


	if(m_Config->Read("StepOfBuild", &long_item))
	{
		m_Step=long_item;
	}
	else
	{
		m_Config->Write("StepOfBuild",m_Step);
	}

	if(m_Config->Read("LastDicomDir", &string_item))
	{
		m_LastDicomDir=string_item.c_str();
	}
	else
	{
		m_Config->Write("LastDicomDir",m_LastDicomDir.c_str());
	}

	if(m_Config->Read("EnableReadCT", &long_item))
	{
		m_CheckOnOff[0]=long_item;
	}
	else
	{
		m_Config->Write("EnableReadCT",m_CheckOnOff[0]);
	}

	if(m_Config->Read("EnableReadSC", &long_item))
	{
		m_CheckOnOff[1]=long_item;
	}
	else
	{
		m_Config->Write("EnableReadSC",m_CheckOnOff[1]);
	}

	if(m_Config->Read("EnableReadMI", &long_item))
	{
		m_CheckOnOff[2]=long_item;
	}
	else
	{
		m_Config->Write("EnableReadMI",m_CheckOnOff[2]);
	}

	if(m_Config->Read("EnableReadXA", &long_item))
	{
		m_CheckOnOff[3]=long_item;
	}
	else
	{
		m_Config->Write("EnableReadXA",m_CheckOnOff[3]);
	}

	if(m_Config->Read("EnableReadOT", &long_item))
	{
		m_CheckOnOff[4]=long_item;
	}
	else
	{
		m_Config->Write("EnableReadOT",m_CheckOnOff[4]);
	}

	if(m_Config->Read("EnableReadCR", &long_item))
	{
		m_CheckOnOff[5]=long_item;
	}
	else
	{
		m_Config->Write("EnableReadCR",m_CheckOnOff[5]);
	}

	if(m_Config->Read("AutoCropPos", &long_item))
	{
		m_AutoCropPos=long_item;
	}
	else
	{
		m_Config->Write("AutoCropPos",m_AutoCropPos);
	}

	if(m_Config->Read("AutoVMEType", &long_item))
	{
		m_AutoVMEType=long_item;
	}
	else
	{
		m_Config->Write("AutoVMEType",m_AutoVMEType);
	}

	if(m_Config->Read("VMEType", &long_item))
	{
		m_OutputType=long_item;
	}
	else
	{
		m_Config->Write("VMEType",m_OutputType);
	}

	if(m_Config->Read("EnableTimeBar", &long_item))
	{
		m_EnableNumberOfTime=long_item;
	}
	else
	{
		m_Config->Write("EnableTimeBar",m_EnableNumberOfTime);
	}

	if(m_Config->Read("EnableNumberOfSlice", &long_item))
	{
		m_EnableNumberOfSlice=long_item;
	}
	else
	{
		m_Config->Write("EnableNumberOfSlice",m_EnableNumberOfSlice);
	}

	if(m_Config->Read("EnableScalarDistance", &long_item))
	{
		m_ScalarTolerance=long_item;
	}
	else
	{
		m_Config->Write("EnableScalarDistance",m_ScalarTolerance);
	}

	if(m_Config->Read("ScalarDistance", &double_item))
	{
		m_ScalarDistanceTolerance=double_item;
	}
	else
	{
		m_Config->Write("ScalarDistance",m_ScalarDistanceTolerance);
	}

	if(m_Config->Read("EnablePercentageDistance", &long_item))
	{
		m_PercentageTolerance=long_item;
	}
	else
	{
		m_Config->Write("EnablePercentageDistance",m_PercentageTolerance);
	}

	if(m_Config->Read("PercentageDistance", &double_item))
	{
		m_PercentageDistanceTolerance=double_item;
	}
	else
	{
		m_Config->Write("PercentageDistance",m_PercentageDistanceTolerance);
	}


	m_Config->Flush();
}
//----------------------------------------------------------------------------
void medGUIDicomSettings::SetEnableToRead(char* type,bool enable)
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
  
  InitializeSettings();
}
//----------------------------------------------------------------------------
bool medGUIDicomSettings::EnableToRead(char* type)
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
		if (strcmp( type, "MR" ) == 0 && (m_DicomModalityListBox->IsItemChecked(ID_MRI_MODALITY)||m_DicomModalityListBox->IsItemChecked(ID_CMRI_MODALITY)))
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

    return false;
  }
}