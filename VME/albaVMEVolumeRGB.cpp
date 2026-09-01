/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEVolumeRGB
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "albaVMEVolumeRGB.h"
#include "vtkImageData.h"
#include "vtkRectilinearGrid.h"
#include "vtkPointData.h"
#include "vtkDataArray.h"

//-------------------------------------------------------------------------
albaCxxTypeMacro(albaVMEVolumeRGB)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
albaVMEVolumeRGB::albaVMEVolumeRGB()
//-------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
albaVMEVolumeRGB::~albaVMEVolumeRGB()
//-------------------------------------------------------------------------
{
  // data pipe destroyed in albaVME
  // data vector destroyed in albaVMEGeneric
}


//-------------------------------------------------------------------------
albaVMEOutput *albaVMEVolumeRGB::GetOutput()
//-------------------------------------------------------------------------
{
  // allocate the right type of output on demand
  if (m_Output==NULL)
  {
    SetOutput(albaVMEOutputVolume::New()); // create the output
  }
  return m_Output;
}

//-------------------------------------------------------------------------
int albaVMEVolumeRGB::SetData(vtkRectilinearGrid *data, albaTimeStamp t, int mode)
//-------------------------------------------------------------------------
{
  if (data->GetPointData()&&data->GetPointData()->GetNumberOfComponents()==3)
    return Superclass::SetData(data,t,mode);

  albaErrorMacro("Trying to set the wrong type of data inside a "<<(const char *)GetTypeName()<<" :"<< (data?data->GetClassName():"NULL"));
  return ALBA_ERROR;
}

//-------------------------------------------------------------------------
int albaVMEVolumeRGB::SetData(vtkImageData *data, albaTimeStamp t, int mode)
//-------------------------------------------------------------------------
{
  if (data->GetPointData()&&data->GetPointData()->GetNumberOfComponents()==3)
    return Superclass::SetData(data,t,mode);

  albaErrorMacro("Trying to set the wrong type of data inside a "<<(const char *)GetTypeName()<<" :"<< (data?data->GetClassName():"NULL"));
  return ALBA_ERROR;
}
//-------------------------------------------------------------------------
int albaVMEVolumeRGB::SetData(vtkDataSet *data, albaTimeStamp t, int mode)
//-------------------------------------------------------------------------
{
  assert(data);
  if (data->IsA("vtkImageData")||data->IsA("vtkRectilinearGrid"))
  {
    if (data->GetPointData()&&data->GetPointData()->GetScalars() != NULL &&
      data->GetPointData()->GetScalars()->GetNumberOfComponents()==3)
      return Superclass::SetData(data,t,mode);
  }
  
  albaErrorMacro("Trying to set the wrong type of data inside a "<<(const char *)GetTypeName()<<" :"<< (data?data->GetClassName():"NULL"));
  return ALBA_ERROR;
}

