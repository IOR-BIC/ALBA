/*=========================================================================

Program: MAF2
Module: mafOpImporterDicomOffis
Authors: Matteo Giacomoni, Roberto Mucci , Stefano Perticoni, Gianluigi Crimi

Copyright (c) B3C
All rights reserved. See Copyright.txt or
http://www.scsitaly.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpImporterDicomOffis_H__
#define __mafOpImporterDicomOffis_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafOp.h"
#include "vtkImageData.h"
#include <map>
#include "mafDicomCardiacMRIHelper.h"
#include "mafGUIWizard.h"
#include "vtkMatrix4x4.h"


//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafDicomSlice;
class mafInteractorDICOMImporter;
class mafGUIWizardPageNew;
class mafString;
class mafTagArray;
class mafVMEImage;
class mafVMEVolumeGray;
class mafGUICheckListBox;
class vtkDirectory;
class vtkLookupTable;
class vtkPlaneSource;
class vtkPolyDataMapper;
class vtkTexture;
class vtkActor;
class vtkActor2D;
class vtkPolyData;
class vtkTextMapper;
class mafVMEGroup;
class mafProgressBarHelper;
class DcmDataset;

class mafDicomSlice;
class medDicomSeriesSliceList;

//----------------------------------------------------------------------------
// mafOpImporterDicomOffis :
//----------------------------------------------------------------------------
/** 
Perform DICOM importer.
From a DICOM dataset return a VME Volume or a VME Image.
*/
class MAF_EXPORT mafOpImporterDicomOffis : public mafOp
{
public:
	/** constructor */
	mafOpImporterDicomOffis(wxString label = "Importer DICOM");
	/** RTTI macro */
	mafTypeMacro(mafOpImporterDicomOffis, mafOp);

	/** Copy. */
	mafOp* Copy();

	/** Return true for the acceptable vme type. */
	bool Accept(mafVME*node) {return true;};

	/** Builds operation's interface calling CreateGui() method. */
	virtual void OpRun();

	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	virtual void OpStop	(int result);

	/** Execute the operation. */
	virtual  void OpDo();

	/** Makes the undo for the operation. */
	void OpUndo(){};

	/** Create the dialog interface for the importer. */
	virtual void CreateGui();

	/** Set the abs file name of the directory containing DICOM slices to import */
	void SetDicomDirectoryABSFileName(const char *dirName){m_DicomDirectoryABSFileName = dirName;};
	const char *GetDicomDirectoryABSFileName() const {return m_DicomDirectoryABSFileName.GetCStr();};

	/** Read Dicom file */
	void ReadDicom();

	/** Create the vtkTexture for slice_num dicom slice: this will be written to m_SliceTexture ivar*/
	void GenerateSliceTexture(int imageID);

	/** Get dicom slice vtkImageData from its local file name */
	vtkImageData* GetSliceImageDataFromLocalDicomFileName(mafString sliceName);
	
	/** Build a volume from the list of dicom files. */
	int BuildOutputVMEGrayVolumeFromDicom();

	/** Build a volume from the list of CineMRI files. */
	int BuildOutputVMEGrayVolumeFromDicomCineMRI();
		
	/** Build images starting from the list of dicom files. */
	int BuildOutputVMEImagesFromDicom();

	/** Build images starting from the list of CineMRI files. */
	int BuildOutputVMEImagesFromDicomCineMRI();

	/** Create the pipeline to read the images. */
	virtual void CreateSliceVTKPipeline();

	/** Open dir containing Dicom images. */
	bool OpenDir();

	/** method allows to handle events from other objects*/
	virtual void OnEvent(mafEventBase *maf_event);
		
protected:

	enum DICOM_IMPORTER_GUI_ID
	{
		ID_FIRST = mafGUIWizard::ID_LAST,
		ID_STUDY_SELECT,
		ID_SERIES_SELECT,
		ID_CROP,
		ID_CANCEL,
		ID_PATIENT_NAME,
		ID_PATIENT_ID,
		ID_SURGEON_NAME,
		ID_SCAN_TIME,
		ID_SCAN_SLICE,
		ID_VOLUME_NAME,
		ID_VME_TYPE,
		ID_SHOW_TEXT,
		ID_SHOW_SP,
	};

	/** OnEvent helper functions */
	void OnScanTime();
	void OnScanSlice();
	void OnRangeModified();
	void OnMouseDown( mafEvent * e );
	void OnMouseMove( mafEvent * e );
	void OnMouseUp();
	void OnWizardChangePage( mafEvent * e );
	void OnSeriesSelect();
	void OnStudySelect();
	
	/** Create load page and his GUI for the wizard. */
	void CreateLoadPage();

	/** Create crop page and   his GUI for the wizard. */
	void CreateCropPage();

	/** Reset the list of files and all the structures that own images information. */
	void ResetStructure();

	/** gets the range of the dicom by walking thought the slices */
	void GetDicomRange(double *range);

	/** Reset the slider that allow to scan the slices. */
	void CreateSliders();

	/** Build the list of dicom filer recognized. */
	bool BuildDicomFileList(const char *dicomDirABSPath);

	/** Read the list of dicom files recognized. */
	bool ReadDicomFileList(mafString& currentSliceABSDirName, mafProgressBarHelper *progressHelper);

	void FindAndGetDicomStrings(DcmDataset * dicomDataset, const char *&birthdate, const char *&date, const char *&description, const char *&patientName, const char *&photometricInterpretation);

	void GetDicomSpacing(DcmDataset * dicomDataset, double * dcmPixelSpacing);

	/** Check if dicom dataset contains rotations */
	bool IsRotated( double dcmImageOrientationPatient[6] );

	/** Return the slice number from the heightId and sliceId*/
	int GetSliceIDInSeries(int heightId, int timeId);

	/** Show the slice slice_num. */
	void ShowSlice();

	/** Fill Study listbox. */
	void FillStudyListBox(mafString studyUID);

	/** Update Study listbox. */
	void UpdateStudyListBox();

	/** Fill Series listbox. */
	void FillSeriesListBox();

	/** Import dicom tags into vme tags. */
	void ImportDicomTags();

	/** Perform gui update in several wizard pages */
	void GuiUpdate();

	/** Enable slice slider. */
	void EnableSliceSlider(bool enable);

	/** Enable time slider. */
	void EnableTimeSlider(bool enable);

	/** Update camera. */
	void CameraUpdate();

	/** reset camera. */
	void CameraReset();

	/** On page changing. */
	void OnWizardPageChanging(){};

	/** On wizard start. */
	virtual int RunWizard();
		
	/** Performe crop of dicom data. */
	void Crop();

	void SetVMEName(mafDicomSlice * currentSliceData);
		
	/** Delete all istances of used objects. */
	void Destroy();
		
	vtkDirectory			*m_DICOMDirectoryReader; 
	vtkLookupTable	*m_SliceLookupTable;
	vtkPlaneSource		*m_SlicePlane;
	vtkPolyDataMapper	*m_SliceMapper;
	vtkTexture				*m_SliceTexture;
	vtkActor					*m_SliceActor;
	vtkPlaneSource		*m_CropPlane;
	vtkActor					*m_CropActor;
	vtkActor					*m_SliceActorInCropPage;

	// text stuff
	vtkActor2D    *m_TextActor;
	vtkTextMapper	*m_TextMapper;
	wxString m_Text;

	mafInteractorDICOMImporter *m_DicomInteractor;

	mafGUIWizard			*m_Wizard;
	mafGUIWizardPageNew	*m_LoadPage;
	mafGUIWizardPageNew	*m_CropPage;
	mafVMEGroup       *m_ImagesGroup;

	mafGUI	*m_LoadGuiLeft;
	mafGUI	*m_LoadGuiUnderLeft;
	mafGUI	*m_CropGuiLeft;
	mafGUI	*m_CropGuiCenter;
	mafGUI	*m_LoadGuiCenter;
	mafGUI  *m_LoadGuiUnderCenter;

	int       m_OutputType;
	mafString	m_DicomDirectoryABSFileName;
	mafString m_PatientName;
	mafString m_SurgeonName;
	mafString m_PatientPosition;
	mafString m_VMEName;

	int				m_DicomReaderModality; ///<Type DICOM Read from file
	int				m_SortAxes;
	int				m_NumberOfTimeFrames;

	int m_SkipAllNoPosition;

	std::map <mafString,int> m_dcm_dim;

	medDicomSeriesSliceList	*m_SelectedSeriesSlicesList; ///< Selected study slices list

	std::vector<mafString> m_SelectedSeriesID; ///< Selected StudyUID-SeriesUIDWithPlanesNumber-SeriesUIDWithoutPlanesNumber vector

	std::map<std::vector<mafString>,medDicomSeriesSliceList*> m_SeriesIDToSlicesListMap; ///< StudyUID-SeriesUIDWithPlanesNumber-SeriesUIDWithoutPlanesNumber vector to slices list map 

	std::map<std::vector<mafString>,int> m_SeriesIDstringToSeriesIDint;

	std::map<std::vector<mafString>,std::map<mafString,int>> m_SeriesIDstringToSeriesDimensionMap;

	std::map<std::vector<mafString>,mafDicomCardiacMRIHelper*> m_SeriesIDToCardiacMRIHelperMap; ///< StudyUID-SeriesUIDWithPlanesNumber-SeriesUIDWithoutPlanesNumber vector to slices list map 

	std::map<std::vector<mafString>,bool> m_SeriesIDContainsRotationsMap; ///< StudyUID-SeriesUIDWithPlanesNumber-SeriesUIDWithoutPlanesNumber vector to boolean map 

	wxString  m_CurrentSliceABSFileName;
	
	int				 m_NumberOfStudies; ///<Number of study present in the DICOM directory
	int				 m_NumberOfSlices;
	wxListBox	*m_StudyListbox;
	//	wxListCtrl  *m_StudyListctrl;
	//	wxListBox	*m_SeriesListbox;
	wxListCtrl  *m_SeriesListctrl;

	int           m_ZCropBounds[2];
	int						m_CurrentSlice;
	wxSlider		 *m_SliceScannerLoadPage;
	wxSlider		 *m_SliceScannerCropPage;
	
	int						m_CurrentTime;
	wxSlider		 *m_TimeScannerLoadPage;
	wxSlider		 *m_TimeScannerCropPage;
	
	mafTagArray	*m_TagArray;

	double	m_SliceBounds[6];

	int	m_GizmoStatus;
	int	m_SideToBeDragged;

	bool m_BoxCorrect;
	bool m_CropFlag;
	bool m_ApplyRotation; //<<< true il current series contains rotated slices
	bool m_ConstantRotation;
	
	int m_ShowOrientationPosition;

	mafVMEImage				*m_Image;
	mafVMEVolumeGray	*m_Volume;

	mafGUICheckListBox *m_DicomModalityListBox;
	int m_CurrentImageID;

	double m_TotalDicomRange[2]; ///< contains the scalar range og the full dicom
	double m_TotalDicomSubRange[2]; ///< contains the scalar range og the full dicom

	/** destructor */
	~mafOpImporterDicomOffis();

};

/**
class name: mafDicomSlice
Holds information on a single dicom slice

We are using Image Position (Patient) (0020,0032) dicom tag to set dicom slice position. 
See here for the motivation behind this decision:
http://www.cmake.org/pipermail/insight-users/2005-September/014711.html
*/
class MAF_EXPORT mafDicomSlice
{
public:
	
	/** constructor */
	mafDicomSlice() 
	{
		m_PatientBirthdate = "";
		m_PatientName = "";
		m_Description = "";
		m_Date = "";
		m_SliceABSFileName = "";
		m_DcmImagePositionPatient[0] = -9999;
		m_DcmImagePositionPatient[1] = -9999;
		m_DcmImagePositionPatient[2] = -9999;
		m_DcmImagePositionPatientOriginal[0] = m_DcmImagePositionPatient[0];
		m_DcmImagePositionPatientOriginal[1] = m_DcmImagePositionPatient[1];
		m_DcmImagePositionPatientOriginal[2] = m_DcmImagePositionPatient[2];
		m_DcmImageOrientationPatient[0] = 0.0;
		m_DcmImageOrientationPatient[1] = 0.0; 
		m_DcmImageOrientationPatient[2] = 0.0; 
		m_DcmImageOrientationPatient[3] = 0.0; 
		m_DcmImageOrientationPatient[4] = 0.0; 
		m_DcmImageOrientationPatient[5] = 0.0; 
		m_DcmInstanceNumber = -1;
		m_DcmTriggerTime = -1.0;
		m_DcmCardiacNumberOfImages = -1;
		m_Data = NULL;
	};

	/** overloaded constructor */
	mafDicomSlice(mafString sliceABSFilename,double dcmImagePositionPatient[3], double dcmImageOrientationPatient[6],\
		vtkImageData *data , mafString description , mafString date , mafString patientName , mafString patientBirthdate ,int dcmInstanceNumber=-1, int dcmCardiacNumberOfImages=-1,\
		double dcmTtriggerTime=-1.0)  
	{
		m_PatientBirthdate = patientBirthdate;
		m_PatientName = patientName;
		m_Description = description;
		m_Date = date;
		m_SliceABSFileName = sliceABSFilename;
		m_DcmImagePositionPatient[0] = dcmImagePositionPatient[0];
		m_DcmImagePositionPatient[1] = dcmImagePositionPatient[1];
		m_DcmImagePositionPatient[2] = dcmImagePositionPatient[2];
		m_DcmImagePositionPatientOriginal[0] = m_DcmImagePositionPatient[0];
		m_DcmImagePositionPatientOriginal[1] = m_DcmImagePositionPatient[1];
		m_DcmImagePositionPatientOriginal[2] = m_DcmImagePositionPatient[2];
		m_DcmImageOrientationPatient[0] = dcmImageOrientationPatient[0];
		m_DcmImageOrientationPatient[1] = dcmImageOrientationPatient[1];
		m_DcmImageOrientationPatient[2] = dcmImageOrientationPatient[2];
		m_DcmImageOrientationPatient[3] = dcmImageOrientationPatient[3];
		m_DcmImageOrientationPatient[4] = dcmImageOrientationPatient[4];
		m_DcmImageOrientationPatient[5] = dcmImageOrientationPatient[5];
		m_DcmInstanceNumber = dcmInstanceNumber;
		m_DcmCardiacNumberOfImages = dcmCardiacNumberOfImages;
		m_DcmTriggerTime = dcmTtriggerTime;
		if (data != NULL)
		{
			vtkNEW(m_Data);
			m_Data->DeepCopy(data);
		}
		else
		{
			m_Data = NULL;
		}
	};

	/** destructor */
	~mafDicomSlice() {vtkDEL(m_Data);};

	/** Return patient birthday */
	mafString GetPatientBirthday(){return m_PatientBirthdate;};

	/** Return patient name */
	mafString GetPatientName(){return m_PatientName;};

	/** Return the filename of the corresponding dicom slice. */
	const char *GetSliceABSFileName() const {return m_SliceABSFileName.GetCStr();};

	/** Set the filename of the corresponding dicom slice. */
	void SetSliceABSFileName(char *fileName){m_SliceABSFileName = fileName;};

	/** Return the image number of the dicom slice*/
	int GetDcmInstanceNumber() const {return m_DcmInstanceNumber;};

	/** Set the image number of the dicom slice*/
	void SetDcmInstanceNumber(int number){m_DcmInstanceNumber = number;};

	/** Return the number of cardiac timeframes*/
	int GetDcmCardiacNumberOfImages() const {return m_DcmCardiacNumberOfImages;};

	/** Set the number of cardiac timeframes*/
	void SetDcmCardiacNumberOfImages(int number){m_DcmCardiacNumberOfImages = number;};

	/** Return the trigger time of the dicom slice*/
	double GetDcmTriggerTime() const {return m_DcmTriggerTime;};

	/** Set the trigger time of the dicom slice*/
	void SetDcmTriggerTime(double time){m_DcmTriggerTime = time;};

	/** Retrieve image data*/
	vtkImageData* GetVTKImageData(){return m_Data;};

	/** Set vtkImageData */
	void SetVTKImageData(vtkImageData *data);

	/** Set the DcmImagePositionPatient tag for the slice */
	void SetDcmImagePositionPatient(double dcmImagePositionPatient[3])
	{
		m_DcmImagePositionPatient[0]=dcmImagePositionPatient[0];
		m_DcmImagePositionPatient[1]=dcmImagePositionPatient[1];
		m_DcmImagePositionPatient[2]=dcmImagePositionPatient[2];
	};

	/** Get the DcmImagePositionPatient tag for the slice */
	void GetDcmImagePositionPatient(double dcmImagePositionPatient[3])
	{
		dcmImagePositionPatient[0]=m_DcmImagePositionPatient[0];
		dcmImagePositionPatient[1]=m_DcmImagePositionPatient[1];
		dcmImagePositionPatient[2]=m_DcmImagePositionPatient[2];
	};

	/** Get the DcmImagePositionPatient tag original for the slice */
	void GetDcmImagePositionPatientOriginal(double dcmImagePositionPatient[3])
	{
		dcmImagePositionPatient[0]=m_DcmImagePositionPatientOriginal[0];
		dcmImagePositionPatient[1]=m_DcmImagePositionPatientOriginal[1];
		dcmImagePositionPatient[2]=m_DcmImagePositionPatientOriginal[2];
	};


	/** Set the DcmImageOrientationPatient tag for the slice*/
	void SetDcmImageOrientationPatient(double dcmImageOrientationPatient[6])
	{
		m_DcmImageOrientationPatient[0]=dcmImageOrientationPatient[0];
		m_DcmImageOrientationPatient[1]=dcmImageOrientationPatient[1]; 
		m_DcmImageOrientationPatient[2]=dcmImageOrientationPatient[2]; 
		m_DcmImageOrientationPatient[3]=dcmImageOrientationPatient[3]; 
		m_DcmImageOrientationPatient[4]=dcmImageOrientationPatient[4]; 
		m_DcmImageOrientationPatient[5]=dcmImageOrientationPatient[5]; 
	};

	/** Get the DcmImageOrientationPatient tag for the slice*/
	void GetDcmImageOrientationPatient(double dcmImageOrientationPatient[6])
	{
		dcmImageOrientationPatient[0]= m_DcmImageOrientationPatient[0];
		dcmImageOrientationPatient[1]= m_DcmImageOrientationPatient[1];
		dcmImageOrientationPatient[2]= m_DcmImageOrientationPatient[2];
		dcmImageOrientationPatient[3]= m_DcmImageOrientationPatient[3];
		dcmImageOrientationPatient[4]= m_DcmImageOrientationPatient[4];
		dcmImageOrientationPatient[5]= m_DcmImageOrientationPatient[5];
	};

	/** 
	Write dicom slice orientation on matrix*/
	void GetOrientation( vtkMatrix4x4 * matrix );

	/** return the description */
	mafString GetDescription(){return m_Description;};

	/** return the date */
	mafString GetDate(){return m_Date;};

	/** Get the DCM modality */
	mafString GetDcmModality(){return m_DcmModality;};

	/** Set the DCM modality */
	void SetDcmModality(mafString dcmModality){m_DcmModality=dcmModality;};


	/** Returns PhotometricInterpretation */
	mafString GetPhotometricInterpretation() const { return m_PhotometricInterpretation; }

	/** Sets PhotometricInterpretation */
	void SetPhotometricInterpretation(mafString photometricInterpretation) { m_PhotometricInterpretation = photometricInterpretation; }

protected:
	double m_DcmImagePositionPatient[3];
	double m_DcmImagePositionPatientOriginal[3];
	double m_DcmImageOrientationPatient[6];
	mafString m_SliceABSFileName;
	mafString m_Description;
	mafString m_Date;
	mafString m_PatientName;
	mafString m_PatientBirthdate;
	mafString m_DcmModality;
	mafString m_PhotometricInterpretation;

	double m_DcmTriggerTime;
	int m_DcmInstanceNumber;
	int m_DcmCardiacNumberOfImages;

	vtkImageData *m_Data;
};
#endif
