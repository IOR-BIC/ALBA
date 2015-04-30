/*=========================================================================

 Program: MAF2
 Module: vtkMAFDataPipeTest
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
#include "mafDataPipesTests.h"
#include "vtkMAFDataPipeTest.h"

#include "vtkMAFDataPipe.h"
#include "mafDataPipe.h"
#include "vtkMAFSmartPointer.h"
#include "mafSmartPointer.h"
#include "mafVMESurface.h"
#include "vtkCubeSource.h"
#include "vtkPolyData.h"

#define TEST_RESULT CPPUNIT_ASSERT(result);




//----------------------------------------------------------------------------
void vtkMAFDataPipeTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void vtkMAFDataPipeTest::setUp()
//----------------------------------------------------------------------------
{
  result = false;
}
//----------------------------------------------------------------------------
void vtkMAFDataPipeTest::tearDown()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void vtkMAFDataPipeTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  vtkMAFSmartPointer<vtkMAFDataPipe> dp;

	vtkMAFDataPipe * dp2;
	vtkNEW(dp2);
	vtkDEL(dp2);
}
//----------------------------------------------------------------------------
void vtkMAFDataPipeTest::TestSetNthInput()
//----------------------------------------------------------------------------
{
	vtkPolyData *poly0 = vtkPolyData::New();
	vtkPolyData *poly1 = vtkPolyData::New();
	vtkMAFSmartPointer<vtkMAFDataPipe> dp;
	dp->SetNthInput(0, poly0);
	dp->SetNthInput(1, poly1);

	result = 2 == dp->GetNumberOfInputs();
	TEST_RESULT;
	poly0->Delete();
	poly1->Delete();
}
//----------------------------------------------------------------------------
void vtkMAFDataPipeTest::TestGetOutput()
//----------------------------------------------------------------------------
{
	vtkCubeSource *poly0 = vtkCubeSource::New();
	poly0->Update();

	vtkDataSet *pDataSet = NULL;
	vtkMAFSmartPointer<vtkMAFDataPipe> vdp;

	vdp->SetNthInput(0, poly0->GetOutput());

  pDataSet = vdp->GetOutput();

	result = NULL != pDataSet;
	TEST_RESULT;

	pDataSet = vdp->GetOutput(0);

	result = NULL != pDataSet;
	TEST_RESULT;

	poly0->Delete();

}
//----------------------------------------------------------------------------
void vtkMAFDataPipeTest::TestGetMTime()
//----------------------------------------------------------------------------
{
	vtkMAFSmartPointer<vtkMAFDataPipe> dp;
	
	//m_DataPipe == NULL
	long time1, time2;
	time1 = dp->GetMTime();
	dp->Modified();
	dp->Update();
	time2 = dp->GetMTime();

	result = time2 > time1;
  TEST_RESULT;

	//m_DataPipe != NULL
	mafSmartPointer<mafDataPipe> mdp;
	dp->SetDataPipe(mdp);
	time1 = dp->GetMTime();
	dp->Modified();
	dp->Update();
	time2 = dp->GetMTime();

	result = time2 > time1;
	TEST_RESULT;


}
//----------------------------------------------------------------------------
void vtkMAFDataPipeTest::TestGetInformationTime()
//----------------------------------------------------------------------------
{
	vtkMAFSmartPointer<vtkMAFDataPipe> dp;
  long time1, time2;
	time1 = dp->GetInformationTime();
	dp->UpdateInformation();
	time2 = dp->GetInformationTime();

	result = time2 > time1;
	TEST_RESULT;
}
//----------------------------------------------------------------------------
void vtkMAFDataPipeTest::TestSetGetDataPipe()
//----------------------------------------------------------------------------
{
	vtkMAFSmartPointer<vtkMAFDataPipe> vdp;
  mafSmartPointer<mafDataPipe> mdp;

	vdp->SetDataPipe(mdp);

	result = vdp->GetDataPipe() == mdp.GetPointer();
  
	TEST_RESULT;
}
//----------------------------------------------------------------------------
void vtkMAFDataPipeTest::TestUpdateInformation()
//----------------------------------------------------------------------------
{
	//catch event inside vme

	mafVMESurfaceTestClass *vmeTest;
	mafNEW(vmeTest);

	vtkMAFSmartPointer<vtkMAFDataPipe> vdp;
	mafSmartPointer<mafDataPipe> dp;
	dp->SetVME(vmeTest);
	vdp->SetDataPipe(dp);
  vdp->UpdateInformation();

	result = vmeTest->Name.Equals("CATCHED");
	TEST_RESULT;

	mafDEL(vmeTest);


	TEST_RESULT;
}