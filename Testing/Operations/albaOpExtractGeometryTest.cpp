/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpExtractGeometryTest
 Authors: Gianluigi Crimi
 
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

#include "albaOpExtractGeometryTest.h"
#include "albaOpExtractGeometry.h"

#include "albaVMEVolumeGray.h"
#include "albaVMEComputeWrapping.h"
#include "vtkALBASmartPointer.h"
#include "albaSmartPointer.h"

#include <string>
#include <assert.h>
#include <string>
#include <assert.h>

//-----------------------------------------------------------
void albaOpExtractGeometryTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  albaOpExtractGeometry *create=new albaOpExtractGeometry();
  albaDEL(create);
}

//-----------------------------------------------------------
void albaOpExtractGeometryTest::TestCopy() 
//-----------------------------------------------------------
{
  albaOpExtractGeometry *create=new albaOpExtractGeometry();
  albaOpExtractGeometry *create2 = albaOpExtractGeometry::SafeDownCast(create->Copy());

  CPPUNIT_ASSERT(create2 != NULL);

  albaDEL(create2);
  albaDEL(create);
}
//-----------------------------------------------------------
void albaOpExtractGeometryTest::TestAccept() 
//-----------------------------------------------------------
{
  albaSmartPointer<albaVMEVolumeGray> volume;

  albaOpExtractGeometry *create=new albaOpExtractGeometry();
  CPPUNIT_ASSERT(create->Accept(volume));
  CPPUNIT_ASSERT(!create->Accept(NULL));

  albaDEL(create);
}