/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAPointCloudCutterTest
 Authors: Test Suite

 Copyright (c) BIC
 All rights reserved. See Copyright.txt or

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_VTKALBA_POINT_CLOUD_CUTTER_TEST_H__
#define __CPP_UNIT_VTKALBA_POINT_CLOUD_CUTTER_TEST_H__

#include "albaTest.h"
#include "vtkALBASmartPointer.h"
#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkPointData.h"
#include "vtkFloatArray.h"
#include "vtkPlane.h"
#include "vtkTransform.h"
#include "vtkMatrix4x4.h"
#include "vtkUnstructuredGrid.h"
#include <cppunit/extensions/HelperMacros.h>
#include <math.h>

class vtkALBAPointCloudCutter;

//------------------------------------------------------------------------------
// Test class for vtkALBAPointCloudCutter
//------------------------------------------------------------------------------
class vtkALBAPointCloudCutterTest : public albaTest
{
  public:

    CPPUNIT_TEST_SUITE( vtkALBAPointCloudCutterTest );
/*
    CPPUNIT_TEST( TestFixture );
    CPPUNIT_TEST( TestSetCutFunction );
    CPPUNIT_TEST( TestGetCutFunction );
    CPPUNIT_TEST( TestInitialize );
    CPPUNIT_TEST( TestSlicePointsSimple );*/
    CPPUNIT_TEST( TestSlicePointsWithTolerance );
 /*   CPPUNIT_TEST( TestSlicePointsWithTransform );
    CPPUNIT_TEST( TestGetIntersectionOfBoundsWithPlane );
    CPPUNIT_TEST( TestGetIntersectionOfBoundsWithPlane_NoIntersection );
    CPPUNIT_TEST( TestTransferScalars );
    CPPUNIT_TEST( TestTransferScalarsMultipleArrays );
    CPPUNIT_TEST( TestCreateSlice );
    CPPUNIT_TEST( TestGetMTime );
    CPPUNIT_TEST( TestPrintSelf );*/
    CPPUNIT_TEST_SUITE_END();

  protected:

    void TestFixture();
    void TestSetCutFunction();
    void TestGetCutFunction();
    void TestInitialize();
    void TestSlicePointsSimple();
    void TestSlicePointsWithTolerance();
    void TestSlicePointsWithTransform();
    void TestGetIntersectionOfBoundsWithPlane();
    void TestGetIntersectionOfBoundsWithPlane_NoIntersection();
    void TestTransferScalars();
    void TestTransferScalarsMultipleArrays();
    void TestCreateSlice();
    void TestGetMTime();
    void TestPrintSelf();

    // Helper functions
    vtkPolyData* CreateSimplePointCloud(int numPoints, double spacing = 1.0);
    vtkPolyData* CreatePointCloudWithScalars(int numPoints, double spacing = 1.0);
    vtkPolyData* CreatePointCloudWithMultipleArrays(int numPoints, double spacing = 1.0);
    vtkPlane* CreatePlane(double originX, double originY, double originZ, 
                          double normalX, double normalY, double normalZ);

    // Utility functions
    bool PointsEqual(double *v0, double *v1, double tolerance = 1.e-5);
    bool PointInPlane(double *point, double *origin, double *normal, double tolerance = 1.e-5);
    double DistancePointToPlane(double *point, double *origin, double *normal);

    // Data members
    vtkALBAPointCloudCutter *m_Cutter;
    vtkPolyData *m_InputPolydata;
    vtkPolyData *m_OutputPolydata;
};

#endif