/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMESurface.h,v $
  Language:  C++
  Date:      $Date: 2006-07-14 15:12:36 $
  Version:   $Revision: 1.12 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafVMESurface_h
#define __mafVMESurface_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafVMEGeneric.h"
#include "mafVMEOutputSurface.h"
//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkDataSet;
class vtkPolyData;
class mmaMaterial;

/** mafVMESurface - a VME featuring an internal array for matrices and VTK datasets.
  mafVMESurface is a specialized VME inheriting the VME-Generic features to internally
  store data, and specialized for VTK data. This specialization consists in creating
  a specialized data pipe and to redefining some APIs for casting to concrete classes.
  @sa mafVME mafMatrixVector mafDataVector  */
class MAF_EXPORT mafVMESurface : public mafVMEGeneric
{
public:
  mafTypeMacro(mafVMESurface,mafVMEGeneric);

  /** Set data for the give timestamp. 
  This function automatically creates a VMEItem for the data to be stored. 
  The optional parameter specify if data must be copied, referenced, or detached from original source. 
  Default is to copy data which creates a new data set copying the original one.
  Referencing means the data is kept connected to its original source, and
  detaching means disconnect is like referencing but also disconnect the data
  from the source.
  Return MAF_OK if succeeded, MAF_ERROR if they kind of data is not accepted by
  this type of VME. The accepted surface is composed by polygonal cells (eg: triangles) or strips.*/
  virtual int SetData(vtkPolyData *data, mafTimeStamp t, int mode=MAF_VME_COPY_DATA);

  /** return the right type of output */
  mafVMEOutputSurface *GetSurfaceOutput() {return (mafVMEOutputSurface *)GetOutput();}

  /** return the right type of output */
  virtual mafVMEOutput *GetOutput();

  /** return icon */
  static char** GetIcon();

  /** Return pointer to material attribute. */
  mmaMaterial *GetMaterial();

  /** Return the suggested pipe-typename for the visualization of this vme */
  virtual mafString GetVisualPipe() {return mafString("mafPipeSurface");};

protected:
  mafVMESurface();
  virtual ~mafVMESurface();

  /** used to initialize and create the material attribute if not yet present */
  virtual int InternalInitialize();

private:
  mafVMESurface(const mafVMESurface&); // Not implemented
  void operator=(const mafVMESurface&); // Not implemented
  
  /** private to avoid calling by external classes */
  virtual int SetData(vtkDataSet *data, mafTimeStamp t, int mode=MAF_VME_COPY_DATA);
};
#endif
