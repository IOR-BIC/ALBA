/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEVector
 Authors: Roberto Mucci
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaVMEVector_h
#define __albaVMEVector_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaVMEPolyline.h"
#include "albaVMEOutputPolyline.h" //?
//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkDataSet;
class vtkPolyData;
class mmaMaterial;

/** albaVMEVector - a VME featuring an internal array for matrices and VTK datasets.
  albaVMEVector is a specialized VME inheriting the VME-Generic features to internally
  store data, and specialized for VTK data. This specialization consists in creating
  a specialized data pipe and to redefining some APIs for casting to concrete classes.
  @sa albaVME albaMatrixVector albaDataVector  */
class ALBA_EXPORT albaVMEVector : public albaVMEPolyline
{
public:
  albaTypeMacro(albaVMEVector, albaVMEPolyline);

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
    virtual int SetData(vtkPolyData *data, albaTimeStamp t, int mode=ALBA_VME_COPY_DATA);

    /** Return the module of the vector.*/
    double GetModule();

    /** Return the suggested pipe-typename for the visualization of this vme */
    virtual albaString GetVisualPipe() {return albaString("albaPipeVector");};

    /** return icon */
    static char** GetIcon();

protected:
  albaVMEVector();
  virtual ~albaVMEVector();

private:
  albaVMEVector(const albaVMEVector&); // Not implemented
  void operator=(const albaVMEVector&); // Not implemented
  
  /** private to avoid calling by external classes */
  virtual int SetData(vtkDataSet *data, albaTimeStamp t, int mode=ALBA_VME_COPY_DATA);
};
#endif
