/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpVolumeResampleTest
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaOpVolumeResampleTest_H__
#define __CPP_UNIT_albaOpVolumeResampleTest_H__

#include "albaTest.h"
#include "vtkDataSet.h"
#include "vtkTransform.h"

class albaOpVolumeResampleTest : public albaTest
{
  CPPUNIT_TEST_SUITE( albaOpVolumeResampleTest );
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
