/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeLandmarkCloudTest
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaPipeLandmarkCloudTest_H__
#define __CPP_UNIT_albaPipeLandmarkCloudTest_H__

#include "albaTest.h"

class vtkRenderWindowInteractor;

/** Test for albaVMELandmarkCloud */
class albaPipeLandmarkCloudTest : public albaTest
{

public:
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  CPPUNIT_TEST_SUITE( albaPipeLandmarkCloudTest );
  CPPUNIT_TEST( DynamicAllocationTest );
  CPPUNIT_TEST( TestPipeExecution );
  CPPUNIT_TEST_SUITE_END();

protected:
  void DynamicAllocationTest();
  void TestPipeExecution();

};

#endif
