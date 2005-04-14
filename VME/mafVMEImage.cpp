/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEImage.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-14 18:16:48 $
  Version:   $Revision: 1.2 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------



#include "mafVMEImage.h"
#include "mafMatrixInterpolator.h"
#include "mafDataVector.h"
#include "mafVTKInterpolator.h"
#include "mafVMEItemVTK.h"
#include "mafAbsMatrixPipe.h"
#include "vtkDataSet.h"
#include "vtkImageData.h"
//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMEImage)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMEImage::mafVMEImage()
//-------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
mafVMEImage::~mafVMEImage()
//-------------------------------------------------------------------------
{
  // data pipe destroyed in mafVME
  // data vector destroyed in mafVMEGeneric
}


//-------------------------------------------------------------------------
mafVMEOutput *mafVMEImage::GetOutput()
//-------------------------------------------------------------------------
{
  // allocate the right type of output on demand
  if (m_Output==NULL)
  {
    SetOutput(mafVMEOutputImage::New()); // create the output
  }
  return m_Output;
}

//-------------------------------------------------------------------------
int mafVMEImage::SetData(vtkImageData *data, mafTimeStamp t, int mode)
//-------------------------------------------------------------------------
{
  return Superclass::SetData(data,t,mode);
}
//-------------------------------------------------------------------------
int mafVMEImage::SetData(vtkDataSet *data, mafTimeStamp t, int mode)
//-------------------------------------------------------------------------
{
  assert(data);
  if (data->IsA("vtkImageData"))
  {
    return Superclass::SetData(data,t,mode);
  }
  
  mafErrorMacro("Trying to set the wrong type of fata inside a VME Image :"<< (data?data->GetClassName():"NULL"));
  return MAF_ERROR;
}

