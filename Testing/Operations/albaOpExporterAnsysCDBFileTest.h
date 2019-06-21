/*=========================================================================

Program: ALBA
Module: albaOpExporterAnsysCDBFileTest.h
Authors: Nicola Vanella

Copyright (c) BIC
All rights reserved. See Copyright.txt or
http://www.scsitaly.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaOpExporterAnsysCDBFileTest_H__
#define __CPP_UNIT_albaOpExporterAnsysCDBFileTest_H__

#include "albaTest.h"
#include "albaString.h"

class albaOpExporterAnsysCDBFileTest;

class albaOpExporterAnsysCDBFileTest : public albaTest
{
  public:
    CPPUNIT_TEST_SUITE( albaOpExporterAnsysCDBFileTest );	
    CPPUNIT_TEST(TestAnsysCDBFile_FileExist);
    CPPUNIT_TEST(TestAnsysCDBFile_ReadData);
    CPPUNIT_TEST_SUITE_END();

  protected:
    void TestAnsysCDBFile_FileExist();
    void TestAnsysCDBFile_ReadData();
    void TestAnsysCDBFile_AutoFillData();

private:
    void Check_CDBFile(albaString fileName);
    void CompareCDBFilesData(albaString fileName);
};

#endif
