/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporterRAWVolume
 Authors: Paolo Quadrani     Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaOpImporterRAWVolume_H__
#define __albaOpImporterRAWVolume_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaOp.h"
#include "albaEvent.h"
#include "albaString.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVME;
class vtkImageReader;
class vtkActor;
class albaGUIDialogPreview;
class albaVMEVolumeGray;
class albaVMEVolumeRGB;
class vtkWindowLevelLookupTable;

//----------------------------------------------------------------------------
// albaOpImporterRAWVolume :
//----------------------------------------------------------------------------
class ALBA_EXPORT albaOpImporterRAWVolume: public albaOp
{
public:
             
            	 albaOpImporterRAWVolume(const wxString &label = "RAWImporterVolume");
	virtual     ~albaOpImporterRAWVolume();
	virtual void OnEvent(albaEventBase *alba_event);
	
  albaTypeMacro(albaOpImporterRAWVolume, albaOp);

  albaOp* Copy();

  enum RAW_SCALAR_TYPE
  {
    CHAR_SCALAR = 0,
    SHORT_SCALAR,
    INT_SCALAR,
    FLOAT_SCALAR,
    DOUBLE_SCALAR
  };

	void OpRun();

  /** Set the RAW volume filename to import.*/
  void SetFileName(const char *raw_file);

  /** Set the scalar type according to enum defined into the importer.*/
  void SetScalarType(int scalar_type = albaOpImporterRAWVolume::SHORT_SCALAR);

  /** Turn On the flag to read the data scalars as signed. (Default is not signed)*/
  void ScalarSignedOn();

  /** Turn Off the flag to read the data scalars as not signed.  (Default is not signed)*/
  void ScalarSignedOff();

  /** Set the scalar data to be read as Big Endian. (Default is Little Endian)*/
  void SetScalarDataToBigEndian();
  
  /** Set the scalar data to be read as Little Endian. (Default)*/
  void SetScalarDataToLittleEndian();

  /** Set the dimensions of the data to import.*/
  void SetDataDimensions(int dims[3]);

  /** Allow to define a sub extent on z axes. (Default is imported all the volume)*/
  void SetDataVOI(int zVOI[2]);

  /** Set the spacing of the data to import.*/
  void SetDataSpacing(double spc[3]);

  /** Import RAW data according to the parameters set. Return true on success.*/
  bool Import();

protected:

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node) { return true; };

  void EnableWidgets(bool enable);
  void UpdateReader();
  int  GetFileLength(const char * filename); 
	
 	albaString		m_RawFile;

	int			m_Endian;
	int     m_ScalarType;
	int			m_Signed;
	int		  m_DataDimemsion[3];
  int     m_SliceVOI[2];
	double	m_DataSpacing[3];
	int			m_FileHeader;
	int			m_NumberOfByte;
	bool		m_BuildRectilinearGrid;
	albaString	m_CoordFile;
  albaGUI *m_GuiSlider;

  albaVMEVolumeGray  *m_VolumeGray;
  albaVMEVolumeRGB   *m_VolumeRGB;

	//preview pipeline
  vtkWindowLevelLookupTable *m_LookupTable;
	vtkImageReader*m_Reader;
	vtkActor			*m_Actor;
  
	//slice selector
	int					  m_CurrentSlice;
	wxSlider		 *m_SliceSlider;
  
	albaGUIDialogPreview *m_Dialog;
};
#endif
