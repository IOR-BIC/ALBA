/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMESurface.h,v $
  Language:  C++
  Date:      $Date: 2005-07-05 06:03:04 $
  Version:   $Revision: 1.10 $
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
class mmgMaterialButton;

/** mafVMESurface - a VME featuring an internal array for matrices and VTK datasets.
  mafVMESurface is a specialized VME inheriting the VME-Generic features to internally
  store data, and specialized for VTK data. This specialization consists in creating
  a specialized data pipe and to redefining some APIs for casting to concrete classes.
  @sa mafVME mafMatrixVector mafDataVector  */
class MAF_EXPORT mafVMESurface : public mafVMEGeneric
{
public:
  mafTypeMacro(mafVMESurface,mafVMEGeneric);

  /** 
  Set data for the give timestamp. This function automatically creates a
  a VMEItem for the data to be stored. The optional parameter specify if
  data must be copied, referenced, or detached from original source. Default
  is to copy data which creates a new data set copying the original one.
  Referencing means the data is kept connected to its original source, and
  detaching means disconnect is like referencing but also disconnect the data
  from the source.
  Return MAF_OK if succeeded, MAF_ERROR if they kind of data is not accepted by
  this type of VME. */
  virtual int SetData(vtkPolyData *data, mafTimeStamp t, int mode=MAF_VME_COPY_DATA);

  /** Precess events coming from other objects */ 
  virtual void OnEvent(mafEventBase *maf_event);

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

  /** Internally used to create a new instance of the GUI.*/
  virtual mmgGui *CreateGui();
  mmgMaterialButton *m_MaterialButton;

private:
  mafVMESurface(const mafVMESurface&); // Not implemented
  void operator=(const mafVMESurface&); // Not implemented
  
  /** private to avoid calling by external classes */
  virtual int SetData(vtkDataSet *data, mafTimeStamp t, int mode=MAF_VME_COPY_DATA);
};
#endif
