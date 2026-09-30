/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpExporterVTK
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpExporterVTK_H__
#define __albaOpExporterVTK_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVME;

//----------------------------------------------------------------------------
// albaOpExporterVTK :
//----------------------------------------------------------------------------
/** */
class ALBA_EXPORT albaOpExporterVTK: public albaOp
{
public:
  albaOpExporterVTK(const wxString &label = "VTKExporter");
 ~albaOpExporterVTK(); 
  
  albaTypeMacro(albaOpExporterVTK, albaOp);

  albaOp* Copy();
	void OnEvent(albaEventBase *alba_event);

	/** Builds operation's interface. */
  void OpRun();

  void ApplyABSMatrixOn() {m_ABSMatrixFlag = 1;};
  void ApplyABSMatrixOff() {m_ABSMatrixFlag = 0;};
  void SetApplyABSMatrix(int apply_matrix) {m_ABSMatrixFlag = apply_matrix;};

  void ExportAsBynaryOn() {m_Binary = 1;};
  void ExportAsBynaryOff() {m_Binary = 0;};
  void SetExportAsBynary(int binary_file) {m_Binary = binary_file;};

  /** Set the filename for the .stl to export */
  void SetFileName(const char *file_name) {m_File = file_name;};

  /** Export vtk data. */
  void ExportVTK();

  /** vtk pipeline to save data on disk. */
  void SaveVTKData();

	/** Return an xpm-icon that can be used to represent this operation */
	virtual char ** GetIcon();

protected:

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

  albaString  m_File;
	albaString  m_FileDir;
  albaVME   *m_Vme; 
	int				m_Binary;
	int				m_ABSMatrixFlag;
	int       m_LegacyMode;
  int       m_ForceUnsignedShortScalarOutputForStructuredPoints;
};
#endif
