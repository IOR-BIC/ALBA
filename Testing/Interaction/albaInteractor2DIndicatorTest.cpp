/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractor2DIndicatorTest
 Authors: Matteo Giacomoni
 
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
#include "albaInteractor2DIndicatorTest.h"

#include "albaInteractor2DIndicator.h"

#include "albaDeviceButtonsPadMouse.h"

//-----------------------------------------------------------
void albaInteractor2DIndicatorTest::TestFixture()
//-----------------------------------------------------------
{

}

//-----------------------------------------------------------
void albaInteractor2DIndicatorTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  albaInteractor2DIndicator *interactor = albaInteractor2DIndicator::New();
	
  CPPUNIT_ASSERT(interactor->m_RendererVector.size() == 0);
  
  interactor->Delete();
}

//-----------------------------------------------------------
void albaInteractor2DIndicatorTest::TestStaticAllocation() 
//-----------------------------------------------------------
{

}

//-----------------------------------------------------------
void albaInteractor2DIndicatorTest::TestIsDisableUndoAndOkCancel() 
//-----------------------------------------------------------
{
	albaInteractor2DIndicator *interactor = albaInteractor2DIndicator::New();

	CPPUNIT_ASSERT( interactor->IsDisableUndoAndOkCancel() == false );

	interactor->Delete();
}

//-----------------------------------------------------------
void albaInteractor2DIndicatorTest::TestUndoMeasure() 
//-----------------------------------------------------------
{
	albaInteractor2DIndicator *interactor = albaInteractor2DIndicator::New();
	interactor->UndoMeasure();

	CPPUNIT_ASSERT(interactor->m_RendererVector.size() == 0);

	interactor->Delete();
}

//-----------------------------------------------------------
void albaInteractor2DIndicatorTest::TestRemoveMeter() 
//-----------------------------------------------------------
{
	albaInteractor2DIndicator *interactor = albaInteractor2DIndicator::New();
	interactor->RemoveMeter();

	CPPUNIT_ASSERT(interactor->m_CurrentRenderer == NULL);
	CPPUNIT_ASSERT(interactor->m_Mouse->GetRenderer() == NULL);

	interactor->Delete();
}

//-----------------------------------------------------------
void albaInteractor2DIndicatorTest::TestSetLabel()
//-----------------------------------------------------------
{
	albaInteractor2DIndicator *interactor = albaInteractor2DIndicator::New();

	// Crash when called this way :P
	// interactor->SetLabel("Pippo");

	interactor->Delete();
}

//-----------------------------------------------------------
void albaInteractor2DIndicatorTest::TestGetLabel()
//-----------------------------------------------------------
{
	albaInteractor2DIndicator *interactor = albaInteractor2DIndicator::New();	
	albaString label = interactor->GetLabel();

	CPPUNIT_ASSERT(label == "");

	interactor->Delete();
}

//-----------------------------------------------------------
void albaInteractor2DIndicatorTest::TestGetRegisterMeasure()
//-----------------------------------------------------------
{
	albaInteractor2DIndicator *interactor = albaInteractor2DIndicator::New();	
	bool registerMeasure = interactor->GetRegisterMeasure();

	CPPUNIT_ASSERT(registerMeasure == false);

	interactor->Delete();

}

//-----------------------------------------------------------
void albaInteractor2DIndicatorTest::TestSizeMeasureVector()
//-----------------------------------------------------------
{
	albaInteractor2DIndicator *interactor = albaInteractor2DIndicator::New();	

	int returnValue = interactor->SizeMeasureVector();

	CPPUNIT_ASSERT(returnValue == 0);

	interactor->Delete();

}
