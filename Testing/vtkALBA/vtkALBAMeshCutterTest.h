/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAMeshCutterTest
 Authors: Nigel McFarlane, Roberto Mucci
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_vtkALBAMeshCutterTest_H__
#define __CPP_UNIT_vtkALBAMeshCutterTest_H__

#include "albaTest.h"

class vtkUnstructuredGrid;
class vtkALBAMeshCutter;


//------------------------------------------------------------------------------
// Test class for vtkALBAMeshCutter
//------------------------------------------------------------------------------
class vtkALBAMeshCutterTest : public albaTest
{
  public:

    CPPUNIT_TEST_SUITE( vtkALBAMeshCutterTest );
    CPPUNIT_TEST( TestFixture );
    CPPUNIT_TEST( TestGetOutputHex8 );
    CPPUNIT_TEST( TestGetOutputHex8_VerticalCut1 );
    CPPUNIT_TEST( TestGetOutputHex8_VerticalCut2 );
    CPPUNIT_TEST( TestGetOutputHex8_FaceInPlane );
    CPPUNIT_TEST( TestGetOutputHex8_IncludesEdge1 );
    CPPUNIT_TEST( TestGetOutputHex8_IncludesEdge2 );
    CPPUNIT_TEST( TestGetOutputHex8_IncludesCorner );
    CPPUNIT_TEST( TestGetOutputHex8_EdgeOnly );
    CPPUNIT_TEST( TestGetOutputHex8_CornerOnly );
    CPPUNIT_TEST( TestGetOutputTet4 );
    CPPUNIT_TEST( TestUpdateChangeCutFunction );
    CPPUNIT_TEST( TestUpdateChangeInput );
    CPPUNIT_TEST_SUITE_END();

  protected:

    void Test();
    void TestFixture();
    void TestGetOutputHex8();
    void TestGetOutputHex8_VerticalCut1();
    void TestGetOutputHex8_VerticalCut2();
    void TestGetOutputHex8_FaceInPlane();
    void TestGetOutputHex8_IncludesEdge1();
    void TestGetOutputHex8_IncludesEdge2();
    void TestGetOutputHex8_IncludesCorner();
    void TestGetOutputHex8_EdgeOnly();
    void TestGetOutputHex8_CornerOnly();
    void TestGetOutputTet4();
    void TestUpdateChangeCutFunction();
    void TestUpdateChangeInput() ;

    // return true if v = (1-lambda)*v0 + lambda*v1
    bool ColinearVectors(double *v, double *v0, double *v1, double lambda, double ftol) ;

    // return true if point is in the plane
    bool PointInPlane(double *v, double *origin, double *norm, double ftol) ;

    // are two points equal
    bool PointsEqual(double *v, double *v0, double ftol) ;

    // render input and output data
    void RenderPointScalars(vtkUnstructuredGrid *UG, vtkPolyData *polydata) ;
    void RenderCellScalars(vtkUnstructuredGrid *UG, vtkPolyData *polydata) ;

    // test the interpolation of scalars
    void ScalarTest(vtkALBAMeshCutter *MMC, vtkUnstructuredGrid *UG, vtkPolyData *polydata) ;
};

#endif
