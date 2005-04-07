/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEGenericVTK.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-07 20:44:59 $
  Version:   $Revision: 1.2 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafVMEGenericVTK.h"
#include "mafMatrixInterpolator.h"
#include "mafDataVector.h"
#include "mafVTKInterpolator.h"
#include "mafVMEItemVTK.h"
#include "mafVMEOutputVTK.h"
#include "mafAbsMatrixPipe.h"
#include "vtkDataSet.h"

#include "vtkPolyData.h"
#include "vtkPoints.h"

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMEGenericVTK)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMEGenericVTK::mafVMEGenericVTK()
//-------------------------------------------------------------------------
{
  m_Output = mafVMEOutputVTK::New();
  m_Output->SetVME(this);
  m_DataVector = mafDataVector::New();
  m_DataVector->SetItemTypeName(mafVMEItemVTK::GetStaticTypeName());
  SetDataPipe(mafVTKInterpolator::New()); // interpolator data pipe
  SetMatrixPipe(mafMatrixInterpolator::New()); // interpolator matrix pipe
  // force abs matrix pipe to update its inputs
  m_AbsMatrixPipe->SetVME(this); 
}

//-------------------------------------------------------------------------
mafVMEGenericVTK::~mafVMEGenericVTK()
//-------------------------------------------------------------------------
{
  // data pipe destroyed in mafVME
  // data vector destroyed in mafVMEGeneric
}

//-------------------------------------------------------------------------
int mafVMEGenericVTK::SetData(vtkDataSet *data, mafTimeStamp t, int mode)
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

