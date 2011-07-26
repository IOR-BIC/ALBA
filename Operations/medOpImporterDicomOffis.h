/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medOpImporterDicomOffis.h,v $
Language:  C++
Date:      $Date: 2011-07-26 10:20:17 $
Version:   $Revision: 1.1.2.58 $
Authors:   Matteo Giacomoni, Roberto Mucci , Stefano Perticoni
==========================================================================
Copyright (c) 2002/2007
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)

MafMedical Library use license agreement

The software named MafMedical Library and any accompanying documentation, 
manuals or data (hereafter collectively "SOFTWARE") is property of the SCS s.r.l.
This is an open-source copyright as follows:
Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, 
this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation and/or 
other materials provided with the distribution.
* Modified source versions must be plainly marked as such, and must not be misrepresented 
as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

MafMedical is partially based on OpenMAF.
=========================================================================*/

#ifndef __medOpImporterDicomOffis_H__
#define __medOpImporterDicomOffis_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "medDefines.h"
#include "mafOp.h"
#include "vtkImageData.h"
#include <map>
#include "medDicomCardiacMRIHelper.h"
#include "vtkMatrix4x4.h"


//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class medDicomSlice;
class medInteractorDICOMImporter;
class medGUIWizard;
class medGUIWizardPageNew;
class mafString;
class mafTagArray;
class mafVMEImage;
class mafVMEVolumeGray;
class mafGUICheckListBox;
class mafVMEMesh;
class vtkDirectory;
class vtkWindowLevelLookupTable;
class vtkPlaneSource;
class vtkPolyDataMapper;
class vtkTexture;
class vtkActor;
class vtkActor2D;
class vtkPolyData;
class vtkTextMapper;
class mafVMEGroup;

class medDicomSlice;
class medDicomSeriesSliceList;

//----------------------------------------------------------------------------
// medOpImporterDicomOffis :
//----------------------------------------------------------------------------
/** 
Perform DICOM importer.
From a DICOM dataset return a VME Volume, a VME Image or a VME Mesh.
*/
class MED_EXPORT medOpImporterDicomOffis : public mafOp
{
public:
	/** constructor */
	medOpImporterDicomOffis(wxString label = "Importer DICOM");
	/** RTTI macro */
	mafTypeMacro(medOpImporterDicomOffis, mafOp);

	/** Copy. */
	mafOp* Copy();

	/** Return true for the acceptable vme type. */
	bool Accept(mafNode *node) {return true;};

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

	/** Apply only to BuildVolume and BuildVolumeCineMRI: 
	Set if output will be imagedata (by resampling) or original rectilinear grid (default) */
	void SetResampleFlag(int enable){m_ResampleFlag = enable;}
	int GetResampleFlag(){return m_ResampleFlag;};

	/** Build a volume from the list of dicom files. */
	int BuildOutputVMEGrayVolumeFromDicom();

	/** Build a volume from the list of CineMRI files. */
	int BuildOutputVMEGrayVolumeFromDicomCineMRI();



	/** Build a mesh from the list of dicom files. */
	int BuildOutputVMEMeshFromDicom();

	/** Build a mesh from the list of CineMRI files. */
	int BuildOutputVMEMeshFromDicomCineMRI();

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

	/** Print the dicom list to the log area */
	void PrintDicomList(medDicomSeriesSliceList *dicomList);

protected:

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
	void OnVmeTypeSelected();

	/** Create load page and his GUI for the wizard. */
	void CreateLoadPage();

	/** Create crop page and   his GUI for the wizard. */
	void CreateCropPage();

	/** Create build page and his GUI for the wizard. */
	void CreateBuildPage();

	/** Reset the list of files and all the structures that own images information. */
	void ResetStructure();

	/** Reset the slider that allow to scan the slices. */
	void ResetSliders();

	/** Build the list of dicom filer recognized. */
	bool BuildDicomFileList(const char *dicomDirABSPath);

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
	int RunWizard();

	/** Auto position of the crop plane in way of Volume side. */
	void AutoPositionCropPlane();

	/** Performe crop of dicom data. */
	void Crop();

	/** Performe Undo crop of dicom data. */
	void OnUndoCrop();

	/** Delete all istances of used objects. */
	void Destroy();

	/** function that resample volume with rectilinear grid output. */
	void ResampleVolume();

	/** Extract a rotated polydata from original dicom image. */
	vtkPolyData * ExtractPolyData(int ts, int silceId);

	/** Rescale to 16 Bit */
	void RescaleTo16Bit(vtkImageData *dataSet);

	vtkDirectory			*m_DICOMDirectoryReader; 
	vtkWindowLevelLookupTable	*m_SliceLookupTable;
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

	medInteractorDICOMImporter *m_DicomInteractor;

	medGUIWizard			*m_Wizard;
	medGUIWizardPageNew	*m_LoadPage;
	medGUIWizardPageNew	*m_CropPage;
	medGUIWizardPageNew	*m_BuildPage;
	mafVMEMesh        *m_Mesh;
	mafVMEGroup       *m_ImagesGroup;

	mafGUI	*m_LoadGuiLeft;
	mafGUI	*m_LoadGuiUnderLeft;
	mafGUI	*m_CropGuiLeft;
	mafGUI	*m_CropGuiCenter;
	mafGUI	*m_BuildGuiLeft;
	mafGUI	*m_BuildGuiUnderLeft;
	mafGUI	*m_BuildGuiCenter;

	mafGUI  *m_LoadGuiUnderCenter;

	int       m_RadioButton;
	int       m_OutputType;
	mafString	m_DicomDirectoryABSFileName;
	mafString m_PatientName;
	mafString m_SurgeonName;
	mafString	m_Identifier;
	mafString m_PatientPosition;
  mafString m_VolumeName;

	int				m_BuildStepValue;
	int				m_DicomReaderModality; ///<Type DICOM Read from file
	long		  m_HighBit; ///<High bit DICOM Read from file
	double	  m_RescaleIntercept; ///<Rescale Intercept DICOM Read from file
	int				m_SortAxes;
	int				m_NumberOfTimeFrames;

	medDicomSeriesSliceList	*m_SelectedSeriesSlicesList; ///< Selected study slices list

	std::vector<mafString> m_SelectedSeriesID; ///< Selected StudyUID-SeriesUIDWithPlanesNumber-SeriesUIDWithoutPlanesNumber vector

	std::map<std::vector<mafString>,medDicomSeriesSliceList*> m_SeriesIDToSlicesListMap; ///< StudyUID-SeriesUIDWithPlanesNumber-SeriesUIDWithoutPlanesNumber vector to slices list map 

	std::map<std::vector<mafString>,medDicomCardiacMRIHelper*> m_SeriesIDToCardiacMRIHelperMap; ///< StudyUID-SeriesUIDWithPlanesNumber-SeriesUIDWithoutPlanesNumber vector to slices list map 

	std::map<std::vector<mafString>,bool> m_SeriesIDContainsRotationsMap; ///< StudyUID-SeriesUIDWithPlanesNumber-SeriesUIDWithoutPlanesNumber vector to boolean map 

	wxString  m_CurrentSliceABSFileName;
	int				m_VolumeSide;

	int				 m_NumberOfStudies; ///<Number of study present in the DICOM directory
	int				 m_NumberOfSlices;
	wxListBox	*m_StudyListbox;
	wxListBox	*m_SeriesListbox;

	int           m_ZCropBounds[2];
	int						m_CurrentSlice;
	wxSlider		 *m_SliceScannerLoadPage;
	wxSlider		 *m_SliceScannerCropPage;
	wxSlider		 *m_SliceScannerBuildPage;

	int						m_CurrentTime;
	wxSlider		 *m_TimeScannerLoadPage;
	wxSlider		 *m_TimeScannerCropPage;
	wxSlider		 *m_TimeScannerBuildPage;

	mafTagArray	*m_TagArray;

	double	m_SliceBounds[6];

	int	m_GizmoStatus;
	int	m_SideToBeDragged;

	bool m_BoxCorrect;
	bool m_CropFlag;
	bool m_CropExecuted; //<<<To check if a crop as been executed
	bool m_ApplyRotation; //<<< true il current series contains rotated slices
	bool m_ConstantRotation;
	bool m_ZCrop;

	int m_ResampleFlag;
	int m_DiscardPosition;
	int m_RescaleTo16Bit;

	mafVMEImage				*m_Image;
	mafVMEVolumeGray	*m_Volume;

	mafGUICheckListBox *m_DicomModalityListBox;

	/** destructor */
	~medOpImporterDicomOffis();

};

/**
class name: medDicomSlice
Holds information on a single dicom slice

We are using Image Position (Patient) (0020,0032) dicom tag to set dicom slice position. 
See here for the motivation behind this decision:
http://www.cmake.org/pipermail/insight-users/2005-September/014711.html
*/
class MED_EXPORT medDicomSlice
{
public:
	/** constructor */
	medDicomSlice() 
	{
    m_Description = "###";
    m_Date = "###";
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
	medDicomSlice(mafString sliceABSFilename,double dcmImagePositionPatient[3], double dcmImageOrientationPatient[6],\
		vtkImageData *data , mafString description, mafString date,int dcmInstanceNumber=-1, int dcmCardiacNumberOfImages=-1, double dcmTtriggerTime=-1.0)  
	{
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
	~medDicomSlice() {vtkDEL(m_Data);};

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

protected:
	double m_DcmImagePositionPatient[3];
  double m_DcmImagePositionPatientOriginal[3];
	double m_DcmImageOrientationPatient[6];
	mafString m_SliceABSFileName;
  mafString m_Description;
  mafString m_Date;

	double m_DcmTriggerTime;
	int m_DcmInstanceNumber;
	int m_DcmCardiacNumberOfImages;

	vtkImageData *m_Data;

};
#endif
