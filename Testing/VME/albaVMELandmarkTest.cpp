/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMELandmarkTest
 Authors: Roberto Mucci
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>

#include "albaVMELandmarkTest.h"
#include <wx/dir.h>

#include "albaVMELandmark.h"
#include "albaVMELandmarkCloud.h"
#include "albaMatrix.h"

#include <iostream>
#include <set>


//----------------------------------------------------------------------------
void albaVMELandmarkTest::DynamicAllocationTest()
//----------------------------------------------------------------------------
{
  albaVMELandmark *landmark;
  albaNEW(landmark);
  albaDEL(landmark);
}

//----------------------------------------------------------------------------
void albaVMELandmarkTest::SetTimeStampTest()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMELandmark> landmark;

  albaTimeStamp time = 4;
  landmark->SetTimeStamp(time);

  albaTimeStamp timeRet = -1;
  
  timeRet = landmark->GetTimeStamp();

  bool result = false;
  //Check if time is correctly set
  result = albaEquals(time, timeRet);
  CPPUNIT_ASSERT(result);
}

//----------------------------------------------------------------------------
void albaVMELandmarkTest::SetPointTest()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMELandmark> landmark;

  double points[] = {2,200.2,50}; 
  albaTimeStamp t = 5; 

  //SetPoint with array of double
  landmark->SetPoint(points, t);

  double pointsRet[3];
  landmark->GetPoint(pointsRet, t);

  bool result = false;
  //Check if point is correctly set
  result = (albaEquals(points[0], pointsRet[0]) && albaEquals(points[1], pointsRet[1] ) && albaEquals(points[2], pointsRet[2]));
  CPPUNIT_ASSERT(result);
}

//----------------------------------------------------------------------------
void albaVMELandmarkTest::SetPointTest2()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMELandmark> landmark;

  double point1 = 5;
  double point2 = 100.2;
  double point3 = -12; 
  albaTimeStamp t = 5; 

  //SetPoint with 3 double
  landmark->SetPoint(point1,point2,point3, t);

  double pointRet1 = 0;
  double pointRet2 = 0;
  double pointRet3 = 0;
  landmark->GetPoint(pointRet1,pointRet2,pointRet3, t);

  bool result = false;
  //Check if point is correctly set
  result = (albaEquals(point1, pointRet1) && albaEquals(point2, pointRet2 ) && albaEquals(point3, pointRet3));
  CPPUNIT_ASSERT(result);
}

//----------------------------------------------------------------------------
void albaVMELandmarkTest::SetRadiusTest()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMELandmarkCloud> landmarkCloud;
  albaSmartPointer<albaVMELandmark> landmark;

  landmarkCloud->TestModeOn();
  landmarkCloud->AddChild(landmark);

  double radius = 20;
  landmark->SetRadius(radius);

  bool result = false;
  //Check if radius is correctly set
  result = albaEquals(radius, landmark->GetRadius());
  CPPUNIT_ASSERT(result);
}

//----------------------------------------------------------------------------
void albaVMELandmarkTest::SetSphereResolutionTest()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMELandmarkCloud> landmarkCloud;
  albaSmartPointer<albaVMELandmark> landmark;

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
void albaVMELandmarkTest::SetLandmarkVisibilityTest()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMELandmark> landmark;

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
void albaVMELandmarkTest::CanReparentToTest()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMELandmarkCloud> landmarkCloud;
  albaSmartPointer<albaVMELandmark> landmark;

  landmarkCloud->TestModeOn();

  bool result = false;
  //Check if landmark can be reparent under an open landmark cloud
  result = landmark->CanReparentTo(landmarkCloud);
  CPPUNIT_ASSERT(result);
	
  albaSmartPointer<albaVMELandmark> newLandmark;

  //Check if landmark can be reparent under a landmark
  result = landmark->CanReparentTo(newLandmark);
  CPPUNIT_ASSERT(!result);
}

//----------------------------------------------------------------------------
void albaVMELandmarkTest::SetMatrixTest()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMELandmark> landmark;
  albaMatrix mat;

  mat.SetTimeStamp(2);
  mat.SetElement(0, 3, 5);
  mat.SetElement(1, 3, 20);
  mat.SetElement(2, 3, -5);

  landmark->SetMatrix(mat);

  albaSmartPointer<albaMatrix> matRet;
  landmark->SetTimeStamp(2);
  matRet = landmark->GetOutput()->GetMatrix();

  bool result = false;
  result = mat.Equals(matRet);
  //Check if matrix is correctly set
  CPPUNIT_ASSERT(result);
}

//----------------------------------------------------------------------------
void albaVMELandmarkTest::DeepCopyTest()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMELandmarkCloud> landmarkCloud;
  albaSmartPointer<albaVMELandmarkCloud> newLandmarkCloud;
  albaSmartPointer<albaVMELandmark> landmark;
  albaSmartPointer<albaVMELandmark> newLandmark;

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
