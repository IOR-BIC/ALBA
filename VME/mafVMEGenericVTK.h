/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEGenericVTK.h,v $
  Language:  C++
  Date:      $Date: 2005-04-07 20:44:59 $
  Version:   $Revision: 1.2 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafVMEGenericVTK_h
#define __mafVMEGenericVTK_h

#include "mafVMEGeneric.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkDataSet;

/** mafVMEGenericVTK - a VME featuring an internal array for matrices and VTK datasets.
  mafVMEGenericVTK is a specialized VME inheriting the VME-Generic features to internally
  store data, and specialized for VTK data. This specialization consists in creating
  a specialized data pipe and to redefining some APIs for casting to concrete classes.
  @sa mafVME mafMatrixVector mafDataVector

  @todo
  - 
  */
class MAF_EXPORT mafVMEGenericVTK : public mafVMEGeneric
{
public:
  mafTypeMacro(mafVMEGenericVTK,mafVMEGeneric);

  enum DATA_REFERENCE_TYPE {MAF_VME_COPY_DATA,MAF_VME_REFERENCE_DATA,MAF_VME_DETACH_DATA};

  /** 
    Set data for the give timestamp. This function automatically creates a
    a VMEIteam for the data to be stored. The optional parameter specify if
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
  
protected:
  mafVMEGenericVTK();
  virtual ~mafVMEGenericVTK();

private:
  mafVMEGenericVTK(const mafVMEGenericVTK&); // Not implemented
  void operator=(const mafVMEGenericVTK&); // Not implemented
};

#endif
