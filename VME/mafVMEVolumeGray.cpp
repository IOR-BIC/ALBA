/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEVolumeGray.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-21 14:05:14 $
  Version:   $Revision: 1.1 $
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

#include "mafVMEVolumeGray.h"
#include "vtkImageData.h"
#include "vtkRectilinearGrid.h"
#include "vtkPointData.h"

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMEVolumeGray)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMEVolumeGray::mafVMEVolumeGray()
//-------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
mafVMEVolumeGray::~mafVMEVolumeGray()
//-------------------------------------------------------------------------
{
  // data pipe destroyed in mafVME
  // data vector destroyed in mafVMEGeneric
}


//-------------------------------------------------------------------------
mafVMEOutput *mafVMEVolumeGray::GetOutput()
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
int mafVMEVolumeGray::SetData(vtkRectilinearGrid *data, mafTimeStamp t, int mode)
//-------------------------------------------------------------------------
{
  return Superclass::SetData(data,t,mode);
}

//-------------------------------------------------------------------------
int mafVMEVolumeGray::SetData(vtkImageData *data, mafTimeStamp t, int mode)
//-------------------------------------------------------------------------
{
  return Superclass::SetData(data,t,mode);
}
//-------------------------------------------------------------------------
int mafVMEVolumeGray::SetData(vtkDataSet *data, mafTimeStamp t, int mode)
//-------------------------------------------------------------------------
{
  assert(data);
  if (data->IsA("vtkImageData")||data->IsA("vtkRectilinearGrid"))
  {
    data->UpdateInformation();
    if (data->GetPointData()&&data->GetPointData()->GetNumberOfComponents()==1)
      return Superclass::SetData(data,t,mode);
  }
  
  mafErrorMacro("Trying to set the wrong type of fata inside a "<<(const char *)GetTypeName()<<" :"<< (data?data->GetClassName():"NULL"));
  return MAF_ERROR;
}
