/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpExporterBmp.h,v $
  Language:  C++
  Date:      $Date: 2008-03-06 11:55:06 $
  Version:   $Revision: 1.1 $
  Authors:   Roberto Mucci
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafOpExporterBmp_H__
#define __mafOpExporterBmp_H__

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
// mafOpExporterBmp :
//----------------------------------------------------------------------------
/** Export a volume as a stack bmp images. User can choose to export two 
different bmp format files: 24 bit file or 8 bit gray scale format file, 
compatible with microCT software. */
class mafOpExporterBmp: public mafOp
{
public:
  mafOpExporterBmp(const wxString &label);
 ~mafOpExporterBmp(); 
  mafOp* Copy();

	/** Return true for the acceptable vme type. */
  bool Accept(mafNode *node);

	/** Builds operation's interface. */
  void OpRun();

	/** Execute the operation. */
  void OpDo();

	/** Makes the undo for the operation. */
  void OpUndo();

  void OnEvent(mafEventBase *maf_event);
	
	//Set the directory where export the .bmp files
  void SetDirName(const char *dir_name){m_DirName = dir_name;};

  /** Set the format of exported bmp file: "false" for 24 bit file, "true" for 8 bit gray scale file. */
  void SetGrayscale(bool grayScale){m_8bit = grayScale;};

  /** Export the volume as a stack of 24 bit bmp images. */
  void SaveBmp();

  /** Export the volume as a stack of 8 bit gray scale bmp images. */
  bool WriteImageDataAsMonocromeBitmap( vtkImageData *img, mafString filename);

protected:
  wxString		m_ProposedDirectory;///<Default directory where to save file .bmp
  mafString		m_FileName;///<Name of the file/files where the exporter will save bmp dat
  mafString m_DirName;
  int m_Offset;
  double m_ScalarRange[2];
  vtkImageData *m_Image;
  int m_8bit;
 };
#endif
