/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpExporterBmp
 Authors: Roberto Mucci
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpExporterBmp_H__
#define __albaOpExporterBmp_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaOp.h"
#include "vtkImageData.h"
#include "vtkImageFlip.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// albaOpExporterBmp :
//----------------------------------------------------------------------------
/** Export a volume as a stack bmp images. User can choose to export two 
different bmp format files: 24 bit file or 8 bit gray scale format file, 
compatible with microCT software. */
class ALBA_EXPORT albaOpExporterImages: public albaOp
{
public:
  albaOpExporterImages(const wxString &label);
 ~albaOpExporterImages(); 
  albaOp* Copy();

	/** Builds operation's interface. */
  void OpRun();

	/** Execute the operation. */
  void OpDo();

	/** Makes the undo for the operation. */
  void OpUndo();

  void OnEvent(albaEventBase *alba_event);
	
	//Set the directory where export the .bmp files
  void SetDirName(const char *dir_name){m_DirName = dir_name;};

  /** Export the volume as a stack of 24 bit bmp images. */
  void SaveBmp();

	/** Return an xpm-icon that can be used to represent this operation */
	virtual char **GetIcon();

protected:

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

  wxString		m_ProposedDirectory;///<Default directory where to save file .bmp
  albaString		m_FileName;///<Name of the file/files where the exporter will save bmp dat
  albaString m_DirName;
  double m_ScalarRange[2];
  int m_FileFormat; 
  vtkImageData *m_Image;
 };
#endif
