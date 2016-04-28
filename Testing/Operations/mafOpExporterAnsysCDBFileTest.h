/*=========================================================================

Program: MAF2
Module: mafOpExporterAnsysCDBFileTest.h
Authors: Nicola Vanella

Copyright (c) B3C
All rights reserved. See Copyright.txt or
http://www.scsitaly.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafOpExporterAnsysCDBFileTest_H__
#define __CPP_UNIT_mafOpExporterAnsysCDBFileTest_H__

#include "mafTest.h"
#include "mafString.h"

class mafOpExporterAnsysCDBFileTest;

class mafOpExporterAnsysCDBFileTest : public mafTest
{
  public:
    CPPUNIT_TEST_SUITE( mafOpExporterAnsysCDBFileTest );	
    CPPUNIT_TEST(TestAnsysCDBFile_FileExist);
    CPPUNIT_TEST(TestAnsysCDBFile_ReadData);
    CPPUNIT_TEST_SUITE_END();

  protected:
    void TestAnsysCDBFile_FileExist();
    void TestAnsysCDBFile_ReadData();
    void TestAnsysCDBFile_AutoFillData();

private:
    void Check_CDBFile(mafString fileName);
    void CompareCDBFilesData(mafString fileName);
};

#endif
