/*=========================================================================

 Program: MAF2
 Module: vtkMAFContourVolumeMapperTest
 Authors: Matteo Giacomoni, Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_VTKCONTOURVOLUMEMAPPERTEST_H__
#define __CPP_UNIT_VTKCONTOURVOLUMEMAPPERTEST_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

class vtkImageData;
class vtkRectilinearGrid;

class vtkMAFContourVolumeMapperTest : public CPPUNIT_NS::TestFixture
{
  public:
  
    // CPPUNIT fixture: executed before each test
    void setUp();

    // CPPUNIT fixture: executed after each test
    void tearDown();

    CPPUNIT_TEST_SUITE( vtkMAFContourVolumeMapperTest );
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