/*=========================================================================

 Program: MAF2
 Module: mafVMELandmarkCloudTest
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

#include "mafVMELandmarkCloudTest.h"
#include <wx/dir.h>

#include "mafVMELandmark.h"
#include "mafVMELandmarkCloud.h"
//#include "mafMatrix.h"

#include <iostream>
#include <set>

//----------------------------------------------------------------------------
void mafVMELandmarkCloudTest::DynamicAllocationTest()
//----------------------------------------------------------------------------
{
  mafVMELandmarkCloud *cloud;
  mafNEW(cloud);
  mafDEL(cloud);
}

//----------------------------------------------------------------------------
void mafVMELandmarkCloudTest::AppendLandmarkTest()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMELandmarkCloud> cloud;
  mafSmartPointer<mafVMELandmark> landmark;
  double x = 5;
  double y = 10;
  double z = 15.5;
  mafString name = "land";

  cloud->TestModeOn();
  cloud->AppendLandmark(x, y, z, name.GetCStr());

  cloud->Open();
  landmark = cloud->GetLandmark(name);

  bool result = false;
  //Check if landmark has been appended
  result = (name.Compare(landmark->GetName()) == 0);
  CPPUNIT_ASSERT(result);

  mafSmartPointer<mafVMELandmarkCloud> cloud1;
  mafSmartPointer<mafVMELandmark> landmark1;

  landmark1->SetName(name);
  landmark1->SetPoint(z,y,z);

  cloud1->TestModeOn();
  cloud1->AppendLandmark(name);
  cloud1->Open();

  //Check if landmark has been appended
  result = false;
  result = (name.Compare(cloud1->GetLandmark(name)->GetName()) == 0);
  CPPUNIT_ASSERT(result);
}

//----------------------------------------------------------------------------
void mafVMELandmarkCloudTest::SetLandmarkTest()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMELandmarkCloud> cloud;
  mafSmartPointer<mafVMELandmark> landmark;
  double x = 5;
  double y = 10;
  double z = 15.5;
  mafString name = "land";

  landmark->SetName(name);
  landmark->SetPoint(z,y,z);

  cloud->TestModeOn();
  cloud->SetLandmark(landmark);
  cloud->Open();

  bool result = false;
  //Check if landmark has been set
  result = (name.Compare(cloud->GetLandmark(name)->GetName()) == 0);
  CPPUNIT_ASSERT(result);

  mafSmartPointer<mafVMELandmarkCloud> cloud1;
  mafSmartPointer<mafVMELandmark> landmark1;

  cloud1->TestModeOn();
  cloud1->AppendLandmark(name);
  cloud1->SetLandmark(name,x,y,z);
  cloud1->Open();

  double xyz[3];
  cloud1->GetLandmark(name, xyz);

  //Check if landmark has been set
  result = false;
  result = mafEquals(x,xyz[0]) && mafEquals(y,xyz[1]) && mafEquals(z,xyz[2]);
  CPPUNIT_ASSERT(result);

  mafSmartPointer<mafVMELandmarkCloud> cloud2;
  mafSmartPointer<mafVMELandmark> landmark2;

  cloud2->TestModeOn();
  cloud2->AppendLandmark(name);
  cloud2->SetLandmark(0,x,y,z);
  cloud2->Open();

  double xyz1[3];
  cloud2->GetLandmarkPosition(0,xyz1);

  //Check if landmark has been set
  result = false;
  result = mafEquals(x,xyz1[0]) && mafEquals(y,xyz1[1]) && mafEquals(z,xyz1[2]);
  CPPUNIT_ASSERT(result);
}

//----------------------------------------------------------------------------
void mafVMELandmarkCloudTest::FindLandmarkIndexTest()
//----------------------------------------------------------------------------
{
  double x = 5;
  double y = 10;
  double z = 15.5;
  mafString name = "land";

  mafSmartPointer<mafVMELandmarkCloud> cloud2;
  mafSmartPointer<mafVMELandmark> landmark2;

  cloud2->TestModeOn();
  cloud2->AppendLandmark(name);
  cloud2->SetLandmark(0,x,y,z);
  cloud2->Open();

  //FindLandmarkIndex
  bool result = false;
  result = mafEquals(cloud2->FindLandmarkIndex(name),0);
  CPPUNIT_ASSERT(result);
}


//----------------------------------------------------------------------------
void mafVMELandmarkCloudTest::RemoveLandmarkTest()
//----------------------------------------------------------------------------
{
  double x = 5;
  double y = 10;
  double z = 15.5;
  mafString name = "land";

  mafSmartPointer<mafVMELandmarkCloud> cloud;
  mafSmartPointer<mafVMELandmark> landmark;

  cloud->TestModeOn();
  cloud->AppendLandmark(name);
  cloud->SetLandmark(0,x,y,z);
  cloud->Open();

  cloud->RemoveLandmark(0);

  //Check if landmark has been removed
  bool result = false;
  result = mafEquals(cloud->GetNumberOfChildren(),0);
  CPPUNIT_ASSERT(result);
}

//----------------------------------------------------------------------------
void mafVMELandmarkCloudTest::SetNumberOfLandmarksTest()
//----------------------------------------------------------------------------
{
  int number;

  mafSmartPointer<mafVMELandmarkCloud> cloud;
  mafSmartPointer<mafVMELandmark> landmark;

  cloud->SetNumberOfLandmarks(10);
  number = cloud->GetNumberOfLandmarks();

  //Check if landmark number is correct
  bool result = false;
  result = mafEquals(number,10);
  CPPUNIT_ASSERT(result);
}

//----------------------------------------------------------------------------
void mafVMELandmarkCloudTest::GetLandmarkNameTest()
//----------------------------------------------------------------------------
{
  double x = 5;
  double y = 10;
  double z = 15.5;
  mafString name = "land";

  mafSmartPointer<mafVMELandmarkCloud> cloud;
  mafSmartPointer<mafVMELandmark> landmark;

  cloud->TestModeOn();
  cloud->AppendLandmark("");
  cloud->SetLandmark(0,x,y,z);
  cloud->SetLandmarkName(0, name);
  cloud->Open();

  mafString nameRet = cloud->GetLandmarkName(0);

  //Check if landmark name is correct
  bool result = false;
  result = (nameRet.Compare(name) == 0);
  CPPUNIT_ASSERT(result);
}

//----------------------------------------------------------------------------
void mafVMELandmarkCloudTest::SetRadiusTest()
//----------------------------------------------------------------------------
{
  double x = 5;
  double y = 10;
  double z = 15.5;
  double radius = 6;
  mafString name = "land";

  mafSmartPointer<mafVMELandmarkCloud> cloud;
  mafSmartPointer<mafVMELandmark> landmark;

  cloud->TestModeOn();
  cloud->AppendLandmark(name);
  cloud->SetLandmark(0,x,y,z);
  cloud->SetRadius(radius);
  cloud->Open();

  //Check if landmark radius has been set
  bool result = false;
  result = mafEquals(cloud->GetRadius(),radius);
  CPPUNIT_ASSERT(result);
}

//----------------------------------------------------------------------------
void mafVMELandmarkCloudTest::SetSphereResolutionTest()
//----------------------------------------------------------------------------
{
  double x = 5;
  double y = 10;
  double z = 15.5;
  double res = 30;
  mafString name = "land";

  mafSmartPointer<mafVMELandmarkCloud> cloud;
  mafSmartPointer<mafVMELandmark> landmark;

  cloud->TestModeOn();
  cloud->AppendLandmark(name);
  cloud->SetLandmark(0,x,y,z);
  cloud->SetSphereResolution(res);
  cloud->Open();

  //Check if landmark resolution has been set
  bool result = false;
  result = mafEquals(cloud->GetSphereResolution(),res);
  CPPUNIT_ASSERT(result);
}

//----------------------------------------------------------------------------
void mafVMELandmarkCloudTest::DeepCopyTest()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMELandmarkCloud> landmarkCloud;
  mafSmartPointer<mafVMELandmarkCloud> newLandmarkCloud;
 
  double rad = 10;
  double res = 30;
  int number = 5;
  
  landmarkCloud->TestModeOn();
  landmarkCloud->SetNumberOfLandmarks(number);
  landmarkCloud->SetRadius(rad);
  landmarkCloud->SetSphereResolution(res);
  

  newLandmarkCloud->DeepCopy(landmarkCloud);
  

  bool result = false;
  result = newLandmarkCloud->Equals(landmarkCloud);
  CPPUNIT_ASSERT(result);


  //Check if landmark cloud has been copied
  result = false;
  result = mafEquals(newLandmarkCloud->GetRadius(),rad);
  CPPUNIT_ASSERT(result);

  result = false;
  result = mafEquals(newLandmarkCloud->GetSphereResolution(),res);
  CPPUNIT_ASSERT(result);

  result = false;
  result = mafEquals(newLandmarkCloud->GetNumberOfLandmarks(),number);
  CPPUNIT_ASSERT(result);
}
