/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEMeshAnsysTextImporter
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaVMEMeshAnsysTextImporter_h
#define __albaVMEMeshAnsysTextImporter_h

#include "vtkUnstructuredGrid.h"

#include "albaVMEMesh.h"

#include "vnl/vnl_matrix.h"

#include <map>
#include <fstream>

/**
 .NAME albaVMEMeshAnsysTextImporter

  Build a albaVMEMesh from information contained
  in some text files. This text files are actually
  ANSYS list files without any header, only numbers.
  Supported elements type are:
  linear tetra, parabolic tetra, linear hexa, parabolic hexa
 
  BEWARE Hybrid meshes are not upported

  See albaVMEMeshAnsysTextImporterTest for use cases

 .SECTION Description

 .SECTION ToDo
*/
class ALBA_EXPORT albaVMEMeshAnsysTextImporter
{
public:

  /**
  // Set/Get nodes file name*/
  void SetNodesFileName(const char *name)   {this->m_NodesFileName = name;};
  const char *GetNodesFileName() {return this->m_NodesFileName;};

  /**
  // Set/Get elements file name*/
  void SetElementsFileName(const char *name)   {this->m_ElementsFileName = name;};
  const char *GetElementsFileName() {return this->m_ElementsFileName;};

  /**
  // Set/Get materials file name*/
  void SetMaterialsFileName(const char *name) {this->m_MaterialsFileName = name;};
  const char *GetMaterialsFileName() {return this->m_MaterialsFileName;};

  void SetMode(int mode) { m_ReaderMode = mode;}

  /**
  // Read files information; returns ALBA_OK if successful otherwise ALBA_ERROR*/
  int Read();

  /**
  Get the vme created by the reader*/
  albaVMEMesh *GetOutput() {return m_Output;};
  
  enum
  {
    ANSYS_MODE = 0,
    GENERIC_MODE,
  };

  // supported elements 
  enum
  {  
    TETRA4 = 0,
	  WEDGE6,
    HEXA8,  
    TETRA10,
    HEXA20,
    NUM_SUPPORTED_ELEMENTS,
    UNSUPPORTED_ELEMENT,
  };

  /** 
  Return the mesh element type; this function read the first line of the elements file
  and return the element type or the UNSUPPORTED_ELEMENT constant; return -1 if the elements 
  file does not esist. This function update ElementType, VtkCellType, MeshType and NodesPerElement ivar.*/
  int GetElementType();

  enum
  {
    LINEAR = 0,
    PARABOLIC,
    UNKNOWN,
    INVALID_MESH_FILE,
  };

  /** return the mesh type */
  int GetMeshType();

  /**
  Return the nodes number for current mesh; -1 if the nodes number is unknown.*/
  int GetNodesPerElement() {return m_NodesPerElement;};
  
  albaVMEMeshAnsysTextImporter();
  ~albaVMEMeshAnsysTextImporter();

protected:
 
  /**
  Read materials info from ansys .lis text file
  and fill the grid field data section. Existing fields are
  overwritten*/
  int ParseMaterialsFile(vtkUnstructuredGrid *grid, const char *filename);

  /**
  Reader Mode 0-Ansys 1-Generic*/
  int m_ReaderMode;

  /**
  Element type*/
  int m_ElementType;

  /**
  Cell type in vtk file format (Internal use only, look http://www.vtk.org/pdf/file-formats.pdf for more info)
  VtkCellType is -1 if the cell type is unknown*/
  int m_VtkCellType;

  /**
  Number of nodes per element for current mesh*/
  int m_NodesPerElement;

  /**
  Register the mesh type*/
  int m_MeshType;

  /**
  // Read nodes*/
  int ParseNodesFile(vtkUnstructuredGrid *grid);

  /**
  // Nodes file name*/
  const char *m_NodesFileName;

  /**
  // Read elements*/
  int ParseElementsFile(vtkUnstructuredGrid *grid);
  
  /**
  // Elements file name*/
  const char *m_ElementsFileName;

  /**
  // Materials file name*/
  const char *m_MaterialsFileName;

  // The output vme fem
  albaVMEMesh *m_Output;

  std::map<int, int> m_NodeIdNodeNumberMap;

  int m_FirstConnectivityColumn;
  
  /** 
  utility functions */
  int ReadMatrix(vnl_matrix<double> &M, const char *fname);
  void FEMDataToCellData(vtkUnstructuredGrid *input, vtkUnstructuredGrid *output );
  void AddIntArrayToUnstructuredGridCellData(vtkUnstructuredGrid *grid, vnl_matrix<double> &elementsFileMatrix, int column, albaString outputArrayName, bool activeScalar = false);


};

#endif


