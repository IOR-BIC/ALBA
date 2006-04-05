/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoDICOMImporter.h,v $
  Language:  C++
  Date:      $Date: 2006-04-05 08:01:02 $
  Version:   $Revision: 1.3 $
  Authors:   Paolo Quadrani    Stefano Perticoni
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mmoDICOMImporter_H__
#define __mmoDICOMImporter_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafNode;
class mmgDialogPreview;
class mmoDICOMImporterListElement;
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

WX_DECLARE_LIST(mmoDICOMImporterListElement, ListDicomFiles);
//----------------------------------------------------------------------------
// mmoDICOMImporter :
//----------------------------------------------------------------------------

class mmoDICOMImporter: public mafOp
{
public:
            	 mmoDICOMImporter(wxString label);
	virtual     ~mmoDICOMImporter();
	virtual void OnEvent(mafEventBase *maf_event);
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

protected:
	/** Build the list of dicom filer recognized. */
	void BuildDicomFileList(const char *dir);

	/** Build the volume starting from the list of dicom files. */
	void BuildVolume();

	/** Reset the list of files and all the structures that own images information. */
	void ResetStructure();

	/** Reset the slider that allow to scan the slices. */
	void ResetSliders();

	/** Show the slice slice_num. */
	void ShowSlice(int slice_num);

	/** Import dicom tags into vme tags. */
	void ImportDicomTags();

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
	mafString									m_CTDir;
	mafString									m_current_slice_name;

	// slices related informations
	wxListBox								 *m_StudyListbox;
	wxString									m_StudySelected;
	ListDicomFiles					 *m_ListSelected;
	ListDicomFiles					 *m_FilesList;
	int												m_NumberOfStudy;
	int												m_NumberOfSlices;
	int												m_CurrentSlice;
	int												m_SortAxes;
	// crop mode variables
	bool											m_CropFlag;
  bool										  m_CropMode;

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
	double									  m_Identifier;

  wxTextCtrl   *m_SliceText;
	wxSlider		 *m_SliceScanner;
	wxStaticText *m_SliceLabel;
};

class mmoDICOMImporterListElement
{
public:
	mmoDICOMImporterListElement() {m_SliceFilename = ""; m_SlicePosition[0] = 0.0;m_SlicePosition[1] = 0.0;m_SlicePosition[2] = 0.0;};
	mmoDICOMImporterListElement(const char *filename, double coord[3]) {m_SliceFilename = filename; m_SlicePosition[0] = coord[0];m_SlicePosition[1] = coord[1];m_SlicePosition[2] = coord[2];};
	~mmoDICOMImporterListElement() {m_SliceFilename = ""; m_SlicePosition[0] = 0.0;m_SlicePosition[1] = 0.0;m_SlicePosition[2] = 0.0;};

	/** Add the filename and the image coordinates to the list. */
	void SetListElement(const char *filename, double coord[3]) {m_SliceFilename = filename; m_SlicePosition[0] = coord[0];m_SlicePosition[1] = coord[1];m_SlicePosition[2] = coord[2];};
	
	/** Return the filename of the corresponding dicom slice. */
	const char*GetFileName() {return m_SliceFilename.GetCStr();};
	
	/**	Return the Coordinate along a specified axes of the dicom slice	*/
	double GetCoordinate(int i) {return m_SlicePosition[i];};

protected:
	float m_SlicePosition[3];
	mafString m_SliceFilename;
};
#endif
