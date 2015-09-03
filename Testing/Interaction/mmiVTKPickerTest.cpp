/*=========================================================================

 Program: MAF2
 Module: mmiVTKPickerTest
 Authors: Stefano Perticoni
 
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
