/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEGeneric
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

#include "albaVMEGeneric.h"
#include "albaDataVector.h"
#include "albaDataPipeInterpolatorVTK.h"
#include "albaVMEItemVTK.h"
#include "vtkDataSet.h"
//-------------------------------------------------------------------------
albaCxxTypeMacro(albaVMEGeneric)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
albaVMEGeneric::albaVMEGeneric()
//-------------------------------------------------------------------------
{
  m_DataVector = albaDataVector::New();
  m_DataVector->SetItemTypeName(albaVMEItemVTK::GetStaticTypeName());  
  m_DataVector->SetListener(this);
  SetDataPipe(albaDataPipeInterpolatorVTK::New()); // interpolator data pipe

  // The output is created on demand in GetOutput() to avoid
  // subclasses to have to destroy base class output
}

//-------------------------------------------------------------------------
albaVMEGeneric::~albaVMEGeneric()
//-------------------------------------------------------------------------
{
  // data pipe destroyed in albaVME
  // data vector destroyed in albaVMEGenericAbstract
}

//-------------------------------------------------------------------------
albaVMEOutput *albaVMEGeneric::GetOutput()
//-------------------------------------------------------------------------
{
  // allocate the right type of output on demand
  if (m_Output==NULL)
  {
    SetOutput(albaVMEOutputVTK::New()); // create the output
  }
  return m_Output;
}

//-------------------------------------------------------------------------
int albaVMEGeneric::SetData(vtkDataSet *data, albaTimeStamp t, int mode)
//-------------------------------------------------------------------------
{
  assert(data);
  albaSmartPointer<albaVMEItemVTK> item;
  
  switch (mode)
  {
  case ALBA_VME_COPY_DATA:
  {
    // update and make a copy of the VTK dataset
    vtkDataSet *new_data=data->NewInstance();
    new_data->DeepCopy(data);
    item->SetData(new_data);
    new_data->Delete();
  }
  break;
  case ALBA_VME_REFERENCE_DATA:
    // simply reference the VTK dataset (keep it attached to the source) 
    item->SetData(data);
  break;
  case ALBA_VME_DETACH_DATA:  
    // reference the dataset and detach it from its source

		//TODO VTK7 add a detach system
    item->SetData(data);
    //data->SetSource(NULL);
  break;
  }

  item->SetTimeStamp(t);
  GetDataVector()->InsertItem(item);

  Modified();
  //GetLogicManager()->VmeModified(this);

  return ALBA_OK;
}

//-------------------------------------------------------------------------
char** albaVMEGeneric::GetIcon() 
//-------------------------------------------------------------------------
{
  #include "albaVMEGeneric.xpm"
  return albaVMEGeneric_xpm;
}
