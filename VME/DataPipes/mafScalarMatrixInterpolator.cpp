/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafScalarMatrixInterpolator.cpp,v $
  Language:  C++
  Date:      $Date: 2007-12-11 11:22:24 $
  Version:   $Revision: 1.2 $
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


#include "mafScalarMatrixInterpolator.h"

#include "mafVME.h"
#include "mafVMEItemScalarMatrix.h"
#include "mafVMEScalarMatrix.h"
#include "mafEventBase.h"

#include <assert.h>

//------------------------------------------------------------------------------
mafCxxTypeMacro(mafScalarMatrixInterpolator)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafScalarMatrixInterpolator::mafScalarMatrixInterpolator()
//------------------------------------------------------------------------------
{
}

//------------------------------------------------------------------------------
mafScalarMatrixInterpolator::~mafScalarMatrixInterpolator()
//------------------------------------------------------------------------------
{
} 

//------------------------------------------------------------------------------
bool mafScalarMatrixInterpolator::Accept(mafVME *vme)
//------------------------------------------------------------------------------
{
  return Superclass::Accept(vme) && vme->IsA(mafVMEScalarMatrix::GetStaticTypeId());
}

//------------------------------------------------------------------------------
vnl_matrix<double> &mafScalarMatrixInterpolator::GetScalarData()
//------------------------------------------------------------------------------
{
  OnEvent(&mafEventBase(this,VME_OUTPUT_DATA_PREUPDATE));
  return m_ScalarData;
}

//------------------------------------------------------------------------------
void mafScalarMatrixInterpolator::PreExecute()
//------------------------------------------------------------------------------
{
  Superclass::PreExecute();

  unsigned long mtime = this->GetMTime();

  // if the current item is changed set the data inside new item as input for the interpolator
  // more specialized interpolators could redefine this to have more inputs (e.g. when 
  // interpolating different items)
  if ( m_CurrentItem && (m_CurrentItem != m_OldItem || \
      mtime > m_UpdateTime.GetMTime() ))
  {
    m_ScalarData = GetCurrentItem()->GetData();
    m_UpdateTime.Modified();
  } 
}

//------------------------------------------------------------------------------
void mafScalarMatrixInterpolator::OnEvent(mafEventBase *e)
//------------------------------------------------------------------------------
{
  Superclass::OnEvent(e); // this also forwards the event to parent class
}
