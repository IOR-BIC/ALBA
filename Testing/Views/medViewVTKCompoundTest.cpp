/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medViewVTKCompoundTest.cpp,v $
Language:  C++
Date:      $Date: 2009-12-01 09:38:24 $
Version:   $Revision: 1.1.2.1 $
Authors:   Eleonora Mambrini
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "medViewVTKCompound.h"
#include "medViewVTKCompoundTest.h"

#include <iostream>

//-------------------------------------------------------------------------
void medViewVTKCompoundTest::CreateDestroyTest()
//-------------------------------------------------------------------------
{
  medViewVTKCompound *icView = new medViewVTKCompound("");

  mafViewVTK *vtkView = new mafViewVTK();
  icView->SetExternalView(vtkView);
  icView->PackageView();
  icView->Delete();

  CPPUNIT_ASSERT(true);

}