/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkPackedImageTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_vtkPackedImagetest_H__
#define __CPP_UNIT_vtkPackedImagetest_H__

#include "albaTest.h"

//------------------------------------------------------------------------------
// Test class for vtkPackedImage
//------------------------------------------------------------------------------
class vtkPackedImageTest : public albaTest
{
  public:

    CPPUNIT_TEST_SUITE( vtkPackedImageTest );
    CPPUNIT_TEST( TestDynamicAllocation );
    CPPUNIT_TEST( TestPrintSelf );
    CPPUNIT_TEST( TestSetGetPackType );
    CPPUNIT_TEST( TestSetGetImageSize );
    CPPUNIT_TEST( TestImportImage_GetImageImporter );
    CPPUNIT_TEST_SUITE_END();

  protected:
    void TestDynamicAllocation();
    void TestPrintSelf();
    void TestSetGetPackType();
    void TestSetGetImageSize();
    void TestImportImage_GetImageImporter();
};

#endif
