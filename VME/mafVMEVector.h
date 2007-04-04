/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEVector.h,v $
  Language:  C++
  Date:      $Date: 2007-04-04 13:52:47 $
  Version:   $Revision: 1.1 $
  Authors:   Roberto Mucci
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafVMEVector_h
#define __mafVMEVector_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafVMEPolyline.h"
#include "mafVMEOutputPolyline.h" //?
//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkDataSet;
class vtkPolyData;
class mmaMaterial;

/** mafVMEVector - a VME featuring an internal array for matrices and VTK datasets.
  mafVMEVector is a specialized VME inheriting the VME-Generic features to internally
  store data, and specialized for VTK data. This specialization consists in creating
  a specialized data pipe and to redefining some APIs for casting to concrete classes.
  @sa mafVME mafMatrixVector mafDataVector  */
class MAF_EXPORT mafVMEVector : public mafVMEPolyline
{
public:
  mafTypeMacro(mafVMEVector,mafVMEGeneric);

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

    /** return the right type of output */  
    mafVMEOutputPolyline *GetPolylineOutput() {return (mafVMEOutputPolyline *)GetOutput();}

    /** return the right type of output */  
    virtual mafVMEOutput *GetOutput();

    /** Return pointer to material attribute. */
    mmaMaterial *GetMaterial();

    /** Return the suggested pipe-typename for the visualization of this vme */
    virtual mafString GetVisualPipe() {return mafString("mafPipePolyline");};

protected:
  mafVMEVector();
  virtual ~mafVMEVector();

  /** used to initialize and create the material attribute if not yet present */
  virtual int InternalInitialize();

private:
  mafVMEVector(const mafVMEVector&); // Not implemented
  void operator=(const mafVMEVector&); // Not implemented
  
  /** private to avoid calling by external classes */
  virtual int SetData(vtkDataSet *data, mafTimeStamp t, int mode=MAF_VME_COPY_DATA);
};

#endif
