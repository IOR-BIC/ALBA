/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEOutputSurface.h,v $
  Language:  C++
  Date:      $Date: 2005-04-12 19:31:17 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafVMEOutputSurface_h
#define __mafVMEOutputSurface_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafVMEOutputVTK.h"
//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafVMEGerericVTK;
class vtkPolyData;

/** NULL output for VME node with a VTK image output data.
  mafVMEOutputSurface is the output produced by a node generating an output
  with a VTK dataset.
*/
class MAF_EXPORT mafVMEOutputSurface : public mafVMEOutputVTK
{
public:
  mafVMEOutputSurface();
  virtual ~mafVMEOutputSurface();

  mafTypeMacro(mafVMEOutputSurface,mafVMEOutputVTK)

  /**
    Return a VTK dataset corresponding to the current time. This is
    the output of the DataPipe currently attached to the VME.
    Usually the output is a  "smart copy" of one of the dataset in 
    the DataArray. In some cases it can be NULL, e.g. in case the number
    of stored Items is 0. Also special VME could not support VTK dataset output.
    An event is rised when the output data changes to allow attached classes to 
    update their input.*/
  virtual vtkPolyData *GetSurfaceData();

private:
  mafVMEOutputSurface(const mafVMEOutputSurface&); // Not implemented
  void operator=(const mafVMEOutputSurface&); // Not implemented
};

#endif
