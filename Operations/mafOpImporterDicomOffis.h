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
#include <vector>
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
class mafProgressBarHelper;
class DcmDataset;

class mafDicomSlice;
class mafDicomSeries;
class mafGUIDicomSettings;
class mafDicomStudyList;
class mafDicomStudy;


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
			
	/** Create the vtkTexture for slice_num dicom slice: this will be written to m_SliceTexture ivar*/
	void GenerateSliceTexture(int imageID);

	void Crop(vtkImageData *slice);

	void CalculateCropExtent();
			
	/** Build a volume from the list of dicom files. */
	int BuildVMEVolumeGrayOutput();
		
	/** Build images starting from the list of dicom files. */
	int BuildVMEImagesOutput();

	/** Create the pipeline to read the images. */
	virtual void CreateSliceVTKPipeline();

	/** Open dir containing Dicom images. */
	bool OpenDir(const char *dirPath);

	/** method allows to handle events from other objects*/
	virtual void OnEvent(mafEventBase *maf_event);
		
protected:

	enum DICOM_IMPORTER_GUI_ID
	{
		ID_FIRST = mafGUIWizard::ID_LAST,
		ID_STUDY_SELECT,
		ID_SERIES_SELECT,
		ID_CANCEL,
		ID_SCAN_TIME,
		ID_SCAN_SLICE,
		ID_VME_TYPE,
		ID_SHOW_TEXT,
		ID_UPDATE_NAME
	};

	/** OnEvent helper functions */
	void OnChangeSlice();
	void OnRangeModified();
	void OnWizardChangePage( mafEvent * e );
	void OnSeriesSelect();
	void OnStudySelect();
	
	/** Create load page and his GUI for the wizard. */
	void CreateLoadPage();

	/** Create crop page and   his GUI for the wizard. */
	void CreateCropPage();
		
	/** gets the range of the dicom by walking thought the slices */
	void GetDicomRange(double *range);

	/** Reset the slider that allow to scan the slices. */
	void CreateSliders();

	/** Load Dicom in forlder */
	bool LoadDicomFromDir(const char *dicomDirABSPath);

	/** Read the list of dicom files recognized. */
	mafDicomSlice *ReadDicomSlice(mafString fileName);

	vtkImageData *CreateImageData(DcmDataset * dicomDataset, double * dcmImagePositionPatient);

	void GetDicomSpacing(DcmDataset * dicomDataset, double * dcmPixelSpacing);
		
	/** Return the slice number from the heightId and sliceId*/
	int GetSliceIDInSeries(int heightId, int timeId);

	/** Show the slice slice_num. */
	void SetPlaneDims();

	/** Fill Study listbox. */
	void FillStudyListBox();
		
	/** Fill Series listbox. */
	void FillSeriesListBox();

	/** Import dicom tags into vme tags. */
	void ImportDicomTags();

	/** Perform gui update in several wizard pages */
	void GuiUpdate();

	/** Update camera. */
	void CameraUpdate();

	/** reset camera. */
	void CameraReset();

	/** On page changing. */
	void OnWizardPageChanging(){};

	/** On wizard start. */
	virtual int RunWizard();
		
	
	void SetVMEName();
		
	vtkLookupTable		*m_SliceLookupTable;
	vtkPlaneSource		*m_SlicePlane;
	vtkPolyDataMapper	*m_SliceMapper;
	vtkTexture				*m_SliceTexture;
	vtkActor					*m_SliceActor;
	
	// text stuff
	vtkActor2D    *m_TextActor;
	vtkTextMapper	*m_TextMapper;

	mafInteractorDICOMImporter *m_DicomInteractor;

	mafGUIWizard			*m_Wizard;
	mafGUIWizardPageNew	*m_LoadPage;
	mafGUIWizardPageNew	*m_CropPage;
	
	mafGUI	*m_LoadGuiLeft;
	mafGUI	*m_LoadGuiUnderLeft;
	mafGUI	*m_CropGuiLeft;
	mafGUI	*m_CropGuiCenter;
	mafGUI	*m_LoadGuiCenter;
	mafGUI  *m_LoadGuiUnderCenter;

	int       m_OutputType;
	int			m_DescrInName;
	int			m_PatientNameInName;
	int			m_SizeInName;
	mafString m_VMEName;

	mafDicomStudyList *m_StudyList;
	mafDicomSeries	*m_SelectedSeries; ///< Selected series slices list
	
	mafGUIDicomSettings* GetSetting();

	wxListBox	*m_StudyListbox;
	wxListBox *m_SeriesListbox;
	int m_SelectedStudy;
	
	int           m_ZCropBounds[2];
	int						m_CurrentSlice;
	wxSlider		 *m_SliceScannerLoadPage;
	wxSlider		 *m_SliceScannerCropPage;
	
	int						m_CurrentTime;
	
	mafTagArray	*m_TagArray;

	double	m_SliceBounds[6];
	bool m_CropEnabled;
	int m_CropExtent[6];

	bool m_ConstantRotation;
	
	int m_ShowOrientationPosition;
	int m_CurrentImageID;

	

	double m_TotalDicomRange[2]; ///< contains the scalar range og the full dicom
	double m_TotalDicomSubRange[2]; ///< contains the scalar range og the full dicom

	/** destructor */
	~mafOpImporterDicomOffis();
};


//----------------------------------------------------------------------------
// mafDicomStudyList:
//----------------------------------------------------------------------------

class mafDicomStudyList
{
public:
	~mafDicomStudyList();

	void AddSlice(mafDicomSlice *slice);

	mafDicomStudy *GetStudy(int num);

	int GetStudiesNum() { return m_Studies.size(); }

	int GetSeriesTotalNum();
protected:
	std::vector<mafDicomStudy *> m_Studies;
};

//----------------------------------------------------------------------------
// mafDicomStudy:
//----------------------------------------------------------------------------

class mafDicomStudy
{
public:

	mafDicomStudy(mafString studyID) { m_StudyID = studyID; } 

	~mafDicomStudy();

	void AddSlice(mafDicomSlice *slice);
	
	/** Returns StudyID */
	mafString GetStudyID() const { return m_StudyID; }

	int GetSeriesNum() { return m_Series.size(); }

	mafDicomSeries *GetSeries(int id) { return m_Series[id]; }

protected:
	mafString m_StudyID;
	std::vector<mafDicomSeries *> m_Series;
};

//----------------------------------------------------------------------------
// mafDicomSeries:
//----------------------------------------------------------------------------

class mafDicomSeries
{
public:

	mafDicomSeries(mafString seriesID) { m_SeriesID = seriesID;  m_IsRotated = false; m_CardiacImagesNum = 0; }

	~mafDicomSeries();

	void AddSlice(mafDicomSlice *slice);

	mafDicomSlice *GetSlice(int id) { return m_Slices[id]; }

	int GetSlicesNum() { return m_Slices.size(); }

	/** Check if dicom dataset contains rotations */
	bool IsRotated(const double dcmImageOrientationPatient[6]);

	/** Returns IsRotated */
	bool IsRotated() const { return m_IsRotated; }
	
	/** Sets IsRotated */
	void SetRotated(bool isRotated) { m_IsRotated = isRotated; }
	
	/** Returns Slices */
	std::vector<mafDicomSlice *> GetSlices() const { return m_Slices; }

	/** Sets Slices */
	void SetSlices(std::vector<mafDicomSlice *> slices) { m_Slices = slices; }
	
	/** Returns SerieID */
	mafString GetSerieID() const { return m_SeriesID; }

	const int *GetDimensions() { return m_Dimensions; }

	/** Returns FramesNum */
	int GetCardiacImagesNum() const { return m_CardiacImagesNum; }

	/** Sort slices internally */
	void SortSlices();
		
protected:

	std::vector<mafDicomSlice *> m_Slices;
	int m_Dimensions[3];
	mafString m_SeriesID;
	bool m_IsRotated;
	int m_CardiacImagesNum;
};


//----------------------------------------------------------------------------
// mafDicomSlice:
//----------------------------------------------------------------------------
/**
class name: mafDicomSlice
Holds information on a single dicom slice
*/
class MAF_EXPORT mafDicomSlice
{
public:

	/** Constructor */
	mafDicomSlice(mafString sliceABSFilename, double dcmImagePositionPatient[3], double dcmImageOrientationPatient[6], \
		vtkImageData *data, mafString description, mafString date, mafString patientName, mafString patientBirthdate, int dcmInstanceNumber = -1, int dcmCardiacNumberOfImages = -1, \
		double dcmTtriggerTime = -1.0)
	{
		m_PatientBirthdate = patientBirthdate;
		m_PatientName = patientName;
		m_Description = description;
		m_Date = date;
		m_SliceABSFileName = sliceABSFilename;
		m_DcmInstanceNumber = dcmInstanceNumber;
		m_DcmCardiacNumberOfImages = dcmCardiacNumberOfImages;
		m_DcmTriggerTime = dcmTtriggerTime;
		m_ImageData = data;
		SetDcmImagePositionPatient(dcmImagePositionPatient);
		SetDcmImagePositionPatientOriginal(dcmImagePositionPatient);
		SetDcmImageOrientationPatient(dcmImageOrientationPatient);
		CalculateUnrotatedOrigin();
	};

	/** destructor */
	~mafDicomSlice() { vtkDEL(m_ImageData); };

	/** Return patient birthday */
	mafString GetPatientBirthday() { return m_PatientBirthdate; };

	/** Return patient name */
	mafString GetPatientName() { return m_PatientName; };

	/** Return the filename of the corresponding Dicom slice. */
	const char *GetSliceABSFileName() const { return m_SliceABSFileName.GetCStr(); };

	/** Set the filename of the corresponding Dicom slice. */
	void SetSliceABSFileName(char *fileName) { m_SliceABSFileName = fileName; };

	/** Return the image number of the Dicom slice*/
	int GetDcmInstanceNumber() const { return m_DcmInstanceNumber; };

	/** Set the image number of the Dicom slice*/
	void SetDcmInstanceNumber(int number) { m_DcmInstanceNumber = number; };

	/** Return the number of cardiac time frames*/
	int GetDcmCardiacNumberOfImages() const { return m_DcmCardiacNumberOfImages; };

	/** Set the number of cardiac time frames*/
	void SetDcmCardiacNumberOfImages(int number) { m_DcmCardiacNumberOfImages = number; };

	/** Return the trigger time of the Dicom slice*/
	double GetDcmTriggerTime() const { return m_DcmTriggerTime; };

	/** Set the trigger time of the Dicom slice*/
	void SetDcmTriggerTime(double time) { m_DcmTriggerTime = time; };

	/** Retrieve image data*/
	vtkImageData* GetVTKImageData() { return m_ImageData; };

	/** Set the DcmImagePositionPatient tag for the slice */
	void SetDcmImagePositionPatient(double dcmImagePositionPatient[3])
	{
		m_DcmImagePositionPatient[0] = dcmImagePositionPatient[0];
		m_DcmImagePositionPatient[1] = dcmImagePositionPatient[1];
		m_DcmImagePositionPatient[2] = dcmImagePositionPatient[2];
	};

	/** Get the DcmImagePositionPatient tag for the slice */
	void GetDcmImagePositionPatient(double dcmImagePositionPatient[3])
	{
		dcmImagePositionPatient[0] = m_DcmImagePositionPatient[0];
		dcmImagePositionPatient[1] = m_DcmImagePositionPatient[1];
		dcmImagePositionPatient[2] = m_DcmImagePositionPatient[2];
	};

	/** Get the DcmImagePositionPatient tag for the slice */
	const double *GetDcmImagePositionPatient() {return m_DcmImagePositionPatient;};

	/** Set the DcmImagePositionPatient tag for the slice */
	void SetDcmImagePositionPatientOriginal(double dcmImagePositionPatient[3])
	{
		m_DcmImagePositionPatientOriginal[0] = dcmImagePositionPatient[0];
		m_DcmImagePositionPatientOriginal[1] = dcmImagePositionPatient[1];
		m_DcmImagePositionPatientOriginal[2] = dcmImagePositionPatient[2];
	};

	/** Get the DcmImagePositionPatient tag original for the slice */
	void GetDcmImagePositionPatientOriginal(double dcmImagePositionPatient[3])
	{
		dcmImagePositionPatient[0] = m_DcmImagePositionPatientOriginal[0];
		dcmImagePositionPatient[1] = m_DcmImagePositionPatientOriginal[1];
		dcmImagePositionPatient[2] = m_DcmImagePositionPatientOriginal[2];
	};

	/** Get the DcmImagePositionPatient tag for the slice */
	const double *GetDcmImagePositionPatientOriginal() { return m_DcmImagePositionPatientOriginal; };

	/** Set the DcmImageOrientationPatient tag for the slice*/
	void SetDcmImageOrientationPatient(double dcmImageOrientationPatient[6])
	{
		m_DcmImageOrientationPatient[0] = dcmImageOrientationPatient[0];
		m_DcmImageOrientationPatient[1] = dcmImageOrientationPatient[1];
		m_DcmImageOrientationPatient[2] = dcmImageOrientationPatient[2];
		m_DcmImageOrientationPatient[3] = dcmImageOrientationPatient[3];
		m_DcmImageOrientationPatient[4] = dcmImageOrientationPatient[4];
		m_DcmImageOrientationPatient[5] = dcmImageOrientationPatient[5];
	};

	/** Get the DcmImageOrientationPatient tag for the slice*/
	void GetDcmImageOrientationPatient(double dcmImageOrientationPatient[6])
	{
		dcmImageOrientationPatient[0] = m_DcmImageOrientationPatient[0];
		dcmImageOrientationPatient[1] = m_DcmImageOrientationPatient[1];
		dcmImageOrientationPatient[2] = m_DcmImageOrientationPatient[2];
		dcmImageOrientationPatient[3] = m_DcmImageOrientationPatient[3];
		dcmImageOrientationPatient[4] = m_DcmImageOrientationPatient[4];
		dcmImageOrientationPatient[5] = m_DcmImageOrientationPatient[5];
	};

	/** Get the DcmImageOrientationPatient tag for the slice*/
	const double *GetDcmImageOrientationPatient() { return m_DcmImageOrientationPatient; }

	/** Return the position unrotated */
	double *GetUnrotatedOrigin() { return m_UnrotatedOrigin; }
	
	/** return the description */
	mafString GetDescription() { return m_Description; };

	/** return the date */
	mafString GetDate() { return m_Date; };

	/** Get the DCM modality */
	mafString GetDcmModality() { return m_DcmModality; };

	/** Set the DCM modality */
	void SetDcmModality(mafString dcmModality) { m_DcmModality = dcmModality; };


	/** Returns PhotometricInterpretation */
	mafString GetPhotometricInterpretation() const { return m_PhotometricInterpretation; }

	/** Sets PhotometricInterpretation */
	void SetPhotometricInterpretation(mafString photometricInterpretation) { m_PhotometricInterpretation = photometricInterpretation; }


	/** Returns SeriesID */
	mafString GetSeriesID() const { return m_SeriesID; }

	/** Sets SeriesID */
	void SetSeriesID(mafString seriesID) { m_SeriesID = seriesID; }


	/** Returns StudyID */
	mafString GetStudyID() const { return m_StudyID; }

	/** Sets StudyID */
	void SetStudyID(mafString studyID) { m_StudyID = studyID; }

protected:

	void CalculateUnrotatedOrigin();

	double m_UnrotatedOrigin[3];
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
	mafString m_SeriesID;
	mafString m_StudyID;

	double m_DcmTriggerTime;
	int m_DcmInstanceNumber;
	int m_DcmCardiacNumberOfImages;

	vtkImageData *m_ImageData;	
};

#endif
