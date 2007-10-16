/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medGUIDicomSettings.cpp,v $
Language:  C++
Date:      $Date: 2007-10-16 13:08:43 $
Version:   $Revision: 1.2 $
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
#include "mmgGui.h"
#include "mmgCheckListBox.h"

//----------------------------------------------------------------------------
medGUIDicomSettings::medGUIDicomSettings(mafObserver *Listener, const mafString &label):
mafGUISettings(Listener, label)
//----------------------------------------------------------------------------
{
	m_Dictionary = "";

	m_CheckOnOff[0] = m_CheckOnOff[1] = m_CheckOnOff[2] = m_CheckOnOff[3] = true;

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
	m_Gui = new mmgGui(this);
	m_Gui->FileOpen(ID_DICTONARY,_("Dictionary"),&m_Dictionary);
	m_Gui->Divider(1);

	m_DicomModalityListBox=m_Gui->CheckList(ID_TYPE_DICOM,_("Modality"));
	m_DicomModalityListBox->AddItem(ID_CT_MODALITY,_("CT"),m_CheckOnOff[0]);
	m_DicomModalityListBox->AddItem(ID_SC_MODALITY,_("SC"),m_CheckOnOff[1]);
	m_DicomModalityListBox->AddItem(ID_MRI_MODALITY,_("MI"),m_CheckOnOff[2]);
	m_DicomModalityListBox->AddItem(ID_XA_MODALITY,_("XA"),m_CheckOnOff[3]);
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

	return false;
}