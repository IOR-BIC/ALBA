/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEGenericVTK.h,v $
  Language:  C++
  Date:      $Date: 2005-04-01 10:16:32 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafVMEGenericVTK_h
#define __mafVMEGenericVTK_h

#include "mafVMEGeneric.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkDataSet;

/** mafVMEGenericVTK - a VME featuring an internal array for matrices and VTK datasets.
  mafVMEGenericVTK is a specialized VME inheriting the VME-Generic features to internally
  store data, and specialized for VTK data. This specialization consists in creating
  a specialized data pipe and to redefining some APIs for casting to concrete classes.
  @sa mafVME mafMatrixVector mafDataVector

  @todo
  - 
  */
class MAF_EXPORT mafVMEGenericVTK : public mafVMEGeneric
{
public:
  mafTypeMacro(mafVMEGenericVTK,mafVMEGeneric);

  /** 
    Set data for the give timestamp. This function automatically creates a
    a VMEIteam for the data to be stored.
    */
  void SetData(vtkDataSet *data, mafTimeStamp t);

protected:
  mafVMEGenericVTK();
  virtual ~mafVMEGenericVTK();

private:
  mafVMEGenericVTK(const mafVMEGenericVTK&); // Not implemented
  void operator=(const mafVMEGenericVTK&); // Not implemented
};

#endif
