/*=========================================================================albaOpExporterAnsysInputFileTest

Program: ALBA
Module: albaOpExporterAnsysInputFileTest.h
Authors: Nicola Vanella

Copyright (c) BIC
All rights reserved. See Copyright.txt or
http://www.scsitaly.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaOpExporterAnsysInputFileTest_H__
#define __CPP_UNIT_albaOpExporterAnsysInputFileTest_H__

#include "albaTest.h"
#include "albaString.h"

class albaOpExporterAnsysInputFileTest;

class albaOpExporterAnsysInputFileTest : public albaTest
{
  public:
    CPPUNIT_TEST_SUITE(albaOpExporterAnsysInputFileTest );	
    CPPUNIT_TEST(TestAnsysINPFile_FileExist);
    CPPUNIT_TEST(TestAnsysINPFile_ReadData);
    CPPUNIT_TEST_SUITE_END();

  protected:
    void TestAnsysINPFile_FileExist();
    void TestAnsysINPFile_ReadData();

private:
    void Check_INPFile(albaString fileName);
    void CompareINPFilesData(albaString fileName);
};

#endif
