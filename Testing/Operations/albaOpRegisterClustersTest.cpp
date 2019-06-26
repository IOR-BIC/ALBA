/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpRegisterClustersTest
 Authors: Alberto Losi
 
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
#include "albaOpRegisterClustersTest.h"
#include "albaOpRegisterClusters.h"
#include "albaTransform.h"
#include "albaVMEOutput.h"
#include "albaVMELandmark.h"

//----------------------------------------------------------------------------
void albaOpRegisterClustersTest::DynamicAllocationTest()
{
  albaOpRegisterClusters *op = new albaOpRegisterClusters();
  CPPUNIT_ASSERT(op != NULL);
  cppDEL(op);
}

//----------------------------------------------------------------------------
void albaOpRegisterClustersTest::CopyTest()
{
  albaOpRegisterClusters *op = new albaOpRegisterClusters();
  albaOp *op_copy = op->Copy();
  CPPUNIT_ASSERT(op_copy != NULL);
  CPPUNIT_ASSERT(op_copy->IsA("albaOpRegisterClusters"));

  cppDEL(op);
  cppDEL(op_copy);
}

//----------------------------------------------------------------------------
void albaOpRegisterClustersTest::AcceptTest()
{
  albaOpRegisterClusters *op = new albaOpRegisterClusters();
  CPPUNIT_ASSERT(op->Accept(NULL) == false);

  albaVMELandmarkCloud *lmc;
  albaNEW(lmc);

  lmc->AppendLandmark(0,0,0,"0");
  lmc->AppendLandmark(1,1,1,"1");
  int id = lmc->AppendLandmark(2,2,2,"2");

//   lmc->Open();
// 
//   // Do not accept opened landmark clouds
//   CPPUNIT_ASSERT(op->Accept(lmc) == false);


  // Accept closed not time varying landmark clouds
  CPPUNIT_ASSERT(op->Accept(lmc) == true);

  lmc->SetLandmark(id,2,2,2,1);

  // Do not accept closed time varying landmark clouds
  CPPUNIT_ASSERT(op->Accept(lmc) == false);

  albaDEL(lmc);

  cppDEL(op);
}

//----------------------------------------------------------------------------
void albaOpRegisterClustersTest::OpRunTest()
{
  albaOpRegisterClusters *op = new albaOpRegisterClusters();

  albaVMELandmarkCloud *lmc;
  albaNEW(lmc);

  lmc->AppendLandmark(0,0,0,"0");
  lmc->AppendLandmark(1,1,1,"1");
  lmc->AppendLandmark(2,2,2,"2");

  op->TestModeOn();
  op->SetInput(lmc);
  op->OpRun();

  cppDEL(op);
  albaDEL(lmc);
}

//----------------------------------------------------------------------------
void albaOpRegisterClustersTest::OpDoUndoTest()
{
  albaOpRegisterClusters *op = new albaOpRegisterClusters();

  albaVMELandmarkCloud *lmc;
  albaNEW(lmc);

  lmc->AppendLandmark(0,0,0,"0");
  lmc->AppendLandmark(1,1,1,"1");
  lmc->AppendLandmark(2,2,2,"2");

  op->TestModeOn();
  op->SetInput(lmc);

  op->OpRun();

  albaVMESurface *surf;
  albaNEW(surf);

  op->SetFollower(surf);
  op->SetTarget(lmc);


  op->OpDo();

  CPPUNIT_ASSERT(op->GetResult() != NULL);

  op->OpUndo();

  CPPUNIT_ASSERT(op->GetResult() == NULL);

  cppDEL(op);
  albaDEL(lmc);
  albaDEL(surf);
}

//----------------------------------------------------------------------------
void albaOpRegisterClustersTest::ClosedCloudAcceptTest()
{
  albaVMELandmarkCloud *lmc;
  albaNEW(lmc);

  lmc->AppendLandmark(0,0,0,"0");
  lmc->AppendLandmark(1,1,1,"1");
  lmc->AppendLandmark(2,2,2,"2");

  // Accept closed not time varying landmark clouds
  CPPUNIT_ASSERT(albaOpRegisterClusters::LMCloudAccept(lmc) == true);

  albaDEL(lmc);
}

//----------------------------------------------------------------------------
void albaOpRegisterClustersTest::SurfaceAcceptTest()
{
  CPPUNIT_ASSERT(albaOpRegisterClusters::SurfaceAccept(NULL) == false);

  albaVMESurface *surf;
  albaNEW(surf);
  CPPUNIT_ASSERT(albaOpRegisterClusters::SurfaceAccept(surf) == true);
  albaDEL(surf);

  albaVMELandmarkCloud *lmc;
  albaNEW(lmc);
  CPPUNIT_ASSERT(albaOpRegisterClusters::SurfaceAccept(lmc) == false);
  albaDEL(lmc);
}

//----------------------------------------------------------------------------
void albaOpRegisterClustersTest::OutputTest()
{
	albaVMELandmarkCloud *lmcInput, *lmcTarget;
	albaNEW(lmcInput);

	lmcInput->AppendLandmark(0, 0, 0, "0");
	lmcInput->AppendLandmark(1, 1, 1, "1");
	lmcInput->AppendLandmark(2, 2, 2, "2");

	albaNEW(lmcTarget);
	lmcTarget->AppendLandmark(0, 0, 0, "0");
	lmcTarget->AppendLandmark(1, 1, 1, "1");
	lmcTarget->AppendLandmark(2, 2, 2, "2");
	
	albaMatrix matrixTarget;
	albaTransform::RotateX(matrixTarget, 1.0, POST_MULTIPLY);
	albaTransform::RotateY(matrixTarget, 2.0, POST_MULTIPLY);
	albaTransform::RotateZ(matrixTarget, 3.0, POST_MULTIPLY);
	albaTransform::Translate(matrixTarget, 2.0, 2.0, 2.0, POST_MULTIPLY);
  // No scaling in RIGID Mode

	lmcTarget->SetAbsMatrix(matrixTarget);

	albaOpRegisterClusters *op = new albaOpRegisterClusters();

	op->TestModeOn();
	op->SetInput(lmcInput);
	op->OpRun();
	op->SetTarget(lmcTarget);	

	//op->SetApplyRegistrationMatrix(false);
	//op->SetRegistrationMode(0); // 0 = RIGID (Default), only translation and rotation is allowed

	op->OpDo();
	
	// Result

	albaVMEGroup *group = op->GetResult();
	CPPUNIT_ASSERT(group);

	albaVMEInfoText *info = albaVMEInfoText::SafeDownCast(group->GetFirstChild());
	CPPUNIT_ASSERT(info);

	albaVMELandmarkCloud *cloud = albaVMELandmarkCloud::SafeDownCast(group->GetChild(1));
	CPPUNIT_ASSERT(cloud);


	albaMatrix *matrixInput, *matrixOutput;
	matrixInput = lmcInput->GetOutput()->GetAbsMatrix();
	matrixOutput = cloud->GetOutput()->GetAbsMatrix();

	// Cloud Matrix
	// Apply Registration Matrix = true -> matrixOutput == matrixInput
	CPPUNIT_ASSERT(matrixOutput->Equals(matrixInput));

	// Result RIGID Mode

	// Points
	double p1[3], p2[3];
	bool result = false;

	for (int i=0; i< lmcTarget->GetNumberOfLandmarks(); i++)
	{
		albaVMELandmark::SafeDownCast(lmcTarget->GetChild(i))->GetPoint(p1);
		albaVMELandmark::SafeDownCast(cloud->GetChild(i))->GetPoint(p2);

		result = AboutEqual(p1[0], p2[0]) && AboutEqual(p1[1], p2[1]) && AboutEqual(p1[2], p2[2]);
		
		if(!result) break;
	}
	
	CPPUNIT_ASSERT(result);

	cppDEL(op);
	albaDEL(lmcInput);
	albaDEL(lmcTarget);
}

//----------------------------------------------------------------------------
void albaOpRegisterClustersTest::Output2Test()
{
	albaVMELandmarkCloud *lmcInput, *lmcTarget;
	albaNEW(lmcInput);

	lmcInput->AppendLandmark(0, 0, 0, "0");
	lmcInput->AppendLandmark(5, 5, 5, "1");
	lmcInput->AppendLandmark(10, 10, 10, "2");
	lmcInput->AppendLandmark(20, 20, 20, "3");
	lmcInput->AppendLandmark(20, 30, 25, "4");
	lmcInput->AppendLandmark(25, 25, 30, "5");

	albaNEW(lmcTarget);
	lmcTarget->AppendLandmark(0, 0, 0, "0");
	lmcTarget->AppendLandmark(5, 5, 5, "1");
	lmcTarget->AppendLandmark(10, 10, 10, "2");
	lmcTarget->AppendLandmark(20, 20, 20, "3");
	lmcTarget->AppendLandmark(20, 30, 25, "4");
	lmcTarget->AppendLandmark(25, 25, 30, "5");

	albaMatrix matrixTarget;
	albaTransform::RotateX(matrixTarget, 1.0, POST_MULTIPLY);
	albaTransform::RotateY(matrixTarget, 2.0, POST_MULTIPLY);
	albaTransform::RotateZ(matrixTarget, 3.0, POST_MULTIPLY);
	albaTransform::Translate(matrixTarget, 2.0, 2.0, 2.0, POST_MULTIPLY);
	albaTransform::Scale(matrixTarget, 0.5, 0.5, 0.5, POST_MULTIPLY); // Enable Scale

	lmcTarget->SetAbsMatrix(matrixTarget);

	albaOpRegisterClusters *op = new albaOpRegisterClusters();

	op->TestModeOn();
	op->SetInput(lmcInput);

	op->OpRun();
	op->SetTarget(lmcTarget);

	op->SetApplyRegistrationMatrix(false);
	op->SetRegistrationMode(1); // 1 = SIMILARITY, only translation, rotation and scaling is allowed

	op->OpDo();

	// Result

	albaVMEGroup *group = op->GetResult();
	CPPUNIT_ASSERT(group);
	
	albaVMELandmarkCloud *cloud = albaVMELandmarkCloud::SafeDownCast(group->GetChild(1));
	CPPUNIT_ASSERT(cloud);
	
	albaMatrix *matrixInput, *matrixOutput;
	matrixInput = lmcInput->GetOutput()->GetAbsMatrix();
	matrixOutput = cloud->GetOutput()->GetAbsMatrix();
	
	// Cloud Matrix
	// Apply Registration Matrix = false -> matrixOutput != matrixInput
	CPPUNIT_ASSERT(!matrixOutput->Equals(matrixInput)); 

	// Result SIMILARITY Mode

	// Points
	double p1[3], p2[3];
	bool result = false;

	for (int i = 0; i < lmcTarget->GetNumberOfLandmarks(); i++)
	{
		albaVMELandmark::SafeDownCast(lmcTarget->GetChild(i))->GetPoint(p1);
		albaVMELandmark::SafeDownCast(cloud->GetChild(i))->GetPoint(p2);

		result = AboutEqual(p1[0], p2[0]) && AboutEqual(p1[1], p2[1]) && AboutEqual(p1[2], p2[2]);

		if (!result) break;
	}

	CPPUNIT_ASSERT(result);

	// Result AFFINE Mode

	op->SetRegistrationMode(2); // 2 = AFFINE, any affine transformation is allowed (e.g., shear)
	op->OpDo();

	for (int i = 0; i < lmcTarget->GetNumberOfLandmarks(); i++)
	{
		albaVMELandmark::SafeDownCast(lmcTarget->GetChild(i))->GetPoint(p1);
		albaVMELandmark::SafeDownCast(cloud->GetChild(i))->GetPoint(p2);

		result = AboutEqual(p1[0], p2[0]) && AboutEqual(p1[1], p2[1]) && AboutEqual(p1[2], p2[2]);

		if (!result) break;
	}

	CPPUNIT_ASSERT(result);

	cppDEL(op);
	albaDEL(lmcInput);
	albaDEL(lmcTarget);
}

//----------------------------------------------------------------------------
bool albaOpRegisterClustersTest::AboutEqual(double a, double b, double epsilon)
{
	//epsilon = MAX(abs(a), abs(b)) * 1E-15;

	return abs(a - b) <= epsilon;
}