/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaDataPipeTest
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
#include "albaDataPipeTest.h"

#include "albaDataPipe.h"
#include "albaSmartPointer.h"
#include "albaOBB.h"
#include "albaVMESurface.h"
#include "albaEvent.h"
#include "albaAbsMatrixPipe.h"
#include "albaMatrixPipe.h"

#define TEST_RESULT CPPUNIT_ASSERT(result);

/** avme test  class used to test event reception. */
class albaVMETestClass: public albaVMESurface
{
public:
	albaVMETestClass(const char *name=NULL):Name(name) {}
	albaTypeMacro(albaVMETestClass,albaVMESurface);

	virtual void OnEvent(albaEventBase *event) {Name = "CATCHED";};

	albaString     Name;
};
//-------------------------------------------------------------------------
albaCxxTypeMacro(albaVMETestClass)
//-------------------------------------------------------------------------

//----------------------------------------------------------------------------
void albaDataPipeTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaDataPipeTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaDataPipe> dp;

	albaDataPipe * dp2;
	albaNEW(dp2);
	albaDEL(dp2);
}
//----------------------------------------------------------------------------
void albaDataPipeTest::TestUpdateBounds()
//----------------------------------------------------------------------------
{
	albaSmartPointer<albaDataPipe> dp;
	dp->UpdateBounds();  //empty method
}
//----------------------------------------------------------------------------
void albaDataPipeTest::TestUpdate()
//----------------------------------------------------------------------------
{
	albaSmartPointer<albaDataPipe> dp;
	dp->Update();  //empty method
}
//----------------------------------------------------------------------------
void albaDataPipeTest::TestGetBounds()
//----------------------------------------------------------------------------
{
	albaSmartPointer<albaDataPipe> dp;
	albaOBB *b = dp->GetBounds();  //return m_Bounds

	result = (b->m_Bounds[0] == 0.0 &&
		       b->m_Bounds[1] == -1.0 &&
					 b->m_Bounds[2] == 0.0 &&
					 b->m_Bounds[3] == -1.0 &&
					 b->m_Bounds[4] == 0.0 &&
					 b->m_Bounds[5] == -1.0 );

  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaDataPipeTest::TestGetVTKData()
//----------------------------------------------------------------------------
{
	albaSmartPointer<albaDataPipe> dp;
	vtkDataSet *ds = dp->GetVTKData();  //return NULL

	result = ds == NULL;
	TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaDataPipeTest::TestSetGetTimeStamp()
//----------------------------------------------------------------------------
{
	albaTimeStamp value = 5.0;
	albaSmartPointer<albaDataPipe> dp;
	dp->SetTimeStamp(value);

	result = value == dp->GetTimeStamp();

	TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaDataPipeTest::TestGetMTime()
//----------------------------------------------------------------------------
{
	albaVMESurface *surface;
	albaNEW(surface);

	long firstTime, secondTime;

	//if it not depends from anything
	albaSmartPointer<albaDataPipe> dp;
	dp->SetDependOnVMETime(false);
	dp->SetDependOnAbsPose(false);
	dp->SetDependOnPose(false);
	dp->Modified();
	firstTime = dp->GetMTime();
	dp->Modified();
	secondTime = dp->GetMTime();
	result = secondTime > firstTime;
  TEST_RESULT;

	//if it depends on vme time
	albaSmartPointer<albaDataPipe> dp2;
	dp2->SetVME(surface);
	dp2->SetDependOnVMETime(true);
	dp2->SetDependOnAbsPose(false);
	dp2->SetDependOnPose(false);

	surface->Modified();

	firstTime = surface->GetMTime();
	secondTime = dp2->GetMTime();
	result = secondTime == firstTime;
  TEST_RESULT;

	//if it depends on vme abs pose
	albaSmartPointer<albaDataPipe> dp3;
	dp3->SetVME(surface);
	dp3->SetDependOnVMETime(false);
	dp3->SetDependOnAbsPose(true);
	dp3->SetDependOnPose(false);

	surface->Modified();

	firstTime = surface->GetAbsMatrixPipe()->GetMTime();
	secondTime = dp3->GetMTime();
	result = secondTime == firstTime;
  TEST_RESULT;

	//if it depends on vme pose
	albaSmartPointer<albaDataPipe> dp4;
	dp4->SetVME(surface);
	dp4->SetDependOnVMETime(false);
	dp4->SetDependOnAbsPose(false);
	dp4->SetDependOnPose(true);

	surface->Modified();

	firstTime = surface->GetMatrixPipe()->GetMTime();
	secondTime = dp4->GetMTime();
	result = secondTime == firstTime;
	TEST_RESULT;

	albaDEL(surface);
	
}
//----------------------------------------------------------------------------
void albaDataPipeTest::TestAccept()
//----------------------------------------------------------------------------
{
	albaVMESurface *surface;
	albaNEW(surface);

	albaSmartPointer<albaDataPipe> dp;
	result = dp->Accept(surface);

	albaDEL(surface);
	TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaDataPipeTest::TestMakeACopy()
//----------------------------------------------------------------------------
{
	albaVMESurface *surface;
	albaNEW(surface);

	albaSmartPointer<albaDataPipe> dp;
	dp->SetVME(surface);

	albaDataPipe *copyDataPipe;
	copyDataPipe = dp->MakeACopy();
	
	result = copyDataPipe != NULL && copyDataPipe->GetVME() == surface;

	albaDEL(surface);
  albaDEL(copyDataPipe);
	TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaDataPipeTest::TestDeepCopy()
//----------------------------------------------------------------------------
{
	albaVMESurface *surface;
	albaNEW(surface);

	albaSmartPointer<albaDataPipe> dp;
	dp->SetVME(surface);

	albaSmartPointer<albaDataPipe> copyDataPipe;
	copyDataPipe->DeepCopy(dp);

	result = copyDataPipe != NULL && copyDataPipe->GetVME() == surface;

	albaDEL(surface);
	TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaDataPipeTest::TestSetGetVME()
//----------------------------------------------------------------------------
{
	albaVMESurface *surface;
	albaNEW(surface);

	albaSmartPointer<albaDataPipe> dp;
	dp->SetVME(surface);

	result = dp->GetVME() == surface;
  albaDEL(surface);
	TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaDataPipeTest::TestSetGetDependOnPose()
//----------------------------------------------------------------------------
{
	albaSmartPointer<albaDataPipe> dp;
	dp->SetDependOnPose(true);
	result = dp->GetDependOnPose() == true;
	TEST_RESULT;

	dp->SetDependOnPose(false);
	result = dp->GetDependOnPose() == false;
	TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaDataPipeTest::TestSetGetDependOnVMETime()
//----------------------------------------------------------------------------
{
	albaSmartPointer<albaDataPipe> dp;
	dp->SetDependOnVMETime(true);
	result = dp->GetDependOnVMETime() == true;
	TEST_RESULT;

	dp->SetDependOnVMETime(false);
	result = dp->GetDependOnVMETime() == false;
	TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaDataPipeTest::TestSetGetDependOnAbsPose()
//----------------------------------------------------------------------------
{
	albaSmartPointer<albaDataPipe> dp;
	dp->SetDependOnAbsPose(true);
	result = dp->GetDependOnAbsPose() == true;
	TEST_RESULT;

	dp->SetDependOnAbsPose(false);
	result = dp->GetDependOnAbsPose() == false;
	TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaDataPipeTest::TestPrint()
//----------------------------------------------------------------------------
{
	albaVMESurface *surface;
	albaNEW(surface);
	surface->SetName("VME TEST");
	//only Print
	albaSmartPointer<albaDataPipe> dp;
	dp->SetVME(surface);
	dp->Print(std::cout);
	albaDEL(surface);
}
//----------------------------------------------------------------------------
void albaDataPipeTest::TestOnEvent()
//----------------------------------------------------------------------------
{
	//VME_OUTPUT_DATA_PREUPDATE , VME_OUTPUT_DATA_UPDATE

	albaVMETestClass *vmeTest;
	albaNEW(vmeTest);

	albaSmartPointer<albaDataPipe> dp;
	dp->SetVME(vmeTest);
	dp->OnEvent(&albaEvent(vmeTest,VME_OUTPUT_DATA_PREUPDATE));
  result = vmeTest->Name.Equals("CATCHED");
  TEST_RESULT;
	dp->OnEvent(&albaEvent(vmeTest,VME_OUTPUT_DATA_UPDATE));
	result = vmeTest->Name.Equals("CATCHED");
  TEST_RESULT;

	albaDEL(vmeTest);
}