/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEGeneric.h,v $
  Language:  C++
  Date:      $Date: 2005-04-19 08:27:00 $
  Version:   $Revision: 1.8 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafVMEGeneric_h
#define __mafVMEGeneric_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafVMEGenericAbstract.h"
#include "mafVMEOutputVTK.h"
//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkDataSet;

/** mafVMEGeneric - a VME featuring an internal array for matrices and VTK datasets.
  mafVMEGeneric is a specialized VME inheriting the VME-Generic features to internally
  store data, and specialized for VTK data. This specialization consists in creating
  a specialized data pipe and to redefining some APIs for casting to concrete classes.
  @sa mafVME mafMatrixVector mafDataVector  */
class MAF_EXPORT mafVMEGeneric : public mafVMEGenericAbstract
{
public:
  mafTypeMacro(mafVMEGeneric,mafVMEGenericAbstract);

  enum DATA_REFERENCE_TYPE {MAF_VME_COPY_DATA,MAF_VME_REFERENCE_DATA,MAF_VME_DETACH_DATA};

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
  virtual int SetData(vtkDataSet *data, mafTimeStamp t, int mode=MAF_VME_COPY_DATA);

  /** Set data by referencing it. See SetData() for more details. */
  int SetDataByReference(vtkDataSet *data, mafTimeStamp t) {return SetData(data,t,MAF_VME_REFERENCE_DATA);}
  
  /** Set data by detaching it from its original source. See SetData() for more details. */
  int SetDataByDetaching(vtkDataSet *data, mafTimeStamp t) {return SetData(data,t,MAF_VME_REFERENCE_DATA);}

  /** return the right type of output.*/  
  mafVMEOutputVTK *GetVTKOutput() {return (mafVMEOutputVTK *)GetOutput();}

  /** 
    Return the output. This create the output object on demand. */  
  virtual mafVMEOutput *GetOutput();

  /** return icon */
  static char** GetIcon();
  
protected:
  mafVMEGeneric();
  virtual ~mafVMEGeneric();

private:
  mafVMEGeneric(const mafVMEGeneric&); // Not implemented
  void operator=(const mafVMEGeneric&); // Not implemented
};

#endif
