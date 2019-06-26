/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEMapsTest
 Authors: Eleonora Mambrini
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaVMEMapsTEST_H__
#define __CPP_UNIT_albaVMEMapsTEST_H__

#include "albaTest.h"

class albaVMESurface;
class albaVMEVolumeGray;
class albaVMERoot;
class albaVMEMaps;

class vtkRectilinearGrid;
class vtkImageData;
class vtkFloatArray;
class vtkPolyData;
class vtkSphereSource;

class albaVMEMapsTest : public albaTest
{
public: 
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  CPPUNIT_TEST_SUITE( albaVMEMapsTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestPrint );
  CPPUNIT_TEST( TestDeepCopy );
  CPPUNIT_TEST( TestEquals );
  CPPUNIT_TEST( TestGetDensityDistance );
  CPPUNIT_TEST( TestGetFirstThreshold );
  CPPUNIT_TEST( TestGetSecondThreshold );
  CPPUNIT_TEST( TestGetMaxDistance );
  CPPUNIT_TEST( TestGetVisualPipe );
  CPPUNIT_TEST( TestGetVolume );
  CPPUNIT_TEST( TestGetSurfaceOutput );
  CPPUNIT_TEST( TestGetSourceVMELink );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestDynamicAllocation();
  void TestPrint();
  void TestGetDensityDistance();
  void TestGetFirstThreshold();
  void TestGetSecondThreshold();
  void TestGetMaxDistance();
  void TestGetMappedVMELink();
  void TestGetSourceVMELink();
  void TestGetVisualPipe();
  void TestGetVolume();
  void TestGetSurfaceOutput();
  void TestDeepCopy();
  void TestEquals();

  void CreateVMEMaps();

  bool m_Result;

  albaVMERoot        *m_Root;
  albaVMEVolumeGray  *m_Volume;
  albaVMESurface     *m_SurfaceToMap;
  albaVMEMaps        *m_Maps;

  vtkPolyData       *polydata;
  vtkSphereSource   *sphere;

  int m_FirstThreshold;
  int m_SecondThreshold;
  int m_MaxDistance;
  int m_DensityDistance;

};

#endif
