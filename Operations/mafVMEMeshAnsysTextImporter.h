/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafVMEMeshAnsysTextImporter.h,v $
Language:  C++
Date:      $Date: 2007-04-05 14:49:13 $
Version:   $Revision: 1.3 $
Authors:   Stefano Perticoni
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafVMEMeshAnsysTextImporter_h
#define __mafVMEMeshAnsysTextImporter_h

#include "vtkUnstructuredGrid.h"

#include "mafVMEMesh.h"

#include "vnl/vnl_matrix.h"

#include <map>
#include <fstream>

/**
 .NAME mafVMEMeshAnsysTextImporter

  Build a mafVMEMesh from information contained
  in some text files. This text files are actually
  ANSYS list files without any header, only numbers.
  Supported elements type are:
  linear tetra, parabolic tetra, linear hexa, parabolic hexa
 
  BEWARE Hybrid meshes are not upported

 .SECTION Description

 .SECTION ToDo
*/
class mafVMEMeshAnsysTextImporter
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


  /**
  // Read files information; returns MAF_OK if successful otherwise MAF_ERROR*/
  int Read();

  /**
  Get the vme created by the reader*/
  mafVMEMesh *GetOutput() {return m_Output;};
  
  /**
  TO BE PORTED... DO NOT USE!
  Read materials info from ansys .lis text file
  and fill the grid field data section. Existing fields are
  overwritten*/
  int ParseMaterialsFile(vtkUnstructuredGrid *grid, const char *filename);

  // supported elements 
  enum
  {  
    TETRA4 = 0,
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

  /**
  Return the nodes number for current mesh; -1 if the nodes number is unknown.*/
  int GetNodesPerElement() {return m_NodesPerElement;};
  
  mafVMEMeshAnsysTextImporter();
  ~mafVMEMeshAnsysTextImporter();

protected:
 
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
  // Parse materials file*/
  int ParseMaterialsFile(vtkUnstructuredGrid *grid)
  {return mafVMEMeshAnsysTextImporter::ParseMaterialsFile(grid, this->m_MaterialsFileName);};

  /**
  // Materials file name*/
  const char *m_MaterialsFileName;

  // The output vme fem
  mafVMEMesh *m_Output;

  std::map<int, int> m_NodeIdNodeNumberMap;
  
  int ReadMatrix(vnl_matrix<double> &M, const char *fname);
  
};

#endif


