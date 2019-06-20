/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEOutputImage
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaVMEOutputImage_h
#define __albaVMEOutputImage_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaVMEOutputVTK.h"
//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class albaVMEGerericVTK;
class vtkImageData;
class mmaMaterial;

/** 
Class Name: albaVMEOutputImage.
NULL output for VME node with a VTK image output data. albaVMEOutputImage is the output produced by a node generating an output
  with a VTK dataset.
*/
class ALBA_EXPORT albaVMEOutputImage : public albaVMEOutputVTK
{
public:

   /** constructor. */
  albaVMEOutputImage();
  
   /** destructor. */
  virtual ~albaVMEOutputImage();

  /** RTTI macro.*/
  albaTypeMacro(albaVMEOutputImage,albaVMEOutputVTK);

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

  albaGUI *CreateGui();


private:
  albaVMEOutputImage(const albaVMEOutputImage&); // Not implemented
  void operator=(const albaVMEOutputImage&); // Not implemented
};

#endif
