/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEGenericVTK.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-01 10:16:32 $
  Version:   $Revision: 1.1 $
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
void mafVMEGenericVTK::SetData(vtkDataSet *data, mafTimeStamp t)
//-------------------------------------------------------------------------
{
  assert(data);
  mafSmartPointer<mafVMEItemVTK> item;
  item->SetData(data);
  item->SetTimeStamp(t);
  GetDataVector()->InsertItem(item);
}

