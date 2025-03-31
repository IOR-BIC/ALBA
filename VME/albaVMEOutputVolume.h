/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEOutputVolume
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaVMEOutputVolume_h
#define __albaVMEOutputVolume_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaVMEOutputVTK.h"
//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkImageData;
class vtkRectilinearGrid;
class vtkUnstructuredGrid;
class mmaVolumeMaterial;

/** NULL output for VME node with a VTK image output data.
  albaVMEOutputVolume is the output produced by a node generating an output
  with a VTK dataset. */
class ALBA_EXPORT albaVMEOutputVolume : public albaVMEOutputVTK
{
public:
  albaVMEOutputVolume();
  virtual ~albaVMEOutputVolume();

  albaTypeMacro(albaVMEOutputVolume,albaVMEOutputVTK);

  /** return the output data casting to vtkImageData */
  virtual vtkImageData *GetStructuredData();
  
  /** return the output data casting to vtkRectilinearGrid */
  virtual vtkRectilinearGrid *GetRectilinearData();

  /** return the output data casting to vtkUnstructuredGrid */
  virtual vtkUnstructuredGrid *GetUnstructuredData();

  /** return material attribute of this surface if present */
  mmaVolumeMaterial *GetMaterial();

  /** set the material of the surface */ 
  void SetMaterial(mmaVolumeMaterial *material);

  /** Update all the output data structures (data, bounds, matrix and abs matrix).*/
  virtual void Update();

  /** Retrieve as string the dataset type*/
  const char* GetVTKDataTypeAsString(){return m_VtkDataType.GetCStr();}

protected:
  albaString m_VtkDataType;
	albaString m_VolumeBounds[3];
	albaString m_VolumeDims;
	albaString m_ScaralRangeString;
  mmaVolumeMaterial *m_Material;

  albaGUI *CreateGui();

private:
  albaVMEOutputVolume(const albaVMEOutputVolume&); // Not implemented
  void operator=(const albaVMEOutputVolume&); // Not implemented
};
#endif
