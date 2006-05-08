/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEVolume.h,v $
  Language:  C++
  Date:      $Date: 2006-05-08 14:40:25 $
  Version:   $Revision: 1.3 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafVMEVolume_h
#define __mafVMEVolume_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafVMEGeneric.h"
#include "mafVMEOutputVolume.h"
//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkDataSet;
class vtkVolumeData;
class mmaVolumeMaterial;

/** mafVMEVolume - a VME featuring an internal array for matrices and VTK datasets.
  mafVMEVolume is a specialized VME inheriting the VME-Generic features to internally
  store data, and specialized for VTK data. This specialization consists in creating
  a specialized data pipe and to redefining some APIs for casting to concrete classes.
  @sa mafVME mafMatrixVector mafDataVector  */
class MAF_EXPORT mafVMEVolume : public mafVMEGeneric
{
public:
  mafAbstractTypeMacro(mafVMEVolume,mafVMEGeneric);

  /** return the right type of output */  
  mafVMEOutputVolume *GetVolumeOutput() {return (mafVMEOutputVolume *)GetOutput();}

  /** return the right type of output */  
  virtual mafVMEOutput *GetOutput();

  /** private to avoid calling by external classes */
  virtual int SetData(vtkDataSet *data, mafTimeStamp t, int mode=MAF_VME_COPY_DATA);

  /** return icon */
  static char** GetIcon();

  /** Return pointer to material attribute. */
  mmaVolumeMaterial *GetMaterial();

  /** Return the suggested pipe-typename for the visualization of this vme */
  virtual mafString GetVisualPipe() {return mafString("mafPipeBox");};

protected:
  mafVMEVolume();
  virtual ~mafVMEVolume();

  /** used to initialize and create the material attribute if not yet present */
  virtual int InternalInitialize();

private:
  mafVMEVolume(const mafVMEVolume&); // Not implemented
  void operator=(const mafVMEVolume&); // Not implemented
};

#endif
