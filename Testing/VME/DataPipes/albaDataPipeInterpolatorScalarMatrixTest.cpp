/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaDataPipeInterpolatorScalarMatrixTest
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
// Failing in doing this will m_Result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "albaDataPipeInterpolatorScalarMatrixTest.h"

#include "albaDataPipeInterpolatorScalarMatrix.h"
#include "albaDataPipe.h"
#include "vtkALBASmartPointer.h"
#include "albaSmartPointer.h"
#include "albaVMEScalarMatrix.h"
#include "albaOBB.h"
#include "albaVMEItem.h"
#include "albaVMEItemScalarMatrix.h"

#define TEST_RESULT CPPUNIT_ASSERT(m_Result);

/** a vme test  class used to test event reception. */
class albaVMEScalarMatrixTestClass: public albaVMEScalarMatrix
{
public:
  /* constructor */
	albaVMEScalarMatrixTestClass(const char *name=NULL):Name(name) {}
  /** RTTI macro */
	albaTypeMacro(albaVMEScalarMatrixTestClass,albaVMEScalarMatrix);

  /** process events */
	virtual void OnEvent(albaEventBase *event) {Name = "CATCHED";};

	albaString     Name;
};

//-------------------------------------------------------------------------
albaCxxTypeMacro(albaVMEScalarMatrixTestClass)
//-------------------------------------------------------------------------

//----------------------------------------------------------------------------
void albaDataPipeInterpolatorScalarMatrixTest::BeforeTest()
//----------------------------------------------------------------------------
{
  m_DataMatrix.set_size(10,20);
  m_DataMatrix.fill(1.0);
}

//----------------------------------------------------------------------------
void albaDataPipeInterpolatorScalarMatrixTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaDataPipeInterpolatorScalarMatrix> smi;

	albaDataPipeInterpolatorScalarMatrix *smi2;
	albaNEW(smi2);
	albaDEL(smi2);
}
//----------------------------------------------------------------------------
void albaDataPipeInterpolatorScalarMatrixTest::TestAccept()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEScalarMatrixTestClass> matrixTest;
  matrixTest->SetData(m_DataMatrix, 0.);

  albaSmartPointer<albaDataPipeInterpolatorScalarMatrix> di;
  m_Result = di->Accept(matrixTest);
  TEST_RESULT;

  m_Result = !(di->Accept(NULL));
  TEST_RESULT;
}

//----------------------------------------------------------------------------
void albaDataPipeInterpolatorScalarMatrixTest::TestOnEvent()
//----------------------------------------------------------------------------
{
  //catch events inside vme : VME_OUTPUT_DATA_PREUPDATE, VME_OUTPUT_DATA_UPDATE

  albaSmartPointer<albaVMEScalarMatrixTestClass> matrixTest1;

  albaSmartPointer<albaDataPipeInterpolatorScalarMatrix> smi;
  smi->SetVME(matrixTest1);
  smi->OnEvent(&albaEvent(NULL,VME_OUTPUT_DATA_PREUPDATE));
  m_Result = matrixTest1->Name.Equals("CATCHED");
  TEST_RESULT;

  albaSmartPointer<albaVMEScalarMatrixTestClass> matrixTest2;
  smi->SetVME(matrixTest2);
  smi->OnEvent(&albaEvent(NULL,VME_OUTPUT_DATA_UPDATE)); 

  m_Result = matrixTest2->Name.Equals("CATCHED");
  TEST_RESULT;

}

//----------------------------------------------------------------------------
void albaDataPipeInterpolatorScalarMatrixTest::TestGetScalarData()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEScalarMatrixTestClass> matrixTest;

  matrixTest->SetData(m_DataMatrix, 0.0);
  matrixTest->Update();

  albaSmartPointer<albaDataPipeInterpolatorScalarMatrix> smi;
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
void albaDataPipeInterpolatorScalarMatrixTest::TestGetCurrentItem()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEScalarMatrixTestClass> matrixTest;
  
  int numberOfRows = m_DataMatrix.rows();
  int numberOfCols = m_DataMatrix.columns();
  
  matrixTest->SetData(m_DataMatrix, 0.0);
  matrixTest->Update();

  albaSmartPointer<albaDataPipeInterpolatorScalarMatrix> smi;
  smi->SetVME(matrixTest);
  smi->UpdateBounds();

  albaVMEItem *currentItem;
  currentItem = smi->GetCurrentItem();
  
  int checkNumberOfRows = albaVMEItemScalarMatrix::SafeDownCast(currentItem)->GetData().rows();
  int checkNumberOfCols = albaVMEItemScalarMatrix::SafeDownCast(currentItem)->GetData().columns();

  m_Result = currentItem != NULL && 
             currentItem->IsA("albaVMEItemScalarMatrix") && 
             numberOfRows == checkNumberOfRows && 
             numberOfCols == checkNumberOfCols;

  TEST_RESULT;

  //check if the elements have the same value
  int i=0, j=0;
  for(;i< numberOfRows; i++)
  {
    for(;j< numberOfCols; j++)
    {
      if(m_DataMatrix.get(i,j) != albaVMEItemScalarMatrix::SafeDownCast(currentItem)->GetData().get(i,j))
      {
        m_Result = false;
      }
    }
  }

  TEST_RESULT;

}