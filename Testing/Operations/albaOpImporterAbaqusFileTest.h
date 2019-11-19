/*=========================================================================
Program:   ALBA
Module:    albaOpImporterAbaqusFileTest.h
Language:  C++
Date:      $Date: 2009-05-19 14:29:53 $
Version:   $Revision: 1.1 $
Authors:   Nicola Vanella
==========================================================================
Copyright (c) BIC-IOR 2019 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#ifndef __CPP_UNIT_albaOpImporterAbaqusFileTest_H__
#define __CPP_UNIT_albaOpImporterAbaqusFileTest_H__

#include "albaTest.h"
#include "albaString.h"

class albaOpImporterAbaqusFileTest;

class albaOpImporterAbaqusFileTest : public albaTest
{
  public:

    CPPUNIT_TEST_SUITE(albaOpImporterAbaqusFileTest );	
    CPPUNIT_TEST(TestFileName);
    CPPUNIT_TEST(TestAbaqusFile1);
    CPPUNIT_TEST(TestAbaqusFile2);
    CPPUNIT_TEST(TestAbaqusFile3);
    CPPUNIT_TEST(TestAbaqusFile4);
    CPPUNIT_TEST(TestAnsysHypermeshInputFile);
    CPPUNIT_TEST_SUITE_END();

  protected:
    void TestFileName();
    void TestAbaqusFile1();
    void TestAbaqusFile2();
    void TestAbaqusFile3();
    void TestAbaqusFile4();
    void TestAnsysHypermeshInputFile();

private:
    void ImportInputFile(albaString fileName, int numNodesExpected, int numMatsExpected, int numElemsExpected);
};

#endif
