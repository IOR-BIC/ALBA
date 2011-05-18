/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medOpExtractGeometryTest.cpp,v $
Language:  C++
Date:      $Date: 2011-05-18 13:26:18 $
Version:   $Revision: 1.1.2.1 $
Authors:   Daniele Giunchi
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

#include "medOpExtractGeometryTest.h"
#include "medOpExtractGeometry.h"

#include "mafVMEVolumeGray.h"
#include "medVMEComputeWrapping.h"
#include "vtkMAFSmartPointer.h"
#include "mafSmartPointer.h"

#include <string>
#include <assert.h>
#include <string>
#include <assert.h>

//-----------------------------------------------------------
void medOpExtractGeometryTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  medOpExtractGeometry *create=new medOpExtractGeometry();
  mafDEL(create);
}

//-----------------------------------------------------------
void medOpExtractGeometryTest::TestCopy() 
//-----------------------------------------------------------
{
  medOpExtractGeometry *create=new medOpExtractGeometry();
  medOpExtractGeometry *create2 = medOpExtractGeometry::SafeDownCast(create->Copy());

  CPPUNIT_ASSERT(create2 != NULL);

  mafDEL(create2);
  mafDEL(create);
}
//-----------------------------------------------------------
void medOpExtractGeometryTest::TestAccept() 
//-----------------------------------------------------------
{
  mafSmartPointer<mafVMEVolumeGray> volume;

  medOpExtractGeometry *create=new medOpExtractGeometry();
  CPPUNIT_ASSERT(create->Accept(volume));
  CPPUNIT_ASSERT(!create->Accept(NULL));

  mafDEL(create);
}