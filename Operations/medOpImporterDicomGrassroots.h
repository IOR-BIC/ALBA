/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medOpImporterDicomGrassroots.h,v $
  Language:  C++
  Date:      $Date: 2009-04-08 10:18:03 $
  Version:   $Revision: 1.1.2.4 $
  Authors:   Roberto Mucci
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __medOpImporterDicomGrassroots_H__
#define __medOpImporterDicomGrassroots_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"
#include <map>

//#define VME_VOLUME_LARGE
/*#ifdef VME_VOLUME_LARGE
#include "vtkObject.h"
#include "../openMAF/vtkMAF/vtkMAFIdType64.h"
#include "../openMAF/VME/mafVMEVolumeLarge.h"
#include "../openMAF/VME/mafVMEVolumeLargeUtils.h"
#endif*/


//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafNode;
class mafGUIDialogPreview;
class mmoDICOMImporterListElementGrassroots;
class mmiDICOMImporterInteractor;
class vtkPlaneSource;
class vtkGDCMImageReader;
class vtkDirectory;
class vtkTexture;
class vtkPolyDataMapper;
class vtkActor;
class vtkWindowLevelLookupTable;
class mafTagArray;
class mafVMEVolumeGray;
class mafVMEImage;
class vtkImageData;
class vtkRectilinearGrid;
class vtkDoubleArray;


//WX_DECLARE_LIST(mmoCineMRIImporterListElement, ListDicomCineMRIFiles);
WX_DECLARE_LIST(mmoDICOMImporterListElementGrassroots, ListDicomFilesGrassroots);
//----------------------------------------------------------------------------
// medOpImporterDicomGrassroots :
//----------------------------------------------------------------------------

class medOpImporterDicomGrassroots: public mafOp
{
public:
            	 medOpImporterDicomGrassroots(wxString label = "DICOMImporter");
	virtual     ~medOpImporterDicomGrassroots();
	virtual void OnEvent(mafEventBase *maf_event);
	
  mafTypeMacro(medOpImporterDicomGrassroots, mafOp);

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

  /** Set the directory name which contains DICOM slices to import */
  void SetDirName(const char *dirName){m_DICOMDir = dirName;};

  /** Return the directory name which contains DICOM slices to import. */
  const char *GetDirName() const {return m_DICOMDir.GetCStr();};

  /** Import data contained in DICOM dir */
  void ImportDicom(const char *dir);

  /** Read Dicom file */
  void ReadDicom();

  void SetCurrentID(int id){m_CurrentID = id;};

  /** Return vtkImageData of the selected slice */
  vtkImageData* GetSliceImageData(int slice_num);

  /** Build the volume starting from the list of dicom files. */
  void BuildVolume();

  /** Build the volume starting from the list of CineMRI files. */
  void BuildVolumeCineMRI();

  /** Create the slice slice_num. */
  void CreateSlice(int slice_num);

protected:
	/** Build the list of dicom filer recognized. */
	void BuildDicomFileList(const char *dir);

  /** Show the slice slice_num. */
  void ShowSlice();

  /** Fill listbox. */
  void FillListBox();

	/** Reset the list of files and all the structures that own images information. */
	void ResetStructure();

	/** Reset the slider that allow to scan the slices. */
	void ResetSliders();

	/** Import dicom tags into vme tags. */
	void ImportDicomTags();

	/** Return the slice number from the heightId and sliceId*/
  int GetImageId(int heightId, int timeId);



//#ifdef VME_VOLUME_LARGE
  /** Detects whether the currently selected volume is large
  Returns true, if chosen ROI*given number of slices is larger than memory limit*/
  //virtual bool IsVolumeLarge(int nSlices);

  /** 
  if the volume (or VOI) is large, it displays a warning that the volume 
  to be imported is large and returns true, if the operation should continue,
  false otherwise (user canceled the import)*/
  //bool VolumeLargeCheck(int nSlices);

  /** Creates VMEVolumeLarge by importing data from the given lpszFileName
  pSCoord contain the grid coordinates in m_SortAxis direction (its count must correspond
  to the number of slices stored in the RAW file lpszFileName
  N.B. File is deleted upon the return. The routine returns false, if an error occurs*/
  //bool ImportLargeRAWFile(const char* lpszFileName, vtkDoubleArray* pSCoords);
//#endif

  /** Converts the given rectilinear grid into a regular grid.
  If the operation cannot be successfully completed (e.g., because
  it is not allowed or it would needed sampling of data),
  it returns NULL, otherwise it constructs a new object*/
  vtkImageData* GetImageData(vtkRectilinearGrid* pInput);

  /** Detects spacing in the given array of coordinates.
  It returns false, if the spacing between values is non-uniform*/
  bool DetectSpacing(vtkDoubleArray* pCoords, double* pOutSpacing);


  mmiDICOMImporterInteractor *m_DicomInteractor;

  // vtk objects
	vtkDirectory							*m_CTDirectoryReader;
	vtkGDCMImageReader 				*m_DicomReader;
	vtkWindowLevelLookupTable	*m_SliceLookupTable;

	vtkPlaneSource		*m_SlicePlane;
	vtkPolyDataMapper	*m_SliceMapper;
  vtkTexture				*m_SliceTexture;
	vtkActor					*m_SliceActor;

	vtkPlaneSource		*m_CropPlane;
	vtkActor					*m_CropActor;	

  // gui related variables
	mafString									m_DICOMDir;
	mafString									m_CurrentSliceName;
  //int												m_DICOM;
	
	// slices related informations
	wxListBox								 *m_StudyListbox;
	wxString									m_StudySelected;
	ListDicomFilesGrassroots					 *m_ListSelected;
	ListDicomFilesGrassroots					 *m_FilesList;
  std::map<int,ListDicomFilesGrassroots*> m_DicomMap;
	//ListDicomCineMRIFiles		 *m_FilesListCineMRI;
	int												m_NumberOfStudy;
	int												m_NumberOfSlices;
	int												m_CurrentSlice;
	int												m_CurrentTime;
	int												m_SortAxes;
	int												m_NumberOfTimeFrames;
  int                       m_CurrentID;

	// crop mode variables
	bool											m_CropFlag;
  bool										  m_CropMode;
  bool                      m_BoxCorrect;

  // build volume mode variables
	int												m_BuildStepValue;
	wxString									m_BuildStepChoices[4];

	mafTagArray							 *m_TagArray;
	mafGUIDialogPreview				 *m_DicomDialog;
	mafVMEVolumeGray				 *m_Volume;
  mafVMEImage              *m_Image;

	double									  m_DicomBounds[6];
	int											  m_GizmoStatus;
	int											  m_SideToBeDragged;
	mafString 							  m_PatientName;
	mafString                 m_SurgeonName;
	mafString								  m_Identifier;
	int												m_DICOMType;
  int                       m_BuildRLG;   //<non-zero, if a rectilinear grid is the preferred output

/*#ifdef VME_VOLUME_LARGE
  mafVMEVolumeLarge				 *m_VolumeLarge;
  mafString                 m_OutputFileName;    
  int                       m_MemLimit;   //<memory limit in MB
#endif // VME_VOLUME_LARGE*/

  wxTextCtrl   *m_SliceText;
	wxSlider		 *m_SliceScanner;
	wxStaticText *m_SliceLabel;

	wxTextCtrl   *m_TimeText;
	wxSlider		 *m_TimeScanner;
	wxStaticText *m_TimeLabel;
  
};

/*class mmoDICOMImporterListElementGrassroots
{
public:
	mmoDICOMImporterListElementGrassroots() {m_SliceFilename = ""; m_SlicePosition[0] = 0.0;m_SlicePosition[1] = 0.0;m_SlicePosition[2] = 0.0;};
	mmoDICOMImporterListElementGrassroots(const char *filename, double coord[3]) {m_SliceFilename = filename; m_SlicePosition[0] = coord[0];m_SlicePosition[1] = coord[1];m_SlicePosition[2] = coord[2];};
	~mmoDICOMImporterListElementGrassroots() {m_SliceFilename = ""; m_SlicePosition[0] = 0.0;m_SlicePosition[1] = 0.0;m_SlicePosition[2] = 0.0;};

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

class mmoDICOMImporterListElementGrassroots
{
public:
	mmoDICOMImporterListElementGrassroots() 
  {
    m_SliceFilename = "";
    m_Pos[0] = -9999;
    m_Pos[1] = -9999;
    m_Pos[2] = -9999;
    m_ImageNumber = -1;
    m_TriggerTime = -1.0;
    m_CardiacNumberOfImages = -1;
  };

	mmoDICOMImporterListElementGrassroots(mafString filename,double coord[3], int imageNumber=-1, int cardNumImages=-1, double trigTime=-1.0)  
  {
    m_SliceFilename = filename;
    m_Pos[0] = coord[0];
    m_Pos[1] = coord[1];
    m_Pos[2] = coord[2];
    m_ImageNumber = imageNumber;
    m_CardiacNumberOfImages = cardNumImages;
    m_TriggerTime = trigTime;
  };

  ~mmoDICOMImporterListElementGrassroots() {};

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
