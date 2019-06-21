/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEVolumeRGB
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaVMEVolumeRGB_h
#define __albaVMEVolumeRGB_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaVMEVolume.h"
#include "albaVMEOutputVolume.h"
//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkDataSet;
class vtkVolumeData;

/** albaVMEVolumeRGB - a VME featuring an internal array for matrices and VTK datasets.
  albaVMEVolumeRGB is a specialized VME inheriting the VME-Generic features to internally
  store data, and specialized for VTK data. This specialization consists in creating
  a specialized data pipe and to redefining some APIs for casting to concrete classes.
  @sa albaVME albaMatrixVector albaDataVector  */
class ALBA_EXPORT albaVMEVolumeRGB : public albaVMEVolume
{
public:
  albaTypeMacro(albaVMEVolumeRGB,albaVMEVolume);

  /** Set Volume data. */
  virtual int SetData(vtkImageData *data, albaTimeStamp t, int mode=ALBA_VME_COPY_DATA);
  /** Set Volume data. */
  virtual int SetData(vtkRectilinearGrid *data, albaTimeStamp t, int mode=ALBA_VME_COPY_DATA);

  /** return the right type of output */  
  albaVMEOutputVolume *GetVolumeOutput() {return (albaVMEOutputVolume *)GetOutput();}

  /** return the right type of output */  
  virtual albaVMEOutput *GetOutput();

protected:
  albaVMEVolumeRGB();
  virtual ~albaVMEVolumeRGB();

private:
  albaVMEVolumeRGB(const albaVMEVolumeRGB&); // Not implemented
  void operator=(const albaVMEVolumeRGB&); // Not implemented
  
  /** private to avoid calling by external classes */
  virtual int SetData(vtkDataSet *data, albaTimeStamp t, int mode=ALBA_VME_COPY_DATA);
};

#endif

