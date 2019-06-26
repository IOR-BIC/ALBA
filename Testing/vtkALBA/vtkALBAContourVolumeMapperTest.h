/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAContourVolumeMapperTest
 Authors: Matteo Giacomoni, Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_VTKCONTOURVOLUMEMAPPERTEST_H__
#define __CPP_UNIT_VTKCONTOURVOLUMEMAPPERTEST_H__

#include "albaTest.h"

class vtkImageData;
class vtkRectilinearGrid;

class vtkALBAContourVolumeMapperTest : public albaTest
{
  public:
  
    // Executed before each test
    void BeforeTest();

    // Executed after each test
    void AfterTest();

    CPPUNIT_TEST_SUITE( vtkALBAContourVolumeMapperTest );
    CPPUNIT_TEST( TestFixture );
    CPPUNIT_TEST( TestImageData );
    CPPUNIT_TEST( TestRectilinearGrid );
    CPPUNIT_TEST_SUITE_END();

  protected:

    void Test();
    void TestFixture();
    void TestImageData();
    void TestRectilinearGrid();

		void CreateDataID();
		void CreateDataRG();
    vtkImageData *m_ImageData;
    vtkRectilinearGrid *m_RGData;

};

#endif
