/*=========================================================================

 Program: MAF2
 Module: mafVMELandmarkTest
 Authors: Roberto Mucci
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>

#include "mafVMELandmarkTest.h"
#include <wx/dir.h>

#include "mafVMELandmark.h"
#include "mafVMELandmarkCloud.h"
#include "mafMatrix.h"

#include <iostream>
#include <set>


//----------------------------------------------------------------------------
void mafVMELandmarkTest::DynamicAllocationTest()
//----------------------------------------------------------------------------
{
  mafVMELandmark *landmark;
  mafNEW(landmark);
  mafDEL(landmark);
}

//----------------------------------------------------------------------------
void mafVMELandmarkTest::SetTimeStampTest()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMELandmark> landmark;

  mafTimeStamp time = 4;
  landmark->SetTimeStamp(time);

  mafTimeStamp timeRet = -1;
  
  timeRet = landmark->GetTimeStamp();

  bool result = false;
  //Check if time is correctly set
  result = mafEquals(time, timeRet);
  CPPUNIT_ASSERT(result);
}

//----------------------------------------------------------------------------
void mafVMELandmarkTest::SetPointTest()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMELandmark> landmark;

  double points[] = {2,200.2,50}; 
  mafTimeStamp t = 5; 

  //SetPoint with array of double
  landmark->SetPoint(points, t);

  double pointsRet[3];
  landmark->GetPoint(pointsRet, t);

  bool result = false;
  //Check if point is correctly set
  result = (mafEquals(points[0], pointsRet[0]) && mafEquals(points[1], pointsRet[1] ) && mafEquals(points[2], pointsRet[2]));
  CPPUNIT_ASSERT(result);
}

//----------------------------------------------------------------------------
void mafVMELandmarkTest::SetPointTest2()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMELandmark> landmark;

  double point1 = 5;
  double point2 = 100.2;
  double point3 = -12; 
  mafTimeStamp t = 5; 

  //SetPoint with 3 double
  landmark->SetPoint(point1,point2,point3, t);

  double pointRet1 = 0;
  double pointRet2 = 0;
  double pointRet3 = 0;
  landmark->GetPoint(pointRet1,pointRet2,pointRet3, t);

  bool result = false;
  //Check if point is correctly set
  result = (mafEquals(point1, pointRet1) && mafEquals(point2, pointRet2 ) && mafEquals(point3, pointRet3));
  CPPUNIT_ASSERT(result);
}

//----------------------------------------------------------------------------
void mafVMELandmarkTest::SetRadiusTest()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMELandmarkCloud> landmarkCloud;
  mafSmartPointer<mafVMELandmark> landmark;

  landmarkCloud->TestModeOn();
  landmarkCloud->AddChild(landmark);

  double radius = 20;
  landmark->SetRadius(radius);

  bool result = false;
  //Check if radius is correctly set
  result = mafEquals(radius, landmark->GetRadius());
  CPPUNIT_ASSERT(result);
}

//----------------------------------------------------------------------------
void mafVMELandmarkTest::SetSphereResolutionTest()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMELandmarkCloud> landmarkCloud;
  mafSmartPointer<mafVMELandmark> landmark;

  landmarkCloud->TestModeOn();
  landmarkCloud->AddChild(landmark);

  int res = 30;
  landmark->SetSphereResolution(res);

  bool result = false;
  //Check if sphere resolution is correctly set
  result = (res == landmark->GetSphereResolution());
  CPPUNIT_ASSERT(result);
}

//----------------------------------------------------------------------------
void mafVMELandmarkTest::SetLandmarkVisibilityTest()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMELandmark> landmark;

  bool visibility = false;
  landmark->SetLandmarkVisibility(visibility, 5);

  bool visibilityRet = true;
  visibilityRet = landmark->GetLandmarkVisibility(5);

  bool result = false;
  //Check if visibility is correctly set
  result = (visibilityRet == visibility);
  CPPUNIT_ASSERT(result);
}

//----------------------------------------------------------------------------
void mafVMELandmarkTest::CanReparentToTest()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMELandmarkCloud> landmarkCloud;
  mafSmartPointer<mafVMELandmark> landmark;

  landmarkCloud->TestModeOn();

  bool result = false;
  //Check if landmark can be reparent under an open landmark cloud
  result = landmark->CanReparentTo(landmarkCloud);
  CPPUNIT_ASSERT(result);
	
  mafSmartPointer<mafVMELandmark> newLandmark;

  //Check if landmark can be reparent under a landmark
  result = landmark->CanReparentTo(newLandmark);
  CPPUNIT_ASSERT(!result);
}

//----------------------------------------------------------------------------
void mafVMELandmarkTest::SetMatrixTest()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMELandmark> landmark;
  mafMatrix mat;

  mat.SetTimeStamp(2);
  mat.SetElement(0, 3, 5);
  mat.SetElement(1, 3, 20);
  mat.SetElement(2, 3, -5);

  landmark->SetMatrix(mat);

  mafSmartPointer<mafMatrix> matRet;
  landmark->SetTimeStamp(2);
  matRet = landmark->GetOutput()->GetMatrix();

  bool result = false;
  result = mat.Equals(matRet);
  //Check if matrix is correctly set
  CPPUNIT_ASSERT(result);
}

//----------------------------------------------------------------------------
void mafVMELandmarkTest::DeepCopyTest()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMELandmarkCloud> landmarkCloud;
  mafSmartPointer<mafVMELandmarkCloud> newLandmarkCloud;
  mafSmartPointer<mafVMELandmark> landmark;
  mafSmartPointer<mafVMELandmark> newLandmark;

  landmarkCloud->TestModeOn();
  landmarkCloud->AddChild(landmark);

  newLandmarkCloud->TestModeOn();
  newLandmarkCloud->AddChild(newLandmark);

  landmark->SetTimeStamp(3);
  landmark->SetRadius(12);
  landmark->SetSphereResolution(20);
  landmark->SetLandmarkVisibility(true);

  newLandmark->DeepCopy(landmark);
 
  //Check is VME has been correctly copied
  bool result = newLandmark->Equals(landmark);
  CPPUNIT_ASSERT( result );
}
