/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEWrappedMeterTest
 Authors: Daniele Giunchi
 
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

#include "albaVMEWrappedMeterTest.h"
#include "albaVMEWrappedMeter.h"

#include "albaVMESurfaceParametric.h"
#include "albaVMELandmarkCloud.h"
#include "albaVMELandmark.h"
#include "albaOpImporterLandmark.h"
#include "vtkMath.h"
#include "albaVMERoot.h"
#include "albaVMEStorage.h"
#include "albaString.h"


#include "vtkDataSet.h"
#include "vtkPolyData.h"
#include "vtkImageData.h"
#include "vtkLookupTable.h"

enum ENUM_TEST_AXIS
{
	X = 0,
	Y,
	Z,
};


//---------------------------------------------------------------
void albaVMEWrappedMeterTest::TestWithGenericVME()
//---------------------------------------------------------------
{
	albaVMEStorage *storage = albaVMEStorage::New();
	storage->GetRoot()->SetName("root");
	storage->GetRoot()->Initialize();


	//create a parametric surface
	albaVMESurfaceParametric *vmeParametricSurfaceSTART;
	albaNEW(vmeParametricSurfaceSTART);	
	vmeParametricSurfaceSTART->ReparentTo(storage->GetRoot());
	vmeParametricSurfaceSTART->Update();

	albaVMESurfaceParametric *vmeParametricSurfaceEND1;
	albaNEW(vmeParametricSurfaceEND1);	
	vmeParametricSurfaceEND1->ReparentTo(storage->GetRoot());
	vmeParametricSurfaceEND1->Update();

	
	albaMatrix matrix;
	matrix.SetElement(X,3,4); //set a translation value on x axis of 4.0
	matrix.SetElement(Y,3,3); //set a translation value on x axis of 3.0
	vmeParametricSurfaceEND1->SetAbsMatrix(matrix);

	albaVMEWrappedMeter *wrappedMeter;
	albaNEW(wrappedMeter);
	
	wrappedMeter->SetMeterLink("StartVME",vmeParametricSurfaceSTART);
	wrappedMeter->SetMeterLink("EndVME1",vmeParametricSurfaceEND1);
	wrappedMeter->ReparentTo(storage->GetRoot());
	wrappedMeter->Modified();
	wrappedMeter->Update();

	wrappedMeter->GetEnd1VME()->Modified();
	wrappedMeter->GetEnd1VME()->Update();

	
	CPPUNIT_ASSERT(wrappedMeter->GetDistance() == 5.0);


	wrappedMeter->ReparentTo(NULL);
	vmeParametricSurfaceSTART->ReparentTo(NULL);
	vmeParametricSurfaceEND1->ReparentTo(NULL);


	albaDEL(wrappedMeter);
	albaDEL(vmeParametricSurfaceSTART);
	albaDEL(vmeParametricSurfaceEND1);

	albaDEL(storage);
}

//---------------------------------------------------------------
void albaVMEWrappedMeterTest::TestWithGenericVMEWithMiddlePoints()
//---------------------------------------------------------------
{
	albaVMEStorage *storage = albaVMEStorage::New();
	storage->GetRoot()->SetName("root");
	storage->GetRoot()->Initialize();


	//create a parametric surface
	albaVMESurfaceParametric *vmeParametricSurfaceSTART;
	albaNEW(vmeParametricSurfaceSTART);	
	vmeParametricSurfaceSTART->ReparentTo(storage->GetRoot());
	vmeParametricSurfaceSTART->Update();

	albaVMESurfaceParametric *vmeParametricSurfaceEND1;
	albaNEW(vmeParametricSurfaceEND1);	
	vmeParametricSurfaceEND1->ReparentTo(storage->GetRoot());
	vmeParametricSurfaceEND1->Update();

	albaVMESurfaceParametric *vmeParametricSurfaceMiddlePoint;
	albaNEW(vmeParametricSurfaceMiddlePoint);
	vmeParametricSurfaceMiddlePoint->SetName("MiddlePoint");
	vmeParametricSurfaceMiddlePoint->ReparentTo(storage->GetRoot());
	vmeParametricSurfaceMiddlePoint->Update();


	albaMatrix matrix;
	matrix.SetElement(X,3,4); //set a translation value on x axis of 4.0
	matrix.SetElement(Y,3,3); //set a translation value on x axis of 3.0
	vmeParametricSurfaceMiddlePoint->SetAbsMatrix(matrix);


	albaVMEWrappedMeter *wrappedMeter;
	albaNEW(wrappedMeter);

	wrappedMeter->SetMeterLink("StartVME",vmeParametricSurfaceSTART);
	wrappedMeter->SetMeterLink("EndVME1",vmeParametricSurfaceEND1);
	
	wrappedMeter->SetMeterLink(vmeParametricSurfaceMiddlePoint->GetName(),vmeParametricSurfaceMiddlePoint);
	//wrappedMeter->PushIdVector(vmeParametricSurfaceMiddlePoint->GetId()); //this is for the vector syncronized with the gui widget, that is not used in gui test
	
	wrappedMeter->ReparentTo(storage->GetRoot());
	wrappedMeter->Modified();
	wrappedMeter->Update();

	wrappedMeter->GetEnd1VME()->Modified();
	wrappedMeter->GetEnd1VME()->Update();

	CPPUNIT_ASSERT(wrappedMeter->GetDistance() == 10.0);


	wrappedMeter->ReparentTo(NULL);
	vmeParametricSurfaceSTART->ReparentTo(NULL);
	vmeParametricSurfaceEND1->ReparentTo(NULL);
	vmeParametricSurfaceMiddlePoint->ReparentTo(NULL);


	albaDEL(wrappedMeter);
	albaDEL(vmeParametricSurfaceSTART);
	albaDEL(vmeParametricSurfaceEND1);
	albaDEL(vmeParametricSurfaceMiddlePoint);

	albaDEL(storage);
}

//---------------------------------------------------------------
void albaVMEWrappedMeterTest::TestWithGenericLandmark()
//---------------------------------------------------------------
{
	albaVMEStorage *storage = albaVMEStorage::New();
	storage->GetRoot()->SetName("root");
	storage->GetRoot()->Initialize();


	//create landmarks and relative landmark cloud
	albaOpImporterLandmark *importer=new albaOpImporterLandmark("importer");
	importer->TestModeOn();
	importer->SetInput(storage->GetRoot());
	albaString filename=ALBA_DATA_ROOT;
	filename<<"/RAW_MAL/cloud_to_be_imported";
	importer->SetFileName(filename.GetCStr());
	importer->SetTypeSeparation(3);
	importer->Read();
	albaVMELandmarkCloud *cloud=(albaVMELandmarkCloud *)importer->GetOutput();
	cloud->GetLandmark(0)->Update();
	cloud->GetLandmark(1)->Update();

	cloud->ReparentTo(storage->GetRoot());

	albaVMEWrappedMeter *wrappedMeter;
	albaNEW(wrappedMeter);
  

	wrappedMeter->SetMeterLink("StartVME",cloud->GetLandmark(0));
	wrappedMeter->SetMeterLink("EndVME1",cloud->GetLandmark(1));

	wrappedMeter->ReparentTo(storage->GetRoot());
	wrappedMeter->Modified();
	wrappedMeter->Update();

  double l1[3], l2[3]; 
  cloud->GetLandmark(0)->GetPoint(l1);
	cloud->GetLandmark(1)->GetPoint(l2);
  printf("\n%.2f %.2f %.2f\n", l1[0] , l1[1], l1[2]);
	printf("\n%.2f %.2f %.2f\n", l2[0] , l2[1], l2[2]);
	
	CPPUNIT_ASSERT(wrappedMeter->GetDistance() >= 8.33 &&
		             wrappedMeter->GetDistance() <= 8.34    );


  wrappedMeter->ReparentTo(NULL);
  cloud->ReparentTo(NULL);


  albaDEL(wrappedMeter);
  albaDEL(importer);

  albaDEL(storage);
}

//---------------------------------------------------------------
void albaVMEWrappedMeterTest::TestWithGenericLandmarkWithMiddlePoints()
//---------------------------------------------------------------
{
	albaVMEStorage *storage = albaVMEStorage::New();
	storage->GetRoot()->SetName("root");
	storage->GetRoot()->Initialize();


	//create landmarks and relative landmark cloud
	albaOpImporterLandmark *importer=new albaOpImporterLandmark("importer");
	importer->TestModeOn();
	importer->SetInput(storage->GetRoot());
	albaString filename=ALBA_DATA_ROOT;
	filename<<"/RAW_MAL/cloud_to_be_imported";
	importer->SetFileName(filename.GetCStr());
	importer->SetTypeSeparation(3);
	importer->Read();
	albaVMELandmarkCloud *cloud=(albaVMELandmarkCloud *)importer->GetOutput();
	cloud->GetLandmark(0)->Update();
	cloud->GetLandmark(1)->Update();

	cloud->ReparentTo(storage->GetRoot());

	albaVMEWrappedMeter *wrappedMeter;
	albaNEW(wrappedMeter);


	wrappedMeter->SetMeterLink("StartVME",cloud->GetLandmark(0));
	wrappedMeter->SetMeterLink("EndVME1",cloud->GetLandmark(1));

	wrappedMeter->SetMeterLink(cloud->GetLandmark(2)->GetName(),cloud->GetLandmark(2));
	//wrappedMeter->PushIdVector(cloud->GetId()); // for landmark middlepoint is memorized as sequence of cloud id and interal id of the landmark
	//wrappedMeter->PushIdVector(2); //this is for the vector syncronized with the gui widget, that is not used in gui test


	wrappedMeter->ReparentTo(storage->GetRoot());
	wrappedMeter->Modified();
	wrappedMeter->Update();

	double l1[3], l2[3], lm[3]; 
	cloud->GetLandmark(0)->GetPoint(l1);
	cloud->GetLandmark(1)->GetPoint(l2);
	cloud->GetLandmark(2)->GetPoint(lm);

	/*printf("\n%.2f %.2f %.2f\n", l1[0] , l1[1], l1[2]);
	printf("\n%.2f %.2f %.2f\n", lm[0] , lm[1], lm[2]);
	printf("\n%.2f %.2f %.2f\n", l2[0] , l2[1], l2[2]);*/

	//printf("\n%.2f\n", wrappedMeter->GetDistance());

	//double distance;
	//distance = sqrt(vtkMath::Distance2BetweenPoints(l1,lm)) + sqrt(vtkMath::Distance2BetweenPoints(lm,l2));

	//printf("\n%.2f\n", distance);

	CPPUNIT_ASSERT(wrappedMeter->GetDistance() < 23.09 &&
		             wrappedMeter->GetDistance() > 23.08);


	wrappedMeter->ReparentTo(NULL);
	cloud->ReparentTo(NULL);


	albaDEL(wrappedMeter);
	albaDEL(importer);

	albaDEL(storage);
}
//---------------------------------------------------------------
void albaVMEWrappedMeterTest::TestWrappedGeometry()
//---------------------------------------------------------------
{
  albaVMEStorage *storage = albaVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();


  //create a parametric surface
  albaVMESurfaceParametric *vmeParametricSurfaceSTART;
  albaNEW(vmeParametricSurfaceSTART);	
  vmeParametricSurfaceSTART->ReparentTo(storage->GetRoot());
  vmeParametricSurfaceSTART->Update();

  albaVMESurfaceParametric *vmeParametricSurfaceEND1;
  albaNEW(vmeParametricSurfaceEND1);	
  vmeParametricSurfaceEND1->ReparentTo(storage->GetRoot());
  vmeParametricSurfaceEND1->Update();

  albaVMESurfaceParametric *vmeParametricSurfaceWrapped;
  albaNEW(vmeParametricSurfaceWrapped);	
  vmeParametricSurfaceWrapped->ReparentTo(storage->GetRoot());
  vmeParametricSurfaceWrapped->Update();


  albaMatrix matrix1;
  matrix1.SetElement(X,3,-10); //set a translation value on x axis of -10.0
  matrix1.SetElement(Y,3,1.5); //set a translation value on y axis of 1.5
	vmeParametricSurfaceSTART->SetAbsMatrix(matrix1);

  albaMatrix matrix2;
  matrix2.SetElement(X,3,10); //set a translation value on x axis of 10.0
  matrix2.SetElement(Y,3,1.5); //set a translation value on y axis of 1.5
	vmeParametricSurfaceEND1->SetAbsMatrix(matrix2);

  //this create 3 spheres, o--  10 --  -- 10 -- o , total distance is 10, and tangent points are the same
  //                                  o  <-- this is wrapped surface

  albaVMEWrappedMeter *wrappedMeter;
  albaNEW(wrappedMeter);

  wrappedMeter->SetMeterLink("StartVME",vmeParametricSurfaceSTART);
  wrappedMeter->SetMeterLink("EndVME1",vmeParametricSurfaceEND1);
  wrappedMeter->SetMeterLink("WrappedVME",vmeParametricSurfaceWrapped);
  wrappedMeter->SetWrappedMode(albaVMEWrappedMeter::AUTOMATED_WRAP);
  wrappedMeter->ReparentTo(storage->GetRoot());
  
  wrappedMeter->Modified();
  wrappedMeter->Update();


  printf("\ndist:%.2f\n", wrappedMeter->GetDistance());
  CPPUNIT_ASSERT(wrappedMeter->GetDistance() > 21.5 && wrappedMeter->GetDistance() < 21.6);
	//CPPUNIT_ASSERT(wrappedMeter->GetDistance() == 20);

  CPPUNIT_ASSERT(wrappedMeter->GetWrappedGeometryTangent1()[0] - wrappedMeter->GetWrappedGeometryTangent2()[0] < 0.001 && //x is opposite
                 fabs(wrappedMeter->GetWrappedGeometryTangent1()[1]- wrappedMeter->GetWrappedGeometryTangent2()[1]) < 0.001 &&
                 fabs(wrappedMeter->GetWrappedGeometryTangent1()[2]- wrappedMeter->GetWrappedGeometryTangent2()[2]) < 0.001);


  wrappedMeter->ReparentTo(NULL);


  albaVMEWrappedMeter *wrappedMeter2;
  albaNEW(wrappedMeter2);

  wrappedMeter2->SetMeterLink("StartVME",vmeParametricSurfaceSTART);
  wrappedMeter2->SetMeterLink("EndVME1",vmeParametricSurfaceEND1);
  wrappedMeter2->SetMeterLink("WrappedVME",vmeParametricSurfaceWrapped);
  wrappedMeter2->SetWrappedMode(albaVMEWrappedMeter::AUTOMATED_WRAP);
  wrappedMeter2->ReparentTo(storage->GetRoot());

  wrappedMeter2->SetWrapSide(1);
  wrappedMeter2->Modified();
  wrappedMeter2->Update();


  printf("\ndist:%.2f\n", wrappedMeter->GetDistance());
  

  printf("\ndist:%.2f\n", wrappedMeter->GetDistance());
  CPPUNIT_ASSERT(wrappedMeter2->GetDistance() > 20.0);

  CPPUNIT_ASSERT(wrappedMeter2->GetWrappedGeometryTangent1()[0] - wrappedMeter2->GetWrappedGeometryTangent2()[0] < 0.001 && //x is opposite
    fabs(wrappedMeter2->GetWrappedGeometryTangent1()[1]- wrappedMeter2->GetWrappedGeometryTangent2()[1]) < 0.001 &&
    fabs(wrappedMeter2->GetWrappedGeometryTangent1()[2]- wrappedMeter2->GetWrappedGeometryTangent2()[2]) < 0.001);


  wrappedMeter->ReparentTo(NULL);
  wrappedMeter2->ReparentTo(NULL);


  vmeParametricSurfaceSTART->ReparentTo(NULL);
  vmeParametricSurfaceEND1->ReparentTo(NULL);
  vmeParametricSurfaceWrapped->ReparentTo(NULL);


  albaDEL(wrappedMeter);
  albaDEL(wrappedMeter2);
  albaDEL(vmeParametricSurfaceSTART);
  albaDEL(vmeParametricSurfaceEND1);
  albaDEL(vmeParametricSurfaceWrapped);

  albaDEL(storage);
}
