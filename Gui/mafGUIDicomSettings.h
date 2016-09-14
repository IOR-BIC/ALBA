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

#ifndef __mafGUIDicomSettings_H__
#define __mafGUIDicomSettings_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafGUISettings.h"

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------
class mafGUICheckListBox;

/**
  Class Name : mafGUIDicomSettings.
  Class that contain specific variables regard DICOM, that can be changed from Application Settings, in GUI.
  - Dictionary path
  - DICOM type
  - time bar enable
  - number of slice enable
  - step 
  - conversion units
  - rescale to 16Bit
*/
class MAF_EXPORT mafGUIDicomSettings : public mafGUISettings
{
public:
  /** constructor.*/
	mafGUIDicomSettings(mafObserver *Listener, const mafString &label = _("Dicom"));
  /** destructor.*/
	~mafGUIDicomSettings(); 

  /** GUI IDs*/
	enum DICOM_SETTINGS_WIDGET_ID
	{
		ID_DICTONARY = MINID,
		ID_TYPE_DICOM,
    ID_STEP,
    ID_CONVERT_UNITS,
    ID_AUTO_VME_TYPE,
    ID_SETTING_VME_TYPE,
    ID_DCM_POSITION_PATIENT_CHOICE
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
	void OnEvent(mafEventBase *maf_event);
	  
  /** Retrieve the build step. */
  int GetBuildStep(){return m_Step;};
  
  /** Retrieve auto VME type flag. */
  int AutoVMEType(){return m_AutoVMEType;};
  /** Retrieve VME type. */
  int GetVMEType(){return m_OutputType;};
	  
  /** Helper function to store and retrieve the last Dicom dir opened */
	/** Helper function to store and retrieve the last dicom dir opened */
  void SetLastDicomDir(wxString lastDicomDir);;
  wxString GetLastDicomDir() {return m_LastDicomDir;};
	
  /** Return if an element of custom name check list is checked */
  int GetEnabledCustomName(enum NAME_COMPOSITOR type);

  /** Return the DCM_ImagePositionPatient choice */
  int GetDCMImagePositionPatientExceptionHandling(){return m_DCM_ImagePositionPatientchoice;};

protected:
	/** Create the GUI for the setting panel.*/
	void CreateGui();

	/** Initialize the application settings.*/
	void InitializeSettings();

	/** Used to enable/disable items according to the current widgets state.*/
	void EnableItems();

	// mafString m_Dictionary;
	mafGUICheckListBox *m_DicomModalityListBox;
  
  int m_CheckNameCompositor[5];
	
  int m_OutputType;

	int m_Step;
  int m_AutoVMEType;
  wxString m_LastDicomDir;
  int m_DCM_ImagePositionPatientchoice;

  friend class mafGUIDicomSettingsTest;
};
#endif
