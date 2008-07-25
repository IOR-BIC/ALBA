/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEOutputVolume.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 07:06:00 $
  Version:   $Revision: 1.8 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafVMEOutputVolume_h
#define __mafVMEOutputVolume_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafVMEOutputVTK.h"
//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkImageData;
class vtkRectilinearGrid;
class vtkUnstructuredGrid;
class mmaVolumeMaterial;

/** NULL output for VME node with a VTK image output data.
  mafVMEOutputVolume is the output produced by a node generating an output
  with a VTK dataset. */
class MAF_EXPORT mafVMEOutputVolume : public mafVMEOutputVTK
{
public:
  mafVMEOutputVolume();
  virtual ~mafVMEOutputVolume();

  mafTypeMacro(mafVMEOutputVolume,mafVMEOutputVTK);

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

protected:
  mafString m_VtkDataType;
  mafString m_VolumeBounds[3];
  mafString m_ScaralRangeString;
  mmaVolumeMaterial *m_Material;

  mafGUI *CreateGui();

private:
  mafVMEOutputVolume(const mafVMEOutputVolume&); // Not implemented
  void operator=(const mafVMEOutputVolume&); // Not implemented
};
#endif
