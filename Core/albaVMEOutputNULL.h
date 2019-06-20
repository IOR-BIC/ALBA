/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEOutputNULL
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaVMEOutputNULL_h
#define __albaVMEOutputNULL_h
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "albaVMEOutput.h"
//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class albaVMERoot;

/** NULL output for VME node without output data (only pose).
  albaVMEOutputNULL is the output produced by a VME node which does not produce any
  output data, only the pose matrix.
*/
class ALBA_EXPORT albaVMEOutputNULL : public albaVMEOutput
{
public:
  albaTypeMacro(albaVMEOutputNULL,albaVMEOutput)

#ifdef ALBA_USE_VTK
  /**
    Return a VTK dataset corresponding to the current time. This is
    the output of the DataPipe currently attached to the VME.
    Usually the output is a  "smart copy" of one of the dataset in 
    the DataArray. In some cases it can be NULL, e.g. in case the number
    of stored Items is 0. Also special VME could not support VTK dataset output.
    An event is rised when the output data changes to allow attached classes to 
    update their input.*/
  virtual vtkDataSet *GetVTKData() {return NULL;}
#endif

  /**
    Update all the output data structures (data, bounds, matrix and abs matrix).*/
  virtual void Update() {}
    
protected:
  albaVMEOutputNULL(); // to be allocated with New()
  virtual ~albaVMEOutputNULL(); // to be deleted with Delete()

private:
  albaVMEOutputNULL(const albaVMEOutputNULL&); // Not implemented
  void operator=(const albaVMEOutputNULL&); // Not implemented
};

#endif
