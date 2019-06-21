/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaEventInteractionTest
 Authors: Daniele Giunchi
 
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
#include "albaEventInteractionTest.h"

#include "albaEventInteraction.h"

//----------------------------------------------------------------------------
void albaEventInteractionTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  albaMatrix *mat;
  albaNEW(mat);
  mat->SetElement(0,1,2.0);

  int check1 = 1 << ID_MODIFIER_1;
  int check2 = 1 << ID_MODIFIER_2;
  int Modifiers;
  Modifiers|= check1;
  Modifiers |= check2;

  albaEventInteraction eventA;
  albaEventInteraction eventB(NULL, 10, ID_BUTTON_RIGHT, Modifiers);

  
  CPPUNIT_ASSERT(eventB.GetSender() == NULL);
  CPPUNIT_ASSERT(eventB.GetId() == 10);
  CPPUNIT_ASSERT(eventB.GetButton() == ID_BUTTON_RIGHT);
  CPPUNIT_ASSERT(eventB.GetModifiers() == Modifiers);

  albaEventInteraction eventC(NULL, 20, mat ,ID_BUTTON_RIGHT, Modifiers);
  CPPUNIT_ASSERT(eventC.GetSender() == NULL);
  CPPUNIT_ASSERT(eventC.GetId() == 20);
  CPPUNIT_ASSERT(eventC.GetButton() == ID_BUTTON_RIGHT);
  CPPUNIT_ASSERT(eventC.GetMatrix()->Equals(mat));
  CPPUNIT_ASSERT(eventC.GetModifiers() == Modifiers);

  albaEventInteraction eventD(NULL, 30, 10.,20.,ID_BUTTON_RIGHT,Modifiers);

  double Position2d[2];
  eventD.Get2DPosition(Position2d);

  CPPUNIT_ASSERT(eventD.GetSender() == NULL);
  CPPUNIT_ASSERT(eventD.GetId() == 30);
  CPPUNIT_ASSERT(Position2d[0] == 10. && Position2d[1] == 20.);
  CPPUNIT_ASSERT(eventD.GetButton() == ID_BUTTON_RIGHT);
  CPPUNIT_ASSERT(eventD.GetModifiers() == Modifiers);

  
  albaDEL(mat);
}
//----------------------------------------------------------------------------
void albaEventInteractionTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaMatrix *mat;
  albaNEW(mat);
  mat->SetElement(0,1,2.0);

  int check1 = 1 << ID_MODIFIER_1;
  int check2 = 1 << ID_MODIFIER_2;
  int Modifiers;
  Modifiers|= check1;
  Modifiers |= check2;

  albaEventInteraction *eventA = new albaEventInteraction();
  albaEventInteraction *eventB = new albaEventInteraction(NULL, 10, ID_BUTTON_RIGHT, Modifiers);


  CPPUNIT_ASSERT(eventB->GetSender() == NULL);
  CPPUNIT_ASSERT(eventB->GetId() == 10);
  CPPUNIT_ASSERT(eventB->GetButton() == ID_BUTTON_RIGHT);
  CPPUNIT_ASSERT(eventB->GetModifiers() == Modifiers);

  albaEventInteraction *eventC = new albaEventInteraction(NULL, 20, mat ,ID_BUTTON_RIGHT, Modifiers);
  CPPUNIT_ASSERT(eventC->GetSender() == NULL);
  CPPUNIT_ASSERT(eventC->GetId() == 20);
  CPPUNIT_ASSERT(eventC->GetButton() == ID_BUTTON_RIGHT);
  CPPUNIT_ASSERT(eventC->GetMatrix()->Equals(mat));
  CPPUNIT_ASSERT(eventC->GetModifiers() == Modifiers);

  albaEventInteraction *eventD = new albaEventInteraction(NULL, 30, 10.,20.,ID_BUTTON_RIGHT,Modifiers);

  double Position2d[2];
  eventD->Get2DPosition(Position2d);

  CPPUNIT_ASSERT(eventD->GetSender() == NULL);
  CPPUNIT_ASSERT(eventD->GetId() == 30);
  CPPUNIT_ASSERT(Position2d[0] == 10. && Position2d[1] == 20.);
  CPPUNIT_ASSERT(eventD->GetButton() == ID_BUTTON_RIGHT);
  CPPUNIT_ASSERT(eventD->GetModifiers() == Modifiers);

  albaDEL(mat);
  cppDEL(eventA);
  cppDEL(eventB);
  cppDEL(eventC);
  cppDEL(eventD);
}
//----------------------------------------------------------------------------
void albaEventInteractionTest::TestSet2DPosition_Get2DPosition()
//----------------------------------------------------------------------------
{
  albaEventInteraction *eventA = new albaEventInteraction;
  eventA->Set2DPosition(10.,20.);
  double Position2d[2];
  eventA->Get2DPosition(Position2d);
  
  CPPUNIT_ASSERT(Position2d[0] == 10. && Position2d[1] == 20.);

  Position2d[0] = 30.;
  Position2d[1] = 40.;
  eventA->Set2DPosition(Position2d);
  eventA->Get2DPosition(Position2d);

  CPPUNIT_ASSERT(Position2d[0] == 30. && Position2d[1] == 40.);
  CPPUNIT_ASSERT(eventA->GetXYFlag() == true); // flag is true

  cppDEL(eventA);
}
//----------------------------------------------------------------------------
void albaEventInteractionTest::TestSetXYFlag_GetXYFlag()
//----------------------------------------------------------------------------
{
  albaEventInteraction *eventA = new albaEventInteraction();
  eventA->SetXYFlag(false);
  CPPUNIT_ASSERT(eventA->GetXYFlag() == false);
  eventA->SetXYFlag(true);
  CPPUNIT_ASSERT(eventA->GetXYFlag() == true);

  cppDEL(eventA);
}
//----------------------------------------------------------------------------
void albaEventInteractionTest::TestSetButton_GetButton()
//----------------------------------------------------------------------------
{
  albaEventInteraction *eventA = new albaEventInteraction();
  enum
  {
    ID_BUTTON_LEFT,
    ID_BUTTON_RIGHT,
  };
  eventA->SetButton(ID_BUTTON_LEFT);
  CPPUNIT_ASSERT(eventA->GetButton() == ID_BUTTON_LEFT);
  eventA->SetButton(ID_BUTTON_RIGHT);
  CPPUNIT_ASSERT(eventA->GetButton() == ID_BUTTON_RIGHT);

  cppDEL(eventA);
}
//----------------------------------------------------------------------------
void albaEventInteractionTest::TestSetKey_GetKey()
//----------------------------------------------------------------------------
{
  albaEventInteraction *eventA = new albaEventInteraction();
  
  eventA->SetKey(ID_KEY_1);
  CPPUNIT_ASSERT(eventA->GetKey() == ID_KEY_1);
  eventA->SetKey(ID_KEY_2);
  CPPUNIT_ASSERT(eventA->GetKey() == ID_KEY_2);

  cppDEL(eventA);
}
//----------------------------------------------------------------------------
void albaEventInteractionTest::TestSetMatrix_GetMatrix()
//----------------------------------------------------------------------------
{
  albaMatrix *mat;
  albaNEW(mat);
  mat->SetElement(0,1,2.0);

  albaEventInteraction *eventA = new albaEventInteraction();
  eventA->SetMatrix(mat);
  CPPUNIT_ASSERT(eventA->GetMatrix()->Equals(mat));
  albaDEL(mat);
  cppDEL(eventA);
  
}
//----------------------------------------------------------------------------
void albaEventInteractionTest::TestSetModifier_GetModifier()
//----------------------------------------------------------------------------
{
  albaEventInteraction *eventA = new albaEventInteraction();
  
  int Modifiers = 0;
  eventA->SetModifier(ID_MODIFIER_1, true);

  bool result = eventA->GetModifier(ID_MODIFIER_1);
  CPPUNIT_ASSERT(true == result);
  int check = 1 << ID_MODIFIER_1;
  Modifiers |= check;
  CPPUNIT_ASSERT(eventA->GetModifiers() == Modifiers);
  cppDEL(eventA);
}
//----------------------------------------------------------------------------
void albaEventInteractionTest::TestSetModifiers_GetModifiers()
//----------------------------------------------------------------------------
{
  albaEventInteraction *eventA = new albaEventInteraction();
  
  int Modifiers = 0;
  eventA->SetModifier(ID_MODIFIER_1, true);
  eventA->SetModifier(ID_MODIFIER_2, true);

  int check1 = 1 << ID_MODIFIER_1;
  int check2 = 1 << ID_MODIFIER_2;
  Modifiers |= check1;
  Modifiers |= check2;

  CPPUNIT_ASSERT(eventA->GetModifiers() == Modifiers);

  Modifiers = Modifiers&(~check1);
  Modifiers = Modifiers&(~check2);

  eventA->SetModifiers(Modifiers);
  CPPUNIT_ASSERT(eventA->GetModifiers() == Modifiers);

  cppDEL(eventA);
}
//----------------------------------------------------------------------------
void albaEventInteractionTest::TestDeepCopy()
//----------------------------------------------------------------------------
{
  albaEventInteraction *eventA = new albaEventInteraction();
  
  albaMatrix *mat;
  albaNEW(mat);
  mat->SetElement(0,1,2.0);
  eventA->Set2DPosition(10.,20.);
  eventA->SetXYFlag(true);
  eventA->SetButton(ID_BUTTON_RIGHT);
  eventA->SetKey(ID_KEY_2);
  eventA->SetMatrix(mat);
  eventA->SetModifier(ID_MODIFIER_1, true);
  eventA->SetModifier(ID_MODIFIER_2, true);

  albaEventInteraction *eventB = new albaEventInteraction();
  eventB->DeepCopy(eventA);

  double Position2dA[2],Position2dB[2];
  eventA->Get2DPosition(Position2dA);
  eventB->Get2DPosition(Position2dB);
  CPPUNIT_ASSERT(Position2dA[0] == Position2dB[0] && Position2dA[1] == Position2dB[1]);
  CPPUNIT_ASSERT(eventA->GetXYFlag() == eventB->GetXYFlag());
  CPPUNIT_ASSERT(eventA->GetButton() == eventB->GetButton());
  CPPUNIT_ASSERT(eventA->GetKey() == eventB->GetKey());
  CPPUNIT_ASSERT(eventA->GetMatrix()->Equals(eventB->GetMatrix()));
  CPPUNIT_ASSERT(eventA->GetModifiers() == eventB->GetModifiers());

  albaDEL(mat);
  cppDEL(eventB);
  cppDEL(eventA);
}
