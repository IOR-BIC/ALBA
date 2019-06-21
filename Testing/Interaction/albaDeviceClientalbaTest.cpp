/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaDeviceClientALBATest
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
#include "albaDeviceClientALBATest.h"
#include "albaDeviceClientALBA.h"

//----------------------------------------------------------------------------
void albaDeviceClientALBATest::TestFixture()
{

}
//----------------------------------------------------------------------------
void albaDeviceClientALBATest::TestConstructorDestructor()
{
  albaDeviceClientALBA *dummyDevice = albaDeviceClientALBA::New();
  CPPUNIT_ASSERT(dummyDevice);

  dummyDevice->Delete();
}
//----------------------------------------------------------------------------
void albaDeviceClientALBATest::TestConnectClient()
{
  albaDeviceClientALBA *dummyDevice = albaDeviceClientALBA::New();
  CPPUNIT_ASSERT(dummyDevice);
  
  wxIPV4address fakeAdress;

  int result = -1;
  result = dummyDevice->ConnectClient(fakeAdress);
  CPPUNIT_ASSERT(result == ALBA_ERROR);
  
  dummyDevice->Delete();
}
//----------------------------------------------------------------------------
void albaDeviceClientALBATest::TestDisconnectClient()
{
  albaDeviceClientALBA *dummyDevice = albaDeviceClientALBA::New();
  CPPUNIT_ASSERT(dummyDevice);

  wxIPV4address fakeAdress;

  int result = -1;
  result = dummyDevice->DisconnectClient();
  CPPUNIT_ASSERT(result == ALBA_OK);

  dummyDevice->Delete();
}
//----------------------------------------------------------------------------
void albaDeviceClientALBATest::TestSendMessageToServer()
{
  // mostly a test for leaks...
  albaDeviceClientALBA *dummyDevice = albaDeviceClientALBA::New();
  CPPUNIT_ASSERT(dummyDevice);

  albaString dummyMessage = "dummy";
  dummyDevice->SendMessageToServer(dummyMessage);

  dummyDevice->Delete();
}
//----------------------------------------------------------------------------
void albaDeviceClientALBATest::TestIsConnected()
{
  albaDeviceClientALBA *dummyDevice = albaDeviceClientALBA::New();
  CPPUNIT_ASSERT(dummyDevice);

  bool isConnected = dummyDevice->IsConnected() ;
  CPPUNIT_ASSERT(isConnected == false);
  
  dummyDevice->Delete();
}
//----------------------------------------------------------------------------
void albaDeviceClientALBATest::TestStart()
{
  albaDeviceClientALBA *dummyDevice = albaDeviceClientALBA::New();
  CPPUNIT_ASSERT(dummyDevice);

  int result = -1;
  result = dummyDevice->Start();
  CPPUNIT_ASSERT(result == ALBA_ERROR);
  
  dummyDevice->Delete();
}
//----------------------------------------------------------------------------
void albaDeviceClientALBATest::TestStop()
{
  // mostly a test for leaks...
  
  albaDeviceClientALBA *dummyDevice = albaDeviceClientALBA::New();
  CPPUNIT_ASSERT(dummyDevice);

  dummyDevice->Stop();
  dummyDevice->Delete();  
}
//----------------------------------------------------------------------------
void albaDeviceClientALBATest::TestIsBusy()
{
  albaDeviceClientALBA *dummyDevice = albaDeviceClientALBA::New();
  CPPUNIT_ASSERT(dummyDevice);
  
  bool isBusy = dummyDevice->IsBusy();
  CPPUNIT_ASSERT(isBusy == false);

  dummyDevice->Delete();
}
//----------------------------------------------------------------------------
void albaDeviceClientALBATest::TestOnEvent()
{
  // mostly a test for leaks...

  albaDeviceClientALBA *dummyDevice = albaDeviceClientALBA::New();
  CPPUNIT_ASSERT(dummyDevice);

  albaEventBase *dummyEvent = new albaEventBase(this, ID_NO_EVENT);

  dummyDevice->OnEvent(dummyEvent);

  dummyDevice->Delete();
  cppDEL(dummyEvent);
}
