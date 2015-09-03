/*=========================================================================

 Program: MAF2
 Module: medInteractor2DIndicatorTest
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
#include "medInteractor2DIndicatorTest.h"

#include "medInteractor2DIndicator.h"

#include "mafDeviceButtonsPadMouse.h"

//-----------------------------------------------------------
void medInteractor2DIndicatorTest::TestFixture()
//-----------------------------------------------------------
{

}

//-----------------------------------------------------------
void medInteractor2DIndicatorTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  medInteractor2DIndicator *interactor = medInteractor2DIndicator::New();
	
  CPPUNIT_ASSERT(interactor->m_RendererVector.size() == 0);
  
  interactor->Delete();
}

//-----------------------------------------------------------
void medInteractor2DIndicatorTest::TestStaticAllocation() 
//-----------------------------------------------------------
{

}

//-----------------------------------------------------------
void medInteractor2DIndicatorTest::TestIsDisableUndoAndOkCancel() 
//-----------------------------------------------------------
{
	medInteractor2DIndicator *interactor = medInteractor2DIndicator::New();

	CPPUNIT_ASSERT( interactor->IsDisableUndoAndOkCancel() == false );

	interactor->Delete();
}

//-----------------------------------------------------------
void medInteractor2DIndicatorTest::TestUndoMeasure() 
//-----------------------------------------------------------
{
	medInteractor2DIndicator *interactor = medInteractor2DIndicator::New();
	interactor->UndoMeasure();

	CPPUNIT_ASSERT(interactor->m_RendererVector.size() == 0);

	interactor->Delete();
}

//-----------------------------------------------------------
void medInteractor2DIndicatorTest::TestRemoveMeter() 
//-----------------------------------------------------------
{
	medInteractor2DIndicator *interactor = medInteractor2DIndicator::New();
	interactor->RemoveMeter();

	CPPUNIT_ASSERT(interactor->m_CurrentRenderer == NULL);
	CPPUNIT_ASSERT(interactor->m_Mouse->GetRenderer() == NULL);

	interactor->Delete();
}

//-----------------------------------------------------------
void medInteractor2DIndicatorTest::TestSetLabel()
//-----------------------------------------------------------
{
	medInteractor2DIndicator *interactor = medInteractor2DIndicator::New();

	// Crash when called this way :P
	// interactor->SetLabel("Pippo");

	interactor->Delete();
}

//-----------------------------------------------------------
void medInteractor2DIndicatorTest::TestGetLabel()
//-----------------------------------------------------------
{
	medInteractor2DIndicator *interactor = medInteractor2DIndicator::New();	
	mafString label = interactor->GetLabel();

	CPPUNIT_ASSERT(label == "");

	interactor->Delete();
}

//-----------------------------------------------------------
void medInteractor2DIndicatorTest::TestGetRegisterMeasure()
//-----------------------------------------------------------
{
	medInteractor2DIndicator *interactor = medInteractor2DIndicator::New();	
	bool registerMeasure = interactor->GetRegisterMeasure();

	CPPUNIT_ASSERT(registerMeasure == false);

	interactor->Delete();

}

//-----------------------------------------------------------
void medInteractor2DIndicatorTest::TestSizeMeasureVector()
//-----------------------------------------------------------
{
	medInteractor2DIndicator *interactor = medInteractor2DIndicator::New();	

	int returnValue = interactor->SizeMeasureVector();

	CPPUNIT_ASSERT(returnValue == 0);

	interactor->Delete();

}
