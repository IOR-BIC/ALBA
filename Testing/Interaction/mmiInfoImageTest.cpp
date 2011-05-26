/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mmiInfoImageTest.cpp,v $
Language:  C++
Date:      $Date: 2011-05-26 08:33:56 $
Version:   $Revision: 1.1.2.2 $
Authors:   Roberto Mucci
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

#include "mmiInfoImageTest.h"

#include <cppunit/config/SourcePrefix.h>
#include <iostream>
#include "mmiInfoImage.h"
#include "mafEventInteraction.h"
#include "mafDeviceButtonsPadMouse.h"
#include "mafVMESurface.h"

void mmiInfoImageTest::setUp()
{
  
}

void mmiInfoImageTest::tearDown()
{

}

void mmiInfoImageTest::TestFixture()
{
	
}

void mmiInfoImageTest::TestConstructorDestructor()
{
	mmiInfoImage *info = mmiInfoImage::New();
	info->Delete();
}

void mmiInfoImageTest::TestOnEvent()
{
  mmiInfoImage *info = mmiInfoImage::New();
  CPPUNIT_ASSERT(info);

  mafDeviceButtonsPadMouse *dummyDevice = mafDeviceButtonsPadMouse::New();
  dummyDevice->UpdateRWIDuringMotionOn();

  mafVMESurface *dummy = mafVMESurface::New();
  info->OnVmeSelected(dummy);

  mafEventInteraction dummyMouse2DMoveEvent;
  dummyMouse2DMoveEvent.SetChannel(MCH_INPUT);
  dummyMouse2DMoveEvent.SetSender(dummyDevice);
  dummyMouse2DMoveEvent.SetId(mafDeviceButtonsPadMouse::GetMouse2DMoveId());

  info->OnEvent(&dummyMouse2DMoveEvent);
  dummyDevice->Delete();
  dummy->Delete();
  info->Delete();
  CPPUNIT_ASSERT(true);
}

