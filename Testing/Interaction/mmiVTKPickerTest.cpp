/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mmiVTKPickerTest.cpp,v $
Language:  C++
Date:      $Date: 2010-11-03 16:32:01 $
Version:   $Revision: 1.1.2.1 $
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

#include "mmiVTKPickerTest.h"

#include <cppunit/config/SourcePrefix.h>
#include <iostream>
#include "mmiVTKPicker.h"

void mmiVTKPickerTest::setUp()
{
  
}

void mmiVTKPickerTest::tearDown()
{

}

void mmiVTKPickerTest::TestFixture()
{
	
}

void mmiVTKPickerTest::TestConstructorDestructor()
{
	mmiVTKPicker *picker = mmiVTKPicker::New();

	picker->Delete();
}

void mmiVTKPickerTest::TestSetGetContinuousPicking()
{
	mmiVTKPicker *picker = mmiVTKPicker::New();

	CPPUNIT_ASSERT(picker->GetContinuousPicking() == false);

	picker->SetContinuousPicking(false);
	CPPUNIT_ASSERT(picker->GetContinuousPicking() == false);

	picker->SetContinuousPicking(true);
	CPPUNIT_ASSERT(picker->GetContinuousPicking() == true);

	picker->Delete();
}

void mmiVTKPickerTest::OnMouseMoveTest()
{
	mmiVTKPicker *picker = mmiVTKPicker::New();
	// smoke test
	picker->OnMouseMove();
	picker->Delete();
}

void mmiVTKPickerTest::OnLeftButtonDownTest()
{
	mmiVTKPicker *picker = mmiVTKPicker::New();
	// smoke test
	picker->OnLeftButtonDown(NULL);
	picker->Delete();
}

void mmiVTKPickerTest::OnLeftButtonUpTest()
{
	mmiVTKPicker *picker = mmiVTKPicker::New();
	// smoke test
	picker->OnLeftButtonUp();
	picker->Delete();
}
