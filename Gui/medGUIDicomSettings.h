/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medGUIDicomSettings.h,v $
Language:  C++
Date:      $Date: 2009-02-25 16:43:09 $
Version:   $Revision: 1.5.2.2 $
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
class mafGUICheckListBox;

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
		ID_ENALBLE_TIME_BAR,
		ID_ENALBLE_NUMBER_OF_SLICE,
		ID_STEP,
    ID_SIDE,
    ID_CONVERT_UNITS,
	};

	enum DICOM_MODALITY
	{
		ID_CT_MODALITY = 0,
		ID_SC_MODALITY,
		ID_MRI_MODALITY,
		ID_XA_MODALITY,
    ID_OT_MODALITY,
		ID_CMRI_MODALITY,
	};

	enum DICOM_STEP
	{
		ID_1X = 0,
		ID_2X,
		ID_3X,
		ID_4X,
	};

  enum DICOM_CONVERSION
  {
    NONE = 0,
    mm2m,
  };

	/** Answer to the messages coming from interface. */
	void OnEvent(mafEventBase *maf_event);

	mafString GetDictionary(){return m_Dictionary;};

	int AutoCropPosition(){return m_AutoCropPos;};
	int EnableNumberOfTime(){return m_EnableNumberOfTime;};
	int EnableNumberOfSlice(){return m_EnableNumberOfSlice;};
  int GetBuildStep(){return m_Step;};
  int EnableChangeSide(){return m_EnableChangeSide;};

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
	mafGUICheckListBox *m_DicomModalityListBox;

	int m_CheckOnOff[5];

	int m_AutoCropPos;
	int m_EnableNumberOfTime;
	int m_EnableNumberOfSlice;
	int m_Step;
  int m_EnableChangeSide;
};
#endif
