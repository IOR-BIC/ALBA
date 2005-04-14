/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEOutputPolyline.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-14 18:16:22 $
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

#include "mafVMEOutputPolyline.h"
#include "mafIndent.h"
#include "vtkPolyData.h"

#include <assert.h>

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMEOutputPolyline)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMEOutputPolyline::mafVMEOutputPolyline()
//-------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
mafVMEOutputPolyline::~mafVMEOutputPolyline()
//-------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
vtkPolyData *mafVMEOutputPolyline::GetPolylineData()
//-------------------------------------------------------------------------
{
  return (vtkPolyData *)GetVTKData();
}
