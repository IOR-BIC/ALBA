/*=========================================================================

 Program: MAF2
 Module: mafMatrixInterpolatorTest
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
#include "mafMatrixInterpolatorTest.h"

#include "mafMatrixInterpolator.h"
#include "mafDataPipe.h"
#include "vtkMAFSmartPointer.h"
#include "mafSmartPointer.h"
#include "mafVMESurface.h"
#include "mafOBB.h"
#include "mafVMEItem.h"
#include "mafVMEItemVTK.h"
#include "vtkMAFDataPipe.h"

#include "vtkSphereSource.h"
#include "vtkPolyData.h"

#define TEST_RESULT CPPUNIT_ASSERT(m_Result);

/** a vme test  class used to test event reception. */
class  mafVMETestClass: public mafVMESurface
{
public:
  /**constructor.*/
	mafVMETestClass(const char *name=NULL):m_Name(name) {}
  /**rtti macro.*/
	mafTypeMacro(mafVMETestClass,mafVMESurface);

  /** catch event sent from other classes.*/
	virtual void OnEvent(mafEventBase *event) {m_Name = "CATCHED";};

	mafString m_Name;
};

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMETestClass)
//-------------------------------------------------------------------------

//----------------------------------------------------------------------------
void mafMatrixInterpolatorTest::setUp()
//----------------------------------------------------------------------------
{
  m_Result = false;
}
//----------------------------------------------------------------------------
void mafMatrixInterpolatorTest::tearDown()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafMatrixInterpolatorTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafMatrixInterpolator> mi;

	mafMatrixInterpolator *mi2;
	mafNEW(mi2);
	mafDEL(mi2);
}
//----------------------------------------------------------------------------
void mafMatrixInterpolatorTest::TestAccept()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMETestClass> surfaceTest;
  vtkMAFSmartPointer<vtkSphereSource> sphere;
  
  surfaceTest->SetData(sphere->GetOutput(), 0.0);
  surfaceTest->Update();

  /*mafMatrix identity;
  surfaceTest->SetTimeStamp(0.0);
  surfaceTest->SetMatrix(identity);
  surfaceTest->Update();

  mafMatrix translated;
  double translation[3] = {2,0,0};
  translated.SetElement(0,3, translation[0]);

  surfaceTest->SetTimeStamp(1.0);
  surfaceTest->SetMatrix(translated);
  surfaceTest->Update();*/

  mafSmartPointer<mafMatrixInterpolator> mi;
  m_Result = mi->Accept(surfaceTest);
  TEST_RESULT;

  m_Result = !(mi->Accept(NULL));
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafMatrixInterpolatorTest::TestGetCurrentItem_SetTimeStamp_Update()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMETestClass> surfaceTest;
  vtkMAFSmartPointer<vtkSphereSource> sphere;
  sphere->Update();
  surfaceTest->SetData(sphere->GetOutput(), 0.0);
  surfaceTest->Update();

  mafMatrix identity;
  identity.SetTimeStamp(0.0);
  surfaceTest->SetMatrix(identity);
  surfaceTest->Update();

  mafMatrix translated;
  translated.SetTimeStamp(1.0);
  double translation[3] = {2,0,0};
  translated.SetElement(0,3, translation[0]);

  surfaceTest->SetMatrix(translated);
  /* if tested with SetTimeStamp of vme and SetMatrix, there is a different response
  because the developer expects that the matrix will be referred to current time stamp
  of the vme,  instead the last matrix is referred to time 0.0. The time used is mafMatrix timestamp;
  if you want to use vme time use SetPose(matrix, timestamp). */
  surfaceTest->Update();


  mafSmartPointer<mafMatrixInterpolator> mi;
  mi->SetVME(surfaceTest);
  mi->SetTimeStamp(0.0);
  mi->Update();

  //time 0
  mafMatrix *currentMatrix;
  currentMatrix = mi->GetCurrentItem();
  
  mafMatrix control;
  control.Identity();

  m_Result = true;
  int countR = 0, sizeR = 4;
  int countC = 0, sizeC = 4;
  for(;countR < sizeR; countR++)
  {
    for(;countC < sizeC; countC++)
    {
      if(control.GetElement(countR,countC) != currentMatrix->GetElement(countR,countC))
      {
        m_Result = false;
        break;
      }
    }
  }
  

  TEST_RESULT;

  //time 1.0

  mi->SetTimeStamp(1.0);
  mi->Update();
  currentMatrix = mi->GetCurrentItem();

  
  control.DeepCopy(&translated);

  m_Result = true;
  countR = 0;
  countC = 0;
  for(;countR < sizeR; countR++)
  {
    for(;countC < sizeC; countC++)
    {
      if(control.GetElement(countR,countC) != currentMatrix->GetElement(countR,countC))
      {
        m_Result = false;
        break;
      }
    }
  }

}
//----------------------------------------------------------------------------
void mafMatrixInterpolatorTest::TestGetMTime()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafMatrixInterpolator> mi;
  int time1 = mi->GetMTime();
  mi->Modified();
  int time2 = mi->GetMTime();

  m_Result = time2 - time1 > 0;
  TEST_RESULT;
}