/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medGUIDicomSettings.cpp,v $
Language:  C++
Date:      $Date: 2009-02-25 16:43:09 $
Version:   $Revision: 1.7.2.2 $
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
	m_Dictionary = "";

	m_CheckOnOff[0] = m_CheckOnOff[1] = m_CheckOnOff[2] = m_CheckOnOff[3] = m_CheckOnOff[4] = true;

	m_AutoCropPos = FALSE;
	m_EnableNumberOfSlice = FALSE;
	m_EnableNumberOfTime = FALSE; 
  m_EnableChangeSide = FALSE;
	m_Step = ID_4X;

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
	m_Gui->FileOpen(ID_DICTONARY,_("Dictionary"),&m_Dictionary);
	m_Gui->Bool(ID_AUTO_POS_CROP,_("Auto Crop"),&m_AutoCropPos,1);
	m_Gui->Bool(ID_ENALBLE_TIME_BAR,_("Enable Time Bar"),&m_EnableNumberOfTime,1);
	m_Gui->Bool(ID_ENALBLE_NUMBER_OF_SLICE,_("Enable Number of Slice"),&m_EnableNumberOfSlice,1);
  m_Gui->Bool(ID_SIDE,_("Enable Change Side"),&m_EnableChangeSide,1);
	wxString choices[4]={_("1x"),_("2x"),_("3x"),_("4x")};
	m_Gui->Combo(ID_STEP,_("Build Step"),&m_Step,4,choices);
	m_DicomModalityListBox=m_Gui->CheckList(ID_TYPE_DICOM,_("Modality"));
	m_DicomModalityListBox->AddItem(ID_CT_MODALITY,_("CT"),m_CheckOnOff[0] != 0);
	m_DicomModalityListBox->AddItem(ID_SC_MODALITY,_("SC"),m_CheckOnOff[1] != 0);
	m_DicomModalityListBox->AddItem(ID_MRI_MODALITY,_("MI"),m_CheckOnOff[2] != 0);
	m_DicomModalityListBox->AddItem(ID_XA_MODALITY,_("XA"),m_CheckOnOff[3] != 0);
  m_DicomModalityListBox->AddItem(ID_OT_MODALITY,_("OT"),m_CheckOnOff[4] != 0);
	m_Gui->Divider(1);

	m_Gui->Update();
}
//----------------------------------------------------------------------------
void medGUIDicomSettings::EnableItems()
//----------------------------------------------------------------------------
{
	m_Gui->Enable(ID_DICTONARY,true);
}
//----------------------------------------------------------------------------
void medGUIDicomSettings::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	switch(maf_event->GetId())
	{
	case ID_DICTONARY:
		{
			m_Config->Write("DicomDictionary",m_Dictionary.GetCStr());
		}
		break;
	case ID_TYPE_DICOM:
		{
			m_Config->Write("EnableReadCT",m_DicomModalityListBox->IsItemChecked(ID_CT_MODALITY));
			m_Config->Write("EnableReadSC",m_DicomModalityListBox->IsItemChecked(ID_SC_MODALITY));
			m_Config->Write("EnableReadMI",m_DicomModalityListBox->IsItemChecked(ID_MRI_MODALITY));
			m_Config->Write("EnableReadXA",m_DicomModalityListBox->IsItemChecked(ID_XA_MODALITY));
		}
		break;
	case ID_AUTO_POS_CROP:
		{
			m_Config->Write("AutoCropPos",m_AutoCropPos);
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

  if(m_Config->Read("EnableSide", &long_item))
  {
    m_EnableChangeSide=long_item;
  }
  else
  {
    m_Config->Write("EnableSide",m_EnableChangeSide);
  }

  if(m_Config->Read("StepOfBuild", &long_item))
  {
    m_Step=long_item;
  }
  else
  {
    m_Config->Write("StepOfBuild",m_Step);
  }

	if(m_Config->Read("DicomDictionary", &string_item))
	{
		m_Dictionary=string_item.c_str();
	}
	else
	{
		m_Config->Write("DicomDictionary",m_Dictionary.GetCStr());
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

	if(m_Config->Read("AutoCropPos", &long_item))
	{
		m_AutoCropPos=long_item;
	}
	else
	{
		m_Config->Write("AutoCropPos",m_AutoCropPos);
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

	m_Config->Flush();
}
//----------------------------------------------------------------------------
bool medGUIDicomSettings::EnableToRead(char* type)
//----------------------------------------------------------------------------
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

	return false;
}