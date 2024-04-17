/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEOutputMeterTest
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

#include <cppunit/config/SourcePrefix.h>
#include "albaVMEOutputMeterTest.h"

#include "albaVMEOutputMeter.h"
#include "albaVMEMeter.h"

#include "albaVMESurfaceParametric.h"
#include "albaVMEStorage.h"
#include "albaVMERoot.h"
#include "albaMatrix.h"
#include "vtkDataSet.h"

#include <iostream>

#define TEST_RESULT CPPUNIT_ASSERT(m_Result)
#define TOLERANCE 1.0e-3

//----------------------------------------------------------------------------
void albaVMEOutputMeterTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaVMEOutputMeterTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  albaVMEOutputMeter outputMeter;
}
//----------------------------------------------------------------------------
void albaVMEOutputMeterTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaVMEOutputMeter *outputMeter = new albaVMEOutputMeter();
  cppDEL(outputMeter);
}
//----------------------------------------------------------------------------
void albaVMEOutputMeterTest::TestUpdate()
//----------------------------------------------------------------------------
{
	albaVMEStorage *storage = albaVMEStorage::New();
	storage->GetRoot()->SetName("root");
	storage->GetRoot()->Initialize();

	albaVMERoot *root=storage->GetRoot();

	//create a parametric surface
	albaVMESurfaceParametric *vmeParametricSurfaceSTART;
	albaNEW(vmeParametricSurfaceSTART);
	vmeParametricSurfaceSTART->ReparentTo(root);
	vmeParametricSurfaceSTART->ReparentTo(storage->GetRoot());
	vmeParametricSurfaceSTART->Update();

	albaVMESurfaceParametric *vmeParametricSurfaceEND1;
	albaNEW(vmeParametricSurfaceEND1);	
	vmeParametricSurfaceEND1->ReparentTo(root);
	vmeParametricSurfaceEND1->ReparentTo(storage->GetRoot());
	vmeParametricSurfaceEND1->Update();

	albaVMESurfaceParametric *vmeParametricSurfaceEND2;
	albaNEW(vmeParametricSurfaceEND2);	
	vmeParametricSurfaceEND2->ReparentTo(root);
	vmeParametricSurfaceEND2->ReparentTo(storage->GetRoot());
	vmeParametricSurfaceEND2->Update();

	enum {X,Y,Z};
	albaMatrix matrix1;
	matrix1.SetElement(X,3,4); //set a translation value on x axis of 4.0
	matrix1.SetElement(Y,3,3); //set a translation value on x axis of 3.0
	vmeParametricSurfaceEND1->SetAbsMatrix(matrix1);

	albaMatrix matrix2;
	matrix2.SetElement(X,3,3); //set a translation value on x axis of 4.0
	matrix2.SetElement(Y,3,-4); //set a translation value on x axis of 3.0
	vmeParametricSurfaceEND2->SetAbsMatrix(matrix2);

	albaVMEMeter *meter;
	albaNEW(meter);
	meter->SetMeterLink("StartVME",vmeParametricSurfaceSTART);
	meter->SetMeterLink("EndVME1",vmeParametricSurfaceEND1);
	meter->SetMeterLink("EndVME2",vmeParametricSurfaceEND2);
	meter->ReparentTo(storage->GetRoot());
	meter->Modified();
	meter->Update();


	albaVMEOutputMeter *outputMeter = albaVMEOutputMeter::SafeDownCast(meter->GetOutput());
	outputMeter->Update();

	albaString distance = outputMeter->GetDistance();
  //test here
	m_Result = distance.Equals("5");
	TEST_RESULT;

	meter->SetMeterMode(albaVMEMeter::LINE_ANGLE);
	meter->Modified();
	meter->Update();

	outputMeter->Update();
	albaString angleString = outputMeter->GetAngle();
	double angle = atof(angleString.GetCStr());
	m_Result = (angle - 90) < TOLERANCE;
	TEST_RESULT;

	meter->ReparentTo(NULL);
	vmeParametricSurfaceSTART->ReparentTo(NULL);
	vmeParametricSurfaceEND1->ReparentTo(NULL);
	vmeParametricSurfaceEND2->ReparentTo(NULL);
	
	albaDEL(meter);
	albaDEL(vmeParametricSurfaceSTART);
	albaDEL(vmeParametricSurfaceEND1);
	albaDEL(vmeParametricSurfaceEND2);
	

	albaDEL(storage);
}