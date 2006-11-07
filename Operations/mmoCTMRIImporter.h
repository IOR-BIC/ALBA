/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoCTMRIImporter.h,v $
  Language:  C++
  Date:      $Date: 2006-11-07 09:33:35 $
  Version:   $Revision: 1.7 $
  Authors:   Paolo Quadrani    Stefano Perticoni
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mmoCTMRIImporter_H__
#define __mmoCTMRIImporter_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafNode;
class mmgDialogPreview;
class mmoCTMRIImporterListElement;
class mmiDICOMImporterInteractor;
class vtkPlaneSource;
class vtkDicomUnPacker;
class vtkDirectory;
class vtkTexture;
class vtkPolyDataMapper;
class vtkActor;
class vtkWindowLevelLookupTable;
class mafTagArray;
class mafVMEVolumeGray;
class mafVMEImage;
class vtkImageData;

//----------------------------------------------------------------------------
// constants :
//----------------------------------------------------------------------------
enum DICOM_IMPORTER_MODALITY
{
	CROP_SELECTED,
	ADD_CROP_ITEM,
	GIZMO_NOT_EXIST,
	GIZMO_RESIZING,
	GIZMO_DONE
};

//WX_DECLARE_LIST(mmoCineMRIImporterListElement, ListDicomCineMRIFiles);
WX_DECLARE_LIST(mmoCTMRIImporterListElement, ListCTMRIFiles);
//----------------------------------------------------------------------------
// mmoCTMRIImporter :
//----------------------------------------------------------------------------

class mmoCTMRIImporter: public mafOp
{
public:
            	 mmoCTMRIImporter(wxString label = "CTMRIImporter");
	virtual     ~mmoCTMRIImporter();
	virtual void OnEvent(mafEventBase *maf_event);
	
  mafTypeMacro(mmoCTMRIImporter, mafOp);

  mafOp* Copy();

	/** Return true for the acceptable vme type. */
	bool Accept(mafNode *node) {return true;};

	/** Builds operation's interface calling CreateGui() method. */
	void OpRun();

	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	void OpStop	(int result);

	/** Execute the operation. */
	void OpDo();

	/** Makes the undo for the operation. */
	void OpUndo();
	
	/** Create the pipeline to read the images. */
	virtual void CreatePipeline();

	/** Create the dialog interface for the importer. */
  virtual void CreateGui();  

	/** Set dictonary location*/
	void SetDictonary(mafString dic);

	/** Set directory of studies*/
	void SetDirectory(mafString dir);

	/** Operation wait the user interact with the dialog*/
	void WaitUser();

	/** Initialize operation's variables according to the parameter's list. */
	virtual void SetParameters(void *param);

	/** Set Type DICOM to read CT/MRI */
	void SetModality(mafString modality);

	/** Set loading modality */
	void SetAutoLoad(bool autoLoad){m_AutoLoad = autoLoad;};

protected:
	/** Build the list of dicom filer recognized. */
	void BuildDicomFileList(const char *dir);

	/** Build the volume starting from the list of dicom files. */
	void BuildVolume();

	/** Build the volume starting from the list of CineMRI files. */
	void BuildVolumeCineMRI();

	/** Reset the list of files and all the structures that own images information. */
	void ResetStructure();

	/** Reset the slider that allow to scan the slices. */
	void ResetSliders();

	/** Show the slice slice_num. */
	void ShowSlice(int slice_num);

	/** Import dicom tags into vme tags. */
	void ImportDicomTags();

	/** Return the slice number from the heightId and sliceId*/
  int GetImageId(int heightId, int timeId);

  mmiDICOMImporterInteractor *m_DicomInteractor;

  // vtk objects
	vtkDirectory							*m_CTDirectoryReader;
	vtkDicomUnPacker					*m_DicomReader;
	vtkWindowLevelLookupTable	*m_SliceLookupTable;

	vtkPlaneSource		*m_SlicePlane;
	vtkPolyDataMapper	*m_SliceMapper;
  vtkTexture				*m_SliceTexture;
	vtkActor					*m_SliceActor;

	vtkPlaneSource		*m_CropPlane;
	vtkActor					*m_CropActor;	

  // gui related variables
	mafString									m_DictionaryFilename;
	mafString									m_DICOMDir;
	mafString									m_current_slice_name;
	int												m_DICOM;

	// slices related informations
	wxListBox								 *m_StudyListbox;
	wxString									m_StudySelected;
	ListCTMRIFiles					 *m_ListSelected;
	ListCTMRIFiles					 *m_FilesList;
	//ListDicomCineMRIFiles		 *m_FilesListCineMRI;
	int												m_NumberOfStudy;
	int												m_NumberOfSlices;
	int												m_CurrentSlice;
	int												m_CurrentTime;
	int												m_SortAxes;
	int												m_NumberOfTimeFrames;

	// crop mode variables
	bool											m_CropFlag;
  bool										  m_CropMode;

	bool											m_AutoLoad; ///<If true the importer start from CRIM connection

  // build volume mode variables
	int												m_BuildStepValue;
	wxString									m_BuildStepChoices[3];

	mafTagArray							 *m_TagArray;
	mmgDialogPreview				 *m_DicomDialog;
	mafVMEVolumeGray				 *m_Volume;
  mafVMEImage              *m_Image;

	double									  m_DicomBounds[6];
	int											  m_GizmoStatus;
	int											  m_SideToBeDragged;
	mafString 							  m_PatientName;
	mafString                 m_SurgeonName;
	mafString									m_Identifier;
	int												m_DICOMType;

  wxTextCtrl   *m_SliceText;
	wxSlider		 *m_SliceScanner;
	wxStaticText *m_SliceLabel;

	wxTextCtrl   *m_TimeText;
	wxSlider		 *m_TimeScanner;
	wxStaticText *m_TimeLabel;
};

/*class mmoDICOMImporterListElement
{
public:
	mmoDICOMImporterListElement() {m_SliceFilename = ""; m_SlicePosition[0] = 0.0;m_SlicePosition[1] = 0.0;m_SlicePosition[2] = 0.0;};
	mmoDICOMImporterListElement(const char *filename, double coord[3]) {m_SliceFilename = filename; m_SlicePosition[0] = coord[0];m_SlicePosition[1] = coord[1];m_SlicePosition[2] = coord[2];};
	~mmoDICOMImporterListElement() {m_SliceFilename = ""; m_SlicePosition[0] = 0.0;m_SlicePosition[1] = 0.0;m_SlicePosition[2] = 0.0;};

	/** Add the filename and the image coordinates to the list. */
	//void SetListElement(const char *filename, double coord[3]) {m_SliceFilename = filename; m_SlicePosition[0] = coord[0];m_SlicePosition[1] = coord[1];m_SlicePosition[2] = coord[2];};
	
	/** Return the filename of the corresponding dicom slice. */
	//const char*GetFileName() {return m_SliceFilename.GetCStr();};
	
	/**	Return the Coordinate along a specified axes of the dicom slice	*/
	//double GetCoordinate(int i) {return m_SlicePosition[i];};

/*protected:
	float m_SlicePosition[3];
	mafString m_SliceFilename;
};*/

class mmoCTMRIImporterListElement
{
public:
	mmoCTMRIImporterListElement() 
  {
    m_SliceFilename = "";
    m_Pos[0] = -9999;
    m_Pos[1] = -9999;
    m_Pos[2] = -9999;
    m_ImageNumber = -1;
    m_TriggerTime = -1.0;
    m_CardiacNumberOfImages = -1;
  };

	mmoCTMRIImporterListElement(mafString filename,double coord[3], int imageNumber=-1, int cardNumImages=-1, double trigTime=-1.0)  
  {
    m_SliceFilename = filename;
    m_Pos[0] = coord[0];
    m_Pos[1] = coord[1];
    m_Pos[2] = coord[2];
    m_ImageNumber = imageNumber;
    m_CardiacNumberOfImages = cardNumImages;
    m_TriggerTime = trigTime;
  };

  ~mmoCTMRIImporterListElement() {};

	/** Add the filename and the image coordinates to the list. */
	void SetListElement(mafString filename,double coord[3], int imageNumber=-1, int cardNumImages=-1, double trigTime=-1.0) 
  {
    m_SliceFilename = filename; 
    m_Pos[0] = coord[0];
    m_Pos[1] = coord[1];
    m_Pos[2] = coord[2];
    m_ImageNumber = imageNumber;
    m_CardiacNumberOfImages = cardNumImages;
    m_TriggerTime = trigTime;
  };
	
	/** Return the filename of the corresponding dicom slice. */
	const char *GetFileName() const {return m_SliceFilename.GetCStr();};
	
	/**	Return the Coordinate along a specified axes of the dicom slice	*/
	double	GetCoordinate(int i) const {return m_Pos[i];};

  /** Return the image number of the dicom slice*/
  int GetImageNumber() const {return m_ImageNumber;};

  /** Return the image number of the dicom slice*/
  int GetCardiacNumberOfImages() const {return m_CardiacNumberOfImages;};

  /** Return the trigger time of the dicom slice*/
  int GetTriggerTime() const {return m_TriggerTime;};

  

protected:
	double m_Pos[3];
	mafString m_SliceFilename;
  
  double m_TriggerTime;
  int m_ImageNumber;
  int m_CardiacNumberOfImages;

};
#endif
