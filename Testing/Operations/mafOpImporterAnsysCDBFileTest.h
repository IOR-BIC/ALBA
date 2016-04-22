/*=========================================================================

Program: MAF2
Module: mafOpImporterAnsysCDBFileTest.h
Authors: Nicola Vanella

Copyright (c) B3C
All rights reserved. See Copyright.txt or
http://www.scsitaly.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafOpImporterAnsysCDBFileTest_H__
#define __CPP_UNIT_mafOpImporterAnsysCDBFileTest_H__

#include "mafTest.h"
#include "mafString.h"

class mafOpImporterAnsysCDBFileTest;

class mafOpImporterAnsysCDBFileTest : public mafTest
{
  public:

    CPPUNIT_TEST_SUITE( mafOpImporterAnsysCDBFileTest );	
    CPPUNIT_TEST(TestFileName);
    CPPUNIT_TEST(TestAnsysCDBFileBlock);
 		CPPUNIT_TEST(TestAnsysCDBFileNoBlock);
 		CPPUNIT_TEST(TestAnsysHyperMeshFileSmall);
 		CPPUNIT_TEST(TestAnsysHyperMeshFileMed);
 		CPPUNIT_TEST(TestAnsysENotation);
    CPPUNIT_TEST_SUITE_END();

  protected:
    void TestFileName();
    void TestAnsysCDBFileBlock();
    void TestAnsysCDBFileNoBlock();
    void TestAnsysHyperMeshFileSmall();
    void TestAnsysHyperMeshFileMed();
    void TestAnsysENotation();

private:
    void ImportCDBFile(mafString fileName, int numNodesExpected, int numMatsExpected, int numElemsExpected);
};

#endif
