/*=========================================================================mafOpExporterAnsysInputFileTest

Program: MAF2
Module: mafOpExporterAnsysInputFileTest.h
Authors: Nicola Vanella

Copyright (c) B3C
All rights reserved. See Copyright.txt or
http://www.scsitaly.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafOpExporterAnsysInputFileTest_H__
#define __CPP_UNIT_mafOpExporterAnsysInputFileTest_H__

#include "mafTest.h"
#include "mafString.h"

class mafOpExporterAnsysInputFileTest;

class mafOpExporterAnsysInputFileTest : public mafTest
{
  public:
    CPPUNIT_TEST_SUITE(mafOpExporterAnsysInputFileTest );	
    CPPUNIT_TEST(TestAnsysINPFile_FileExist);
    CPPUNIT_TEST(TestAnsysINPFile_ReadData);
    CPPUNIT_TEST_SUITE_END();

  protected:
    void TestAnsysINPFile_FileExist();
    void TestAnsysINPFile_ReadData();

private:
    void Check_INPFile(mafString fileName);
    void CompareINPFilesData(mafString fileName);
};

#endif
