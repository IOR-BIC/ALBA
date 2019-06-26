/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMELandmarkCloudTest
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

#include "albaVMELandmarkCloudTest.h"
#include <wx/dir.h>

#include "albaVMELandmark.h"
#include "albaVMELandmarkCloud.h"
//#include "albaMatrix.h"

#include <iostream>
#include <set>

//----------------------------------------------------------------------------
void albaVMELandmarkCloudTest::DynamicAllocationTest()
//----------------------------------------------------------------------------
{
  albaVMELandmarkCloud *cloud;
  albaNEW(cloud);
  albaDEL(cloud);
}

//----------------------------------------------------------------------------
void albaVMELandmarkCloudTest::AppendLandmarkTest()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMELandmarkCloud> cloud;
  albaSmartPointer<albaVMELandmark> landmark;
  double x = 5;
  double y = 10;
  double z = 15.5;
  albaString name = "land";

  cloud->TestModeOn();
  cloud->AppendLandmark(x, y, z, name.GetCStr());

  landmark = cloud->GetLandmark(name);

  bool result = false;
  //Check if landmark has been appended
  result = (name.Compare(landmark->GetName()) == 0);
  CPPUNIT_ASSERT(result);

  albaSmartPointer<albaVMELandmarkCloud> cloud1;
  albaSmartPointer<albaVMELandmark> landmark1;

  landmark1->SetName(name);
  landmark1->SetPoint(z,y,z);

  cloud1->TestModeOn();
  cloud1->AppendLandmark(name);

  //Check if landmark has been appended
  result = false;
  result = (name.Compare(cloud1->GetLandmark(name)->GetName()) == 0);
  CPPUNIT_ASSERT(result);
}

//----------------------------------------------------------------------------
void albaVMELandmarkCloudTest::SetLandmarkTest()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMELandmarkCloud> cloud;
  albaSmartPointer<albaVMELandmark> landmark;
  double x = 5;
  double y = 10;
  double z = 15.5;
  albaString name = "land";

  landmark->SetName(name);
  landmark->SetPoint(z,y,z);

  cloud->TestModeOn();
	landmark->ReparentTo(cloud);

  bool result = false;
  //Check if landmark has been set
  result = (name.Compare(cloud->GetLandmark(name)->GetName()) == 0);
  CPPUNIT_ASSERT(result);

  albaSmartPointer<albaVMELandmarkCloud> cloud1;
  albaSmartPointer<albaVMELandmark> landmark1;

  cloud1->TestModeOn();
  cloud1->AppendLandmark(name);
  cloud1->SetLandmark(name,x,y,z);

  double xyz[3];
  cloud1->GetLandmark(name, xyz);

  //Check if landmark has been set
  result = false;
  result = albaEquals(x,xyz[0]) && albaEquals(y,xyz[1]) && albaEquals(z,xyz[2]);
  CPPUNIT_ASSERT(result);

  albaSmartPointer<albaVMELandmarkCloud> cloud2;
  albaSmartPointer<albaVMELandmark> landmark2;

  cloud2->TestModeOn();
  cloud2->AppendLandmark(name);
  cloud2->SetLandmark(0,x,y,z);

  double xyz1[3];
  cloud2->GetLandmarkPosition(0,xyz1);

  //Check if landmark has been set
  result = false;
  result = albaEquals(x,xyz1[0]) && albaEquals(y,xyz1[1]) && albaEquals(z,xyz1[2]);
  CPPUNIT_ASSERT(result);
}

//----------------------------------------------------------------------------
void albaVMELandmarkCloudTest::FindLandmarkIndexTest()
//----------------------------------------------------------------------------
{
  double x = 5;
  double y = 10;
  double z = 15.5;
  albaString name = "land";

  albaSmartPointer<albaVMELandmarkCloud> cloud2;
  albaSmartPointer<albaVMELandmark> landmark2;

  cloud2->TestModeOn();
  cloud2->AppendLandmark(name);
  cloud2->SetLandmark(0,x,y,z);

  //FindLandmarkIndex
  bool result = false;
  result = albaEquals(cloud2->GetLandmarkIndex(name),0);
  CPPUNIT_ASSERT(result);
}


//----------------------------------------------------------------------------
void albaVMELandmarkCloudTest::RemoveLandmarkTest()
//----------------------------------------------------------------------------
{
  double x = 5;
  double y = 10;
  double z = 15.5;
  albaString name = "land";

  albaSmartPointer<albaVMELandmarkCloud> cloud;
  albaSmartPointer<albaVMELandmark> landmark;

  cloud->TestModeOn();
  cloud->AppendLandmark(name);
  cloud->SetLandmark(0,x,y,z);

  cloud->RemoveLandmark(0);

  //Check if landmark has been removed
  bool result = false;
  result = albaEquals(cloud->GetNumberOfChildren(),0);
  CPPUNIT_ASSERT(result);
}

//----------------------------------------------------------------------------
void albaVMELandmarkCloudTest::SetNumberOfLandmarksTest()
//----------------------------------------------------------------------------
{
  int number;

  albaSmartPointer<albaVMELandmarkCloud> cloud;
  albaSmartPointer<albaVMELandmark> landmark;

  cloud->SetNumberOfLandmarks(10);
  number = cloud->GetNumberOfLandmarks();

  //Check if landmark number is correct
  bool result = false;
  result = albaEquals(number,10);
  CPPUNIT_ASSERT(result);
}

//----------------------------------------------------------------------------
void albaVMELandmarkCloudTest::GetLandmarkNameTest()
//----------------------------------------------------------------------------
{
  double x = 5;
  double y = 10;
  double z = 15.5;
  albaString name = "land";

  albaSmartPointer<albaVMELandmarkCloud> cloud;
  albaSmartPointer<albaVMELandmark> landmark;

  cloud->TestModeOn();
  cloud->AppendLandmark("");
  cloud->SetLandmark(0,x,y,z);
  cloud->SetLandmarkName(0, name);

  albaString nameRet = cloud->GetLandmarkName(0);

  //Check if landmark name is correct
  bool result = false;
  result = (nameRet.Compare(name) == 0);
  CPPUNIT_ASSERT(result);
}

//----------------------------------------------------------------------------
void albaVMELandmarkCloudTest::SetRadiusTest()
//----------------------------------------------------------------------------
{
  double x = 5;
  double y = 10;
  double z = 15.5;
  double radius = 6;
  albaString name = "land";

  albaSmartPointer<albaVMELandmarkCloud> cloud;
  albaSmartPointer<albaVMELandmark> landmark;

  cloud->TestModeOn();
  cloud->AppendLandmark(name);
  cloud->SetLandmark(0,x,y,z);
  cloud->SetRadius(radius);

  //Check if landmark radius has been set
  bool result = false;
  result = albaEquals(cloud->GetRadius(),radius);
  CPPUNIT_ASSERT(result);
}

//----------------------------------------------------------------------------
void albaVMELandmarkCloudTest::SetSphereResolutionTest()
//----------------------------------------------------------------------------
{
  double x = 5;
  double y = 10;
  double z = 15.5;
  double res = 30;
  albaString name = "land";

  albaSmartPointer<albaVMELandmarkCloud> cloud;
  albaSmartPointer<albaVMELandmark> landmark;

  cloud->TestModeOn();
  cloud->AppendLandmark(name);
  cloud->SetLandmark(0,x,y,z);
  cloud->SetSphereResolution(res);

  //Check if landmark resolution has been set
  bool result = false;
  result = albaEquals(cloud->GetSphereResolution(),res);
  CPPUNIT_ASSERT(result);
}

//----------------------------------------------------------------------------
void albaVMELandmarkCloudTest::DeepCopyTest()
//----------------------------------------------------------------------------
{
	albaSmartPointer<albaVMELandmarkCloud> landmarkCloud;
	albaSmartPointer<albaVMELandmark> landmark1;
	albaSmartPointer<albaVMELandmark> landmark2;

  albaSmartPointer<albaVMELandmarkCloud> newLandmarkCloud;
 
  double rad = 10;
  double res = 30;

	landmark1->SetName("1");
	landmark2->SetName("2");

  landmarkCloud->TestModeOn();
  landmarkCloud->SetRadius(rad);
  landmarkCloud->SetSphereResolution(res);
	landmark1->ReparentTo(landmarkCloud);
	landmark2->ReparentTo(landmarkCloud);


  newLandmarkCloud->DeepCopy(landmarkCloud);
  
  //Check if landmark cloud has been copied
  bool result = false;
  result = albaEquals(newLandmarkCloud->GetRadius(),rad);
  CPPUNIT_ASSERT(result);

  result = false;
  result = albaEquals(newLandmarkCloud->GetSphereResolution(),res);
  CPPUNIT_ASSERT(result);

  result = false;
  result = albaEquals(newLandmarkCloud->GetNumberOfLandmarks(),0);
  CPPUNIT_ASSERT(result);
}

//----------------------------------------------------------------------------
void albaVMELandmarkCloudTest::CopyTreeTest()
{
	albaSmartPointer<albaVMELandmarkCloud> landmarkCloud;
	albaSmartPointer<albaVMELandmark> landmark1;
	albaSmartPointer<albaVMELandmark> landmark2;

	double rad = 10;
	double res = 30;

	landmark1->SetName("1");
	landmark2->SetName("2");

	landmarkCloud->TestModeOn();
	landmarkCloud->SetRadius(rad);
	landmarkCloud->SetSphereResolution(res);
	landmark1->ReparentTo(landmarkCloud);
	landmark2->ReparentTo(landmarkCloud);

	albaVMELandmarkCloud *newLandmarkCloud = (albaVMELandmarkCloud *)landmarkCloud->CopyTree();

	//Check if landmark cloud has been copied
	bool result = false;
	result = albaEquals(newLandmarkCloud->GetRadius(), rad);
	CPPUNIT_ASSERT(result);

	result = false;
	result = albaEquals(newLandmarkCloud->GetSphereResolution(), res);
	CPPUNIT_ASSERT(result);

	result = false;
	result = albaEquals(newLandmarkCloud->GetNumberOfLandmarks(), landmarkCloud->GetNumberOfLandmarks());
	CPPUNIT_ASSERT(result);
}
