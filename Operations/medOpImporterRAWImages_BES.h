/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medOpImporterRAWImages_BES.h,v $
  Language:  C++
  Date:      $Date: 2009-05-14 15:04:48 $
  Version:   $Revision: 1.1.2.1 $
  Authors:   Stefania Paperini porting Matteo Giacomoni
             Modified by Josef Kohout to support large volumes 
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __medOpImporterRAWImages_BES_H__
#define __medOpImporterRAWImages_BES_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"
#include "mafEvent.h"
#include "mafString.h"

#include "vtkObject.h"

#define VME_VOLUME_LARGE
#ifdef VME_VOLUME_LARGE
#include "vtkMAFIdType64.h"
#include "mafVMEVolumeLarge.h"
#include "mafVMEVolumeLargeUtils.h"
#endif

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
class mafGUIDialogPreview;
class vtkTexture;
class mmiDICOMImporterInteractor;
class mafString;
class mafVMEVolumeGray;
class mafVMEVolumeRGB;
#ifdef VME_VOLUME_LARGE
class vtkMAFLargeImageReader;
#endif // VME_VOLUME_LARGE

//----------------------------------------------------------------------------
// medOpImporterRAWImages_BES :
//----------------------------------------------------------------------------
/** Importer for Raw volume data. The raw data must be stored in multiple files.
Every file is a single slice of the volume data.
In the directory must be present only raw slices.
If the first slice is not 0, the slice offset (m_offset) must be setted. */
class medOpImporterRAWImages_BES: public mafOp
{
public:
            	 medOpImporterRAWImages_BES(wxString label="RAWImporterImage");
	virtual     ~medOpImporterRAWImages_BES();

	mafTypeMacro(medOpImporterRAWImages_BES, mafOp);
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

#ifdef VME_VOLUME_LARGE
  //Sets the output file (with bricks)
  void SetOutputFile(const char* szOutputFile);

  //returns true, if the volume to be imported is too large
  //and should be processed as VMEVolumeLarge
  bool IsVolumeLarge();

  /** Configure the data provider of the given reader
  if bNonInterleaved is set to true, the underlaying physical medium is supposed
  to keep the data in non-interleaved mode, i.e., RRRRR...R, GGGG...G, B....B*/
  virtual void SetDataLayout(vtkMAFLargeImageReader* r, bool bNonInterleaved);
#endif  //VME_VOLUME_LARGE

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
  /** Converts the internal data type into VTK data type */
  inline int GetVTKDataType();

	/** Enable the widgets of the interface. */
	void EnableWidgets(bool enable);

	/** Update the preview */
	void UpdateReader();

	/** Calculate the length of the file in order to give a guessed value for the header size. */
	int  GetFileLength(const char * filename);

  /** Control file list before read to alert for possible incoherences*/
  bool ControlFilenameList();

#ifdef VME_VOLUME_LARGE
  //if the volume (or VOI) is large, it displays a warning that the volume 
  //to be imported is large and returns true, if the operation should continue,
  //false otherwise (user canceled the import)
  bool VolumeLargeCheck();
#endif // VME_VOLUME_LARGE


	mafNode			 *m_Vme; 

 	mafString			m_RawDirectory;
	vtkDirectory *m_VtkRawDirectory;
#ifdef VME_VOLUME_LARGE
  mafString		m_OutputFileName;
#endif // VME_VOLUME_LARGE

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
#ifdef VME_VOLUME_LARGE
  int     m_MemLimit;   //<memory limit in MB
#endif // VME_VOLUME_LARGE
  int     m_UseLookupTable; //<zero, if the preview should use the values in the data directly

	bool		 m_Rect;
	wxString m_CoordFile;	

	//preview pipeline
#ifdef VME_VOLUME_LARGE
  vtkMAFLargeImageReader* m_Reader;
#else
  vtkImageReader		*m_Reader;
#endif // VME_VOLUME_LARGE	
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
	
	mafGUIDialogPreview	*m_Dialog;
  mafGUI* m_GuiSlider;

	mmiDICOMImporterInteractor *m_DicomInteractor;

	mafVMEVolumeGray  *m_VolumeGray;
  mafVMEVolumeRGB   *m_VolumeRGB;
#ifdef VME_VOLUME_LARGE
  mafVMEVolumeLarge* m_VolumeLarge;
#endif // VME_VOLUME_LARGE

  mafString m_DimXCrop;
  mafString m_DimYCrop;

};

//------------------------------------------------------------------------
// Converts the internal data type into VTK data type
inline int medOpImporterRAWImages_BES::GetVTKDataType()
//------------------------------------------------------------------------
{
  const static int BIT2VTK[] = {
    VTK_UNSIGNED_CHAR, VTK_CHAR, 
    VTK_UNSIGNED_SHORT, VTK_SHORT, 
    VTK_UNSIGNED_SHORT, VTK_SHORT,     
    VTK_UNSIGNED_CHAR, VTK_UNSIGNED_CHAR,
  };

  return BIT2VTK[2*m_Bit + m_Signed];
}
#endif
