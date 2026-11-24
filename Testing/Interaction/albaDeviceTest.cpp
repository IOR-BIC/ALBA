/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaDeviceTest
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

#include <cppunit/config/SourcePrefix.h>
#include "albaDeviceTest.h"
#include "albaEventInteraction.h"
#include "albaDeviceButtonsPadMouse.h"
#include "albaDevice.h"
#include "albaView.h"
#include "albaEvent.h"

//----------------------------------------------------------------------------
class albaDeviceTestView : public albaView // minimal concrete view for test
//----------------------------------------------------------------------------
{
public:
  albaTypeMacro(albaDeviceTestView, albaView);
  virtual void SetBackgroundColor(wxColor color) {};
};

albaCxxTypeMacro(albaDeviceTestView);

//----------------------------------------------------------------------------
void albaDeviceTest::TestFixture()
{

}
//----------------------------------------------------------------------------
void albaDeviceTest::TestConstructorDestructor()
{
  albaDevice *dummyDevice = albaDevice::New();
  CPPUNIT_ASSERT(dummyDevice);

  dummyDevice->Delete();
}
//----------------------------------------------------------------------------
void albaDeviceTest::TestSetName()
{
  albaDevice *dummyDevice = albaDevice::New();
  CPPUNIT_ASSERT(dummyDevice);
  albaString defaultName = dummyDevice->GetName();
  CPPUNIT_ASSERT(defaultName.Equals(""));

  const char *newName = "Dummy";
  dummyDevice->SetName(newName);

  CPPUNIT_ASSERT(albaString::Compare(dummyDevice->GetName(), newName) == 0);
  dummyDevice->Delete();
}
//----------------------------------------------------------------------------
void albaDeviceTest::TestOnEvent()
{
  albaDevice *dummyDevice = albaDevice::New();
  CPPUNIT_ASSERT(dummyDevice);

  albaEventInteraction dummyMouse2DMoveEvent;
  dummyMouse2DMoveEvent.SetChannel(MCH_INPUT);
  dummyMouse2DMoveEvent.SetSender(this);
  dummyMouse2DMoveEvent.SetId(DEVICE_MOUSE_2D_MOVE);

  dummyDevice->OnEvent(&dummyMouse2DMoveEvent);
  dummyDevice->Delete();
}
//----------------------------------------------------------------------------
void albaDeviceTest::TestGetViewAndRenderer()
//----------------------------------------------------------------------------
{
  // a generic device has neither a view nor a renderer
  albaDevice *device;
  albaNEW(device);
  CPPUNIT_ASSERT(device->GetView() == NULL);
  CPPUNIT_ASSERT(device->GetRenderer() == NULL);
  albaDEL(device);

  // the mouse returns its selected view also through the albaDevice interface
  albaDeviceButtonsPadMouse *mouse;
  albaNEW(mouse);
  albaDevice *mouseDevice = mouse;
  CPPUNIT_ASSERT(mouseDevice->GetView() == NULL);
  albaDeviceTestView *view = new albaDeviceTestView();
  mouse->SetView(view);
  CPPUNIT_ASSERT(mouseDevice->GetView() == view);
  CPPUNIT_ASSERT(mouseDevice->GetRenderer() == NULL); // no render window selected
  albaDEL(mouse);
  delete view;
}
