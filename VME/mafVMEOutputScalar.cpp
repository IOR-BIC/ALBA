/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEOutputScalar.cpp,v $
  Language:  C++
  Date:      $Date: 2006-06-23 08:09:32 $
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



#include "mafVMEOutputScalar.h"
#include "mmgGui.h"

#include "mafVMEScalar.h"
#include "mafVMEItemScalar.h"
#include "mafScalarInterpolator.h"
#include "mafIndent.h"

#include <assert.h>

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMEOutputScalar)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMEOutputScalar::mafVMEOutputScalar()
//-------------------------------------------------------------------------
{
  m_NumberOfRows = "0";
  m_NumberOfColumns = "0";
}

//-------------------------------------------------------------------------
mafVMEOutputScalar::~mafVMEOutputScalar()
//-------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
vnl_matrix<double> &mafVMEOutputScalar::GetScalarData()
//-------------------------------------------------------------------------
{
  assert(m_VME);
  return ((mafScalarInterpolator *)m_VME->GetDataPipe())->GetScalarData();
}
//-------------------------------------------------------------------------
mmgGui* mafVMEOutputScalar::CreateGui()
//-------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = mafVMEOutput::CreateGui();

  if (m_VME && m_VME->GetDataPipe() && m_VME->GetDataPipe())
  {
    this->Update();
  }
  vnl_matrix<double> data = GetScalarData();
  m_NumberOfRows = "";
  m_NumberOfRows << (int)data.rows();
  m_NumberOfColumns = "";
  m_NumberOfColumns << (int)data.columns();
  m_Gui->Label("rows: ",&m_NumberOfRows);
  m_Gui->Label("columns: ",&m_NumberOfColumns);
  return m_Gui;
}

//-------------------------------------------------------------------------
void mafVMEOutputScalar::Update()
//-------------------------------------------------------------------------
{
  vnl_matrix<double> data = GetScalarData();
  m_NumberOfRows = "";
  m_NumberOfRows << (int)data.rows();
  m_NumberOfColumns = "";
  m_NumberOfColumns << (int)data.columns();
  if (m_Gui)
  {
    m_Gui->Update();
  }
}
