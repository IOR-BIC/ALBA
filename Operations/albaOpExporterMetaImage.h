/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpExporterMetaImage
 Authors: Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpExporterMetaImage_H__
#define __albaOpExporterMetaImage_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVME;

//----------------------------------------------------------------------------
// albaOpExporterMetaImage :
//----------------------------------------------------------------------------
/** */
class ALBA_EXPORT albaOpExporterMetaImage: public albaOp
{
public:
  albaOpExporterMetaImage(const wxString &label = "VTKExporter");
 ~albaOpExporterMetaImage(); 
  
  albaTypeMacro(albaOpExporterMetaImage, albaOp);

  albaOp* Copy();
	void OnEvent(albaEventBase *alba_event);

	/** Builds operation's interface. */
  void OpRun();

  void ApplyABSMatrixOn() {m_ABSMatrixFlag = 1;};
  void ApplyABSMatrixOff() {m_ABSMatrixFlag = 0;};
  void SetApplyABSMatrix(int apply_matrix) {m_ABSMatrixFlag = apply_matrix;};

  void ExportAsBynaryOn() {m_Compression = 1;};
  void ExportAsBynaryOff() {m_Compression = 0;};
  void SetExportAsBynary(int binary_file) {m_Compression = binary_file;};

  /** Set the filename for the .stl to export */
  void SetFileName(const char *file_name) {m_File = file_name;};

  /** Export image data. */
  void ExportMetaImage();


	/** Return an xpm-icon that can be used to represent this operation */
	virtual char ** GetIcon();

protected:

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

  albaString  m_File;
	albaString  m_FileDir;
  albaVME   *m_Vme; 
	int				m_Compression;
	int				m_ABSMatrixFlag;
};
#endif
