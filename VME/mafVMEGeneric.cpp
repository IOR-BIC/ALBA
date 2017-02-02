/*=========================================================================

 Program: MAF2
 Module: mafVMEGeneric
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

#include "mafVMEGeneric.h"
#include "mafDataVector.h"
#include "mafDataPipeInterpolatorVTK.h"
#include "mafVMEItemVTK.h"
#include "vtkDataSet.h"
//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMEGeneric)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMEGeneric::mafVMEGeneric()
//-------------------------------------------------------------------------
{
  m_DataVector = mafDataVector::New();
  m_DataVector->SetItemTypeName(mafVMEItemVTK::GetStaticTypeName());  
  m_DataVector->SetListener(this);
  SetDataPipe(mafDataPipeInterpolatorVTK::New()); // interpolator data pipe

  // The output is created on demand in GetOutput() to avoid
  // subclasses to have to destroy base class output
}

//-------------------------------------------------------------------------
mafVMEGeneric::~mafVMEGeneric()
//-------------------------------------------------------------------------
{
  // data pipe destroyed in mafVME
  // data vector destroyed in mafVMEGenericAbstract
}

//-------------------------------------------------------------------------
mafVMEOutput *mafVMEGeneric::GetOutput()
//-------------------------------------------------------------------------
{
  // allocate the right type of output on demand
  if (m_Output==NULL)
  {
    SetOutput(mafVMEOutputVTK::New()); // create the output
  }
  return m_Output;
}

//-------------------------------------------------------------------------
int mafVMEGeneric::SetData(vtkDataSet *data, mafTimeStamp t, int mode)
//-------------------------------------------------------------------------
{
  assert(data);
  mafSmartPointer<mafVMEItemVTK> item;
  
  switch (mode)
  {
  case MAF_VME_COPY_DATA:
  {
    // update and make a copy of the VTK dataset
    data->Update();
    vtkDataSet *new_data=data->NewInstance();
    new_data->DeepCopy(data);
    item->SetData(new_data);
    new_data->Delete();
  }
  break;
  case MAF_VME_REFERENCE_DATA:
    // simply reference the VTK dataset (keep it attached to the source) 
    item->SetData(data);
  break;
  case MAF_VME_DETACH_DATA:  
    // reference the dataset and detach it from its source
    item->SetData(data);
    data->SetSource(NULL);
  break;
  }

  item->SetTimeStamp(t);
  GetDataVector()->InsertItem(item);

  Modified();
  //GetLogicManager()->VmeModified(this);

  return MAF_OK;
}

//-------------------------------------------------------------------------
char** mafVMEGeneric::GetIcon() 
//-------------------------------------------------------------------------
{
  #include "mafVMEGeneric.xpm"
  return mafVMEGeneric_xpm;
}
