/*=========================================================================

 Program: MAF2
 Module: mafDeviceClientMAFTest
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

#include <cppunit/config/SourcePrefix.h>
#include "mafDeviceClientMAFTest.h"
#include "mafDeviceClientMAF.h"

//----------------------------------------------------------------------------
void mafDeviceClientMAFTest::TestFixture()
{

}
//----------------------------------------------------------------------------
void mafDeviceClientMAFTest::TestConstructorDestructor()
{
  mafDeviceClientMAF *dummyDevice = mafDeviceClientMAF::New();
  CPPUNIT_ASSERT(dummyDevice);

  dummyDevice->Delete();
}
//----------------------------------------------------------------------------
void mafDeviceClientMAFTest::TestConnectClient()
{
  mafDeviceClientMAF *dummyDevice = mafDeviceClientMAF::New();
  CPPUNIT_ASSERT(dummyDevice);
  
  wxIPV4address fakeAdress;

  int result = -1;
  result = dummyDevice->ConnectClient(fakeAdress);
  CPPUNIT_ASSERT(result == MAF_ERROR);
  
  dummyDevice->Delete();
}
//----------------------------------------------------------------------------
void mafDeviceClientMAFTest::TestDisconnectClient()
{
  mafDeviceClientMAF *dummyDevice = mafDeviceClientMAF::New();
  CPPUNIT_ASSERT(dummyDevice);

  wxIPV4address fakeAdress;

  int result = -1;
  result = dummyDevice->DisconnectClient();
  CPPUNIT_ASSERT(result == MAF_OK);

  dummyDevice->Delete();
}
//----------------------------------------------------------------------------
void mafDeviceClientMAFTest::TestSendMessageToServer()
{
  // mostly a test for leaks...
  mafDeviceClientMAF *dummyDevice = mafDeviceClientMAF::New();
  CPPUNIT_ASSERT(dummyDevice);

  mafString dummyMessage = "dummy";
  dummyDevice->SendMessageToServer(dummyMessage);

  dummyDevice->Delete();
}
//----------------------------------------------------------------------------
void mafDeviceClientMAFTest::TestIsConnected()
{
  mafDeviceClientMAF *dummyDevice = mafDeviceClientMAF::New();
  CPPUNIT_ASSERT(dummyDevice);

  bool isConnected = dummyDevice->IsConnected() ;
  CPPUNIT_ASSERT(isConnected == false);
  
  dummyDevice->Delete();
}
//----------------------------------------------------------------------------
void mafDeviceClientMAFTest::TestStart()
{
  mafDeviceClientMAF *dummyDevice = mafDeviceClientMAF::New();
  CPPUNIT_ASSERT(dummyDevice);

  int result = -1;
  result = dummyDevice->Start();
  CPPUNIT_ASSERT(result == MAF_ERROR);
  
  dummyDevice->Delete();
}
//----------------------------------------------------------------------------
void mafDeviceClientMAFTest::TestStop()
{
  // mostly a test for leaks...
  
  mafDeviceClientMAF *dummyDevice = mafDeviceClientMAF::New();
  CPPUNIT_ASSERT(dummyDevice);

  dummyDevice->Stop();
  dummyDevice->Delete();  
}
//----------------------------------------------------------------------------
void mafDeviceClientMAFTest::TestIsBusy()
{
  mafDeviceClientMAF *dummyDevice = mafDeviceClientMAF::New();
  CPPUNIT_ASSERT(dummyDevice);
  
  bool isBusy = dummyDevice->IsBusy();
  CPPUNIT_ASSERT(isBusy == false);

  dummyDevice->Delete();
}
//----------------------------------------------------------------------------
void mafDeviceClientMAFTest::TestOnEvent()
{
  // mostly a test for leaks...

  mafDeviceClientMAF *dummyDevice = mafDeviceClientMAF::New();
  CPPUNIT_ASSERT(dummyDevice);

  mafEventBase *dummyEvent = new mafEventBase(this, ID_NO_EVENT);

  dummyDevice->OnEvent(dummyEvent);

  dummyDevice->Delete();
  cppDEL(dummyEvent);
}
