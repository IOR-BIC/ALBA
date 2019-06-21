/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: mmiInfoImageTest
 Authors: Roberto Mucci
 
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

#include "mmiInfoImageTest.h"

#include <cppunit/config/SourcePrefix.h>
#include <iostream>
#include "mmiInfoImage.h"
#include "albaEventInteraction.h"
#include "albaDeviceButtonsPadMouse.h"
#include "albaVMESurface.h"

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

  albaDeviceButtonsPadMouse *dummyDevice = albaDeviceButtonsPadMouse::New();

  albaVMESurface *dummy = albaVMESurface::New();
  info->OnVmeSelected(dummy);

  albaEventInteraction dummyMouse2DMoveEvent;
  dummyMouse2DMoveEvent.SetChannel(MCH_INPUT);
  dummyMouse2DMoveEvent.SetSender(dummyDevice);
  dummyMouse2DMoveEvent.SetId(albaDeviceButtonsPadMouse::GetMouse2DMoveId());

  info->OnEvent(&dummyMouse2DMoveEvent);
  dummyDevice->Delete();
  dummy->Delete();
  info->Delete();
}

