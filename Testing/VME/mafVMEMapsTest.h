/*=========================================================================

 Program: MAF2
 Module: mafVMEMapsTest
 Authors: Eleonora Mambrini
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafVMEMapsTEST_H__
#define __CPP_UNIT_mafVMEMapsTEST_H__

#include "mafTest.h"

class mafVMESurface;
class mafVMEVolumeGray;
class mafVMERoot;
class mafVMEMaps;

class vtkRectilinearGrid;
class vtkImageData;
class vtkFloatArray;
class vtkPolyData;
class vtkSphereSource;

class mafVMEMapsTest : public mafTest
{
public: 
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  CPPUNIT_TEST_SUITE( mafVMEMapsTest );
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

  mafVMERoot        *m_Root;
  mafVMEVolumeGray  *m_Volume;
  mafVMESurface     *m_SurfaceToMap;
  mafVMEMaps        *m_Maps;

  vtkPolyData       *polydata;
  vtkSphereSource   *sphere;

  int m_FirstThreshold;
  int m_SecondThreshold;
  int m_MaxDistance;
  int m_DensityDistance;

};

#endif
