/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEOutputVTK.h,v $
  Language:  C++
  Date:      $Date: 2005-04-07 08:35:13 $
  Version:   $Revision: 1.2 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafVMEOutputVTK_h
#define __mafVMEOutputVTK_h

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
  mafTypeMacro(mafVMEOutputVTK,mafVMEOutput)

#ifdef MAF_USE_VTK
  /**
    Return a VTK dataset corresponding to the current time. This is
    the output of the DataPipe currently attached to the VME.
    Usually the output is a  "smart copy" of one of the dataset in 
    the DataArray. In some cases it can be NULL, e.g. in case the number
    of stored Items is 0. Also special VME could not support VTK dataset output.
    An event is rised when the output data changes to allow attached classes to 
    update their input.*/
  virtual vtkDataSet *GetVTKData();
#endif

  /**
    Update all the output data structures (data, bounds, matrix and abs matrix).*/
  //virtual void Update();
    
protected:
  mafVMEOutputVTK(); // to be allocated with New()
  virtual ~mafVMEOutputVTK(); // to be deleted with Delete()

private:
  mafVMEOutputVTK(const mafVMEOutputVTK&); // Not implemented
  void operator=(const mafVMEOutputVTK&); // Not implemented
};

#endif
