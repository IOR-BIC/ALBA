/*=========================================================================

 Program: MAF2
 Module: mafVMEOutputVTK
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafVMEOutputVTK_h
#define __mafVMEOutputVTK_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafVMEOutput.h"
//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafVMEGerericVTK;

/** NULL output for VME node with a VTK output data.
  mafVMEOutputVTK is the output produced by a node generating an output
  with a VTK dataset.
*/
class MAF_EXPORT mafVMEOutputVTK : public mafVMEOutput
{
public:
  /** RTTI Macro. */
  mafTypeMacro(mafVMEOutputVTK,mafVMEOutput);

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
  mafVMEOutputVTK(); 

  /** destructor protected method. To be deleted with delete(). */
  virtual ~mafVMEOutputVTK(); 

private:
  /** constructor: not implemented. */
  mafVMEOutputVTK(const mafVMEOutputVTK&); 

  /** Overloaded = operator: not implemented. */
  void operator=(const mafVMEOutputVTK&); 
};

#endif
