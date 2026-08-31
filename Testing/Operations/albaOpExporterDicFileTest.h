/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpExporterDicFileTest
 Authors: Gianluigi Crimi

 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_albaOpExporterDicFileTEST_H
#define CPP_UNIT_albaOpExporterDicFileTEST_H

#include "albaTest.h"

class albaOpExporterDicFileTest : public albaTest
{

public:

  CPPUNIT_TEST_SUITE( albaOpExporterDicFileTest );
  CPPUNIT_TEST( TestExportDicFile );
  CPPUNIT_TEST( TestExportDicFileWithABSMatrix );
  CPPUNIT_TEST( TestExportAndReimport );
  CPPUNIT_TEST_SUITE_END();

  protected:
    void TestExportDicFile();
    void TestExportDicFileWithABSMatrix();
    void TestExportAndReimport();
};

#endif