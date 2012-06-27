/*=========================================================================

 Program: MAF2
 Module: mafDataPipeInterpolatorScalarMatrix
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


#include "mafDataPipeInterpolatorScalarMatrix.h"

#include "mafVME.h"
#include "mafVMEItemScalarMatrix.h"
#include "mafVMEScalarMatrix.h"
#include "mafEventBase.h"

#include <assert.h>

//------------------------------------------------------------------------------
mafCxxTypeMacro(mafDataPipeInterpolatorScalarMatrix)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafDataPipeInterpolatorScalarMatrix::mafDataPipeInterpolatorScalarMatrix()
//------------------------------------------------------------------------------
{
}

//------------------------------------------------------------------------------
mafDataPipeInterpolatorScalarMatrix::~mafDataPipeInterpolatorScalarMatrix()
//------------------------------------------------------------------------------
{
} 

//------------------------------------------------------------------------------
bool mafDataPipeInterpolatorScalarMatrix::Accept(mafVME *vme)
//------------------------------------------------------------------------------
{
  return Superclass::Accept(vme) && vme->IsA(mafVMEScalarMatrix::GetStaticTypeId());
}

//------------------------------------------------------------------------------
vnl_matrix<double> &mafDataPipeInterpolatorScalarMatrix::GetScalarData()
//------------------------------------------------------------------------------
{
  OnEvent(&mafEventBase(this,VME_OUTPUT_DATA_PREUPDATE));
  return m_ScalarData;
}

//------------------------------------------------------------------------------
void mafDataPipeInterpolatorScalarMatrix::PreExecute()
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
void mafDataPipeInterpolatorScalarMatrix::OnEvent(mafEventBase *e)
//------------------------------------------------------------------------------
{
  Superclass::OnEvent(e); // this also forwards the event to parent class
}
