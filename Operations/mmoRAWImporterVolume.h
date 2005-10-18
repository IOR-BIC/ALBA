/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoRAWImporterVolume.h,v $
  Language:  C++
  Date:      $Date: 2005-10-18 11:56:44 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani     Silvano Imboden
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __mmoRAWImporterVolume_H__
#define __mmoRAWImporterVolume_H__

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
// mmoRAWImporterVolume :
//----------------------------------------------------------------------------
class mmoRAWImporterVolume: public mafOp
{
public:
             
            	 mmoRAWImporterVolume(wxString label);
	virtual     ~mmoRAWImporterVolume();
	virtual void OnEvent(mafEventBase *maf_event);
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
	
protected:
  void EnableWidgets(bool enable);
  bool Import();
  void UpdateReader();
  int  GetFileLength(const char * filename); 
	
 	mafString		m_RawFile;

	int					m_Endian;
	int         m_ScalarType;
	int					m_Signed;
	int					m_DataDimemsion[3];
	double			m_DataSpacing[3];
	int					m_FileHeader;
	int					m_NumberOfByte;

  mmgGui  *m_GuiSlider;
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
