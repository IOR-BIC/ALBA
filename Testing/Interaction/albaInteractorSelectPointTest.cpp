/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractorSelectPointTest
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

#include "albaInteractorSelectPointTest.h"

#include <cppunit/config/SourcePrefix.h>
#include <iostream>
#include "albaInteractorSelectPoint.h"
#include "albaEventInteraction.h"

void albaInteractorSelectPointTest::TestFixture()
{
	
}

void albaInteractorSelectPointTest::TestConstructorDestructor()
{
	albaInteractorSelectPoint *selectPoint = albaInteractorSelectPoint::New();
	selectPoint->Delete();
}

void albaInteractorSelectPointTest::TestOnMouseMove()
{
  albaInteractorSelectPoint *selectPoint = albaInteractorSelectPoint::New();
  // smoke test
  selectPoint->OnMouseMove();
  selectPoint->Delete();
}

void albaInteractorSelectPointTest::TestSetCtrlModifierOnOff()
{
	albaInteractorSelectPoint *selectPoint = albaInteractorSelectPoint::New();
  
  bool ctrlModifierSatus = selectPoint->GetCtrlModifier();
  CPPUNIT_ASSERT(ctrlModifierSatus == true);

  selectPoint->SetCtrlModifierOn();
  ctrlModifierSatus = selectPoint->GetCtrlModifier();
  CPPUNIT_ASSERT(ctrlModifierSatus == true);

  selectPoint->SetCtrlModifierOff();
  ctrlModifierSatus = selectPoint->GetCtrlModifier();
  CPPUNIT_ASSERT(ctrlModifierSatus == false);

  selectPoint->SetCtrlModifierOff();

	selectPoint->Delete();

}

void albaInteractorSelectPointTest::TestSetGetCtrlModifier()
{
  albaInteractorSelectPoint *selectPoint = albaInteractorSelectPoint::New();

  bool ctrlModifierSatus = selectPoint->GetCtrlModifier();
  CPPUNIT_ASSERT(ctrlModifierSatus == true);

  selectPoint->SetCtrlModifier(true);
  ctrlModifierSatus = selectPoint->GetCtrlModifier();
  CPPUNIT_ASSERT(ctrlModifierSatus == true);
 
  selectPoint->SetCtrlModifier(false);
  ctrlModifierSatus = selectPoint->GetCtrlModifier();
  CPPUNIT_ASSERT(ctrlModifierSatus == false);

  selectPoint->Delete();
}

void albaInteractorSelectPointTest::TestOnLeftButtonDown()
{
	albaInteractorSelectPoint *selectPoint = albaInteractorSelectPoint::New();
  // smoke test
  selectPoint->OnLeftButtonDown(NULL);
	selectPoint->Delete();
}

void albaInteractorSelectPointTest::TestOnButtonUp()
{
  albaInteractorSelectPoint *selectPoint = albaInteractorSelectPoint::New();
  // smoke test 
  selectPoint->OnButtonUp(NULL);
  selectPoint->Delete();
}