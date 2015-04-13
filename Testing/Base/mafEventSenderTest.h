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

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

#include "mafObserver.h"

class DummyObserver;
class mafEventBase;
class mafEventSender;

class mafEventSenderTest : public CPPUNIT_NS::TestFixture
{
  public:

  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

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
