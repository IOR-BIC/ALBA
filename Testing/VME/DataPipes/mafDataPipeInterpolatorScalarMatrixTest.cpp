/*=========================================================================

 Program: MAF2
 Module: mafDataPipeInterpolatorScalarMatrixTest
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
// Failing in doing this will m_Result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "mafDataPipeInterpolatorScalarMatrixTest.h"

#include "mafDataPipeInterpolatorScalarMatrix.h"
#include "mafDataPipe.h"
#include "vtkMAFSmartPointer.h"
#include "mafSmartPointer.h"
#include "mafVMEScalarMatrix.h"
#include "mafOBB.h"
#include "mafVMEItem.h"
#include "mafVMEItemScalarMatrix.h"

#define TEST_RESULT CPPUNIT_ASSERT(m_Result);

/** a vme test  class used to test event reception. */
class mafVMETestClass: public mafVMEScalarMatrix
{
public:
  /* constructor */
	mafVMETestClass(const char *name=NULL):Name(name) {}
  /** RTTI macro */
	mafTypeMacro(mafVMETestClass,mafVMEScalarMatrix);

  /** process events */
	virtual void OnEvent(mafEventBase *event) {Name = "CATCHED";};

	mafString     Name;
};

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMETestClass)
//-------------------------------------------------------------------------

//----------------------------------------------------------------------------
void mafDataPipeInterpolatorScalarMatrixTest::setUp()
//----------------------------------------------------------------------------
{
  m_Result = false;

  m_DataMatrix.set_size(10,20);
  m_DataMatrix.fill(1.0);
}
//----------------------------------------------------------------------------
void mafDataPipeInterpolatorScalarMatrixTest::tearDown()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafDataPipeInterpolatorScalarMatrixTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafDataPipeInterpolatorScalarMatrix> smi;

	mafDataPipeInterpolatorScalarMatrix *smi2;
	mafNEW(smi2);
	mafDEL(smi2);
}
//----------------------------------------------------------------------------
void mafDataPipeInterpolatorScalarMatrixTest::TestAccept()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMETestClass> matrixTest;
  matrixTest->SetData(m_DataMatrix, 0.);

  mafSmartPointer<mafDataPipeInterpolatorScalarMatrix> di;
  m_Result = di->Accept(matrixTest);
  TEST_RESULT;

  m_Result = !(di->Accept(NULL));
  TEST_RESULT;
}

//----------------------------------------------------------------------------
void mafDataPipeInterpolatorScalarMatrixTest::TestOnEvent()
//----------------------------------------------------------------------------
{
  //catch events inside vme : VME_OUTPUT_DATA_PREUPDATE, VME_OUTPUT_DATA_UPDATE

  mafSmartPointer<mafVMETestClass> matrixTest1;

  mafSmartPointer<mafDataPipeInterpolatorScalarMatrix> smi;
  smi->SetVME(matrixTest1);
  smi->OnEvent(&mafEvent(NULL,VME_OUTPUT_DATA_PREUPDATE));
  m_Result = matrixTest1->Name.Equals("CATCHED");
  TEST_RESULT;

  mafSmartPointer<mafVMETestClass> matrixTest2;
  smi->SetVME(matrixTest2);
  smi->OnEvent(&mafEvent(NULL,VME_OUTPUT_DATA_UPDATE)); 

  m_Result = matrixTest2->Name.Equals("CATCHED");
  TEST_RESULT;

}

//----------------------------------------------------------------------------
void mafDataPipeInterpolatorScalarMatrixTest::TestGetScalarData()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMETestClass> matrixTest;

  matrixTest->SetData(m_DataMatrix, 0.0);
  matrixTest->Update();

  mafSmartPointer<mafDataPipeInterpolatorScalarMatrix> smi;
  smi->SetVME(matrixTest);
  smi->Update();

  int numberOfRows = m_DataMatrix.rows();
  int numberOfCols = m_DataMatrix.columns();
  int checkNumberOfRows = smi->GetScalarData().rows();
  int checkNumberOfCols = smi->GetScalarData().columns();
  
  m_Result =  numberOfRows == checkNumberOfRows && 
              numberOfCols == checkNumberOfCols;
  TEST_RESULT;

  //check if the elements have the same value
  int i=0, j=0;
  for(;i< numberOfRows; i++)
  {
    for(;j< numberOfCols; j++)
    {
      if(m_DataMatrix.get(i,j) != smi->GetScalarData().get(i,j))
      {
        m_Result = false;
      }
    }
  }

  TEST_RESULT;
}

//----------------------------------------------------------------------------
void mafDataPipeInterpolatorScalarMatrixTest::TestGetCurrentItem()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMETestClass> matrixTest;
  
  int numberOfRows = m_DataMatrix.rows();
  int numberOfCols = m_DataMatrix.columns();
  
  matrixTest->SetData(m_DataMatrix, 0.0);
  matrixTest->Update();

  mafSmartPointer<mafDataPipeInterpolatorScalarMatrix> smi;
  smi->SetVME(matrixTest);
  smi->UpdateBounds();

  mafVMEItem *currentItem;
  currentItem = smi->GetCurrentItem();
  
  int checkNumberOfRows = mafVMEItemScalarMatrix::SafeDownCast(currentItem)->GetData().rows();
  int checkNumberOfCols = mafVMEItemScalarMatrix::SafeDownCast(currentItem)->GetData().columns();

  m_Result = currentItem != NULL && 
             currentItem->IsA("mafVMEItemScalarMatrix") && 
             numberOfRows == checkNumberOfRows && 
             numberOfCols == checkNumberOfCols;

  TEST_RESULT;

  //check if the elements have the same value
  int i=0, j=0;
  for(;i< numberOfRows; i++)
  {
    for(;j< numberOfCols; j++)
    {
      if(m_DataMatrix.get(i,j) != mafVMEItemScalarMatrix::SafeDownCast(currentItem)->GetData().get(i,j))
      {
        m_Result = false;
      }
    }
  }

  TEST_RESULT;

}