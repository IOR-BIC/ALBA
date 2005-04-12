/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEOutputImage.h,v $
  Language:  C++
  Date:      $Date: 2005-04-12 19:31:17 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafVMEOutputImage_h
#define __mafVMEOutputImage_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafVMEOutputVTK.h"
//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafVMEGerericVTK;
class vtkImageData;

/** NULL output for VME node with a VTK image output data.
  mafVMEOutputImage is the output produced by a node generating an output
  with a VTK dataset.
*/
class MAF_EXPORT mafVMEOutputImage : public mafVMEOutputVTK
{
public:
  mafVMEOutputImage();
  virtual ~mafVMEOutputImage();

  mafTypeMacro(mafVMEOutputImage,mafVMEOutputVTK)

  /**
    Return a VTK dataset corresponding to the current time. This is
    the output of the DataPipe currently attached to the VME.
    Usually the output is a  "smart copy" of one of the dataset in 
    the DataArray. In some cases it can be NULL, e.g. in case the number
    of stored Items is 0. Also special VME could not support VTK dataset output.
    An event is rised when the output data changes to allow attached classes to 
    update their input.*/
  virtual vtkImageData *GetImageData();

private:
  mafVMEOutputImage(const mafVMEOutputImage&); // Not implemented
  void operator=(const mafVMEOutputImage&); // Not implemented
};

#endif
