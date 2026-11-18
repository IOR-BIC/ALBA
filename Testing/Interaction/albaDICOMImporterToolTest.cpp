/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaDICOMImporterToolTest
 Authors: Paolo Quadrani, Gianluigi Crimi

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

#include "albaDICOMImporterToolTest.h"
#include "albaDICOMImporterTool.h"

#include <cppunit/config/SourcePrefix.h>

//----------------------------------------------------------------------------
void albaDICOMImporterToolTest::TestConstructorDestructor()
{
  albaDICOMImporterTool *tool = albaDICOMImporterTool::New();
  tool->Delete();
}

//----------------------------------------------------------------------------
void albaDICOMImporterToolTest::TestEventsWithoutRenderer()
{
  albaDICOMImporterTool *tool = albaDICOMImporterTool::New();
  double pos[2] = {10, 10};

  // no renderer: the events are not used
  CPPUNIT_ASSERT(!tool->OnMove(NULL, NULL, pos));
  CPPUNIT_ASSERT(!tool->OnLeftButtonDown(NULL, NULL, pos));
  CPPUNIT_ASSERT(!tool->OnDrag(NULL, NULL, pos));

  tool->Delete();
}
