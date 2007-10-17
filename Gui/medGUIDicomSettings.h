/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medGUIDicomSettings.h,v $
Language:  C++
Date:      $Date: 2007-10-17 16:27:47 $
Version:   $Revision: 1.2 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __medGUIDicomSettings_H__
#define __medGUIDicomSettings_H__

#include "mafGUISettings.h"

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------
class mmgCheckListBox;

//----------------------------------------------------------------------------
// medGUIDicomSettings :
//----------------------------------------------------------------------------
/**
*/
class medGUIDicomSettings : public mafGUISettings
{
public:
	medGUIDicomSettings(mafObserver *Listener, const mafString &label = _("Dicom"));
	~medGUIDicomSettings(); 

	enum DICOM_SETTINGS_WIDGET_ID
	{
		ID_DICTONARY = MINID,
		ID_TYPE_DICOM,
		ID_AUTO_POS_CROP,
	};

	enum DICOM_MODALITY
	{
		ID_CT_MODALITY = 0,
		ID_SC_MODALITY,
		ID_MRI_MODALITY,
		ID_XA_MODALITY,
		ID_CMRI_MODALITY,
	};


	/** Answer to the messages coming from interface. */
	void OnEvent(mafEventBase *maf_event);

	mafString GetDictionary(){return m_Dictionary;};

	int AutoCropPosition(){return m_AutoCropPos;};

	/** Return if a particular type of Dicom is Enabled to be read */
	bool EnableToRead(char* type);

protected:
	/** Create the GUI for the setting panel.*/
	void CreateGui();

	/** Initialize the application settings.*/
	void InitializeSettings();

	/** Used to enable/disable items according to the current widgets state.*/
	void EnableItems();

	mafString m_Dictionary;
	mmgCheckListBox *m_DicomModalityListBox;

	int m_CheckOnOff[4];

	int m_AutoCropPos;
};
#endif
