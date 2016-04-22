/*=========================================================================

Program: MAF2
Module: mafOpImporterAnsysInputFileTest.h
Authors: Nicola Vanella

Copyright (c) B3C
All rights reserved. See Copyright.txt or
http://www.scsitaly.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafOpImporterAnsysInputFileTest_H__
#define __CPP_UNIT_mafOpImporterAnsysInputFileTest_H__

#include "mafTest.h"
#include "mafString.h"

class mafOpImporterAnsysInputFileTest;

class mafOpImporterAnsysInputFileTest : public mafTest
{
  public:

    CPPUNIT_TEST_SUITE(mafOpImporterAnsysInputFileTest );	
    CPPUNIT_TEST(TestFileName);
    CPPUNIT_TEST(TestAnsysInputFile1);
    CPPUNIT_TEST(TestAnsysInputFile2);
    CPPUNIT_TEST(TestAnsysInputFile3);
    CPPUNIT_TEST(TestAnsysInputFile4);
    CPPUNIT_TEST(TestAnsysHypermeshInputFile);
    CPPUNIT_TEST_SUITE_END();

  protected:
    void TestFileName();
    void TestAnsysInputFile1();
    void TestAnsysInputFile2();
    void TestAnsysInputFile3();
    void TestAnsysInputFile4();
    void TestAnsysHypermeshInputFile();

private:
    void ImportInputFile(mafString fileName, int numNodesExpected, int numMatsExpected, int numElemsExpected);
};

#endif
