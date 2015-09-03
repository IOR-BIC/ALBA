/*=========================================================================

 Program: MAF2
 Module: mafEventSenderTest
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_MAFEVENTSENDER_H__
#define __CPP_UNIT_MAFEVENTSENDER_H__

#include "mafTest.h"
#include "mafObserver.h"

class DummyObserver;
class mafEventBase;
class mafEventSender;

class mafEventSenderTest : public mafTest
{
  public:

  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  CPPUNIT_TEST_SUITE( mafEventSenderTest );
  CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
  CPPUNIT_TEST( TestSetListener );
  CPPUNIT_TEST( TestGetListener );
  CPPUNIT_TEST( TestHasListener );
  CPPUNIT_TEST( TestInvokeEvent );
  CPPUNIT_TEST_SUITE_END();

  DummyObserver *m_DummyObserver;
  mafEventSender *m_Sender;

  protected:
  
  void TestFixture();
  void TestSetListener();
  void TestGetListener();
  void TestHasListener();
  void TestInvokeEvent();
};



class DummyObserver : public mafObserver
{
public:

  DummyObserver() {};
  ~DummyObserver() {};

  virtual void OnEvent(mafEventBase *maf_event);

};

#endif
