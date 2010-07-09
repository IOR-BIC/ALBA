/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medDeviceButtonsPadMouseDialogTest.cpp,v $
Language:  C++
Date:      $Date: 2010-07-09 08:16:59 $
Version:   $Revision: 1.1.2.1 $
Authors:   Daniele Giunchi
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

#include <cppunit/config/SourcePrefix.h>
#include "medDeviceButtonsPadMouseDialogTest.h"
#include "medDeviceButtonsPadMouseDialog.h"

#include "mafViewSlice.h" 

void medDeviceButtonsPadMouseDialogTest::setUp()
{

}

void medDeviceButtonsPadMouseDialogTest::tearDown()
{

}


void medDeviceButtonsPadMouseDialogTest::TestConstructorDestructor()
{
 medDeviceButtonsPadMouseDialog *bp = medDeviceButtonsPadMouseDialog::New();
 bp->Delete();
}

void medDeviceButtonsPadMouseDialogTest::TestSetView()
{
  mafViewSlice *sliceView = new mafViewSlice();
  medDeviceButtonsPadMouseDialog *buttonsPad = medDeviceButtonsPadMouseDialog::New();
  buttonsPad->SetView(sliceView);
  
  CPPUNIT_ASSERT(buttonsPad->GetView() == sliceView);
  sliceView->Delete();
  buttonsPad->Delete();
}

void medDeviceButtonsPadMouseDialogTest::TestFixture()
{
}
