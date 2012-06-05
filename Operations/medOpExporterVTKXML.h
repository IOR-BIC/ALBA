/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medOpExporterVTKXML.h,v $
  Language:  C++
  Date:      $Date: 2010-12-22 10:04:00 $
  Version:   $Revision: 1.1.2.1 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __medOpExporterVTKXML_H__
#define __medOpExporterVTKXML_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"
#include "medOperationsDefines.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVME;

//----------------------------------------------------------------------------
// medOpExporterVTKXML :
//----------------------------------------------------------------------------
/** */
class MED_OPERATION_EXPORT medOpExporterVTKXML: public mafOp
{
public:
  medOpExporterVTKXML(const wxString &label = "VTKXMLExporter");
 ~medOpExporterVTKXML(); 
  
  mafTypeMacro(medOpExporterVTKXML, mafOp);

  mafOp* Copy();
	void OnEvent(mafEventBase *maf_event);

	/** Return true for the acceptable vme type. */
  bool Accept(mafNode *node);

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

protected:
  mafString  m_File;
	mafString  m_FileDir;
  mafVME   *m_Vme; 
	int				m_Binary;
	int				m_ABSMatrixFlag;
  int       m_ForceUnsignedShortScalarOutputForStructuredPoints;
};
#endif
