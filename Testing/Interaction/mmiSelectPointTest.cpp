/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mmiSelectPointTest.cpp,v $
Language:  C++
Date:      $Date: 2010-11-12 17:27:35 $
Version:   $Revision: 1.1.2.2 $
Authors:   Stefano Perticoni
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

#include "mmiSelectPointTest.h"

#include <cppunit/config/SourcePrefix.h>
#include <iostream>
#include "mmiSelectPoint.h"
#include "mafEventInteraction.h"

void mmiSelectPointTest::setUp()
{
  
}

void mmiSelectPointTest::tearDown()
{

}

void mmiSelectPointTest::TestFixture()
{
	
}

void mmiSelectPointTest::TestConstructorDestructor()
{
	mmiSelectPoint *selectPoint = mmiSelectPoint::New();
	selectPoint->Delete();
}

void mmiSelectPointTest::TestOnMouseMove()
{
  mmiSelectPoint *selectPoint = mmiSelectPoint::New();
  // smoke test
  selectPoint->OnMouseMove();
  selectPoint->Delete();
}

void mmiSelectPointTest::TestSetCtrlModifierOnOff()
{
	mmiSelectPoint *selectPoint = mmiSelectPoint::New();
  
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

void mmiSelectPointTest::TestSetGetCtrlModifier()
{
  mmiSelectPoint *selectPoint = mmiSelectPoint::New();

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

void mmiSelectPointTest::TestOnLeftButtonDown()
{
	mmiSelectPoint *selectPoint = mmiSelectPoint::New();
  // smoke test
  selectPoint->OnLeftButtonDown(NULL);
	selectPoint->Delete();
}

void mmiSelectPointTest::TestOnButtonUp()
{
  mmiSelectPoint *selectPoint = mmiSelectPoint::New();
  // smoke test 
  selectPoint->OnButtonUp(NULL);
  selectPoint->Delete();
}