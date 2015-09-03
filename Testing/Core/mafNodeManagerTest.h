/*=========================================================================

 Program: MAF2
 Module: mafNodeManagerTest
 Authors: Alberto Losi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafNodeManagerTest_h__
#define __CPP_UNIT_mafNodeManagerTest_h__

#include "mafTest.h"

#include <wx/config.h>
#include <wx/fileconf.h>

/** mafNodeManagerTest:
Test class for mafNodeManager
*/
//------------------------------------------------------------------------------
class mafNodeManagerTest: public mafTest, public mafObserver
//------------------------------------------------------------------------------
{

public:

  /** Executed before each test.*/
  void BeforeTest();

  /** Test suite begin. */
  CPPUNIT_TEST_SUITE( mafNodeManagerTest );

  CPPUNIT_TEST( MSFNewTest );
  CPPUNIT_TEST( VmeAddRemoveTest );
  CPPUNIT_TEST( TimeGetBoundsTest );
  CPPUNIT_TEST( SetGetFileNameTest );
  CPPUNIT_TEST( GetRootTest );

  CPPUNIT_TEST_SUITE_END();

  void OnEvent(mafEventBase *maf_event);

protected:

  void MSFNewTest();

  // Other MSF methods are no longer implemented (commented code, see mafNodeManager.cpp) 

  /** test add and remove vme */
  void VmeAddRemoveTest();

  /** test get time bounds */
  void TimeGetBoundsTest();

  /** test get and set file name */
  void SetGetFileNameTest();

  /** test get root */
  void GetRootTest();		

  int m_EventID;
  mafNode *m_Node;
  wxFileConfig *m_Config;
};


#endif // #ifndef __CPP_UNIT_mafNodeManagerTest_h__