/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEVolumeRGB.h,v $
  Language:  C++
  Date:      $Date: 2005-05-31 09:47:33 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafVMEVolumeRGB_h
#define __mafVMEVolumeRGB_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafVMEVolume.h"
#include "mafVMEOutputVolume.h"
//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkDataSet;
class vtkVolumeData;

/** mafVMEVolumeRGB - a VME featuring an internal array for matrices and VTK datasets.
  mafVMEVolumeRGB is a specialized VME inheriting the VME-Generic features to internally
  store data, and specialized for VTK data. This specialization consists in creating
  a specialized data pipe and to redefining some APIs for casting to concrete classes.
  @sa mafVME mafMatrixVector mafDataVector  */
class MAF_EXPORT mafVMEVolumeRGB : public mafVMEVolume
{
public:
  mafTypeMacro(mafVMEVolumeRGB,mafVMEVolume);

  /** Set Volume data. */
  virtual int SetData(vtkImageData *data, mafTimeStamp t, int mode=MAF_VME_COPY_DATA);
  /** Set Volume data. */
  virtual int SetData(vtkRectilinearGrid *data, mafTimeStamp t, int mode=MAF_VME_COPY_DATA);

  /** return the right type of output */  
  mafVMEOutputVolume *GetVolumeOutput() {return (mafVMEOutputVolume *)GetOutput();}

  /** return the right type of output */  
  virtual mafVMEOutput *GetOutput();

protected:
  mafVMEVolumeRGB();
  virtual ~mafVMEVolumeRGB();

private:
  mafVMEVolumeRGB(const mafVMEVolumeRGB&); // Not implemented
  void operator=(const mafVMEVolumeRGB&); // Not implemented
  
  /** private to avoid calling by external classes */
  virtual int SetData(vtkDataSet *data, mafTimeStamp t, int mode=MAF_VME_COPY_DATA);
};

#endif

