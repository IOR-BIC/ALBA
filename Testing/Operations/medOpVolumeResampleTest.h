/*=========================================================================

 Program: MAF2
 Module: medOpVolumeResampleTest
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_medOpVolumeResampleTest_H__
#define __CPP_UNIT_medOpVolumeResampleTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

#include "vtkDataSet.h"
#include "vtkTransform.h"

class medOpVolumeResampleTest : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( medOpVolumeResampleTest );
  CPPUNIT_TEST( TestSetBounds);
  CPPUNIT_TEST( TestSetGetSpacing);
  CPPUNIT_TEST( TestResample);
  CPPUNIT_TEST_SUITE_END();

  protected:
    void TestSetBounds();
    void TestResample();
    void TestSetGetSpacing();

  private:
    void WriteVTKDatasetToFile( vtkDataSet * outputVolumeVTKData, const char *outputFilename = "outDataset.vtk" );
    void TestResampleInternal( const char *inFileName, const char *outVTKFileName );

};

#endif
