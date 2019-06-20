/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEVolumeRGBTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaVMEVolumeRGBTEST_H__
#define __CPP_UNIT_albaVMEVolumeRGBTEST_H__

#include "albaTest.h"
#include "albaVMEVolumeRGB.h"

class albaVMEVolumeRGBTest : public albaTest
{
public:
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  CPPUNIT_TEST_SUITE( albaVMEVolumeRGBTest );
  CPPUNIT_TEST( TestSetDataWithImageData );
  CPPUNIT_TEST( TestSetDataWithRectilinearGrid );
  CPPUNIT_TEST( TestGetVolumeOutput );
  CPPUNIT_TEST( TestGetOutput );
  CPPUNIT_TEST_SUITE_END();

private:
  void TestSetDataWithImageData();
  void TestSetDataWithRectilinearGrid();
  void TestGetVolumeOutput();
  void TestGetOutput();

  //help methods
  int CreateVolumeWithImageData();
  int CreateVolumeWithRectilinearGrid();

  albaVMEVolumeRGB *m_VmeVolumeRGB;
};

#endif
