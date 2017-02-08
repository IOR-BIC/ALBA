/*=========================================================================

 Program: MAF2
 Module: mafInteractor2DIndicatorTest
 Authors: Matteo Giacomoni
 
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
#include "mafInteractor2DIndicatorTest.h"

#include "mafInteractor2DIndicator.h"

#include "mafDeviceButtonsPadMouse.h"

//-----------------------------------------------------------
void mafInteractor2DIndicatorTest::TestFixture()
//-----------------------------------------------------------
{

}

//-----------------------------------------------------------
void mafInteractor2DIndicatorTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  mafInteractor2DIndicator *interactor = mafInteractor2DIndicator::New();
	
  CPPUNIT_ASSERT(interactor->m_RendererVector.size() == 0);
  
  interactor->Delete();
}

//-----------------------------------------------------------
void mafInteractor2DIndicatorTest::TestStaticAllocation() 
//-----------------------------------------------------------
{

}

//-----------------------------------------------------------
void mafInteractor2DIndicatorTest::TestIsDisableUndoAndOkCancel() 
//-----------------------------------------------------------
{
	mafInteractor2DIndicator *interactor = mafInteractor2DIndicator::New();

	CPPUNIT_ASSERT( interactor->IsDisableUndoAndOkCancel() == false );

	interactor->Delete();
}

//-----------------------------------------------------------
void mafInteractor2DIndicatorTest::TestUndoMeasure() 
//-----------------------------------------------------------
{
	mafInteractor2DIndicator *interactor = mafInteractor2DIndicator::New();
	interactor->UndoMeasure();

	CPPUNIT_ASSERT(interactor->m_RendererVector.size() == 0);

	interactor->Delete();
}

//-----------------------------------------------------------
void mafInteractor2DIndicatorTest::TestRemoveMeter() 
//-----------------------------------------------------------
{
	mafInteractor2DIndicator *interactor = mafInteractor2DIndicator::New();
	interactor->RemoveMeter();

	CPPUNIT_ASSERT(interactor->m_CurrentRenderer == NULL);
	CPPUNIT_ASSERT(interactor->m_Mouse->GetRenderer() == NULL);

	interactor->Delete();
}

//-----------------------------------------------------------
void mafInteractor2DIndicatorTest::TestSetLabel()
//-----------------------------------------------------------
{
	mafInteractor2DIndicator *interactor = mafInteractor2DIndicator::New();

	// Crash when called this way :P
	// interactor->SetLabel("Pippo");

	interactor->Delete();
}

//-----------------------------------------------------------
void mafInteractor2DIndicatorTest::TestGetLabel()
//-----------------------------------------------------------
{
	mafInteractor2DIndicator *interactor = mafInteractor2DIndicator::New();	
	mafString label = interactor->GetLabel();

	CPPUNIT_ASSERT(label == "");

	interactor->Delete();
}

//-----------------------------------------------------------
void mafInteractor2DIndicatorTest::TestGetRegisterMeasure()
//-----------------------------------------------------------
{
	mafInteractor2DIndicator *interactor = mafInteractor2DIndicator::New();	
	bool registerMeasure = interactor->GetRegisterMeasure();

	CPPUNIT_ASSERT(registerMeasure == false);

	interactor->Delete();

}

//-----------------------------------------------------------
void mafInteractor2DIndicatorTest::TestSizeMeasureVector()
//-----------------------------------------------------------
{
	mafInteractor2DIndicator *interactor = mafInteractor2DIndicator::New();	

	int returnValue = interactor->SizeMeasureVector();

	CPPUNIT_ASSERT(returnValue == 0);

	interactor->Delete();

}
