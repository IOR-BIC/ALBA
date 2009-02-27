/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEOutputSurface.h,v $
  Language:  C++
  Date:      $Date: 2009-02-27 15:32:04 $
  Version:   $Revision: 1.8.2.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafVMEOutputSurface_h
#define __mafVMEOutputSurface_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafVMEOutputVTK.h"
//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkPolyData;
class vtkImageData;
class mmaMaterial;

/** Output for VME Surface with a VTK PolyData output data.
  mafVMEOutputSurface is the output produced by a node generating an output
  with a VTK dataset.
  @todo
  - add a method to extract/set material properties
*/
class MAF_EXPORT mafVMEOutputSurface : public mafVMEOutputVTK
{
public:
  mafVMEOutputSurface();
  virtual ~mafVMEOutputSurface();

  mafTypeMacro(mafVMEOutputSurface,mafVMEOutputVTK);

  /**
    Return a VTK dataset corresponding to the current time. This is
    the output of the DataPipe currently attached to the VME.
    Usually the output is a  "smart copy" of one of the dataset in 
    the DataArray. In some cases it can be NULL, e.g. in case the number
    of stored Items is 0. Also special VME could not support VTK dataset output.
    An event is rised when the output data changes to allow attached classes to 
    update their input.*/
  virtual vtkPolyData *GetSurfaceData();
  
  /** return the texture object: return NULL in case of no texture attached to the surface */
  virtual vtkImageData *GetTexture();

  /** used by VME to set the texture */
  void SetTexture(vtkImageData *tex);

  /** return material attribute of this surface if present */
  mmaMaterial *GetMaterial();

  /** set the material of the surface */ 
  void SetMaterial(mmaMaterial *material);

  /** IDs for the GUI */
  /*enum 
  {
    ID_NUM_TRIANGLES = Superclass::ID_LAST,
    ID_LAST
  };
  */

  /** Retrive number of trinagles (polys) of the polydata*/
  const char *GetNumberOfTriangles(){return m_NumTriangles;}

  /** Update all the output data structures (data, bounds, matrix and abs matrix).*/
  virtual void Update();

protected: 
  vtkImageData *m_Texture; ///< the optional texture to be applied to the surface
  mafString     m_NumTriangles;
  mmaMaterial  *m_Material; ///< material object used to store shading propertied to render the surface

  mafGUI *CreateGui();

private:
  mafVMEOutputSurface(const mafVMEOutputSurface&); // Not implemented
  void operator=(const mafVMEOutputSurface&); // Not implemented
};

#endif
