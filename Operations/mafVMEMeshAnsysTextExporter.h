/*========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafVMEMeshAnsysTextExporter.h,v $
Language:  C++
Date:      $Date: 2008-02-05 10:12:22 $
Version:   $Revision: 1.1 $
Authors:   Stefano Perticoni
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafVMEMeshAnsysTextExporter_h
#define __mafVMEMeshAnsysTextExporter_h

#include "vtkUnstructuredGrid.h"

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

  /** Write output files  */
  void Write();


  mafVMEMeshAnsysTextExporter();
  ~mafVMEMeshAnsysTextExporter();
	
protected:

  void WriteNodesFile(vtkUnstructuredGrid *inputUGrid, const char *outputFileName);
	
  void WriteElementsFile(vtkUnstructuredGrid *inputUGrid, const char *outputFileName);
	
  void WriteMaterialsFile(vtkUnstructuredGrid *inputUGrid, const char *outputFileName);
  
  
  /**
  // Output file name*/
  const char *m_OutputNodesFileName;
  const char *m_OutputElementsFileName;
  const char *m_OutputMaterialsFileName;

  vtkUnstructuredGrid *m_Input;
 
};

#endif


