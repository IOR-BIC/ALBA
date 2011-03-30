/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medInteractor2DIndicatorTest.cpp,v $
Language:  C++
Date:      $Date: 2011-03-30 13:56:08 $
Version:   $Revision: 1.1.2.1 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)

MafMedical Library use license agreement

The software named MafMedical Library and any accompanying documentation, 
manuals or data (hereafter collectively "SOFTWARE") is property of the SCS s.r.l.
This is an open-source copyright as follows:
Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, 
this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation and/or 
other materials provided with the distribution.
* Modified source versions must be plainly marked as such, and must not be misrepresented 
as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

MafMedical is partially based on OpenMAF.
=========================================================================*/

#include "medDefines.h"
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
void medInteractor2DIndicatorTest::setUp()
//-----------------------------------------------------------
{

}
//-----------------------------------------------------------
void medInteractor2DIndicatorTest::tearDown()
//-----------------------------------------------------------
{

}
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
