/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEOutputPointSet.cpp,v $
  Language:  C++
  Date:      $Date: 2005-05-04 11:47:59 $
  Version:   $Revision: 1.1 $
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

#include "mafVMEOutputPointSet.h"
#include "mafIndent.h"
#include "vtkPolyData.h"

#include <assert.h>

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMEOutputPointSet)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMEOutputPointSet::mafVMEOutputPointSet()
//-------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
mafVMEOutputPointSet::~mafVMEOutputPointSet()
//-------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
vtkPolyData *mafVMEOutputPointSet::GetPointSetData()
//-------------------------------------------------------------------------
{
  return (vtkPolyData *)GetVTKData();
}

