/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaAvatarTest
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaAvatarTest_H__
#define __CPP_UNIT_albaAvatarTest_H__

#include "albaTest.h"
#include "albaAvatar.h"

class albaAvatarTest : public albaTest
{
  public:
  
    CPPUNIT_TEST_SUITE( albaAvatarTest );
    CPPUNIT_TEST(TestFixture);
    CPPUNIT_TEST(TestConstructorDestructor);   
    CPPUNIT_TEST(TestSetGetView);
    CPPUNIT_TEST(TestGetActor3D);
    CPPUNIT_TEST(TestPick);
    CPPUNIT_TEST(TestOnEvent);
    CPPUNIT_TEST(TestSetGetTracker);
    CPPUNIT_TEST(TestSetGetMode);
    CPPUNIT_TEST(TestSetModeTo2D);
    CPPUNIT_TEST(TestSetModeTo3D);
    CPPUNIT_TEST(TestGetPicker);
    CPPUNIT_TEST_SUITE_END();
    
  protected:
    
    void TestFixture();
    void TestConstructorDestructor();   
    void TestSetGetView();
    void TestGetActor3D();
    void TestPick();
    void TestOnEvent();
    void TestSetGetTracker();
    void TestSetGetMode();
    void TestSetModeTo2D();
    void TestSetModeTo3D();
    void TestGetPicker();
};


class ConcreteMafAvatar : public albaAvatar
{
public:

  albaTypeMacro(ConcreteMafAvatar,albaAvatar);

  void OnEvent(albaEventBase *alba_event);
  int GetLastReceivedEventID();

protected:
  
  ConcreteMafAvatar();
  virtual ~ConcreteMafAvatar();

private:

  ConcreteMafAvatar(const ConcreteMafAvatar&) {}  // Not implemented.
  void operator=(const ConcreteMafAvatar&) {}  // Not implemented.
 
  int m_LastReceivedEventID;
};


#endif
