/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEOutputVTK
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



#include "albaVMEOutputVTK.h"
#include "albaVME.h"
#include "albaDataPipe.h"
#include "albaSmartPointer.h"
#include "albaTransform.h"
#include "albaIndent.h"

#include <assert.h>

//-------------------------------------------------------------------------
albaCxxTypeMacro(albaVMEOutputVTK)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
albaVMEOutputVTK::albaVMEOutputVTK()
//-------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
albaVMEOutputVTK::~albaVMEOutputVTK()
//-------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
vtkDataSet *albaVMEOutputVTK::GetVTKData()
//-------------------------------------------------------------------------
{
  assert(m_VME);
  return m_VME->GetDataPipe()->GetVTKData();
}

/*
//-------------------------------------------------------------------------
void albaVMEOutputVTK::Update()
//-------------------------------------------------------------------------
{
  
}*/