/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEOutputVTK.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-07 08:35:13 $
  Version:   $Revision: 1.2 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafVMEOutputVTK.h"
#include "mafVME.h"
#include "mafDataPipe.h"
#include "mafSmartPointer.h"
#include "mafTransform.h"
#include "mafIndent.h"

#include <assert.h>

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMEOutputVTK)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMEOutputVTK::mafVMEOutputVTK()
//-------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
mafVMEOutputVTK::~mafVMEOutputVTK()
//-------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
vtkDataSet *mafVMEOutputVTK::GetVTKData()
//-------------------------------------------------------------------------
{
  assert(m_VME);
  return m_VME->GetDataPipe()->GetVTKData();
}

/*
//-------------------------------------------------------------------------
void mafVMEOutputVTK::Update()
//-------------------------------------------------------------------------
{
  
}*/