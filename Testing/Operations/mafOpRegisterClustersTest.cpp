/*=========================================================================

 Program: MAF2
 Module: mafOpRegisterClustersTest
 Authors: Alberto Losi
 
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
#include "mafOpRegisterClustersTest.h"
#include "mafOpRegisterClusters.h"
#include "mafTransform.h"
#include "mafVMEOutput.h"
#include "mafVMELandmark.h"

//----------------------------------------------------------------------------
void mafOpRegisterClustersTest::DynamicAllocationTest()
{
  mafOpRegisterClusters *op = new mafOpRegisterClusters();
  CPPUNIT_ASSERT(op != NULL);
  cppDEL(op);
}

//----------------------------------------------------------------------------
void mafOpRegisterClustersTest::CopyTest()
{
  mafOpRegisterClusters *op = new mafOpRegisterClusters();
  mafOp *op_copy = op->Copy();
  CPPUNIT_ASSERT(op_copy != NULL);
  CPPUNIT_ASSERT(op_copy->IsA("mafOpRegisterClusters"));

  cppDEL(op);
  cppDEL(op_copy);
}

//----------------------------------------------------------------------------
void mafOpRegisterClustersTest::AcceptTest()
{
  mafOpRegisterClusters *op = new mafOpRegisterClusters();
  CPPUNIT_ASSERT(op->Accept(NULL) == false);

  mafVMELandmarkCloud *lmc;
  mafNEW(lmc);

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

  mafDEL(lmc);

  cppDEL(op);
}

//----------------------------------------------------------------------------
void mafOpRegisterClustersTest::OpRunTest()
{
  mafOpRegisterClusters *op = new mafOpRegisterClusters();

  mafVMELandmarkCloud *lmc;
  mafNEW(lmc);

  lmc->AppendLandmark(0,0,0,"0");
  lmc->AppendLandmark(1,1,1,"1");
  lmc->AppendLandmark(2,2,2,"2");

  op->TestModeOn();
  op->SetInput(lmc);
  op->OpRun();

  cppDEL(op);
  mafDEL(lmc);
}

//----------------------------------------------------------------------------
void mafOpRegisterClustersTest::OpDoUndoTest()
{
  mafOpRegisterClusters *op = new mafOpRegisterClusters();

  mafVMELandmarkCloud *lmc;
  mafNEW(lmc);

  lmc->AppendLandmark(0,0,0,"0");
  lmc->AppendLandmark(1,1,1,"1");
  lmc->AppendLandmark(2,2,2,"2");

  op->TestModeOn();
  op->SetInput(lmc);

  op->OpRun();

  mafVMESurface *surf;
  mafNEW(surf);

  op->SetFollower(surf);
  op->SetTarget(lmc);


  op->OpDo();

  CPPUNIT_ASSERT(op->GetResult() != NULL);

  op->OpUndo();

  CPPUNIT_ASSERT(op->GetResult() == NULL);

  cppDEL(op);
  mafDEL(lmc);
  mafDEL(surf);
}

//----------------------------------------------------------------------------
void mafOpRegisterClustersTest::ClosedCloudAcceptTest()
{
  mafVMELandmarkCloud *lmc;
  mafNEW(lmc);

  lmc->AppendLandmark(0,0,0,"0");
  lmc->AppendLandmark(1,1,1,"1");
  lmc->AppendLandmark(2,2,2,"2");

  // Accept closed not time varying landmark clouds
  CPPUNIT_ASSERT(mafOpRegisterClusters::LMCloudAccept(lmc) == true);

  mafDEL(lmc);
}

//----------------------------------------------------------------------------
void mafOpRegisterClustersTest::SurfaceAcceptTest()
{
  CPPUNIT_ASSERT(mafOpRegisterClusters::SurfaceAccept(NULL) == false);

  mafVMESurface *surf;
  mafNEW(surf);
  CPPUNIT_ASSERT(mafOpRegisterClusters::SurfaceAccept(surf) == true);
  mafDEL(surf);

  mafVMELandmarkCloud *lmc;
  mafNEW(lmc);
  CPPUNIT_ASSERT(mafOpRegisterClusters::SurfaceAccept(lmc) == false);
  mafDEL(lmc);
}

//----------------------------------------------------------------------------
void mafOpRegisterClustersTest::OutputTest()
{
	mafVMELandmarkCloud *lmcInput, *lmcTarget;
	mafNEW(lmcInput);

	lmcInput->AppendLandmark(0, 0, 0, "0");
	lmcInput->AppendLandmark(1, 1, 1, "1");
	lmcInput->AppendLandmark(2, 2, 2, "2");

	mafNEW(lmcTarget);
	lmcTarget->AppendLandmark(0, 0, 0, "0");
	lmcTarget->AppendLandmark(1, 1, 1, "1");
	lmcTarget->AppendLandmark(2, 2, 2, "2");
	
	mafMatrix matrixTarget;
	mafTransform::RotateX(matrixTarget, 1.0, POST_MULTIPLY);
	mafTransform::RotateY(matrixTarget, 2.0, POST_MULTIPLY);
	mafTransform::RotateZ(matrixTarget, 3.0, POST_MULTIPLY);
	mafTransform::Translate(matrixTarget, 2.0, 2.0, 2.0, POST_MULTIPLY);
  // No scaling in RIGID Mode

	lmcTarget->SetAbsMatrix(matrixTarget);

	mafOpRegisterClusters *op = new mafOpRegisterClusters();

	op->TestModeOn();
	op->SetInput(lmcInput);
	op->OpRun();
	op->SetTarget(lmcTarget);	

	//op->SetApplyRegistrationMatrix(false);
	//op->SetRegistrationMode(0); // 0 = RIGID (Default), only translation and rotation is allowed

	op->OpDo();
	
	// Result

	mafVMEGroup *group = op->GetResult();
	CPPUNIT_ASSERT(group);

	mafVMEInfoText *info = mafVMEInfoText::SafeDownCast(group->GetFirstChild());
	CPPUNIT_ASSERT(info);

	mafVMELandmarkCloud *cloud = mafVMELandmarkCloud::SafeDownCast(group->GetChild(1));
	CPPUNIT_ASSERT(cloud);


	mafMatrix *matrixInput, *matrixOutput;
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
		mafVMELandmark::SafeDownCast(lmcTarget->GetChild(i))->GetPoint(p1);
		mafVMELandmark::SafeDownCast(cloud->GetChild(i))->GetPoint(p2);

		result = AboutEqual(p1[0], p2[0]) && AboutEqual(p1[1], p2[1]) && AboutEqual(p1[2], p2[2]);
		
		if(!result) break;
	}
	
	CPPUNIT_ASSERT(result);

	cppDEL(op);
	mafDEL(lmcInput);
	mafDEL(lmcTarget);
}

//----------------------------------------------------------------------------
void mafOpRegisterClustersTest::Output2Test()
{
	mafVMELandmarkCloud *lmcInput, *lmcTarget;
	mafNEW(lmcInput);

	lmcInput->AppendLandmark(0, 0, 0, "0");
	lmcInput->AppendLandmark(5, 5, 5, "1");
	lmcInput->AppendLandmark(10, 10, 10, "2");
	lmcInput->AppendLandmark(20, 20, 20, "3");
	lmcInput->AppendLandmark(20, 30, 25, "4");
	lmcInput->AppendLandmark(25, 25, 30, "5");

	mafNEW(lmcTarget);
	lmcTarget->AppendLandmark(0, 0, 0, "0");
	lmcTarget->AppendLandmark(5, 5, 5, "1");
	lmcTarget->AppendLandmark(10, 10, 10, "2");
	lmcTarget->AppendLandmark(20, 20, 20, "3");
	lmcTarget->AppendLandmark(20, 30, 25, "4");
	lmcTarget->AppendLandmark(25, 25, 30, "5");

	mafMatrix matrixTarget;
	mafTransform::RotateX(matrixTarget, 1.0, POST_MULTIPLY);
	mafTransform::RotateY(matrixTarget, 2.0, POST_MULTIPLY);
	mafTransform::RotateZ(matrixTarget, 3.0, POST_MULTIPLY);
	mafTransform::Translate(matrixTarget, 2.0, 2.0, 2.0, POST_MULTIPLY);
	mafTransform::Scale(matrixTarget, 0.5, 0.5, 0.5, POST_MULTIPLY); // Enable Scale

	lmcTarget->SetAbsMatrix(matrixTarget);

	mafOpRegisterClusters *op = new mafOpRegisterClusters();

	op->TestModeOn();
	op->SetInput(lmcInput);

	op->OpRun();
	op->SetTarget(lmcTarget);

	op->SetApplyRegistrationMatrix(false);
	op->SetRegistrationMode(1); // 1 = SIMILARITY, only translation, rotation and scaling is allowed

	op->OpDo();

	// Result

	mafVMEGroup *group = op->GetResult();
	CPPUNIT_ASSERT(group);
	
	mafVMELandmarkCloud *cloud = mafVMELandmarkCloud::SafeDownCast(group->GetChild(1));
	CPPUNIT_ASSERT(cloud);
	
	mafMatrix *matrixInput, *matrixOutput;
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
		mafVMELandmark::SafeDownCast(lmcTarget->GetChild(i))->GetPoint(p1);
		mafVMELandmark::SafeDownCast(cloud->GetChild(i))->GetPoint(p2);

		result = AboutEqual(p1[0], p2[0]) && AboutEqual(p1[1], p2[1]) && AboutEqual(p1[2], p2[2]);

		if (!result) break;
	}

	CPPUNIT_ASSERT(result);

	// Result AFFINE Mode

	op->SetRegistrationMode(2); // 2 = AFFINE, any affine transformation is allowed (e.g., shear)
	op->OpDo();

	for (int i = 0; i < lmcTarget->GetNumberOfLandmarks(); i++)
	{
		mafVMELandmark::SafeDownCast(lmcTarget->GetChild(i))->GetPoint(p1);
		mafVMELandmark::SafeDownCast(cloud->GetChild(i))->GetPoint(p2);

		result = AboutEqual(p1[0], p2[0]) && AboutEqual(p1[1], p2[1]) && AboutEqual(p1[2], p2[2]);

		if (!result) break;
	}

	CPPUNIT_ASSERT(result);

	cppDEL(op);
	mafDEL(lmcInput);
	mafDEL(lmcTarget);
}

//----------------------------------------------------------------------------
bool mafOpRegisterClustersTest::AboutEqual(double a, double b, double epsilon)
{
	//epsilon = MAX(abs(a), abs(b)) * 1E-15;

	return abs(a - b) <= epsilon;
}