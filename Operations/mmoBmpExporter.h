/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoBmpExporter.h,v $
  Language:  C++
  Date:      $Date: 2007-06-19 13:52:42 $
  Version:   $Revision: 1.2 $
  Authors:   Roberto Mucci
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mmoBmpExporter_H__
#define __mmoBmpExporter_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"
#include "vtkImageData.h"
#include "vtkImageFlip.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// mmoBmpExporter :
//----------------------------------------------------------------------------
/** Export a volume as a stack bmp images. User can choose to export two 
different bmp format files: 24 bit file or 8 bit gray scale format file, 
compatible with microCT software. */
class mmoBmpExporter: public mafOp
{
public:
  mmoBmpExporter(const wxString &label);
 ~mmoBmpExporter(); 
  mafOp* Copy();

	/** Return true for the acceptable vme type. */
  bool Accept(mafNode *node);

	/** Builds operation's interface. */
  void OpRun();

	/** Execute the operation. */
  void OpDo();

	/** Makes the undo for the operation. */
  void OpUndo();

  void   OnEvent(mafEventBase *maf_event);
	
	//Set the filename for the .bmp to export
	void SetFileName(const char *file_name){m_FileName = file_name;};

  /** Export the volume as a stack of 24 bit bmp images. */
  void SaveBmp();

  /** Export the volume as a stack of 8 bit gray scale bmp images. */
  bool WriteImageDataAsMonocromeBitmap( vtkImageData *img, mafString filename);

protected:
  wxString		m_ProposedDirectory;///<Default directory where to save file .bmp
  mafString		m_FileName;///<Name of the file/files where the exporter will save bmp dat
  mafString m_DirName;
  int m_Offset;
  double scalarRange[2];
  vtkImageData *m_Image;
  int m_8bit;
 };
#endif
