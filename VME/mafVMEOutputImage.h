/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEOutputImage.h,v $
  Language:  C++
  Date:      $Date: 2010-07-08 15:42:05 $
  Version:   $Revision: 1.2.22.4 $
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
class mmaMaterial;

/** 
Class Name: mafVMEOutputImage.
NULL output for VME node with a VTK image output data. mafVMEOutputImage is the output produced by a node generating an output
  with a VTK dataset.
*/
class MAF_EXPORT mafVMEOutputImage : public mafVMEOutputVTK
{
public:

   /** constructor. */
  mafVMEOutputImage();
  
   /** destructor. */
  virtual ~mafVMEOutputImage();

  /** RTTI macro.*/
  mafTypeMacro(mafVMEOutputImage,mafVMEOutputVTK);

  /**
    Return a VTK dataset corresponding to the current time. This is
    the output of the DataPipe currently attached to the VME.
    Usually the output is a  "smart copy" of one of the dataset in 
    the DataArray. In some cases it can be NULL, e.g. in case the number
    of stored Items is 0. Also special VME could not support VTK dataset output.
    An event is rised when the output data changes to allow attached classes to 
    update their input.*/
  virtual vtkImageData *GetImageData();

  /** return material attribute*/
  mmaMaterial *GetMaterial();

  /** set the material of the output image data*/ 
  void SetMaterial(mmaMaterial *material);

protected:
  mmaMaterial *m_Material;

private:
  mafVMEOutputImage(const mafVMEOutputImage&); // Not implemented
  void operator=(const mafVMEOutputImage&); // Not implemented
};

#endif
