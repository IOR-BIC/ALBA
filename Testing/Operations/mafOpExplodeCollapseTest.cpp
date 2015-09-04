/*=========================================================================

 Program: MAF2
 Module: mafOpExplodeCollapseTest
 Authors: Roberto Mucci
 
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

#include <cppunit/config/SourcePrefix.h>
#include "mafOpExplodeCollapseTest.h"

#include "mafString.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMELandmark.h"
#include "mafOpExplodeCollapse.h"
#include "vtkMAFSmartPointer.h"

#define TEST_RESULT CPPUNIT_ASSERT(result);


//----------------------------------------------------------------------------
void mafOpExplodeCollapseTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafOpExplodeCollapseTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafOpExplodeCollapse *explode = new mafOpExplodeCollapse();
  mafDEL(explode);
}

//----------------------------------------------------------------------------
void mafOpExplodeCollapseTest::TestExplodeCollapse()
//----------------------------------------------------------------------------
{
  mafOpExplodeCollapse *explode = new mafOpExplodeCollapse();

  mafSmartPointer<mafVMELandmarkCloud> landmarkCloud;
  landmarkCloud->TestModeOn();
  landmarkCloud->Open();
  mafSmartPointer<mafVMELandmark> landmark;
  landmark->SetPoint(1.0,1.0,2.0);
  landmark->ReparentTo(landmarkCloud);

  explode->SetInput(landmarkCloud);
  try
  {
    explode->OpDo();
  }
  catch (...)
  {
  }
  
  result = false;
  result = !landmarkCloud->IsOpen();
  TEST_RESULT;

  try
  {
    explode->OpDo();
  }
  catch (...)
  {
  }
  
  result = false;
  result = landmarkCloud->IsOpen();
  TEST_RESULT;

  mafDEL(explode);
}

