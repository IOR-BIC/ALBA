/*=========================================================================

 Program: MAF2
 Module: mafOpExporterBmp
 Authors: Roberto Mucci
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

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
class MAF_EXPORT mafOpExporterBmp: public mafOp
{
public:
  mafOpExporterBmp(const wxString &label);
 ~mafOpExporterBmp(); 
  mafOp* Copy();

	/** Return true for the acceptable vme type. */
  bool Accept(mafVME*node);

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
  void SetGrayscale(bool grayScale){m_Bit8 = grayScale;};

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
  int m_Bit8;
 };
#endif
