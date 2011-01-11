/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mmiDICOMImporterInteractorTest.cpp,v $
Language:  C++
Date:      $Date: 2011-01-11 15:57:39 $
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

#include "mmiDICOMImporterInteractorTest.h"

#include <cppunit/config/SourcePrefix.h>
#include <iostream>
#include "mmiDICOMImporterInteractor.h"
#include "mafEventInteraction.h"
#include "mafDeviceButtonsPadMouse.h"

void mmiDICOMImporterInteractorTest::setUp()
{
  
}

void mmiDICOMImporterInteractorTest::tearDown()
{

}

void mmiDICOMImporterInteractorTest::TestFixture()
{
	
}


void mmiDICOMImporterInteractorTest::TestConstructorDestructor()
{
	mmiDICOMImporterInteractor *dicomImporterInteractor = mmiDICOMImporterInteractor::New();
	dicomImporterInteractor->Delete();
}

void mmiDICOMImporterInteractorTest::TestStartInteraction()
{
	mafDeviceButtonsPadMouse *buttonsPadMouse = mafDeviceButtonsPadMouse::New();
	CPPUNIT_ASSERT(buttonsPadMouse);

	// smoke test (should not crash)
	mmiDICOMImporterInteractor *dicomImporterInteractor = mmiDICOMImporterInteractor::New();
	dicomImporterInteractor->StartInteraction(buttonsPadMouse);
	dicomImporterInteractor->Delete();

	buttonsPadMouse->Delete();
}


void mmiDICOMImporterInteractorTest::TestOnMouseMove()
{
  mmiDICOMImporterInteractor *dicomImporterInteractor = mmiDICOMImporterInteractor::New();
  
  // smoke test (should not crash)
  dicomImporterInteractor->OnMouseMove();
  dicomImporterInteractor->Delete();
}


void mmiDICOMImporterInteractorTest::TestOnLeftButtonDown()
{
  mmiDICOMImporterInteractor *dicomImporterInteractor = mmiDICOMImporterInteractor::New();
  
  // smoke test (should not crash)
  dicomImporterInteractor->OnLeftButtonDown(NULL);
  dicomImporterInteractor->Delete();
}

void mmiDICOMImporterInteractorTest::TestOnLeftButtonUp()
{
	mmiDICOMImporterInteractor *dicomImporterInteractor = mmiDICOMImporterInteractor::New();

	// smoke test (should not crash)
	dicomImporterInteractor->OnLeftButtonUp();
	dicomImporterInteractor->Delete();
}

