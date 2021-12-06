/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpExporterSTL
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpExporterSTL_H__
#define __albaOpExporterSTL_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaEvent.h"
#include "albaOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVME;

//----------------------------------------------------------------------------
// albaOpExporterSTL :
//----------------------------------------------------------------------------
/** */
class ALBA_EXPORT albaOpExporterSTL: public albaOp
{
public:
  albaOpExporterSTL(const wxString &label = "STLExporter");
 ~albaOpExporterSTL(); 
  
  albaTypeMacro(albaOpExporterSTL, albaOp);

  albaOp* Copy();
	void OnEvent(albaEventBase *alba_event);

	/** Builds operation's interface. */
  void OpRun();

  /** Set the filename for the .stl to export */
  void SetFileName(const char *file_name) {m_File = file_name;};

  void ApplyABSMatrixOn() {m_ABSMatrixFlag = 1;};
  void ApplyABSMatrixOff() {m_ABSMatrixFlag = 0;};
  void SetApplyABSMatrix(int apply_matrix) {m_ABSMatrixFlag = apply_matrix;};

  void ExportAsBynaryOn() {m_Binary = 1;};
  void ExportAsBynaryOff() {m_Binary = 0;};
  void SetExportAsBynary(int binary_file) {m_Binary = binary_file;};

  /** Export the surface. */
  void ExportSurface();

	/** Return an xpm-icon that can be used to represent this operation */
	virtual char ** GetIcon();

protected:

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	void OpStop(int result);

  albaString  m_File;
	albaString  m_FileDir;
	int				 m_Binary;
	int				 m_ABSMatrixFlag;
};
#endif
