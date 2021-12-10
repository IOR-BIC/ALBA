/*=========================================================================

Program: ALBA
Module: albaOpImporterDicom
Authors: Matteo Giacomoni, Roberto Mucci , Stefano Perticoni, Gianluigi Crimi

Copyright (c) BIC
All rights reserved. See Copyright.txt or
http://www.scsitaly.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpImporterDicom_H__
#define __albaOpImporterDicom_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaOp.h"
#include "vtkImageData.h"
#include <map>
#include <vector>
#include <set>
#include "albaGUIWizard.h"
#include "vtkMatrix4x4.h"


//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaDicomSlice;
class albaInteractorDICOMImporter;
class albaGUIWizardPageNew;
class albaString;
class albaTagArray;
class albaVME;
class albaVMEImage;
class albaVMEVolumeGray;
class albaGUICheckListBox;
class vtkDirectory;
class vtkLookupTable;
class vtkPlaneSource;
class vtkPolyDataMapper;
class vtkTexture;
class vtkActor;
class vtkActor2D;
class vtkPolyData;
class vtkTextMapper;
class albaProgressBarHelper;
class DcmDataset;

class albaDicomSlice;
class albaDicomSeries;
class albaGUIDicomSettings;
class albaDicomStudyList;
class albaDicomStudy;
namespace gdcm {
	class DataSet;
	class Tag;
	class Image;
	template<uint16_t Group, uint16_t Element,int TVR, int TVM> class Attribute;
}

#define READTAG(t) gdcm::DirectoryHelper::GetStringValueFromTag(t,dcmDataSet)
#define READ_AND_SET_TAGARRAY(X,Y) tagString = READTAG(X); if (!tagString.empty()) m_TagArray->SetTag(albaTagItem(Y, tagString.c_str()));


//----------------------------------------------------------------------------
// albaOpImporterDicomOffis :
//----------------------------------------------------------------------------
/** 
Perform DICOM importer.
From a DICOM dataset return a VME Volume or a VME Image.
*/
class ALBA_EXPORT albaOpImporterDicom : public albaOp
{
public:
	/** constructor */
	albaOpImporterDicom(wxString label = "Importer DICOM", bool justOnce = false);
	/** RTTI macro */
	albaTypeMacro(albaOpImporterDicom, albaOp);

	/** Copy. */
	albaOp* Copy();

	/** Builds operation's interface calling CreateGui() method. */
	virtual void OpRun();

	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	virtual void OpStop	(int result);

	/** Execute the operation. */
	virtual  void OpDo();

	/** Makes the undo for the operation. */
	void OpUndo(){};
			
	/** Create the vtkTexture for slice_num Dicom slice: this will be written to m_SliceTexture*/
	void GenerateSliceTexture(int imageID);

	/** Crops the image data  input depending on calculated crop extent*/
	void Crop(vtkImageData *slice);

	/** Calculate crop Extent depending on interactor data */
	void CalculateCropExtent();
			
	/** Build a volume from the list of Dicom files. */
	virtual int BuildVMEVolumeGrayOutput();
		
	/** Build images starting from the list of Dicom files. */
	virtual int BuildVMEImagesOutput();

	/** Create the pipeline to read the images. */
	virtual void CreateSliceVTKPipeline();

	/** Open dir containing Dicom images. */
	bool OpenDir(const char *dirPath);

	/** Get slice Image data*/
	vtkImageData *GetSliceInCurrentSeries(int id);

	/** Import dicom tags into vme tags. */
	void ImportDicomTags();

	/** method allows to handle events from other objects*/
	virtual void OnEvent(albaEventBase *alba_event);
	
	/*Set JustOnce Import Mode*/
	void SetJustOnceImport(bool justOnce) { m_JustOnceImport = justOnce; };

	/** Return an xpm-icon that can be used to represent this operation */
	virtual char ** GetIcon();

protected:

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

	/* Override this function to add specific tags to the reader, see implementation for an example */
	virtual void InsertAppSpecificTagsToReadList(std::set<gdcm::Tag> &TagsToRead);

	/* Override this function to read and set tags to the reader, see implementation for an example */
	virtual void ReadAndSetAppSpecificTags(albaTagArray * m_TagArray, gdcm::DataSet & dcmDataSet);
		
	enum DICOM_IMPORTER_GUI_ID
	{
		ID_FIRST = albaGUIWizard::ID_LAST,
		ID_STUDY_SELECT,
		ID_SERIES_SELECT,
		ID_CANCEL,
		ID_SCAN_TIME,
		ID_SCAN_SLICE,
		ID_VME_TYPE,
		ID_SHOW_TEXT,
		ID_UPDATE_NAME
	};

	enum
	{
		TYPE_VOLUME,
		TYPE_IMAGE,
	};

	/** OnEvent helper functions */
	void OnChangeSlice();
	void OnRangeModified();
	void OnWizardChangePage( albaEvent * e );
	virtual void SelectSeries(albaDicomSeries * selectedSeries);
	void OnStudySelect();
	
	/** Create load page and his GUI for the wizard. */
	void CreateLoadPage();

	/** Create crop page and   his GUI for the wizard. */
	void CreateCropPage();
		
	/** Reset the slider that allow to scan the slices. */
	void CreateSliders();

	/** Load Dicom in folder */
	bool LoadDicomFromDir(const char *dicomDirABSPath);

	/** Read the list of Dicom files recognized. */
	albaDicomSlice *ReadDicomFile(albaString fileName);

	/** Get the value of a tag inside a dicom dataset. */
	template <uint16_t A, uint16_t B> double GetAttributeValue(gdcm::DataSet &dcmDataSet);

	/** Return the slice number from the heightId and sliceId*/
	int GetSliceIDInSeries(int heightId, int timeId);

	/** Show the slice slice_num. */
	void SetPlaneDims();

	/** Fill Study listbox. */
	void FillStudyListBox();
		
	/** Fill Series listbox. */
	void FillSeriesListBox();
		
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
	
	/** Sets the VME name*/
	void SetVMEName();

	albaVME *FindVolumeInTree(albaVME *node);
		
	vtkLookupTable		*m_SliceLookupTable;
	vtkPlaneSource		*m_SlicePlane;
	vtkPolyDataMapper	*m_SliceMapper;
	vtkTexture				*m_SliceTexture;
	vtkActor					*m_SliceActor;
	vtkImageData			*m_CurrentSliceID;
	
	// text stuff
	vtkActor2D    *m_TextActor;
	vtkTextMapper	*m_TextMapper;

	albaInteractorDICOMImporter *m_DicomInteractor;

	albaGUIWizard			*m_Wizard;
	albaGUIWizardPageNew	*m_LoadPage;
	albaGUIWizardPageNew	*m_CropPage;
	
	albaGUI	*m_LoadGuiLeft;
	albaGUI	*m_LoadGuiUnderLeft;
	albaGUI	*m_CropGuiLeft;
	albaGUI	*m_CropGuiCenter;
	albaGUI	*m_LoadGuiCenter;
	albaGUI  *m_LoadGuiUnderCenter;

	int       m_OutputType;
	int			m_DescrInName;
	int			m_PatientNameInName;
	int			m_SizeInName;
	albaString m_VMEName;

	albaDicomStudyList *m_StudyList;
	albaDicomSeries	*m_SelectedSeries; ///< Selected series slices list
	
	/**Get dicom settings*/
	albaGUIDicomSettings* GetSetting();

	wxListBox	*m_StudyListbox;
	wxListBox *m_SeriesListbox;
	int m_SelectedStudy;
	
	int           m_ZCropBounds[2];
	int						m_CurrentSlice;
	wxSlider		 *m_SliceScannerLoadPage;
	wxSlider		 *m_SliceScannerCropPage;
	
	int						m_CurrentTime;
	
	albaTagArray	*m_TagArray;

	double	m_SliceBounds[6];
	bool m_CropEnabled;
	int m_CropExtent[6];

	bool m_ConstantRotation;
	
	int m_ShowOrientationPosition;
	int m_CurrentImageID;

	bool m_JustOnceImport;

	double m_SliceRange[2]; ///< contains the scalar range og the full dicom
	double m_SliceSubRange[2]; ///< contains the scalar range og the full dicom

	/** destructor */
	~albaOpImporterDicom();
private:
};


//----------------------------------------------------------------------------
// albaDicomStudyList:
//----------------------------------------------------------------------------

class ALBA_EXPORT albaDicomStudyList
{
public:
	~albaDicomStudyList();

	/** Add a slice, if necessary creates a new study/series*/
	void AddSlice(albaDicomSlice *slice);

	/** Get the num-th study*/
	albaDicomStudy *GetStudy(int num);

	/** Returns the number of studies*/
	int GetStudiesNum() { return m_Studies.size(); }

	/** Returns the total number of series*/
	int GetSeriesTotalNum();

	void RemoveSingleImagesFromList();
protected:
	std::vector<albaDicomStudy *> m_Studies;
};

//----------------------------------------------------------------------------
// albaDicomStudy:
//----------------------------------------------------------------------------

class ALBA_EXPORT albaDicomStudy
{
public:

	albaDicomStudy(albaString studyID) { m_StudyID = studyID; } 

	~albaDicomStudy();

	/** Add a slice, if necessary creates a new series */
	void AddSlice(albaDicomSlice *slice);
	
	/** Returns StudyID */
	albaString GetStudyID() const { return m_StudyID; }

	/** Returns the number of series */
	int GetSeriesNum() { return m_Series.size(); }

	/** Gets the id-th series */
	albaDicomSeries *GetSeries(int id) { return m_Series[id]; }


	void RemoveSingleImagesFromSeries();

protected:
	albaString m_StudyID;
	std::vector<albaDicomSeries *> m_Series;
};

//----------------------------------------------------------------------------
// albaDicomSeries:
//----------------------------------------------------------------------------

class ALBA_EXPORT albaDicomSeries
{
public:

	albaDicomSeries(albaString seriesID, albaString acquisitionNumber, albaString imageType) { m_SeriesID = seriesID; m_AcquisitionNumber = acquisitionNumber; m_ImageType = imageType; m_IsRotated = false; m_CardiacImagesNum = 0; }

	~albaDicomSeries();

	/** Add a slice */
	void AddSlice(albaDicomSlice *slice);

	/** Gets the id-th slice */
	albaDicomSlice *GetSlice(int id) { return m_Slices[id]; }

	/** Returns the number of series */
	int GetSlicesNum() { return m_Slices.size(); }

	/** Returns IsRotated */
	bool IsRotated() const { return m_IsRotated; }
	
	/** Returns Slices vector */
	std::vector<albaDicomSlice *> GetSlices() const { return m_Slices; }

	/** Returns SerieID */
	albaString GetSerieID() const { return m_SeriesID; }

	/** Returns Acquisition Number */
	albaString GetAcquisitionNumber() const { return m_AcquisitionNumber; }

	/** Returns Image Type */
	albaString GetImageType() const { return m_ImageType; }

	/** Return the slices dims */
	const int *GetDimensions() { return m_Dimensions; }

	/** Returns FramesNum */
	int GetCardiacImagesNum() const { return m_CardiacImagesNum; }

	/** Sort slices internally */
	void SortSlices();

protected:

	/** Check if Dicom dataset contains rotations */
	bool IsRotated(const double dcmImageOrientationPatient[6]);

	std::vector<albaDicomSlice *> m_Slices;
	int m_Dimensions[3];
	albaString m_SeriesID;
	albaString m_AcquisitionNumber;
	albaString m_ImageType;
	bool m_IsRotated;
	int m_CardiacImagesNum;
};


//----------------------------------------------------------------------------
// albaDicomSlice:
//----------------------------------------------------------------------------
/**
class name: albaDicomSlice
Holds information on a single dicom slice
*/
class ALBA_EXPORT albaDicomSlice
{
public:

	/** Constructor */
	albaDicomSlice(albaString sliceABSFilename, double dcmImageOrientationPatient[6], double dcmImagePositionPatient[3], albaString description, albaString date,
								albaString patientName, albaString patientBirthdate, int dcmCardiacNumberOfImages = -1, double dcmTtriggerTime = -1.0)
	{
		m_PatientBirthdate = patientBirthdate;
		m_PatientName = patientName;
		m_Description = description;
		m_Date = date;
		m_SliceABSFileName = sliceABSFilename;
		m_NumberOfCardiacImages = dcmCardiacNumberOfImages;
		m_TriggerTime = dcmTtriggerTime;
		SetDcmImageOrientationPatient(dcmImageOrientationPatient);
		SetImagePositionPatient(dcmImagePositionPatient);
	};

	/** destructor */
	~albaDicomSlice() {};

	/** Return patient birthday */
	albaString GetPatientBirthday() { return m_PatientBirthdate; };

	/** Return patient name */
	albaString GetPatientName() { return m_PatientName; };

	/** Return the filename of the corresponding Dicom slice. */
	const char *GetSliceABSFileName() const { return m_SliceABSFileName.GetCStr(); };

	/** Set the filename of the corresponding Dicom slice. */
	void SetSliceABSFileName(char *fileName) { m_SliceABSFileName = fileName; };
		
	/** Return the number of cardiac time frames */
	int GetNumberOfCardiacImages() const { return m_NumberOfCardiacImages; };

	/** Set the number of cardiac time frames */
	void SetNumberOfCardiacImages(int number) { m_NumberOfCardiacImages = number; };

	/** Return the trigger time of the Dicom slice */
	double GetTriggerTime() const { return m_TriggerTime; };

	/** Set the trigger time of the Dicom slice*/
	void SetTriggerTime(double time) { m_TriggerTime = time; };

	/* Gets the Dicom spacing by reading tags */
	void GetDicomSpacing(gdcm::DataSet &dcmDataSet, double * dcmPixelSpacing);

	/** Retrieve image data */
	vtkImageData* GetNewVTKImageData();
	
	/** Set the DcmImageOrientationPatient tag for the slice */
	void SetDcmImageOrientationPatient(double dcmImageOrientationPatient[6])
	{
		m_ImageOrientationPatient[0] = dcmImageOrientationPatient[0];
		m_ImageOrientationPatient[1] = dcmImageOrientationPatient[1];
		m_ImageOrientationPatient[2] = dcmImageOrientationPatient[2];
		m_ImageOrientationPatient[3] = dcmImageOrientationPatient[3];
		m_ImageOrientationPatient[4] = dcmImageOrientationPatient[4];
		m_ImageOrientationPatient[5] = dcmImageOrientationPatient[5];
		ComputeUnrotatedOrigin();
	};

	/** Get the DcmImageOrientationPatient tag for the slice */
	void GetDcmImageOrientationPatient(double dcmImageOrientationPatient[6])
	{
		dcmImageOrientationPatient[0] = m_ImageOrientationPatient[0];
		dcmImageOrientationPatient[1] = m_ImageOrientationPatient[1];
		dcmImageOrientationPatient[2] = m_ImageOrientationPatient[2];
		dcmImageOrientationPatient[3] = m_ImageOrientationPatient[3];
		dcmImageOrientationPatient[4] = m_ImageOrientationPatient[4];
		dcmImageOrientationPatient[5] = m_ImageOrientationPatient[5];
	};

	/** Get the DcmImageOrientationPatient tag for the slice */
	const double *GetDcmImageOrientationPatient() { return m_ImageOrientationPatient; }

	/** Return the position unrotated. 
	The origin is set to the output to this value,
	when the rotation matrix is applied the origin will be placed on the "right origin",
	unrotated origin is used also to sort slices depending on unrotated Z value */
	double *GetUnrotatedOrigin() { return m_UnrotatedOrigin; }
	
	/** return the description */
	albaString GetDescription() { return m_Description; };

	/** return the date */
	albaString GetDate() { return m_Date; };

	/** Get the Modality */
	albaString GetModality() { return m_Modality; };

	/** Set the DCM modality */
	void SetModality(albaString dcmModality) { m_Modality = dcmModality; };
	
	/** Returns PhotometricInterpretation */
	albaString GetPhotometricInterpretation() const { return m_PhotometricInterpretation; }

	/** Sets PhotometricInterpretation */
	void SetPhotometricInterpretation(albaString photometricInterpretation) { m_PhotometricInterpretation = photometricInterpretation; }

	/** Returns SeriesID */
	albaString GetSeriesID() const { return m_SeriesID; }

	/** Sets SeriesID */
	void SetSeriesID(albaString seriesID) { m_SeriesID = seriesID; }
	
	/** Returns StudyID */
	albaString GetStudyID() const { return m_StudyID; }

	/** Sets StudyID */
	void SetStudyID(albaString studyID) { m_StudyID = studyID; }
	
	/** Returns ImagePositionPatient */
	void GetImagePositionPatient(double imagePositionPatient[3]) 
	{
		imagePositionPatient[0] = m_ImagePositionPatient[0];
		imagePositionPatient[1] = m_ImagePositionPatient[1];
		imagePositionPatient[2] = m_ImagePositionPatient[2];

	}

	/** Sets ImagePositionPatient */
	void SetImagePositionPatient(double imagePositionPatient[3]) {
		m_ImagePositionPatient[0] = imagePositionPatient[0];
		m_ImagePositionPatient[1] = imagePositionPatient[1];
		m_ImagePositionPatient[2] = imagePositionPatient[2];
		ComputeUnrotatedOrigin();
	}
	
	/** Returns SliceSize */
	int *GetSliceSize() { return m_SliceSize; }

	/** Sets SliceSize */
	void SetSliceSize(int *sliceSize) { m_SliceSize[0] = sliceSize[0]; m_SliceSize[1] = sliceSize[1]; }


	/** Returns AcquisitionNumber */
	albaString GetAcquisitionNumber() const { return m_AcquisitionNumber; }

	/** Sets AcquisitionNumber */
	void SetAcquisitionNumber(albaString acquisitionNumber) { m_AcquisitionNumber = acquisitionNumber; }

	/** Returns Image Type */
	albaString GetImageType() const { return m_ImageType; }

	/** Sets Image Type */
	void SetImageType(albaString imageType) { m_ImageType = imageType; }
protected:

	/** Compute VTK Scalar Type from Dicom image*/
	int ComputeVTKScalarType(int scalarType);

	/** Compute the unrotated origin */
	void ComputeUnrotatedOrigin();

	double m_UnrotatedOrigin[3];
	double m_ImagePositionPatient[3];
	double m_ImageOrientationPatient[6];
	albaString m_SliceABSFileName;
	albaString m_Description;
	albaString m_Date;
	albaString m_PatientName;
	albaString m_PatientBirthdate;
	albaString m_Modality;
	albaString m_PhotometricInterpretation;
	albaString m_SeriesID;
	albaString m_AcquisitionNumber;
	albaString m_ImageType;
	albaString m_StudyID;

	double m_TriggerTime;
	int m_NumberOfCardiacImages;
	int m_SliceSize[2];
};

#endif
