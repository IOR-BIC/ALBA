/*=========================================================================

 Program: MAF2
 Module: mafOpRegisterClustersTest
 Authors: Alberto Losi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafOpRegisterClustersTest_H__
#define __CPP_UNIT_mafOpRegisterClustersTest_H__

#include "mafTest.h"

class mafOpRegisterClustersTest : public mafTest
{
public:

  CPPUNIT_TEST_SUITE( mafOpRegisterClustersTest );
  CPPUNIT_TEST( DynamicAllocationTest );
  //CPPUNIT_TEST( OnEventTest );
  CPPUNIT_TEST( CopyTest );
  CPPUNIT_TEST( AcceptTest );   
  CPPUNIT_TEST( OpRunTest );
  CPPUNIT_TEST( OpDoUndoTest );
  CPPUNIT_TEST( ClosedCloudAcceptTest );
  CPPUNIT_TEST( SurfaceAcceptTest );
  CPPUNIT_TEST_SUITE_END();

protected:

    void DynamicAllocationTest();
    //void OnEventTest(); Only GUI events. not tested
    void CopyTest();
    void AcceptTest();   
    void OpRunTest();
    void OpDoUndoTest();
    void ClosedCloudAcceptTest();
    void SurfaceAcceptTest();

};

#endif
