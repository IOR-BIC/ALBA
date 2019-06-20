/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaMatrixInterpolatorTest
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
#include "albaMatrixInterpolatorTest.h"

#include "albaMatrixInterpolator.h"
#include "albaDataPipe.h"
#include "vtkALBASmartPointer.h"
#include "albaSmartPointer.h"
#include "albaVMESurface.h"
#include "albaOBB.h"
#include "albaVMEItem.h"
#include "albaVMEItemVTK.h"
#include "vtkALBADataPipe.h"

#include "vtkSphereSource.h"
#include "vtkPolyData.h"

#define TEST_RESULT CPPUNIT_ASSERT(m_Result);

/** a vme test  class used to test event reception. */
class  albaVMETestClass: public albaVMESurface
{
public:
  /**constructor.*/
	albaVMETestClass(const char *name=NULL):m_Name(name) {}
  /**rtti macro.*/
	albaTypeMacro(albaVMETestClass,albaVMESurface);

  /** catch event sent from other classes.*/
	virtual void OnEvent(albaEventBase *event) {m_Name = "CATCHED";};

	albaString m_Name;
};

//-------------------------------------------------------------------------
albaCxxTypeMacro(albaVMETestClass)
//-------------------------------------------------------------------------

//----------------------------------------------------------------------------
void albaMatrixInterpolatorTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaMatrixInterpolator> mi;

	albaMatrixInterpolator *mi2;
	albaNEW(mi2);
	albaDEL(mi2);
}
//----------------------------------------------------------------------------
void albaMatrixInterpolatorTest::TestAccept()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMETestClass> surfaceTest;
  vtkALBASmartPointer<vtkSphereSource> sphere;
  
  surfaceTest->SetData(sphere->GetOutput(), 0.0);
  surfaceTest->Update();

  /*albaMatrix identity;
  surfaceTest->SetTimeStamp(0.0);
  surfaceTest->SetMatrix(identity);
  surfaceTest->Update();

  albaMatrix translated;
  double translation[3] = {2,0,0};
  translated.SetElement(0,3, translation[0]);

  surfaceTest->SetTimeStamp(1.0);
  surfaceTest->SetMatrix(translated);
  surfaceTest->Update();*/

  albaSmartPointer<albaMatrixInterpolator> mi;
  m_Result = mi->Accept(surfaceTest);
  TEST_RESULT;

  m_Result = !(mi->Accept(NULL));
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaMatrixInterpolatorTest::TestGetCurrentItem_SetTimeStamp_Update()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMETestClass> surfaceTest;
  vtkALBASmartPointer<vtkSphereSource> sphere;
  sphere->Update();
  surfaceTest->SetData(sphere->GetOutput(), 0.0);
  surfaceTest->Update();

  albaMatrix identity;
  identity.SetTimeStamp(0.0);
  surfaceTest->SetMatrix(identity);
  surfaceTest->Update();

  albaMatrix translated;
  translated.SetTimeStamp(1.0);
  double translation[3] = {2,0,0};
  translated.SetElement(0,3, translation[0]);

  surfaceTest->SetMatrix(translated);
  /* if tested with SetTimeStamp of vme and SetMatrix, there is a different response
  because the developer expects that the matrix will be referred to current time stamp
  of the vme,  instead the last matrix is referred to time 0.0. The time used is albaMatrix timestamp;
  if you want to use vme time use SetPose(matrix, timestamp). */
  surfaceTest->Update();


  albaSmartPointer<albaMatrixInterpolator> mi;
  mi->SetVME(surfaceTest);
  mi->SetTimeStamp(0.0);
  mi->Update();

  //time 0
  albaMatrix *currentMatrix;
  currentMatrix = mi->GetCurrentItem();
  
  albaMatrix control;
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
void albaMatrixInterpolatorTest::TestGetMTime()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaMatrixInterpolator> mi;
  int time1 = mi->GetMTime();
  mi->Modified();
  int time2 = mi->GetMTime();

  m_Result = time2 - time1 > 0;
  TEST_RESULT;
}