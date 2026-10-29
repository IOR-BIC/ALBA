/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAMeshCutterTest
 Authors: Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_vtkALBAMeshCutterTest_H__
#define __CPP_UNIT_vtkALBAMeshCutterTest_H__

#include "albaTest.h"


/**  Test class for vtkALBAMeshCutter */
class vtkALBAMeshCutterTest : public albaTest
{
  public:

    CPPUNIT_TEST_SUITE( vtkALBAMeshCutterTest );
    CPPUNIT_TEST( TestFixture );

    CPPUNIT_TEST_SUITE_END();

  protected:

    void TestFixture();
};

#endif
