/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaDataPipeInterpolatorScalarMatrix
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


#include "albaDataPipeInterpolatorScalarMatrix.h"

#include "albaVME.h"
#include "albaVMEItemScalarMatrix.h"
#include "albaVMEScalarMatrix.h"
#include "albaEventBase.h"

#include <assert.h>

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaDataPipeInterpolatorScalarMatrix)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
albaDataPipeInterpolatorScalarMatrix::albaDataPipeInterpolatorScalarMatrix()
//------------------------------------------------------------------------------
{
}

//------------------------------------------------------------------------------
albaDataPipeInterpolatorScalarMatrix::~albaDataPipeInterpolatorScalarMatrix()
//------------------------------------------------------------------------------
{
} 

//------------------------------------------------------------------------------
bool albaDataPipeInterpolatorScalarMatrix::Accept(albaVME *vme)
//------------------------------------------------------------------------------
{
  return Superclass::Accept(vme) && vme->IsA(albaVMEScalarMatrix::GetStaticTypeId());
}

//------------------------------------------------------------------------------
vnl_matrix<double> &albaDataPipeInterpolatorScalarMatrix::GetScalarData()
//------------------------------------------------------------------------------
{
  OnEvent(&albaEventBase(this,VME_OUTPUT_DATA_PREUPDATE));
  return m_ScalarData;
}

//------------------------------------------------------------------------------
void albaDataPipeInterpolatorScalarMatrix::PreExecute()
//------------------------------------------------------------------------------
{
  Superclass::PreExecute();

  unsigned long mtime = this->GetMTime();

  // if the current item is changed set the data inside new item as input for the interpolator
  // more specialized interpolators could redefine this to have more inputs (e.g. when 
  // interpolating different items)
  if ( m_CurrentItem && (m_CurrentItem != m_OldItem || \
    mtime > m_UpdateTime.GetMTime() ||
    !m_CurrentItem->IsDataPresent() ))
  {
    vnl_matrix<double> scalar = GetCurrentItem()->GetData();
    if (scalar.size() != 0)
    {
      m_ScalarData = GetCurrentItem()->GetData();
      m_UpdateTime.Modified();
    }
  } 
}

//------------------------------------------------------------------------------
void albaDataPipeInterpolatorScalarMatrix::OnEvent(albaEventBase *e)
//------------------------------------------------------------------------------
{
  Superclass::OnEvent(e); // this also forwards the event to parent class
}
