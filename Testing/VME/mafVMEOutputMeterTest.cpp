/*=========================================================================

 Program: MAF2
 Module: mafVMEOutputMeterTest
 Authors: Daniele Giunchi
 
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
#include "mafVMEOutputMeterTest.h"

#include "mafVMEOutputMeter.h"
#include "mafVMEMeter.h"

#include "mafVMESurfaceParametric.h"
#include "mafVMEStorage.h"
#include "mafVMERoot.h"
#include "mafMatrix.h"
#include "vtkDataSet.h"

#include <iostream>

#define TEST_RESULT CPPUNIT_ASSERT(m_Result)
#define TOLERANCE 1.0e-3

//----------------------------------------------------------------------------
void mafVMEOutputMeterTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafVMEOutputMeterTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  mafVMEOutputMeter outputMeter;
}
//----------------------------------------------------------------------------
void mafVMEOutputMeterTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafVMEOutputMeter *outputMeter = new mafVMEOutputMeter();
  cppDEL(outputMeter);
}
//----------------------------------------------------------------------------
void mafVMEOutputMeterTest::TestUpdate()
//----------------------------------------------------------------------------
{
	mafVMEStorage *storage = mafVMEStorage::New();
	storage->GetRoot()->SetName("root");
	storage->GetRoot()->Initialize();

	mafVMERoot *root=storage->GetRoot();

	//create a parametric surface
	mafVMESurfaceParametric *vmeParametricSurfaceSTART;
	mafNEW(vmeParametricSurfaceSTART);
	vmeParametricSurfaceSTART->ReparentTo(root);
	vmeParametricSurfaceSTART->ReparentTo(storage->GetRoot());
	vmeParametricSurfaceSTART->Update();

	mafVMESurfaceParametric *vmeParametricSurfaceEND1;
	mafNEW(vmeParametricSurfaceEND1);	
	vmeParametricSurfaceEND1->ReparentTo(root);
	vmeParametricSurfaceEND1->ReparentTo(storage->GetRoot());
	vmeParametricSurfaceEND1->Update();

	mafVMESurfaceParametric *vmeParametricSurfaceEND2;
	mafNEW(vmeParametricSurfaceEND2);	
	vmeParametricSurfaceEND2->ReparentTo(root);
	vmeParametricSurfaceEND2->ReparentTo(storage->GetRoot());
	vmeParametricSurfaceEND2->Update();

	enum {X,Y,Z};
	mafMatrix matrix1;
	matrix1.SetElement(X,3,4); //set a translation value on x axis of 4.0
	matrix1.SetElement(Y,3,3); //set a translation value on x axis of 3.0
	vmeParametricSurfaceEND1->SetAbsMatrix(matrix1);

	mafMatrix matrix2;
	matrix2.SetElement(X,3,3); //set a translation value on x axis of 4.0
	matrix2.SetElement(Y,3,-4); //set a translation value on x axis of 3.0
	vmeParametricSurfaceEND2->SetAbsMatrix(matrix2);

	mafVMEMeter *meter;
	mafNEW(meter);
	meter->SetMeterLink("StartVME",vmeParametricSurfaceSTART);
	meter->SetMeterLink("EndVME1",vmeParametricSurfaceEND1);
	meter->SetMeterLink("EndVME2",vmeParametricSurfaceEND2);
	meter->ReparentTo(storage->GetRoot());
	meter->Modified();
	meter->Update();


	mafVMEOutputMeter *outputMeter = mafVMEOutputMeter::SafeDownCast(meter->GetOutput());
	outputMeter->Update();

	mafString distance = outputMeter->GetDistance();
  //test here
	m_Result = distance.Equals("5");
	TEST_RESULT;

	meter->SetMeterMode(mafVMEMeter::LINE_ANGLE);
	meter->Modified();
	meter->Update();

	outputMeter->Update();
	mafString angleString = outputMeter->GetAngle();
	double angle = atof(angleString.GetCStr());
	m_Result = (angle - 90) < TOLERANCE;
	TEST_RESULT;

	meter->ReparentTo(NULL);
	vmeParametricSurfaceSTART->ReparentTo(NULL);
	vmeParametricSurfaceEND1->ReparentTo(NULL);
	vmeParametricSurfaceEND2->ReparentTo(NULL);
	
	mafDEL(meter);
	mafDEL(vmeParametricSurfaceSTART);
	mafDEL(vmeParametricSurfaceEND1);
	mafDEL(vmeParametricSurfaceEND2);
	

	mafDEL(storage);
}