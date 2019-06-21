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

#ifndef __CPP_UNIT_albaDeviceClientALBATest_H__
#define __CPP_UNIT_albaDeviceClientALBATest_H__

#include "albaTest.h"

class albaDeviceClientALBATest : public albaTest
{
  public:

  CPPUNIT_TEST_SUITE( albaDeviceClientALBATest );
  CPPUNIT_TEST(TestFixture);
  CPPUNIT_TEST(TestConstructorDestructor);
  CPPUNIT_TEST(TestConnectClient);
  CPPUNIT_TEST(TestDisconnectClient);
  CPPUNIT_TEST(TestSendMessageToServer);
  CPPUNIT_TEST(TestIsConnected);
  CPPUNIT_TEST(TestIsBusy);
  CPPUNIT_TEST(TestOnEvent);
  CPPUNIT_TEST_SUITE_END();

  protected:
    
  void TestFixture();
  void TestConstructorDestructor();
  void TestConnectClient();

  /** 
  wxSocketClient::Close() seems to have 1 line of leaks*/
  void TestDisconnectClient();

  void TestSendMessageToServer();
  void TestIsConnected();
  void TestStart();
  void TestStop();
  void TestIsBusy();
  void TestOnEvent();
  
};

#endif
