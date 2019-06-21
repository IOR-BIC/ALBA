/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEOutputMesh
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaVMEOutputMesh_h
#define __albaVMEOutputMesh_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaVMEOutputVTK.h"
//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkUnstructuredGrid;
class mmaMaterial;

/** 
  albaVMEOutputMesh is the output produced by a node generating an output
  with a VTK unstructured grid dataset.
*/
class ALBA_EXPORT albaVMEOutputMesh : public albaVMEOutputVTK
{
public:
  albaVMEOutputMesh();
  virtual ~albaVMEOutputMesh();

  albaTypeMacro(albaVMEOutputMesh,albaVMEOutputVTK);

  /**
    Return a VTK dataset corresponding to the current time. This is
    the output of the DataPipe currently attached to the VME.
    Usually the output is a  "smart copy" of one of the dataset in 
    the DataArray. In some cases it can be NULL, e.g. in case the number
    of stored Items is 0. Also special VME could not support VTK dataset output.
    An event is rised when the output data changes to allow attached classes to 
    update their input.*/
  virtual vtkUnstructuredGrid *GetUnstructuredGridData();

  /** Update all the output data structures (data, bounds, matrix and abs matrix).*/
  virtual void Update();

	/** return material attribute of this mesh if present */
	mmaMaterial *GetMaterial();

	/** set the material of the mesh */ 
	void SetMaterial(mmaMaterial *material);

protected: 

  albaString  m_NumCells;
	albaString  m_NumNodes;
  albaGUI *CreateGui();

	mmaMaterial  *m_Material; ///< material object used to store shading propertied to render the surface


private:
  albaVMEOutputMesh(const albaVMEOutputMesh&); // Not implemented
  void operator=(const albaVMEOutputMesh&); // Not implemented
};

#endif
