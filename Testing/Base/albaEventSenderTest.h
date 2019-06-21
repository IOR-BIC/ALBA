/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaEventSenderTest
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_ALBAEVENTSENDER_H__
#define __CPP_UNIT_ALBAEVENTSENDER_H__

#include "albaTest.h"
#include "albaObserver.h"

class DummyObserver;
class albaEventBase;
class albaEventSender;

class albaEventSenderTest : public albaTest
{
  public:

  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  CPPUNIT_TEST_SUITE( albaEventSenderTest );
  CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
  CPPUNIT_TEST( TestSetListener );
  CPPUNIT_TEST( TestGetListener );
  CPPUNIT_TEST( TestHasListener );
  CPPUNIT_TEST( TestInvokeEvent );
  CPPUNIT_TEST_SUITE_END();

  DummyObserver *m_DummyObserver;
  albaEventSender *m_Sender;

  protected:
  
  void TestFixture();
  void TestSetListener();
  void TestGetListener();
  void TestHasListener();
  void TestInvokeEvent();
};



class DummyObserver : public albaObserver
{
public:

  DummyObserver() {};
  ~DummyObserver() {};

  virtual void OnEvent(albaEventBase *alba_event);

};

#endif
