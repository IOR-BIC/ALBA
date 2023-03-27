/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEInfoTextTest
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
#include "albaVMEInfoTextTest.h"

#include "albaVMEInfoText.h"
#include "albaSmartPointer.h"

#include <iostream>

#define TEST_RESULT CPPUNIT_ASSERT(result);

//----------------------------------------------------------------------------
void albaVMEInfoTextTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{  
  albaVMEInfoText *infoVme;
  albaNEW(infoVme);
  albaDEL(infoVme);
  result = true; //used for detecting leaks
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMEInfoTextTest::TestGetVTKOutput()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEInfoText> infoVME;
  result = infoVME->GetVTKOutput()->IsA("albaVMEOutputNULL") && 
           infoVME->GetVTKOutput()->GetVTKData() == NULL;
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMEInfoTextTest::TestGetOutput()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEInfoText> infoVME;
  result = infoVME->GetOutput()->IsA("albaVMEOutputNULL") && 
           infoVME->GetOutput()->GetVTKData() == NULL;
  TEST_RESULT;
}

//----------------------------------------------------------------------------
void albaVMEInfoTextTest::TestGetSetLabel()
{
  albaSmartPointer<albaVMEInfoText> infoVME;
  
  infoVME->SetLabel("PosLabel_0");

	result = albaString(infoVME->GetLabel()).Equals("PosLabel_0");
  TEST_RESULT;
}

//----------------------------------------------------------------------------
void albaVMEInfoTextTest::TestGetIcon()
{
  albaSmartPointer<albaVMEInfoText> infoVME;
  char **icon = NULL;
  icon = infoVME->GetIcon();
  result = icon != NULL;
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMEInfoTextTest::TestDeepCopy()
{
  albaSmartPointer<albaVMEInfoText> infoVME;

  enum
  {
    NUMBER_OF_TEST_STRING = 2,
  };

  albaString testString1 = "Test DeepCopy String";
  infoVME->SetLabel(testString1);
  
  albaSmartPointer<albaVMEInfoText> infoVMECopy;
  infoVMECopy->DeepCopy(infoVME);

  result = testString1.Equals(infoVMECopy->GetLabel());

  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMEInfoTextTest::TestEquals()
{
  albaSmartPointer<albaVMEInfoText> infoVME;

  enum
  {
    NUMBER_OF_TEST_STRING = 2,
  };

  albaString testString1 = "First Test Equals String";
  albaString testString2 = "Second Test Equals String";
  infoVME->SetLabel(testString1);

  albaSmartPointer<albaVMEInfoText> infoVMECopy;
  infoVMECopy->DeepCopy(infoVME);

  result = infoVMECopy->Equals(infoVME);

  TEST_RESULT;

  //create a difference
  infoVMECopy->SetLabel(testString2);

  result = !infoVMECopy->Equals(infoVME);
  TEST_RESULT;
}