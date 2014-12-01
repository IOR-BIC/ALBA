/*=========================================================================

 Program: MAF2Medical
 Module: mafOpExtractGeometryTest
 Authors: Gianluigi Crimi
 
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

#include "mafOpExtractGeometryTest.h"
#include "mafOpExtractGeometry.h"

#include "mafVMEVolumeGray.h"
#include "medVMEComputeWrapping.h"
#include "vtkMAFSmartPointer.h"
#include "mafSmartPointer.h"

#include <string>
#include <assert.h>
#include <string>
#include <assert.h>

//-----------------------------------------------------------
void mafOpExtractGeometryTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  mafOpExtractGeometry *create=new mafOpExtractGeometry();
  mafDEL(create);
}

//-----------------------------------------------------------
void mafOpExtractGeometryTest::TestCopy() 
//-----------------------------------------------------------
{
  mafOpExtractGeometry *create=new mafOpExtractGeometry();
  mafOpExtractGeometry *create2 = mafOpExtractGeometry::SafeDownCast(create->Copy());

  CPPUNIT_ASSERT(create2 != NULL);

  mafDEL(create2);
  mafDEL(create);
}
//-----------------------------------------------------------
void mafOpExtractGeometryTest::TestAccept() 
//-----------------------------------------------------------
{
  mafSmartPointer<mafVMEVolumeGray> volume;

  mafOpExtractGeometry *create=new mafOpExtractGeometry();
  CPPUNIT_ASSERT(create->Accept(volume));
  CPPUNIT_ASSERT(!create->Accept(NULL));

  mafDEL(create);
}