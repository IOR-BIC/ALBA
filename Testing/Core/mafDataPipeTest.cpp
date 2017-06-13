/*=========================================================================

 Program: MAF2
 Module: mafDataPipeTest
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
#include "mafDataPipeTest.h"

#include "mafDataPipe.h"
#include "mafSmartPointer.h"
#include "mafOBB.h"
#include "mafVMESurface.h"
#include "mafEvent.h"
#include "mafAbsMatrixPipe.h"
#include "mafMatrixPipe.h"

#define TEST_RESULT CPPUNIT_ASSERT(result);

/** avme test  class used to test event reception. */
class mafVMETestClass: public mafVMESurface
{
public:
	mafVMETestClass(const char *name=NULL):Name(name) {}
	mafTypeMacro(mafVMETestClass,mafVMESurface);

	virtual void OnEvent(mafEventBase *event) {Name = "CATCHED";};

	mafString     Name;
};
//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMETestClass)
//-------------------------------------------------------------------------

//----------------------------------------------------------------------------
void mafDataPipeTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafDataPipeTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafDataPipe> dp;

	mafDataPipe * dp2;
	mafNEW(dp2);
	mafDEL(dp2);
}
//----------------------------------------------------------------------------
void mafDataPipeTest::TestUpdateBounds()
//----------------------------------------------------------------------------
{
	mafSmartPointer<mafDataPipe> dp;
	dp->UpdateBounds();  //empty method
}
//----------------------------------------------------------------------------
void mafDataPipeTest::TestUpdate()
//----------------------------------------------------------------------------
{
	mafSmartPointer<mafDataPipe> dp;
	dp->Update();  //empty method
}
//----------------------------------------------------------------------------
void mafDataPipeTest::TestGetBounds()
//----------------------------------------------------------------------------
{
	mafSmartPointer<mafDataPipe> dp;
	mafOBB *b = dp->GetBounds();  //return m_Bounds

	result = (b->m_Bounds[0] == 0.0 &&
		       b->m_Bounds[1] == -1.0 &&
					 b->m_Bounds[2] == 0.0 &&
					 b->m_Bounds[3] == -1.0 &&
					 b->m_Bounds[4] == 0.0 &&
					 b->m_Bounds[5] == -1.0 );

  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafDataPipeTest::TestGetVTKData()
//----------------------------------------------------------------------------
{
	mafSmartPointer<mafDataPipe> dp;
	vtkDataSet *ds = dp->GetVTKData();  //return NULL

	result = ds == NULL;
	TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafDataPipeTest::TestSetGetTimeStamp()
//----------------------------------------------------------------------------
{
	mafTimeStamp value = 5.0;
	mafSmartPointer<mafDataPipe> dp;
	dp->SetTimeStamp(value);

	result = value == dp->GetTimeStamp();

	TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafDataPipeTest::TestGetMTime()
//----------------------------------------------------------------------------
{
	mafVMESurface *surface;
	mafNEW(surface);

	long firstTime, secondTime;

	//if it not depends from anything
	mafSmartPointer<mafDataPipe> dp;
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
	mafSmartPointer<mafDataPipe> dp2;
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
	mafSmartPointer<mafDataPipe> dp3;
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
	mafSmartPointer<mafDataPipe> dp4;
	dp4->SetVME(surface);
	dp4->SetDependOnVMETime(false);
	dp4->SetDependOnAbsPose(false);
	dp4->SetDependOnPose(true);

	surface->Modified();

	firstTime = surface->GetMatrixPipe()->GetMTime();
	secondTime = dp4->GetMTime();
	result = secondTime == firstTime;
	TEST_RESULT;

	mafDEL(surface);
	
}
//----------------------------------------------------------------------------
void mafDataPipeTest::TestAccept()
//----------------------------------------------------------------------------
{
	mafVMESurface *surface;
	mafNEW(surface);

	mafSmartPointer<mafDataPipe> dp;
	result = dp->Accept(surface);

	mafDEL(surface);
	TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafDataPipeTest::TestMakeACopy()
//----------------------------------------------------------------------------
{
	mafVMESurface *surface;
	mafNEW(surface);

	mafSmartPointer<mafDataPipe> dp;
	dp->SetVME(surface);

	mafDataPipe *copyDataPipe;
	copyDataPipe = dp->MakeACopy();
	
	result = copyDataPipe != NULL && copyDataPipe->GetVME() == surface;

	mafDEL(surface);
  mafDEL(copyDataPipe);
	TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafDataPipeTest::TestDeepCopy()
//----------------------------------------------------------------------------
{
	mafVMESurface *surface;
	mafNEW(surface);

	mafSmartPointer<mafDataPipe> dp;
	dp->SetVME(surface);

	mafSmartPointer<mafDataPipe> copyDataPipe;
	copyDataPipe->DeepCopy(dp);

	result = copyDataPipe != NULL && copyDataPipe->GetVME() == surface;

	mafDEL(surface);
	TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafDataPipeTest::TestSetGetVME()
//----------------------------------------------------------------------------
{
	mafVMESurface *surface;
	mafNEW(surface);

	mafSmartPointer<mafDataPipe> dp;
	dp->SetVME(surface);

	result = dp->GetVME() == surface;
  mafDEL(surface);
	TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafDataPipeTest::TestSetGetDependOnPose()
//----------------------------------------------------------------------------
{
	mafSmartPointer<mafDataPipe> dp;
	dp->SetDependOnPose(true);
	result = dp->GetDependOnPose() == true;
	TEST_RESULT;

	dp->SetDependOnPose(false);
	result = dp->GetDependOnPose() == false;
	TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafDataPipeTest::TestSetGetDependOnVMETime()
//----------------------------------------------------------------------------
{
	mafSmartPointer<mafDataPipe> dp;
	dp->SetDependOnVMETime(true);
	result = dp->GetDependOnVMETime() == true;
	TEST_RESULT;

	dp->SetDependOnVMETime(false);
	result = dp->GetDependOnVMETime() == false;
	TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafDataPipeTest::TestSetGetDependOnAbsPose()
//----------------------------------------------------------------------------
{
	mafSmartPointer<mafDataPipe> dp;
	dp->SetDependOnAbsPose(true);
	result = dp->GetDependOnAbsPose() == true;
	TEST_RESULT;

	dp->SetDependOnAbsPose(false);
	result = dp->GetDependOnAbsPose() == false;
	TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafDataPipeTest::TestPrint()
//----------------------------------------------------------------------------
{
	mafVMESurface *surface;
	mafNEW(surface);
	surface->SetName("VME TEST");
	//only Print
	mafSmartPointer<mafDataPipe> dp;
	dp->SetVME(surface);
	dp->Print(std::cout);
	mafDEL(surface);
}
//----------------------------------------------------------------------------
void mafDataPipeTest::TestOnEvent()
//----------------------------------------------------------------------------
{
	//VME_OUTPUT_DATA_PREUPDATE , VME_OUTPUT_DATA_UPDATE

	mafVMETestClass *vmeTest;
	mafNEW(vmeTest);

	mafSmartPointer<mafDataPipe> dp;
	dp->SetVME(vmeTest);
	dp->OnEvent(&mafEvent(vmeTest,VME_OUTPUT_DATA_PREUPDATE));
  result = vmeTest->Name.Equals("CATCHED");
  TEST_RESULT;
	dp->OnEvent(&mafEvent(vmeTest,VME_OUTPUT_DATA_UPDATE));
	result = vmeTest->Name.Equals("CATCHED");
  TEST_RESULT;

	mafDEL(vmeTest);
}