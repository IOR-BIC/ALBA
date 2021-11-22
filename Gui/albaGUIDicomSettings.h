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

#ifndef __albaGUIDicomSettings_H__
#define __albaGUIDicomSettings_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaGUISettings.h"

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------
class albaGUICheckListBox;

/**
  Class Name : albaGUIDicomSettings.
  Class that contain specific variables regard DICOM, that can be changed from Application Settings, in GUI.
  - Dictionary path
  - DICOM type
  - time bar enable
  - number of slice enable
  - step 
  - conversion units
  - rescale to 16Bit
*/
class ALBA_EXPORT albaGUIDicomSettings : public albaGUISettings
{
public:
  /** constructor.*/
	albaGUIDicomSettings(albaObserver *Listener, const albaString &label = _("Dicom"));
  /** destructor.*/
	~albaGUIDicomSettings(); 

  /** GUI IDs*/
	enum DICOM_SETTINGS_WIDGET_ID
	{
		ID_DICTONARY = MINID,
		ID_TYPE_DICOM,
    ID_STEP,
    ID_CONVERT_UNITS,
    ID_AUTO_VME_TYPE,
    ID_SETTING_VME_TYPE,
		ID_AUTORESAMPLE_OUTPUT,
    ID_DCM_POSITION_PATIENT_CHOICE,
		ID_SKIP_CROP,
		ID_SKIP_NAMING
	};
  

  /** Step IDs*/
	enum DICOM_STEP
	{
		ID_1X = 0,
		ID_2X,
		ID_3X,
		ID_4X,
	};

  /** Conversion IDs*/
  enum DICOM_CONVERSION
  {
    NONE = 0,
    mm2m,
  };

	enum POSITION_EXEPTION_HANDLING
	{
		SKIP_ALL,
		APPLY_DEFAULT_POSITION
	};

  enum NAME_COMPOSITOR
  {
   	ID_DESCRIPTION,
    ID_PATIENT_NAME,
    ID_BIRTHDATE,
    ID_NUM_SLICES,
		ID_SERIES
  };

	/** Answer to the messages coming from interface. */
	void OnEvent(albaEventBase *alba_event);
	  
  /** Retrieve the build step. */
  int GetBuildStep(){return m_Step;};
    
  /** Helper function to store and retrieve the last Dicom dir opened */
	/** Helper function to store and retrieve the last dicom dir opened */
  void SetLastDicomDir(wxString lastDicomDir);
  wxString GetLastDicomDir() {return m_LastDicomDir;};
	
  /** Return if an element of custom name check list is checked */
  int GetEnabledCustomName(enum NAME_COMPOSITOR type);

  /** Return the DCM_ImagePositionPatient choice */
  int GetDCMImagePositionPatientExceptionHandling(){return m_DCM_ImagePositionPatientchoice;};

	/** Sets the DCM_ImagePositionPatient choice */
	void SetDCMImagePositionPatientExceptionHandling(int choice) { m_DCM_ImagePositionPatientchoice = choice; m_Config->Write("DCM_ImagePositionPatientchoice", m_DCM_ImagePositionPatientchoice);};

	/** Returns SkipCrop */
	int GetSkipCrop() const { return m_SkipCrop; }

	/** Sets SkipCrop */
	void SetSkipCrop(int skipCrop) { m_SkipCrop = skipCrop; m_Config->Write("SkipCrop", m_SkipCrop);}

	/** Returns AutoVMEType */
	int GetAutoVMEType() const { return m_AutoVMEType; }

	/** Sets AutoVMEType */
	void SetAutoVMEType(int autoVMEType) { m_AutoVMEType = autoVMEType; m_Config->Write("AutoVMEType", m_AutoVMEType);}

	/** Returns AutoVMEType */
	int GetAutoResample() const { return m_AutoResample; }

	/** Sets AutoVMEType */
	void SetAutoResample(int autoResample) { m_AutoResample = autoResample; m_Config->Write("AutoResample", m_AutoResample); }

	/** Returns OutputType */
	int GetOutputType() const { return m_OutputType; }

	/** Sets OutputType */
	void SetOutputType(int outputType) { m_OutputType = outputType; m_Config->Write("VMEType", m_OutputType);}

protected:
	/** Create the GUI for the setting panel.*/
	void CreateGui();

	/** Initialize the application settings.*/
	void InitializeSettings();

	/** Used to enable/disable items according to the current widgets state.*/
	void EnableItems();

	// albaString m_Dictionary;
	albaGUICheckListBox *m_DicomModalityListBox;
  
  int m_CheckNameCompositor[5];
	
  int m_OutputType;

	int m_Step;
  int m_AutoVMEType;
	int m_AutoResample;
	int m_SkipCrop;
	wxString m_LastDicomDir;
  int m_DCM_ImagePositionPatientchoice;

  friend class albaGUIDicomSettingsTest;
};
#endif
