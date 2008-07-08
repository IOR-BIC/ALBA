/*========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafVMEMeshAnsysTextExporter.h,v $
Language:  C++
Date:      $Date: 2008-07-08 10:33:03 $
Version:   $Revision: 1.4 $
Authors:   Stefano Perticoni
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafVMEMeshAnsysTextExporter_h
#define __mafVMEMeshAnsysTextExporter_h

#include "vtkUnstructuredGrid.h"
#include "vtkMatrix4x4.h"
#include "mafVMEMesh.h"

#include "vnl/vnl_matrix.h"

#include <map>
#include <fstream>

/**
 .NAME mafVMEMeshAnsysTextExporter

  Export mafVMEMesh vtk unstructured grid to
  three ansys-output-like text files:

  nodes file ( geometry )
  elements file ( connectivity )
  materials file ( materials )

  BEWARE Hybrid meshes are not upported

  See mafVMEMeshAnsysTextExporterTest for use cases and
  mafVMEMeshAnsysTextImporter and mafVMEMeshAnsysTextImporterTest 
  also for a complete description of mafVMEMesh data structure and 
  supported elements

 .SECTION ToDo
*/
class mafVMEMeshAnsysTextExporter
{
public:

  /**
  Set/Get the vtk input grid*/
  void SetInput(vtkUnstructuredGrid *input) {m_Input = input;};
  vtkUnstructuredGrid *GetInput() {return m_Input;};
  
  /**
  // Set/Get output nodes file name*/
  void SetOutputNodesFileName(const char *name)   {this->m_OutputNodesFileName = name;};
  const char *GetOutputNodesFileName() {return this->m_OutputNodesFileName;};

  /**
  // Set/Get output nodes file name*/
  void SetOutputElementsFileName(const char *name)   {this->m_OutputElementsFileName = name;};
  const char *GetOutputElementsFileName() {return this->m_OutputElementsFileName;};

  /**
  // Set/Get output nodes file name*/
  void SetOutputMaterialsFileName(const char *name)   {this->m_OutputMaterialsFileName = name;};
  const char *GetOutputMaterialsFileName() {return this->m_OutputMaterialsFileName;};

  /** Set the abs matrix to be applied to the geometry */
  void SetMatrix(vtkMatrix4x4 *absMatrix) {m_MatrixToBeAppliedToGeometry = absMatrix;};
  vtkMatrix4x4 *GetMatrix() {return m_MatrixToBeAppliedToGeometry;};

  /** Apply abs matrix to geometry */
  void ApplyMatrixOn() {m_ApplyMatrixFlag = 1;};
  void ApplyMatrixOff() {m_ApplyMatrixFlag = 0;};
  void SetApplyMatrix(int applyMatrix) {m_ApplyMatrixFlag = applyMatrix;};

  /** Write output files; return MAF_OK if succesful, otherwise MAF_ERROR  */
  int Write();

  mafVMEMeshAnsysTextExporter();
  ~mafVMEMeshAnsysTextExporter();
	
protected:

  int WriteNodesFile(vtkUnstructuredGrid *inputUGrid, const char *outputFileName);
	
  int WriteElementsFile(vtkUnstructuredGrid *inputUGrid, const char *outputFileName);
	
  void WriteMaterialsFile(vtkUnstructuredGrid *inputUGrid, const char *outputFileName);
  
  
  /**
  // Output file name*/
  const char *m_OutputNodesFileName;
  const char *m_OutputElementsFileName;
  const char *m_OutputMaterialsFileName;

  vtkUnstructuredGrid *m_Input;
  int				 m_ApplyMatrixFlag;
  vtkMatrix4x4 *m_MatrixToBeAppliedToGeometry;
};

#endif


