/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEGeneric.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-12 19:34:38 $
  Version:   $Revision: 1.5 $
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



#include "mafVMEGeneric.h"
#include "mafDataVector.h"
#include "mafVTKInterpolator.h"
#include "mafVMEItemVTK.h"
#include "mafVMEOutputVTK.h"
#include "mafAbsMatrixPipe.h"
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
  SetDataPipe(mafVTKInterpolator::New()); // interpolator data pipe

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
    data->Update();
    vtkDataSet *new_data=data->NewInstance();
    new_data->DeepCopy(data);
    item->SetData(new_data);
  }
  break;
  case MAF_VME_REFERENCE_DATA:
    item->SetData(data);
  break;
  case MAF_VME_DETACH_DATA:  
    item->SetData(data);
    data->SetSource(NULL);
  break;
  }

  item->SetTimeStamp(t);
  GetDataVector()->InsertItem(item);

  return MAF_OK;
}

