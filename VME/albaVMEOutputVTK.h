/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEOutputVTK
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaVMEOutputVTK_h
#define __albaVMEOutputVTK_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaVMEOutput.h"
//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class albaVMEGerericVTK;

/** NULL output for VME node with a VTK output data.
  albaVMEOutputVTK is the output produced by a node generating an output
  with a VTK dataset.
*/
class ALBA_EXPORT albaVMEOutputVTK : public albaVMEOutput
{
public:
  /** RTTI Macro. */
  albaTypeMacro(albaVMEOutputVTK,albaVMEOutput);

  /**
    Return a VTK dataset corresponding to the current time. This is
    the output of the DataPipe currently attached to the VME.
    Usually the output is a  "smart copy" of one of the dataset in 
    the DataArray. In some cases it can be NULL, e.g. in case the number
    of stored Items is 0. Also special VME could not support VTK dataset output.
    An event is rised when the output data changes to allow attached classes to 
    update their input.*/
  virtual vtkDataSet *GetVTKData();

  /**
    Update all the output data structures (data, bounds, matrix and abs matrix).*/
  //virtual void Update();
    
protected:
  /** constructor: protected method. To be allocated with New(). */
  albaVMEOutputVTK(); 

  /** destructor protected method. To be deleted with delete(). */
  virtual ~albaVMEOutputVTK(); 

private:
  /** constructor: not implemented. */
  albaVMEOutputVTK(const albaVMEOutputVTK&); 

  /** Overloaded = operator: not implemented. */
  void operator=(const albaVMEOutputVTK&); 
};

#endif
