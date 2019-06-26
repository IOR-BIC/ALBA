/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBADataPipeTest
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
#include "albaDataPipesTests.h"
#include "vtkALBADataPipeTest.h"

#include "vtkALBADataPipe.h"
#include "albaDataPipe.h"
#include "vtkALBASmartPointer.h"
#include "albaSmartPointer.h"
#include "albaVMESurface.h"
#include "vtkCubeSource.h"
#include "vtkPolyData.h"

#define TEST_RESULT CPPUNIT_ASSERT(result);




//----------------------------------------------------------------------------
void vtkALBADataPipeTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void vtkALBADataPipeTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  vtkALBASmartPointer<vtkALBADataPipe> dp;

	vtkALBADataPipe * dp2;
	vtkNEW(dp2);
	vtkDEL(dp2);
}
//----------------------------------------------------------------------------
void vtkALBADataPipeTest::TestSetNthInput()
//----------------------------------------------------------------------------
{
	vtkPolyData *poly0 = vtkPolyData::New();
	vtkPolyData *poly1 = vtkPolyData::New();
	vtkALBASmartPointer<vtkALBADataPipe> dp;
	dp->SetNthInput(0, poly0);
	dp->SetNthInput(1, poly1);

	result = 2 == dp->GetNumberOfInputs();
	TEST_RESULT;
	poly0->Delete();
	poly1->Delete();
}
//----------------------------------------------------------------------------
void vtkALBADataPipeTest::TestGetOutput()
//----------------------------------------------------------------------------
{
	vtkCubeSource *poly0 = vtkCubeSource::New();
	poly0->Update();

	vtkDataSet *pDataSet = NULL;
	vtkALBASmartPointer<vtkALBADataPipe> vdp;

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
void vtkALBADataPipeTest::TestGetMTime()
//----------------------------------------------------------------------------
{
	vtkALBASmartPointer<vtkALBADataPipe> dp;
	
	//m_DataPipe == NULL
	long time1, time2;
	time1 = dp->GetMTime();
	dp->Modified();
	dp->Update();
	time2 = dp->GetMTime();

	result = time2 > time1;
  TEST_RESULT;

	//m_DataPipe != NULL
	albaSmartPointer<albaDataPipe> mdp;
	dp->SetDataPipe(mdp);
	time1 = dp->GetMTime();
	dp->Modified();
	dp->Update();
	time2 = dp->GetMTime();

	result = time2 > time1;
	TEST_RESULT;


}
//----------------------------------------------------------------------------
void vtkALBADataPipeTest::TestGetInformationTime()
//----------------------------------------------------------------------------
{
	vtkALBASmartPointer<vtkALBADataPipe> dp;
  long time1, time2;
	time1 = dp->GetInformationTime();
	dp->UpdateInformation();
	time2 = dp->GetInformationTime();

	result = time2 > time1;
	TEST_RESULT;
}
//----------------------------------------------------------------------------
void vtkALBADataPipeTest::TestSetGetDataPipe()
//----------------------------------------------------------------------------
{
	vtkALBASmartPointer<vtkALBADataPipe> vdp;
  albaSmartPointer<albaDataPipe> mdp;

	vdp->SetDataPipe(mdp);

	result = vdp->GetDataPipe() == mdp.GetPointer();
  
	TEST_RESULT;
}
//----------------------------------------------------------------------------
void vtkALBADataPipeTest::TestUpdateInformation()
//----------------------------------------------------------------------------
{
	//catch event inside vme

	albaVMESurfaceTestClass *vmeTest;
	albaNEW(vmeTest);

	vtkALBASmartPointer<vtkALBADataPipe> vdp;
	albaSmartPointer<albaDataPipe> dp;
	dp->SetVME(vmeTest);
	vdp->SetDataPipe(dp);
  vdp->UpdateInformation();

	result = vmeTest->Name.Equals("CATCHED");
	TEST_RESULT;

	albaDEL(vmeTest);


	TEST_RESULT;
}