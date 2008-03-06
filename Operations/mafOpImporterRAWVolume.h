/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpImporterRAWVolume.h,v $
  Language:  C++
  Date:      $Date: 2008-03-06 11:55:06 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani     Silvano Imboden
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __mafOpImporterRAWVolume_H__
#define __mafOpImporterRAWVolume_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"
#include "mafEvent.h"
#include "mafString.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafNode;
class vtkImageReader;
class vtkActor;
class mmgDialogPreview;
class mafVMEVolumeGray;
class mafVMEVolumeRGB;
class vtkWindowLevelLookupTable;

//----------------------------------------------------------------------------
// mafOpImporterRAWVolume :
//----------------------------------------------------------------------------
class mafOpImporterRAWVolume: public mafOp
{
public:
             
            	 mafOpImporterRAWVolume(const wxString &label = "RAWImporterVolume");
	virtual     ~mafOpImporterRAWVolume();
	virtual void OnEvent(mafEventBase *maf_event);
	
  mafTypeMacro(mafOpImporterRAWVolume, mafOp);

  mafOp* Copy();

  enum RAW_SCALAR_TYPE
  {
    CHAR_SCALAR = 0,
    SHORT_SCALAR,
    INT_SCALAR,
    FLOAT_SCALAR,
    DOUBLE_SCALAR
  };

	bool Accept(mafNode *node) {return true;};

	void OpRun();

  /** Set the RAW volume filename to import.*/
  void SetFileName(const char *raw_file);

  /** Set the scalar type according to enum defined into the importer.*/
  void SetScalarType(int scalar_type = mafOpImporterRAWVolume::SHORT_SCALAR);

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
  void EnableWidgets(bool enable);
  void UpdateReader();
  int  GetFileLength(const char * filename); 
	
 	mafString		m_RawFile;

	int			m_Endian;
	int     m_ScalarType;
	int			m_Signed;
	int		  m_DataDimemsion[3];
  int     m_SliceVOI[2];
	double	m_DataSpacing[3];
	int			m_FileHeader;
	int			m_NumberOfByte;
	bool		m_BuildRectilinearGrid;
	mafString	m_CoordFile;
  mmgGui *m_GuiSlider;

  mafVMEVolumeGray  *m_VolumeGray;
  mafVMEVolumeRGB   *m_VolumeRGB;

	//preview pipeline
  vtkWindowLevelLookupTable *m_LookupTable;
	vtkImageReader*m_Reader;
	vtkActor			*m_Actor;
  
	//slice selector
	int					  m_CurrentSlice;
	wxSlider		 *m_SliceSlider;
  
	mmgDialogPreview *m_Dialog;
};
#endif
