/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMESurface
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaVMESurface_h
#define __albaVMESurface_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaVMEGeneric.h"
#include "albaVMEOutputSurface.h"
//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkDataSet;
class vtkPolyData;
class mmaMaterial;

/** albaVMESurface - a VME featuring an internal array for matrices and VTK datasets.
  albaVMESurface is a specialized VME inheriting the VME-Generic features to internally
  store data, and specialized for VTK data. This specialization consists in creating
  a specialized data pipe and to redefining some APIs for casting to concrete classes.
  @sa albaVME albaMatrixVector albaDataVector  */
class ALBA_EXPORT albaVMESurface : public albaVMEGeneric
{
public:
  albaTypeMacro(albaVMESurface,albaVMEGeneric);

  /** Set data for the give timestamp. 
  This function automatically creates a VMEItem for the data to be stored. 
  The optional parameter specify if data must be copied, referenced, or detached from original source. 
  Default is to copy data which creates a new data set copying the original one.
  Referencing means the data is kept connected to its original source, and
  detaching means disconnect is like referencing but also disconnect the data
  from the source.
  Return ALBA_OK if succeeded, ALBA_ERROR if they kind of data is not accepted by
  this type of VME. The accepted surface is composed by polygonal cells (eg: triangles) or strips.*/
  virtual int SetData(vtkPolyData *data, albaTimeStamp t, int mode=ALBA_VME_COPY_DATA);

  /** return the right type of output */
  albaVMEOutputSurface *GetSurfaceOutput() {return (albaVMEOutputSurface *)GetOutput();}

  /** return the right type of output */
  virtual albaVMEOutput *GetOutput();

  /** return icon */
  static char** GetIcon();

  /** Return pointer to material attribute. */
  mmaMaterial *GetMaterial();

  /** Return the suggested pipe-typename for the visualization of this vme */
  virtual albaString GetVisualPipe() {return albaString("albaPipeSurface");};

protected:
  albaVMESurface();
  virtual ~albaVMESurface();

  /** used to initialize and create the material attribute if not yet present */
  virtual int InternalInitialize();

private:
  albaVMESurface(const albaVMESurface&); // Not implemented
  void operator=(const albaVMESurface&); // Not implemented
  
  /** private to avoid calling by external classes */
  virtual int SetData(vtkDataSet *data, albaTimeStamp t, int mode=ALBA_VME_COPY_DATA);
};
#endif
