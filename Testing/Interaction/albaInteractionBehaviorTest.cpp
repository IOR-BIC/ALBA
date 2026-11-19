/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractionBehaviorTest
 Authors: Alberto Losi
 
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

#include <cppunit/config/SourcePrefix.h>
#include "albaInteractionBehaviorTest.h"
#include "albaInteractionBehavior.h"
#include "vtkRenderer.h"

//----------------------------------------------------------------------------
class albaInteractionBehaviorDummy : public albaInteractionBehavior // concrete class for test
//----------------------------------------------------------------------------
{
public:
  albaTypeMacro(albaInteractionBehaviorDummy, albaInteractionBehavior);
};

albaCxxTypeMacro(albaInteractionBehaviorDummy);

//----------------------------------------------------------------------------
void albaInteractionBehaviorTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaInteractionBehaviorTest::TestConstructorDestructor()
//----------------------------------------------------------------------------
{
  albaInteractionBehaviorDummy *behavior;
  albaNEW(behavior);
  albaDEL(behavior);
}
//----------------------------------------------------------------------------
void albaInteractionBehaviorTest::TestSetGetRenderer()
//----------------------------------------------------------------------------
{
  vtkRenderer *renderer;
  vtkNEW(renderer);

  albaInteractionBehaviorDummy *behavior;
  albaNEW(behavior);

  // renderer and view start empty (they belong to albaInteractionAgent)
  CPPUNIT_ASSERT(behavior->GetRenderer() == NULL);
  CPPUNIT_ASSERT(behavior->GetView() == NULL);
  CPPUNIT_ASSERT(behavior->IsA("albaInteractionTool"));

  behavior->SetRendererAndView(renderer, NULL);
  CPPUNIT_ASSERT(behavior->GetRenderer() == renderer);

  vtkDEL(renderer);
  albaDEL(behavior);
}
//----------------------------------------------------------------------------
void albaInteractionBehaviorTest::TestButtonsNotUsed()
//----------------------------------------------------------------------------
{
  albaInteractionBehaviorDummy *behavior;
  albaNEW(behavior);
  double pos[2] = {10, 20};

  // the methods not redefined leave the buttons to the camera
  CPPUNIT_ASSERT(!behavior->OnLeftButtonDown(NULL, NULL, pos));
  CPPUNIT_ASSERT(!behavior->OnMiddleButtonDown(NULL, NULL, pos));
  CPPUNIT_ASSERT(!behavior->OnRightButtonDown(NULL, NULL, pos));
  CPPUNIT_ASSERT(!behavior->OnDrag(NULL, NULL, pos));
  CPPUNIT_ASSERT(!behavior->OnLeftButtonUp(NULL, NULL, pos));

  albaDEL(behavior);
}
