/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: mmiVTKPickerTest
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

#include "mmiVTKPickerTest.h"

#include <cppunit/config/SourcePrefix.h>
#include <iostream>
#include "mmiVTKPicker.h"


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

void mmiVTKPickerTest::TestOnMouseMove()
{
	mmiVTKPicker *picker = mmiVTKPicker::New();
	// smoke test
	picker->OnMouseMove();
	picker->Delete();
}

void mmiVTKPickerTest::TestOnLeftButtonDown()
{
	mmiVTKPicker *picker = mmiVTKPicker::New();
	// smoke test
	picker->OnLeftButtonDown(NULL);
	picker->Delete();
}

void mmiVTKPickerTest::TestOnLeftButtonUp()
{
	mmiVTKPicker *picker = mmiVTKPicker::New();
	// smoke test
	picker->OnLeftButtonUp();
	picker->Delete();
}
