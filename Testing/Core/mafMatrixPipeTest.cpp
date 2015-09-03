/*=========================================================================

 Program: MAF2
 Module: mafMatrixPipeTest
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
#include "mafMatrixPipeTest.h"

#include "mafMatrixPipe.h"
#include "mafVMESurface.h"
#include "mafEventBase.h"

//for testing update , testing the chain of responsability
#include "mafObserver.h"
#include "mafEventSource.h"


#define TEST_RESULT CPPUNIT_ASSERT(result);

//----------------------------------------------------------------------------
void mafMatrixPipeTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafMatrixPipeTest::BeforeTest()
//----------------------------------------------------------------------------
{
  m_SurfaceTest = NULL;
  mafNEW(m_SurfaceTest);
  result = false;
}
//----------------------------------------------------------------------------
void mafMatrixPipeTest::AfterTest()
//----------------------------------------------------------------------------
{
  mafDEL(m_SurfaceTest);
}
//----------------------------------------------------------------------------
void mafMatrixPipeTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafMatrixPipe *matrixPipe;
  mafNEW(matrixPipe);
  //-----------------

  //-----------------
  mafDEL(matrixPipe);
}
//----------------------------------------------------------------------------
void mafMatrixPipeTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  mafMatrixPipe matrixPipe;
}
//----------------------------------------------------------------------------
void mafMatrixPipeTest::TestUpdateMatrixObserverSet_Get_On_Off()
//----------------------------------------------------------------------------
{
  mafMatrixPipe *matrixPipe;
  mafNEW(matrixPipe);

  matrixPipe->UpdateMatrixObserverOn();
  result = true == matrixPipe->GetUpdateMatrixObserverFlag();
  TEST_RESULT(result);

  matrixPipe->UpdateMatrixObserverOff();
  result = false == matrixPipe->GetUpdateMatrixObserverFlag();
  TEST_RESULT(result);

  matrixPipe->SetUpdateMatrixObserverFlag(1);
  result = true == matrixPipe->GetUpdateMatrixObserverFlag();
  TEST_RESULT(result);

  mafDEL(matrixPipe);
  
}
//----------------------------------------------------------------------------
void mafMatrixPipeTest::TestSetGetVME()
//----------------------------------------------------------------------------
{
  mafMatrixPipe *matrixPipe;
  mafNEW(matrixPipe);
  //-----------------

  matrixPipe->SetVME(m_SurfaceTest);
  result = m_SurfaceTest == matrixPipe->GetVME();
  TEST_RESULT(result);

  //-----------------
  mafDEL(matrixPipe);
}
//----------------------------------------------------------------------------
void mafMatrixPipeTest::TestSetGetTimeStamp()
//----------------------------------------------------------------------------
{
  mafMatrixPipe *matrixPipe;
  mafNEW(matrixPipe);
  //-----------------

  mafTimeStamp testTimeStamp = 5.0;
  matrixPipe->SetTimeStamp(testTimeStamp);
  result = testTimeStamp == matrixPipe->GetTimeStamp();
  TEST_RESULT(result);

  //-----------------
  mafDEL(matrixPipe);
}
//----------------------------------------------------------------------------
void mafMatrixPipeTest::TestGetMTime()
//----------------------------------------------------------------------------
{
  mafMatrixPipe *matrixPipe;
  mafNEW(matrixPipe);
  //-----------------
  unsigned long before, after;
  before = matrixPipe->GetMTime();
  matrixPipe->Modified();
  after = matrixPipe->GetMTime();
  result = after > before ; 
  TEST_RESULT(result);

  //-----------------
  mafDEL(matrixPipe);
}
//----------------------------------------------------------------------------
void mafMatrixPipeTest::TestAccept()
//----------------------------------------------------------------------------
{
  mafMatrixPipe *matrixPipe;
  mafNEW(matrixPipe);
  //-----------------

  result = matrixPipe->Accept(m_SurfaceTest);
  TEST_RESULT(result);

  //-----------------
  mafDEL(matrixPipe);
}
//----------------------------------------------------------------------------
void mafMatrixPipeTest::TestGetMatrix()
//----------------------------------------------------------------------------
{
  mafMatrixPipe *matrixPipe;

  //-----------------
  mafMatrix testMatrix1;
  testMatrix1.SetElement(2,2, 2.0);

  m_SurfaceTest->SetMatrix(testMatrix1);
  m_SurfaceTest->Modified();
  m_SurfaceTest->Update();
  matrixPipe =  m_SurfaceTest->GetMatrixPipe();
  matrixPipe->Modified();
  matrixPipe->Update();

  mafMatrix resultMatrix;
  resultMatrix = matrixPipe->GetMatrix();

  result = resultMatrix.Equals(&testMatrix1);
  TEST_RESULT(result);


}
//----------------------------------------------------------------------------
void mafMatrixPipeTest::TestMakeACopy()
//----------------------------------------------------------------------------
{
  mafMatrixPipe *matrixPipe;

  //-----------------
  mafMatrix testMatrix1;
  testMatrix1.SetElement(2,2, 2.0);
  m_SurfaceTest->SetMatrix(testMatrix1);
  m_SurfaceTest->Modified();
  m_SurfaceTest->Update();
  matrixPipe =  m_SurfaceTest->GetMatrixPipe();

  mafMatrixPipe *resultPipeMatrix;
  resultPipeMatrix = ((mafMatrixPipe *) matrixPipe)->MakeACopy();
  result = resultPipeMatrix != NULL && resultPipeMatrix->GetVME() == matrixPipe->GetVME();

  mafDEL(resultPipeMatrix);

  TEST_RESULT(result);
}
//----------------------------------------------------------------------------
void mafMatrixPipeTest::TestDeepCopy()
//----------------------------------------------------------------------------
{
  mafMatrixPipe *matrixPipe;

  //-----------------
  mafMatrix testMatrix1;
  testMatrix1.SetElement(2,2, 2.0);
  m_SurfaceTest->SetMatrix(testMatrix1);
  m_SurfaceTest->Modified();
  m_SurfaceTest->Update();
  matrixPipe =  m_SurfaceTest->GetMatrixPipe();

  mafMatrixPipe *resultPipeMatrix;
  mafNEW(resultPipeMatrix);
  resultPipeMatrix->DeepCopy(matrixPipe);
  result = resultPipeMatrix != NULL && resultPipeMatrix->GetVME() == matrixPipe->GetVME();

  mafDEL(resultPipeMatrix);

  TEST_RESULT(result);
}

/** an observer class used to test event reception. */
class mafObserverTestClass: public mafObserver
{
public:
  mafObserverTestClass(const char *name=NULL):Name(name) {}

  virtual void OnEvent(mafEventBase *event) {Name = "CATCHED";};

  mafString     Name;
  mafEventBase  LastEvent; // used to store last receive event contents
};


//----------------------------------------------------------------------------
void mafMatrixPipeTest::TestUpdate()
//----------------------------------------------------------------------------
{

  mafMatrixPipe *matrixPipe;
  mafNEW(matrixPipe);
  //-----------------
  //for testing update , testing the chain of responsability
  //  
  //
  mafObserverTestClass testObserver;
  m_SurfaceTest->GetEventSource()->AddObserver(testObserver);

  matrixPipe->SetVME(m_SurfaceTest);
  matrixPipe->Update();
   
  result = testObserver.Name.Equals("CATCHED");
  m_SurfaceTest->GetEventSource()->RemoveObserver(&testObserver);

  TEST_RESULT(result);
  //-----------------
  mafDEL(matrixPipe);

}