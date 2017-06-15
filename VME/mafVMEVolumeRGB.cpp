/*=========================================================================

 Program: MAF2
 Module: mafVMEVolumeRGB
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafVMEVolumeRGB.h"
#include "vtkImageData.h"
#include "vtkRectilinearGrid.h"
#include "vtkPointData.h"
#include "vtkDataSet.h"
#include "vtkDataSetAttributes.h"
#include "vtkDataArray.h"

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMEVolumeRGB)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMEVolumeRGB::mafVMEVolumeRGB()
//-------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
mafVMEVolumeRGB::~mafVMEVolumeRGB()
//-------------------------------------------------------------------------
{
  // data pipe destroyed in mafVME
  // data vector destroyed in mafVMEGeneric
}


//-------------------------------------------------------------------------
mafVMEOutput *mafVMEVolumeRGB::GetOutput()
//-------------------------------------------------------------------------
{
  // allocate the right type of output on demand
  if (m_Output==NULL)
  {
    SetOutput(mafVMEOutputVolume::New()); // create the output
  }
  return m_Output;
}

//-------------------------------------------------------------------------
int mafVMEVolumeRGB::SetData(vtkRectilinearGrid *data, mafTimeStamp t, int mode)
//-------------------------------------------------------------------------
{
  if (data->GetPointData()&&data->GetPointData()->GetNumberOfComponents()==3)
    return Superclass::SetData(data,t,mode);

  mafErrorMacro("Trying to set the wrong type of data inside a "<<(const char *)GetTypeName()<<" :"<< (data?data->GetClassName():"NULL"));
  return MAF_ERROR;
}

//-------------------------------------------------------------------------
int mafVMEVolumeRGB::SetData(vtkImageData *data, mafTimeStamp t, int mode)
//-------------------------------------------------------------------------
{
  if (data->GetPointData()&&data->GetPointData()->GetNumberOfComponents()==3)
    return Superclass::SetData(data,t,mode);

  mafErrorMacro("Trying to set the wrong type of data inside a "<<(const char *)GetTypeName()<<" :"<< (data?data->GetClassName():"NULL"));
  return MAF_ERROR;
}
//-------------------------------------------------------------------------
int mafVMEVolumeRGB::SetData(vtkDataSet *data, mafTimeStamp t, int mode)
//-------------------------------------------------------------------------
{
  assert(data);
  if (data->IsA("vtkImageData")||data->IsA("vtkRectilinearGrid"))
  {
    if (data->GetPointData()&&data->GetPointData()->GetScalars() != NULL &&
      data->GetPointData()->GetScalars()->GetNumberOfComponents()==3)
      return Superclass::SetData(data,t,mode);
  }
  
  mafErrorMacro("Trying to set the wrong type of data inside a "<<(const char *)GetTypeName()<<" :"<< (data?data->GetClassName():"NULL"));
  return MAF_ERROR;
}

