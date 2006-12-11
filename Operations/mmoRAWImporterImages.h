/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoRAWImporterImages.h,v $
  Language:  C++
  Date:      $Date: 2006-12-11 09:35:32 $
  Version:   $Revision: 1.2 $
  Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __mmoRAWImporterImages_H__
#define __mmoRAWImporterImages_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"
#include "mafEvent.h"
#include "mafString.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafOp;
class mafNode;
class vtkImageReader;
class vtkImageImport;
class vtkImageAppendComponents;
class vtkPlaneSource;
class vtkPolyDataMapper;
class vtkActor;
class vtkDirectory;
class mafRWI;
class vtkWindowLevelLookupTable;
class mmgDialogPreview;
class vtkTexture;
class mmiDICOMImporterInteractor;
class mafString;
class mafVMEVolumeGray;
class mafVMEVolumeRGB;

//----------------------------------------------------------------------------
// mmoRAWImporterImages :
//----------------------------------------------------------------------------
/** Importer for Raw volume data. The raw data must be stored in multiple files.
Every file is a single slice of the volume data.
In the directory must be present only raw slices.
If the first slice is not 0, the slice offset (m_offset) must be setted. */
class mmoRAWImporterImages: public mafOp
{
public:
            	 mmoRAWImporterImages(wxString label="RAWImporterImage");
	virtual     ~mmoRAWImporterImages();

	mafTypeMacro(mmoRAWImporterImages, mafOp);
	virtual void OnEvent(mafEventBase *maf_event);
	mafOp* Copy();

	/** Return true for the acceptable vme type. */
	bool Accept(mafNode *node) {return true;};

	/** Build the interface of the operation and the preview pipeline. */
	void OpRun();

	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	void OpStop	(int result);

	/** Execute the operation. */
	void OpDo();

	/** Undo the operation. */
	void OpUndo();

	void OnStringPrefix();
	void OnOpenDir();

	/** Setting for the test */
	void SetRawDirectory(mafString Dir){m_RawDirectory=Dir;};
	void SetSigned(int Signed){m_Signed=Signed;};
	void SetDataType(int Bit){m_Bit=Bit;};
	void SetCoordFile(wxString File){m_CoordFile=File;m_Rect = true;m_Spacing[2] = 1.0;};
	void SetStringPrefix(wxString Prefix){m_Prefix=Prefix;};

	void CreatePipeline();
	void CreateGui();

	/** Import the raw data into the Data Manager*/
	bool Import();

  enum{
    CROP_SELECTED,
	  ADD_CROP_ITEM,
	  GIZMO_NOT_EXIST,
	  GIZMO_RESIZING,
	  GIZMO_DONE
  };
	
protected:
	/** Enable the widgets of the interface. */
	void EnableWidgets(bool enable);

	/** Update the preview */
	void UpdateReader();

	/** Calculate the length of the file in order to give a guessed value for the header size. */
	int  GetFileLength(const char * filename);

	mafNode			 *m_Vme; 

 	mafString			m_RawDirectory;
	vtkDirectory *m_VtkRawDirectory;

	int			 m_Bit;	
  int      m_RgbType;
	int			 m_Signed;
	int			 m_Dimension[3];
	double	 m_Spacing[3];
  double   m_ROI_2D[4];
	int			 m_Header;
	int			 m_NumberByte;
	int			 m_NumberFile;
	int			 m_NumberSlices;	
	int			 m_Offset;
	int			 m_FileSpacing;
  int      m_SideToBeDragged;
  int      m_GizmoStatus;
  int      m_CropMode;
	wxString m_Extension;
	wxString m_Prefix;
	wxString m_Pattern;

	bool		 m_Rect;
	wxString m_CoordFile;	

	//preview pipeline
	vtkImageReader		*m_Reader;
  vtkImageImport		*m_RedImage;
  vtkImageImport		*m_GreenImage;
  vtkImageImport		*m_BlueImage;
  vtkImageAppendComponents *m_AppendComponents;
  vtkImageAppendComponents *m_InterleavedImage;
	vtkPlaneSource   *m_Plane;
	vtkPolyDataMapper*m_Mapper;
	vtkActor			   *m_Actor;
	mafRWI				   *m_Rwi;
	vtkTexture		   *m_Texture;
	vtkPlaneSource   *m_GizmoPlane;
  vtkActor         *m_GizmoActor;

	//slice selector
	int					  m_CurrentSlice;
	wxTextCtrl	 *m_SliceText;
	wxSlider		 *m_SliceSlider;
	wxStaticText *m_SliceLab;

	vtkWindowLevelLookupTable	*m_LookupTable;
	
	mmgDialogPreview	*m_Dialog;

	mmiDICOMImporterInteractor *m_DicomInteractor;

	mafVMEVolumeGray  *m_VolumeGray;
  mafVMEVolumeRGB   *m_VolumeRGB;

};
#endif
