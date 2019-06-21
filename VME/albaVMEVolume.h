/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEVolume
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaVMEVolume_h
#define __albaVMEVolume_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaVMEGeneric.h"
#include "albaVMEOutputVolume.h"
//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkDataSet;
class vtkVolumeData;
class mmaVolumeMaterial;

/** albaVMEVolume - a VME featuring an internal array for matrices and VTK datasets.
  albaVMEVolume is a specialized VME inheriting the VME-Generic features to internally
  store data, and specialized for VTK data. This specialization consists in creating
  a specialized data pipe and to redefining some APIs for casting to concrete classes.
  @sa albaVME albaMatrixVector albaDataVector  */
class ALBA_EXPORT albaVMEVolume : public albaVMEGeneric
{
public:
  albaAbstractTypeMacro(albaVMEVolume,albaVMEGeneric);

  /** return the right type of output */  
  albaVMEOutputVolume *GetVolumeOutput() {return (albaVMEOutputVolume *)GetOutput();}

  /** return the right type of output */  
  virtual albaVMEOutput *GetOutput();

  /** private to avoid calling by external classes */
  virtual int SetData(vtkDataSet *data, albaTimeStamp t, int mode=ALBA_VME_COPY_DATA);

  /** return icon */
  static char** GetIcon();

  /** Return pointer to material attribute. */
  mmaVolumeMaterial *GetMaterial();

  /** Return the suggested pipe-typename for the visualization of this vme */
  virtual albaString GetVisualPipe() {return albaString("albaPipeBox");};

protected:
  albaVMEVolume();
  virtual ~albaVMEVolume();

  /** used to initialize and create the material attribute if not yet present */
  virtual int InternalInitialize();

private:
  albaVMEVolume(const albaVMEVolume&); // Not implemented
  void operator=(const albaVMEVolume&); // Not implemented
};

#endif
