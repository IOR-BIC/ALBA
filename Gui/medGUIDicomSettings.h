/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medGUIDicomSettings.h,v $
Language:  C++
Date:      $Date: 2010-07-23 08:51:32 $
Version:   $Revision: 1.5.2.15 $
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
class medGUIDicomSettings : public mafGUISettings
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
  /** Enable Pprcentage tolerance. */
  int EnablePercentageTolerance(){return m_PercentageTolerance;};
  /** Get percetnage tolerance. */
  double GetPercentageTolerance(){return m_PercentageDistanceTolerance;};



  /** Retrive auto VME type flag. */
  int AutoVMEType(){return m_AutoVMEType;};
  /** Retrive VME type. */
  int GetVMEType(){return m_OutputType;};


	/** Return if a particular type of Dicom is Enabled to be read */
	bool EnableToRead(char* type);

  /** Enable/disable a particular type of Dicom to be read */
  void SetEnableToRead(char *type,bool enable);

  /** Helper function to store and retrieve the last dicom dir opened */
  void SetLastDicomDir(wxString lastDicomDir);;
  wxString GetLastDicomDir() {return m_LastDicomDir;};

protected:
	/** Create the GUI for the setting panel.*/
	void CreateGui();

	/** Initialize the application settings.*/
	void InitializeSettings();

	/** Used to enable/disable items according to the current widgets state.*/
	void EnableItems();

	// mafString m_Dictionary;
	mafGUICheckListBox *m_DicomModalityListBox;

	int m_CheckOnOff[6];

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
  wxString m_LastDicomDir;
  double m_ScalarDistanceTolerance;
  double m_PercentageDistanceTolerance;

  friend class medGUIDicomSettingsTest;
};
#endif
