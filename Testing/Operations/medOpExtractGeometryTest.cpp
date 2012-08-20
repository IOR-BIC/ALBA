/*=========================================================================

 Program: MAF2Medical
 Module: medOpExtractGeometryTest
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