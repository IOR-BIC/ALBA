/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaMatrixPipeTest
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
#include "albaMatrixPipeTest.h"

#include "albaMatrixPipe.h"
#include "albaVMESurface.h"
#include "albaEventBase.h"

//for testing update , testing the chain of responsibility
#include "albaObserver.h"

#define TEST_RESULT CPPUNIT_ASSERT(result);

//----------------------------------------------------------------------------
void albaMatrixPipeTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaMatrixPipeTest::BeforeTest()
//----------------------------------------------------------------------------
{
  m_SurfaceTest = NULL;
  albaNEW(m_SurfaceTest);
  result = false;
}
//----------------------------------------------------------------------------
void albaMatrixPipeTest::AfterTest()
//----------------------------------------------------------------------------
{
  albaDEL(m_SurfaceTest);
}
//----------------------------------------------------------------------------
void albaMatrixPipeTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaMatrixPipe *matrixPipe;
  albaNEW(matrixPipe);
  //-----------------

  //-----------------
  albaDEL(matrixPipe);
}
//----------------------------------------------------------------------------
void albaMatrixPipeTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  albaMatrixPipe matrixPipe;
}
//----------------------------------------------------------------------------
void albaMatrixPipeTest::TestUpdateMatrixObserverSet_Get_On_Off()
//----------------------------------------------------------------------------
{
  albaMatrixPipe *matrixPipe;
  albaNEW(matrixPipe);

  matrixPipe->UpdateMatrixObserverOn();
  result = true == matrixPipe->GetUpdateMatrixObserverFlag();
  TEST_RESULT(result);

  matrixPipe->UpdateMatrixObserverOff();
  result = false == matrixPipe->GetUpdateMatrixObserverFlag();
  TEST_RESULT(result);

  matrixPipe->SetUpdateMatrixObserverFlag(1);
  result = true == matrixPipe->GetUpdateMatrixObserverFlag();
  TEST_RESULT(result);

  albaDEL(matrixPipe);
  
}
//----------------------------------------------------------------------------
void albaMatrixPipeTest::TestSetGetVME()
//----------------------------------------------------------------------------
{
  albaMatrixPipe *matrixPipe;
  albaNEW(matrixPipe);
  //-----------------

  matrixPipe->SetVME(m_SurfaceTest);
  result = m_SurfaceTest == matrixPipe->GetVME();
  TEST_RESULT(result);

  //-----------------
  albaDEL(matrixPipe);
}
//----------------------------------------------------------------------------
void albaMatrixPipeTest::TestSetGetTimeStamp()
//----------------------------------------------------------------------------
{
  albaMatrixPipe *matrixPipe;
  albaNEW(matrixPipe);
  //-----------------

  albaTimeStamp testTimeStamp = 5.0;
  matrixPipe->SetTimeStamp(testTimeStamp);
  result = testTimeStamp == matrixPipe->GetTimeStamp();
  TEST_RESULT(result);

  //-----------------
  albaDEL(matrixPipe);
}
//----------------------------------------------------------------------------
void albaMatrixPipeTest::TestGetMTime()
//----------------------------------------------------------------------------
{
  albaMatrixPipe *matrixPipe;
  albaNEW(matrixPipe);
  //-----------------
  unsigned long before, after;
  before = matrixPipe->GetMTime();
  matrixPipe->Modified();
  after = matrixPipe->GetMTime();
  result = after > before ; 
  TEST_RESULT(result);

  //-----------------
  albaDEL(matrixPipe);
}
//----------------------------------------------------------------------------
void albaMatrixPipeTest::TestAccept()
//----------------------------------------------------------------------------
{
  albaMatrixPipe *matrixPipe;
  albaNEW(matrixPipe);
  //-----------------

  result = matrixPipe->Accept(m_SurfaceTest);
  TEST_RESULT(result);

  //-----------------
  albaDEL(matrixPipe);
}
//----------------------------------------------------------------------------
void albaMatrixPipeTest::TestGetMatrix()
//----------------------------------------------------------------------------
{
  albaMatrixPipe *matrixPipe;

  //-----------------
  albaMatrix testMatrix1;
  testMatrix1.SetElement(2,2, 2.0);

  m_SurfaceTest->SetMatrix(testMatrix1);
  m_SurfaceTest->Modified();
  m_SurfaceTest->Update();
  matrixPipe =  m_SurfaceTest->GetMatrixPipe();
  matrixPipe->Modified();
  matrixPipe->Update();

  albaMatrix resultMatrix;
  resultMatrix = matrixPipe->GetMatrix();

  result = resultMatrix.Equals(&testMatrix1);
  TEST_RESULT(result);


}
//----------------------------------------------------------------------------
void albaMatrixPipeTest::TestMakeACopy()
//----------------------------------------------------------------------------
{
  albaMatrixPipe *matrixPipe;

  //-----------------
  albaMatrix testMatrix1;
  testMatrix1.SetElement(2,2, 2.0);
  m_SurfaceTest->SetMatrix(testMatrix1);
  m_SurfaceTest->Modified();
  m_SurfaceTest->Update();
  matrixPipe =  m_SurfaceTest->GetMatrixPipe();

  albaMatrixPipe *resultPipeMatrix;
  resultPipeMatrix = ((albaMatrixPipe *) matrixPipe)->MakeACopy();
  result = resultPipeMatrix != NULL && resultPipeMatrix->GetVME() == matrixPipe->GetVME();

  albaDEL(resultPipeMatrix);

  TEST_RESULT(result);
}
//----------------------------------------------------------------------------
void albaMatrixPipeTest::TestDeepCopy()
//----------------------------------------------------------------------------
{
  albaMatrixPipe *matrixPipe;

  //-----------------
  albaMatrix testMatrix1;
  testMatrix1.SetElement(2,2, 2.0);
  m_SurfaceTest->SetMatrix(testMatrix1);
  m_SurfaceTest->Modified();
  m_SurfaceTest->Update();
  matrixPipe =  m_SurfaceTest->GetMatrixPipe();

  albaMatrixPipe *resultPipeMatrix;
  albaNEW(resultPipeMatrix);
  resultPipeMatrix->DeepCopy(matrixPipe);
  result = resultPipeMatrix != NULL && resultPipeMatrix->GetVME() == matrixPipe->GetVME();

  albaDEL(resultPipeMatrix);

  TEST_RESULT(result);
}

/** an observer class used to test event reception. */
class albaObserverTestClass: public albaObserver
{
public:
  albaObserverTestClass(const char *name=NULL):Name(name) {}

  virtual void OnEvent(albaEventBase *event) {Name = "CATCHED";};

  albaString     Name;
  albaEventBase  LastEvent; // used to store last receive event contents
};


//----------------------------------------------------------------------------
void albaMatrixPipeTest::TestUpdate()
//----------------------------------------------------------------------------
{

  albaMatrixPipe *matrixPipe;
  albaNEW(matrixPipe);
  //-----------------
  //for testing update , testing the chain of responsibility
  //  
  //
  albaObserverTestClass testObserver;
  m_SurfaceTest->AddObserver(testObserver);

  matrixPipe->SetVME(m_SurfaceTest);
  matrixPipe->Update();
   
  result = testObserver.Name.Equals("CATCHED");
  m_SurfaceTest->RemoveObserver(&testObserver);

  TEST_RESULT(result);
  //-----------------
  albaDEL(matrixPipe);

}