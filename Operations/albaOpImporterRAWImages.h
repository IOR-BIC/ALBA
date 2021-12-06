/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporterRAWImages
 Authors: Stefania Paperini porting Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaOpImporterRAWImages_H__
#define __albaOpImporterRAWImages_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaOp.h"
#include "albaEvent.h"
#include "albaString.h"

#include "vtkObject.h"


//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaOp;
class albaVME;
class vtkImageReader;
class vtkImageImport;
class vtkImageAppendComponents;
class vtkPlaneSource;
class vtkPolyDataMapper;
class vtkActor;
class vtkDirectory;
class albaRWI;
class vtkWindowLevelLookupTable;
class albaGUIDialogPreview;
class vtkTexture;
class albaInteractorDICOMImporter;
class albaString;
class albaVMEVolumeGray;
class albaVMEVolumeRGB;
#ifdef VME_VOLUME_LARGE
class vtkALBALargeImageReader;
#endif // VME_VOLUME_LARGE

//----------------------------------------------------------------------------
// albaOpImporterRAWImages :
//----------------------------------------------------------------------------
/** Importer for Raw volume data. The raw data must be stored in multiple files.
Every file is a single slice of the volume data.
In the directory must be present only raw slices.
If the first slice is not 0, the slice offset (m_offset) must be setted. */
class ALBA_EXPORT albaOpImporterRAWImages: public albaOp
{
public:
  albaOpImporterRAWImages(wxString label="RAWImporterImage");
  virtual     ~albaOpImporterRAWImages();

  albaTypeMacro(albaOpImporterRAWImages, albaOp);
  virtual void OnEvent(albaEventBase *alba_event);
  albaOp* Copy();

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
  void SetRawDirectory(albaString Dir){m_RawDirectory=Dir;};
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
  virtual void SetDataLayout(vtkALBALargeImageReader* r, bool bNonInterleaved);
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

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node) { return true; };

  /** Converts the internal data type into VTK data type */
  inline int GetVTKDataType();

  /** Enable the widgets of the interface. */
  void EnableWidgets(bool enable);

  /** Update the preview */
  void UpdateReader();

  /** Calculate the length of the file in order to give a guessed value for the header size. */
  int  GetFileLength(const char * filename);

  /** Control file list before read to alert for possible incoerences*/
  bool ControlFilenameList();

#ifdef VME_VOLUME_LARGE
  //if the volume (or VOI) is large, it displays a warning that the volume 
  //to be imported is large and returns true, if the operation should continue,
  //false otherwise (user canceled the import)
  bool VolumeLargeCheck();
#endif // VME_VOLUME_LARGE


  albaVME			 *m_Vme; 

  albaString			m_RawDirectory;
  vtkDirectory *m_VtkRawDirectory;
#ifdef VME_VOLUME_LARGE
  albaString		m_OutputFileName;
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
  vtkALBALargeImageReader* m_Reader;
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
  albaRWI				   *m_Rwi;
  vtkTexture		   *m_Texture;
  vtkPlaneSource   *m_GizmoPlane;
  vtkActor         *m_GizmoActor;

  //slice selector
  int					  m_CurrentSlice;
  wxTextCtrl	 *m_SliceText;
  wxSlider		 *m_SliceSlider;
  wxStaticText *m_SliceLab;

  vtkWindowLevelLookupTable	*m_LookupTable;

  albaGUIDialogPreview	*m_Dialog;
  albaGUI* m_GuiSlider;

  albaInteractorDICOMImporter *m_DicomInteractor;

  albaVMEVolumeGray  *m_VolumeGray;
  albaVMEVolumeRGB   *m_VolumeRGB;


  albaString m_DimXCrop;
  albaString m_DimYCrop;

};

//------------------------------------------------------------------------
// Converts the internal data type into VTK data type
inline int albaOpImporterRAWImages::GetVTKDataType()
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
