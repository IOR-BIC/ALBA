/*=========================================================================

 Program: MAF2Medical
 Module: medGUIDicomSettings
 Authors: Matteo Giacomoni, Simone Brazzale
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __medGUIDicomSettings_H__
#define __medGUIDicomSettings_H__

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
  Class Name : medGUIDicomSettings.
  Class that contain specific variables regard DICOM, that can be changed from Application Settings, in GUI.
  - Dictionary path
  - DICOM type
  - automatic crop position
  - time bar enable
  - number of slice enable
  - step 
  - side
  - conversion units
  - discard position (origin)
  - resample volume
  - rescale to 16Bit
*/
class MAF_EXPORT medGUIDicomSettings : public mafGUISettings
{
public:
  /** constructor.*/
	medGUIDicomSettings(mafObserver *Listener, const mafString &label = _("Dicom"));
  /** destructor.*/
	~medGUIDicomSettings(); 

  /** GUI IDs*/
	enum DICOM_SETTINGS_WIDGET_ID
	{
		ID_DICTONARY = MINID,
		ID_TYPE_DICOM,
    ID_VME_TYPE,
		ID_AUTO_POS_CROP,
		ID_ENALBLE_TIME_BAR,
		ID_ENALBLE_NUMBER_OF_SLICE,
		ID_STEP,
    ID_SIDE,
    ID_CONVERT_UNITS,
    ID_DISCARD_ORIGIN,
    ID_RESAMPLE_VOLUME,
    ID_RESCALE_TO_16_BIT,
    ID_AUTO_VME_TYPE,
    ID_SETTING_VME_TYPE,
    ID_Z_CROP,
    ID_ENABLE_POS_INFO,
    ID_SCALAR_DISTANCE_TOLERANCE,
    ID_SCALAR_TOLERANCE,
    ID_PERCENTAGE_DISTANCE_TOLERANCE,
    ID_PERCENTAGE_TOLERANCE,
    ID_OUTPUT_NAME,
    ID_SHOW_ADVANCED_OPTION_SORTING,
    ID_DICOM_FOLDER,
    ID_USE_DEFAULT_DICOM_FOLDER,
    ID_NAME_COMPOSITOR,
	ID_DCM_POSITION_PATIENT_CHOICE
	};
  
  /** Type VMEs*/
	enum VME_TYPE
	{
		ID_VOLUME = 0,
		ID_MESH,
		ID_IMAGE,
	};

  /** Modality IDs*/
	enum DICOM_MODALITY
	{
		ID_CT_MODALITY = 0,
		ID_SC_MODALITY,
		ID_MRI_MODALITY,
		ID_XA_MODALITY,
    ID_OT_MODALITY,
    ID_CR_MODALITY,
    ID_DX_MODALITY,
		ID_CMRI_MODALITY,
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

  enum OUTPUT_NAME_FORMAT
  {
    TRADITIONAL = 0,
 //   DESCRIPTION_DATE,
    DESCRIPTION_NUMSLICES,
    CUSTOM,
  };

  enum NAME_COMPOSITOR
  {
//    ID_SERIES = 0,
	ID_DESCRIPTION = 0,
    ID_PATIENT_NAME,
    ID_BIRTHDATE,
    ID_NUM_SLICES,
	ID_SERIES
  };

	/** Answer to the messages coming from interface. */
	void OnEvent(mafEventBase *maf_event);

  /** Retrieve Dictionary path name. */
	mafString GetDictionary(){return "";}; // Left for backward compatibility. Always return the empty string

  /** Retrieve Automatic Crop Position Flag. */
	int AutoCropPosition(){return m_AutoCropPos;};
  /** Retrieve if is enabled the flag for setting the number of timestamps */
	int EnableNumberOfTime(){return m_EnableNumberOfTime;};
  /** Retrieve if is enabled the the flag for setting the number of slices */
	int EnableNumberOfSlice(){return m_EnableNumberOfSlice;};
  /** Retrieve the build step. */
  int GetBuildStep(){return m_Step;};
  /** Retrieve if the flag of changing side is enabled */
  int EnableChangeSide(){return m_EnableChangeSide;};
  /** Enable discarding origin. */
  int EnableDiscardPosition(){return m_EnableDiscardPosition;};
  /** Enable resampling volume. */
  int EnableResampleVolume(){return m_EnableResampleVolume;};
  /** Enable rescaling to 16 bit. */
  int EnableRescaleTo16Bit(){return m_EnableRescaleTo16Bit;};
  /** Enable visualize position. */
  int EnableVisualizationPosition(){return m_VisualizePosition;};
  /** EnableZ-direction crop. */
  int EnableZCrop(){return m_EnableZCrop;};
  /** Enable scalar tolerance. */
  int EnableScalarTolerance(){return m_ScalarTolerance;};
  /** GReturn scalar tolerance. */
  double GetScalarTolerance(){return m_ScalarDistanceTolerance;};
  /** Enable percentage tolerance. */
  int EnablePercentageTolerance(){return m_PercentageTolerance;};
  /** Get percentage tolerance. */
  double GetPercentageTolerance(){return m_PercentageDistanceTolerance;};
  /** Get m_ShowAdvancedOptionOfSorting */
  int GetShowAdvancedOptionSorting(){return m_ShowAdvancedOptionOfSorting;};
  /** return if default dicom folder should be used */
  int GetUseDefaultDicomFolder(){return m_UseDefaultDicomFolder;};
  /** return if default dicom folder should be used */
  mafString GetDefaultDicomFolder(){return m_DicomFolder;};



  /** Retrive auto VME type flag. */
  int AutoVMEType(){return m_AutoVMEType;};
  /** Retrive VME type. */
  int GetVMEType(){return m_OutputType;};


	/** Return if a particular type of Dicom or Vme is Enabled to be read */
	bool EnableToRead(char* type);

  /** Enable/disable a particular type of Dicom or Vme to be read */
  void SetEnableToRead(char *type,bool enable);

  /** Helper function to store and retrieve the last dicom dir opened */
  void SetLastDicomDir(wxString lastDicomDir);;
  wxString GetLastDicomDir() {return m_LastDicomDir;};

  /** Return the type of output name format */
  int GetOutputNameFormat(){return m_OutputNameType;};

  /** Return if an element of custom name chack list is checked */
  int GetEnabledCustomName(int type);

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
  mafGUICheckListBox *m_DicomVmeTypeListBox;
  mafGUICheckListBox *m_NameCompositorList;

  int m_CheckNameCompositor[5];
	int m_CheckOnOff[7];
  int m_CheckOnOffVmeType[3];

  int m_OutputType;

	int m_AutoCropPos;
	int m_EnableNumberOfTime;
	int m_EnableNumberOfSlice;
	int m_Step;
  int m_EnableChangeSide;
  int m_EnableDiscardPosition;
  int m_EnableResampleVolume;
  int m_EnableRescaleTo16Bit;
  int m_EnableZCrop;
  int m_ScalarTolerance;
  int m_PercentageTolerance;
  int m_AutoVMEType;
  int m_VisualizePosition;
  int m_OutputNameType;
  int m_ShowAdvancedOptionOfSorting;
  int m_UseDefaultDicomFolder;
  wxString m_LastDicomDir;
  mafString m_DicomFolder;
  double m_ScalarDistanceTolerance;
  double m_PercentageDistanceTolerance;
  int m_DCM_ImagePositionPatientchoice;

  friend class medGUIDicomSettingsTest;
};
#endif
