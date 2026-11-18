/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaCompositorMouseBehaviorTest
 Authors: Stefano Perticoni
 
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
#include "albaCompositorMouseBehaviorTest.h"
#include "albaCompositorMouseBehavior.h"


void albaCompositorMouseBehaviorTest::TestFixture()
{

}

void albaCompositorMouseBehaviorTest::TestConstructorDestructor()
{
  albaCompositorMouseBehavior *compositorMouse = NULL;
  compositorMouse = albaCompositorMouseBehavior::New();
  CPPUNIT_ASSERT(compositorMouse != NULL);
  compositorMouse->Delete(); 
}

void albaCompositorMouseBehaviorTest::TestButtonDown()
{
  albaCompositorMouseBehavior *compositorMouse = albaCompositorMouseBehavior::New();
  double pos[2] = {10, 20};

  // no transform behavior for the button: the button is not used
  CPPUNIT_ASSERT(!compositorMouse->OnLeftButtonDown(NULL, NULL, pos));

  // without a renderer the interaction cannot start
  compositorMouse->CreateBehavior(MOUSE_LEFT);
  CPPUNIT_ASSERT(!compositorMouse->OnLeftButtonDown(NULL, NULL, pos));
  CPPUNIT_ASSERT(!compositorMouse->OnLeftButtonUp(NULL, NULL, pos));

  // the modifier keys select another transform behavior
  compositorMouse->SetModifierKeys(true, false, false);
  CPPUNIT_ASSERT(!compositorMouse->OnMiddleButtonDown(NULL, NULL, pos));

  compositorMouse->Delete(); 
}

void albaCompositorMouseBehaviorTest::TestOnDrag()
{
  albaCompositorMouseBehavior *compositorMouse = albaCompositorMouseBehavior::New();
  compositorMouse->CreateBehavior(MOUSE_LEFT);

  // without an interaction the drags are not used
  double pos[2] = {10, 20};
  CPPUNIT_ASSERT(!compositorMouse->OnDrag(NULL, NULL, pos));
  CPPUNIT_ASSERT(compositorMouse->m_ActiveTransform == NULL);

  compositorMouse->Delete(); 
}

void albaCompositorMouseBehaviorTest::TestCreateBehavior()
{
  albaCompositorMouseBehavior *compositorMouse = NULL;
  compositorMouse = albaCompositorMouseBehavior::New();
  CPPUNIT_ASSERT(compositorMouse != NULL);

  albaTransformBehavior *mouseBehavior = NULL;
  mouseBehavior = compositorMouse->GetBehavior(MOUSE_LEFT);
  CPPUNIT_ASSERT(mouseBehavior == NULL);
    
  mouseBehavior = compositorMouse->CreateBehavior(MOUSE_LEFT);
  CPPUNIT_ASSERT(mouseBehavior != NULL);

  mouseBehavior = compositorMouse->GetBehavior(MOUSE_LEFT);
  CPPUNIT_ASSERT(mouseBehavior != NULL);

  compositorMouse->Delete(); 
}

void albaCompositorMouseBehaviorTest::TestGetBehavior()
{
  albaCompositorMouseBehavior *compositorMouse = NULL;
  compositorMouse = albaCompositorMouseBehavior::New();
  CPPUNIT_ASSERT(compositorMouse != NULL);

  albaTransformBehavior *getNotYetCreatedMouseBehavior = NULL;
  getNotYetCreatedMouseBehavior = compositorMouse->GetBehavior(MOUSE_MIDDLE);
  CPPUNIT_ASSERT(getNotYetCreatedMouseBehavior == NULL);

  albaTransformBehavior *createdMouseMiddleBehavior = NULL;
  createdMouseMiddleBehavior = compositorMouse->CreateBehavior(MOUSE_MIDDLE);
  CPPUNIT_ASSERT(createdMouseMiddleBehavior != NULL);
  
  albaTransformBehavior *getMouseMiddleBehavior = NULL;
  getMouseMiddleBehavior = compositorMouse->GetBehavior(MOUSE_MIDDLE);
  CPPUNIT_ASSERT(getMouseMiddleBehavior != NULL);
  CPPUNIT_ASSERT(getMouseMiddleBehavior == createdMouseMiddleBehavior);
  
  albaTransformBehavior *getMouseMiddleCtrlBehavior = NULL;
  getMouseMiddleCtrlBehavior = compositorMouse->GetBehavior(MOUSE_MIDDLE_CONTROL);
  CPPUNIT_ASSERT(getMouseMiddleCtrlBehavior == NULL);

  compositorMouse->Delete(); 
}
