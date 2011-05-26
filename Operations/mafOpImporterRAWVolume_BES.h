/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafOpImporterRAWVolume_BES.h,v $
Language:  C++
Date:      $Date: 2011-05-26 08:00:03 $
Version:   $Revision: 1.1.2.3 $
Authors:   Paolo Quadrani     Silvano Imboden     Josef Kohout
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __mafOpImporterRAWVolume_BES_H__
#define __mafOpImporterRAWVolume_BES_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "medDefines.h"
#include "mafOp.h"
#include "mafEvent.h"
#include "mafString.h"
#define VME_VOLUME_LARGE
#ifdef VME_VOLUME_LARGE
#include "vtkObject.h"
#include "vtkMAFIdType64.h"
#include "mafVMEVolumeLarge.h"
#include "mafVMEVolumeLargeUtils.h"
#endif

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafNode;
class vtkImageReader;
#ifdef VME_VOLUME_LARGE
class vtkMAFLargeImageReader;
#endif // VME_VOLUME_LARGE
class vtkActor;
class mafGUIDialogPreview;
class mafVMEVolumeGray;
class mafVMEVolumeRGB;
class vtkWindowLevelLookupTable;
class vtkDataObject;
class vtkTexture;
class vtkPlaneSource;


/** 
  class name: mafOpImporterRAWVolume_BES
  This importer imports Large Volume. It uses mafVMEVolumeLargeUtils made by BES.
*/
class MED_EXPORT mafOpImporterRAWVolume_BES: public mafOp
{	
public:

  mafOpImporterRAWVolume_BES(const wxString &label = "RAWImporterLargeVolume");
  virtual     ~mafOpImporterRAWVolume_BES();
  virtual void OnEvent(mafEventBase *maf_event);

  mafTypeMacro(mafOpImporterRAWVolume_BES, mafOp);

  mafOp* Copy();

  enum RAW_SCALAR_TYPE
  {
    CHAR_SCALAR = 0,
    SHORT_SCALAR,
    INT_SCALAR,
    FLOAT_SCALAR,
    DOUBLE_SCALAR
    //N.B. when modified, GetVTKDataType must be updated
  };

  bool Accept(mafNode *node) {return true;};

  void OpRun();

  /** Set the RAW volume filename to import.*/
  void SetFileName(const char *raw_file);

  /** Set the scalar type according to enum defined into the importer.*/
  void SetScalarType(int scalar_type = mafOpImporterRAWVolume_BES::SHORT_SCALAR);

  /** Turn On the flag to read the data scalars as signed. (Default is not signed)*/
  void ScalarSignedOn();

  /** Turn Off the flag to read the data scalars as not signed.  (Default is not signed)*/
  void ScalarSignedOff();

  /** Set the scalar data to be read as Big Endian. (Default is Little Endian)*/
  void SetScalarDataToBigEndian();

  /** Set the scalar data to be read as Little Endian. (Default)*/
  void SetScalarDataToLittleEndian();

  /** Set the dimensions of the data to import.
  N.B. It also modifies the data VOI*/
  void SetDataDimensions(int dims[3]);

  /** Allow to define a sub extent on z axes. (Default is imported all the volume)
  N.B. SetDataDimensions also sets data VOI*/
  void SetDataVOI(int zVOI[2]);

  /** Allow to define a sub extent on all axis. (Default is imported all the volume)
  N.B. SetDataDimensions also sets data VOI*/
  void SetDataVOI_XYZ(int VOI[6]);

  /** Set the spacing of the data to import.*/
  void SetDataSpacing(double spc[3]);

  /** Sets the number of scalar components
  By the default, it is one (grey scale data)*/
  void SetNumberOfScalarComponents(int nComps);  

  /** Import RAW data according to the parameters set. Return true on success.*/
  bool Import();

#ifdef VME_VOLUME_LARGE	
  //Sets the output file (with bricks)
  void SetOutputFile(const char* szOutputFile);
#endif // VME_VOLUME_LARGE

protected:
  /** Converts the internal data type into VTK data type */
  inline int GetVTKDataType();

  void EnableWidgets(bool enable);
  void UpdateReader();  

#ifndef VME_VOLUME_LARGE
  int  GetFileLength(const char * filename); 
#endif // VME_VOLUME_LARGE

  //updates the m_Reader or m_ReaderLarge
  template< typename TR >
  void UpdateReaderT(TR* reader);

  //returns the imported object, either vtkImageData or vtkMAFLargeImageData
  //returns NULL in case of error
  template< typename TR >
  vtkDataObject* ImportT(TR* reader);

#ifdef VME_VOLUME_LARGE
  //returns true, if the file is large and must be imported as large volume
  //NB. standard VTK reading processes does not support large files
  inline bool IsFileLarge() {
    return mafVMEVolumeLargeUtils::IsFileLarge(m_RawFile);
  }

  //returns true, if the volume to be imported is too large
  //and should be processed as VMEVolumeLarge
  inline bool IsVolumeLarge() {
    return mafVMEVolumeLargeUtils::IsVolumeLarge(m_VOI, GetVTKDataType(), 
      m_NumberOfScalarComponents, m_MemLimit);
  }

  //if the volume (or VOI) is large, it displays a warning that the volume 
  //to be imported is large and returns true, if the operation should continue,
  //false otherwise (user canceled the import)
  bool VolumeLargeCheck();
#endif // VME_VOLUME_LARGE

  mafString		m_RawFile;
#ifdef VME_VOLUME_LARGE
  mafString		m_OutputFileName;
#endif // VME_VOLUME_LARGE

  int			m_Endian;
  int     m_ScalarType;
  int			m_Signed;
  int		  m_DataDimemsion[3];
  int     m_VOI[6];
  double	m_DataSpacing[3];
  int			m_FileHeader;
  int			m_NumberOfByte;
  bool		m_BuildRectilinearGrid;
  mafString	m_CoordFile;
  mafGUI *m_GuiSlider;
#ifdef VME_VOLUME_LARGE
  int     m_MemLimit;
#endif // VME_VOLUME_LARGE
  int     m_NumberOfScalarComponents;   //<1 for grey scale, 3 for RGB
  int     m_UseLookupTable;

  mafVMEVolumeGray  *m_VolumeGray;
  mafVMEVolumeRGB   *m_VolumeRGB;
#ifdef VME_VOLUME_LARGE
  mafVMEVolumeLarge* m_VolumeLarge;
#endif // VME_VOLUME_LARGE

  //preview pipeline
  vtkWindowLevelLookupTable *m_LookupTable;
  vtkImageReader*		m_Reader;
#ifdef VME_VOLUME_LARGE
  vtkMAFLargeImageReader* m_ReaderLarge;
#endif // VME_VOLUME_LARGE
  vtkActor			*m_Actor;
  vtkTexture* m_Texture;
  vtkPlaneSource* m_Plane;

  //slice selector
  int					  m_CurrentSlice;
  wxSlider		 *m_SliceSlider;

  mafGUIDialogPreview *m_Dialog;

#ifdef VME_VOLUME_LARGE
  //LargeDataSetCallback may access our protected stuff
  friend class LargeDataSetCallback;
#endif // VME_VOLUME_LARGE
};

//------------------------------------------------------------------------
// Converts the internal data type into VTK data type
inline int mafOpImporterRAWVolume_BES::GetVTKDataType()
//------------------------------------------------------------------------
{
  //conversion from RAW_SCALAR_TYPE to VTK data types
  const static int VTK_SCALAR_TYPES[] = { 
    VTK_UNSIGNED_CHAR, VTK_CHAR, 
    VTK_UNSIGNED_SHORT, VTK_SHORT, 
    VTK_UNSIGNED_INT, VTK_INT, 
    VTK_FLOAT, VTK_FLOAT, 
    VTK_DOUBLE, VTK_DOUBLE, 
  };

  return VTK_SCALAR_TYPES[2*m_ScalarType + m_Signed];
}
#endif
