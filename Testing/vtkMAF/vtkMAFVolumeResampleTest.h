/*=========================================================================

 Program: MAF2
 Module: vtkMAFVolumeResampleTest
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_MMOVOLUMERESAMPLETEST_H__
#define __CPP_UNIT_MMOVOLUMERESAMPLETEST_H__

#include "mafTest.h"

#include "vtkDataSet.h"
#include "vtkTransform.h"

class vtkMAFVolumeResampleTest : public mafTest
{
  CPPUNIT_TEST_SUITE( vtkMAFVolumeResampleTest );
  CPPUNIT_TEST(TestResample);
  CPPUNIT_TEST(TestSetGetVolumeOrigin);
  CPPUNIT_TEST(TestSetGetVolumeAxisX);
  CPPUNIT_TEST(TestSetGetVolumeAxisY);
  CPPUNIT_TEST(TestSetGetWindow);
  CPPUNIT_TEST(TestSetGetLevel);

  CPPUNIT_TEST_SUITE_END();

  protected:
    
    void TestResample();
    void TestSetGetVolumeOrigin();
    void TestSetGetVolumeAxisX();
    void TestSetGetVolumeAxisY();
    void TestSetGetWindow();
    void TestSetGetLevel();

    void WriteVTKDatasetToFile( vtkDataSet * outputVolumeVTKData, const char *outputFilename );
    void TestResampleInternal( const char *inFileName , const char *outVTKFileName );

    void PrintDouble6( ostream& os, double array[6], const char *logMessage = NULL );
    void PrintDouble3( ostream& os, double array[3], const char *logMessage = NULL);
    void PrintInt3( ostream& os, int array[3], const char *logMessage = NULL );
};

#endif
