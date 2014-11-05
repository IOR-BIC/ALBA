/*=========================================================================

 Program: MAF2Medical
 Module: medInteractorDICOMImporterTest
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

#include "medInteractorDICOMImporterTest.h"

#include <cppunit/config/SourcePrefix.h>
#include <iostream>
#include "medInteractorDICOMImporter.h"
#include "mafEventInteraction.h"
#include "mafDeviceButtonsPadMouse.h"

void medInteractorDICOMImporterTest::setUp()
{
  
}

void medInteractorDICOMImporterTest::tearDown()
{

}

void medInteractorDICOMImporterTest::TestFixture()
{
	
}


void medInteractorDICOMImporterTest::TestConstructorDestructor()
{
	medInteractorDICOMImporter *dicomImporterInteractor = medInteractorDICOMImporter::New();
	dicomImporterInteractor->Delete();
}

void medInteractorDICOMImporterTest::TestStartInteraction()
{
	mafDeviceButtonsPadMouse *buttonsPadMouse = mafDeviceButtonsPadMouse::New();
	CPPUNIT_ASSERT(buttonsPadMouse);

	// smoke test (should not crash)
	medInteractorDICOMImporter *dicomImporterInteractor = medInteractorDICOMImporter::New();
	dicomImporterInteractor->StartInteraction(buttonsPadMouse);
	dicomImporterInteractor->Delete();

	buttonsPadMouse->Delete();
}


void medInteractorDICOMImporterTest::TestOnMouseMove()
{
  medInteractorDICOMImporter *dicomImporterInteractor = medInteractorDICOMImporter::New();
  
  // smoke test (should not crash)
  dicomImporterInteractor->OnMouseMove();
  dicomImporterInteractor->Delete();
}


void medInteractorDICOMImporterTest::TestOnLeftButtonDown()
{
  medInteractorDICOMImporter *dicomImporterInteractor = medInteractorDICOMImporter::New();
  
  // smoke test (should not crash)
  dicomImporterInteractor->OnLeftButtonDown(NULL);
  dicomImporterInteractor->Delete();
}

void medInteractorDICOMImporterTest::TestOnLeftButtonUp()
{
	medInteractorDICOMImporter *dicomImporterInteractor = medInteractorDICOMImporter::New();

	// smoke test (should not crash)
	dicomImporterInteractor->OnLeftButtonUp();
	dicomImporterInteractor->Delete();
}

