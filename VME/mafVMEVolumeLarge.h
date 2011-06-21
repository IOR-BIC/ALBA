/**=======================================================================

File:    	 mafVMEVolumeLarge.h
Language:  C++
Date:      22:1:2008   10:57
Version:   $Revision: 1.1.2.3 $
Authors:   Josef Kohout (Josef.Kohout@beds.ac.uk)

Copyright (c) 2008
University of Bedfordshire
=========================================================================
This is VME for large volumetric data sets
=========================================================================*/
#ifndef __mafVMEVolumeLarge_h
#define __mafVMEVolumeLarge_h

#pragma warning(disable: 4068)
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
//#define VME_VOLUME_VER1
//#define VME_VOLUME_LARGE_EXCLUDE_CROP

#include "mafVMEVolume.h"
#ifdef VME_VOLUME_VER1
#include "../BES_Beta/vtkMAF/vtkMAFLargeDataSet.h"
#include "../BES_Beta/vtkMAF/vtkMAFLargeDataSetCallback.h"
#else
#include "../BES_Beta/IO/mafVolumeLargeReader.h"
#endif
//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkDataSet;
class mmaVolumeMaterial;
class mafGizmoROI_BES;
class mafGizmoTranslate;

/** mafVMEVolumeLarge - a VME featuring an internal array for matrices and VTK datasets.
mafVMEVolumeLarge is a specialized VME inheriting the VME-Generic features to internally
store data, and specialized for VTK data. This specialization consists in creating
a specialized data pipe and to redefining some APIs for casting to concrete classes.
@sa mafVME mafMatrixVector mafDataVector  */
class MED_EXPORT mafVMEVolumeLarge : public mafVMEVolume
	//mafVMEGenericLarge
{
protected:
	int m_FullExtent[6];

#ifdef VME_VOLUME_VER1
	//large data set
	vtkMAFLargeDataSet* m_LargeData;

	//m_LargeData associated progress callback
	vtkMAFLargeDataSetCallback* m_ProgressCallback;
#else

	//reader of large data
	mafVolumeLargeReader* m_LargeDataReader;
#endif


	//last time of update
	vtkTimeStamp m_UpdateTime;

#pragma region GUIS
#pragma region INFO GUI
	mafGUI* m_InfoGui;
	mafString m_SourceFile;
	mafString m_SourcePath;
	mafString m_SourceDimensions;
	mafString m_SourceSpacing;
	mafString m_SourceBounds[3];
	mafString m_SourceSize;
#pragma endregion //INFO GUI

#pragma region SAMPLE INFO GUI
	mafGUI* m_SampleInfoGui;
	mafString m_SampleDimensions;
	mafString m_SampleBounds[3];
	//mafString m_SampleMemLimit;
	int m_SampleMemLimit;
	mafString m_SampleRate;	
	mafString m_SampleSize;
#pragma endregion //SAMPLE INFO GUI

#pragma region CROP GUI
	mafGUI* m_CropGui;
	mafGizmoROI_BES *m_GizmoROI; ///< Gizmo used to define sub-volume region to crop	

	int m_ShowROI;		///< Flag used to show/hide crop gizmo
	int m_ShowHandles;	///< Flag used to show/hide crop gizmo handles
	int m_ShowAxis;		///< Flag used to show/hide crop gizmo translation axis
	int m_ShowPlanes;	///< Flag used to show/hide crop gizmo translation planes
	
	mafGUI* m_CropEdVxls;
	mafGUI* m_CropEdMm;
	mafGUI* m_ShowROIOpt;
	double m_ROI[6];		// Gizmo (given in mm)
	int m_VOI[6];			// Gizmo in units

	int m_VOIUnits;			//for combo box, determines if m_ROI or m_VOI is used
  int m_AutoProof;    //<determines whether the quality should improve automatically
                      //as the user changes the ROI
  int m_AutoProofZone;  //<number of voxels in the frame of autoproof ROI
#pragma endregion //CROP GUI
#pragma endregion //GUIS
	
#ifdef VME_VOLUME_VER1
	//original VOI data
	vtkDataSet* m_FEOutput;
	int m_FESampleRate[3];
		
	//true, if the original FE is displayed
	bool m_ShowFEOutput;
#endif

public:  
	mafTypeMacro(mafVMEVolumeLarge, mafVMEVolume);
	enum VME_GUI_ID
	{
		ID_VOI_EDITS = Superclass::ID_LAST,
		ID_ROLLOUT_INFO,
			//Info GUI
		ID_ROLLOUT_SAMPLE_INFO,			
			//Sample Info GUI
			ID_SAMPLE_MEMLIMIT,
		ID_ROLLOUT_CROP,
			//Crop GUI
			ID_SHOW_ROI,
			ID_ROLLOUT_SHOW_ROI_OPT,
				ID_SHOW_HANDLES,			
				ID_SHOW_AXIS,
				ID_SHOW_PLANES,
			ID_COMBO_VOI_UNITS,
			ID_CROP_VOXELS,
				ID_CROP_DIR_X,
				ID_CROP_DIR_Y,
				ID_CROP_DIR_Z,
			ID_CROP_MM,
				ID_CROP_DIR_XMM,
				ID_CROP_DIR_YMM,
				ID_CROP_DIR_ZMM,
      ID_AUTOPROOF, 
			ID_RESET_CROPPING_AREA,			
			ID_VIEW_ROI_VOLUME,
			ID_VIEW_ORIGVOI_VOLUME,
			ID_CROP,
#ifdef _TEMP_STUFF
		ID_TEMP_FUNC,
#endif		
		ID_LAST
	};
	
	/** return icon */
	static char** GetIcon();

  /** Return pointer to material attribute. */
  mmaVolumeMaterial *GetMaterial();

	/** this method stores information about the large data set
	//it calls SetData to set it to the Snapshot of the give data
	//Note: SetData is still provided but its use may lead to
	//unpredictable behavior */
#ifdef VME_VOLUME_VER1
	virtual int SetLargeData(vtkMAFLargeDataSet *data, mafTimeStamp t, int mode=MAF_VME_REFERENCE_DATA);
#else
	virtual int SetLargeData(mafVolumeLargeReader *data);
#endif

#pragma region Getters / Setters
	//Set the pathname of the large data file (that is used)
	virtual void SetFileName(const char *filename);

	//Get the pathname of the large data file (that is used)
	virtual const char* GetFileName();
#pragma endregion

	virtual void UnRegister(void *obj);

protected:
	mafVMEVolumeLarge();
	virtual ~mafVMEVolumeLarge();


	/** Internally used to create a new instance of the GUI.*/
	/*virtual*/ mafGUI* CreateGui();

  /** called to prepare the update of output */
  /*virtual*/ void InternalPreUpdate();

	/** Internally called to update the output */
	/*virtual*/ void InternalUpdate();

	//two methods for restoring/storing the content of large data set from MSF  
	/*virtual*/ int InternalStore(mafStorageElement *parent);
	/*virtual*/ int InternalRestore(mafStorageElement *node);

  /** used to initialize and create the material attribute if not yet present */
  /*virtual*/ int InternalInitialize();

	/** Process events coming from other objects */ 
	/*virtual*/ void OnEvent(mafEventBase *e);

#ifdef VME_VOLUME_VER1
	/** Overridden to take into consideration the internal m_LargeData time. */
	/*virtual*/ unsigned long GetMTime();
#endif
protected:


#pragma region GUI stuff

	//creates the GUI with information about the large volume data
	virtual mafGUI* CreateInfoGui();

	//creates the GUI with information about the sampled output
	virtual mafGUI* CreateSampleInfoGui();

	//creates the GUI with the cropping options
	virtual mafGUI* CreateCropGui();	

	//updates all existing GUIs
	virtual void UpdateGui();	

	//updates the existing Info gui
	virtual void UpdateInfoGui(mafGUI* gui);

	//updates the existing sample Info gui
	virtual void UpdateSampleInfoGui(mafGUI* gui);

	//updates the existing Crop gui
	virtual void UpdateCropGui(mafGUI* gui);	

	//adds a new combobox (identified by id) with "voxels", "mm" options
	//onto the given gui; pvar is the combo data variable 
	void AddVoxelsMmCombo(mafGUI* gui, int id, int* pvar);

	//updates the gizmo
	void UpdateGizmo();	

#ifndef VME_VOLUME_VER1
  /** Gets VOI from the underlaying large reader and updates GUI */
  inline void UpdateVOI() {
    UpdateVOI(m_LargeDataReader->GetVOI());
  }

  /** Updates VOI and associated GUIs to the given value */
  void UpdateVOI(int VOI[6]);
#endif
#pragma endregion

#pragma region Events Handlers
	//processes the ROI toggle command, creates or destroys gizmo
	virtual void OnToggleGizmo();

	//resets the VOI into the full VOI
	virtual void OnResetROI();

	//zooms to the selected ROI only
	virtual void OnViewROIVolume();

	/** returns back to the original ROI only */
	virtual void OnViewOrigVOIVolume();

  /** handles the change of ROI in auto proof mode */
  virtual void OnAutoProofROIVolume();

#ifndef VME_VOLUME_LARGE_EXCLUDE_CROP
	//performs the cropping
	virtual void OnCrop();	
#endif // VME_VOLUME_LARGE_EXCLUDE_CROP
#pragma endregion

#ifdef _TEMP_STUFF
#pragma region Temporary Stuff
	//temporary function
	void OnTempFunc();

#pragma endregion
#endif
	
protected:
	//transforms the extent given in units into extent in mm
	void TransformExtent(int extUn[6], double outMm[6]);

	//transforms the extent given in mm into extent in units
	void InverseTransformExtent(double extMm[6], int outUn[6]);

#ifdef VME_VOLUME_VER1
	//this routine creates snapshot object for m_LargeDataSet
	//from the current DataVector item
	void CreateSnapshotObject();

	//if m_FullExtent sampled version does not exist and
	//the current snapshot contains it, it will be stored
	void CheckFEOutput();
#endif
/*
	//Updates the output of data pipeline
	//Supposed to be called after VME generates a new data
	void UpdateOutput();
  */
private:
	mafVMEVolumeLarge(const mafVMEVolumeLarge&); // Not implemented
	void operator=(const mafVMEVolumeLarge&); // Not implemented
};

#endif
