/*=========================================================================
Program:   ALBA
Module:    albaOpExporterAbaqusFileTest.h
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

#ifndef __CPP_UNIT_albaOpExporterAbaqusFileTest_H__
#define __CPP_UNIT_albaOpExporterAbaqusFileTest_H__

#include "albaTest.h"
#include "albaString.h"

class albaOpExporterAbaqusFileTest;

class albaOpExporterAbaqusFileTest : public albaTest
{
  public:
    CPPUNIT_TEST_SUITE(albaOpExporterAbaqusFileTest );	
    CPPUNIT_TEST(TestAbaqusFile_FileExist);
    CPPUNIT_TEST(TestAbaqusFile_ReadData);
    CPPUNIT_TEST_SUITE_END();

  protected:
    void TestAbaqusFile_FileExist();
    void TestAbaqusFile_ReadData();

private:
    void Check_AbaqusFile(albaString fileName);
    void CompareAbaqusFilesData(albaString fileName);
};

#endif
