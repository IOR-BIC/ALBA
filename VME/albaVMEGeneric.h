/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEGeneric
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaVMEGeneric_h
#define __albaVMEGeneric_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaVMEGenericAbstract.h"
#include "albaVMEOutputVTK.h"
//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkDataSet;

/** albaVMEGeneric - a VME featuring an internal array for matrices and VTK datasets.
  albaVMEGeneric is a specialized VME inheriting the VME-Generic features to internally
  store data, and specialized for VTK data. This specialization consists in creating
  a specialized data pipe and to redefining some APIs for casting to concrete classes.
  @sa albaVME albaMatrixVector albaDataVector  */
class ALBA_EXPORT albaVMEGeneric : public albaVMEGenericAbstract
{
public:
  albaTypeMacro(albaVMEGeneric,albaVMEGenericAbstract);

  enum DATA_REFERENCE_TYPE {ALBA_VME_COPY_DATA,ALBA_VME_REFERENCE_DATA,ALBA_VME_DETACH_DATA};

  /** 
    Set data for the give timestamp. This function automatically creates a
    a VMEItem for the data to be stored. The optional parameter specify if
    data must be copied, referenced, or detached from original source. Default
    is to copy data which creates a new data set copying the original one.
    Referencing means the data is kept connected to its original source, and
    detaching means disconnect is like referencing but also disconnect the data
    from the source.
    Return ALBA_OK if succeeded, ALBA_ERROR if they kind of data is not accepted by
    this type of VME. */
  virtual int SetData(vtkDataSet *data, albaTimeStamp t, int mode=ALBA_VME_COPY_DATA);

  /** Set data by referencing it. See SetData() for more details. */
  int SetDataByReference(vtkDataSet *data, albaTimeStamp t) {return SetData(data,t,ALBA_VME_REFERENCE_DATA);}
  
  /** Set data by detaching it from its original source. See SetData() for more details. */
  int SetDataByDetaching(vtkDataSet *data, albaTimeStamp t) {return SetData(data,t,ALBA_VME_DETACH_DATA);}

  /** return the right type of output.*/  
  albaVMEOutputVTK *GetVTKOutput() {return (albaVMEOutputVTK *)GetOutput();}

  /** 
    Return the output. This create the output object on demand. */  
  virtual albaVMEOutput *GetOutput();

  /** return icon */
  static char** GetIcon();
  
  /** Return the suggested pipe-typename for the visualization of this vme */
  virtual albaString GetVisualPipe() {return albaString("albaPipeBox");};

protected:
  albaVMEGeneric();
  virtual ~albaVMEGeneric();

private:
  albaVMEGeneric(const albaVMEGeneric&); // Not implemented
  void operator=(const albaVMEGeneric&); // Not implemented
};

#endif
