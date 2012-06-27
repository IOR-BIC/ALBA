/*=========================================================================

 Program: MAF2
 Module: mafVMEInfoTextTest
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
#include "mafVMEInfoTextTest.h"

#include "mafVMEInfoText.h"
#include "mafSmartPointer.h"

#include <iostream>

#define TEST_RESULT CPPUNIT_ASSERT(result);

//----------------------------------------------------------------------------
void mafVMEInfoTextTest::setUp()
//----------------------------------------------------------------------------
{
  result = false;
}
//----------------------------------------------------------------------------
void mafVMEInfoTextTest::tearDown()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void mafVMEInfoTextTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{  
  mafVMEInfoText *infoVme;
  mafNEW(infoVme);
  mafDEL(infoVme);
  result = true; //used for detecting leaks
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafVMEInfoTextTest::TestGetVTKOutput()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMEInfoText> infoVME;
  result = infoVME->GetVTKOutput()->IsA("mafVMEOutputNULL") && 
           infoVME->GetVTKOutput()->GetVTKData() == NULL;
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafVMEInfoTextTest::TestGetOutput()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMEInfoText> infoVME;
  result = infoVME->GetOutput()->IsA("mafVMEOutputNULL") && 
           infoVME->GetOutput()->GetVTKData() == NULL;
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafVMEInfoTextTest::TestAddString()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMEInfoText> infoVME;

  enum
  {
    NUMBER_OF_TEST_STRING = 3,
  };

  mafString testString1 = "First Test String";
  mafString testString2 = "Second Test String";
  mafString testString3 = "Third Test String";
  infoVME->AddString(testString1);
  infoVME->AddString(testString2);
  infoVME->AddString(testString3);

  result = NUMBER_OF_TEST_STRING == infoVME->GetNumberOfStrings();
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafVMEInfoTextTest::TestGetSetPosShow()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMEInfoText> infoVME;

  infoVME->SetPosLabel("PosLabel0 ", 0);
  infoVME->SetPosLabel("PosLabel1 ", 1);
  infoVME->SetPosLabel("PosLabel2 ", 2);

  mafMatrix matrix;
  matrix.SetElement(0, 3, 10); //translate 10 in x
  matrix.SetElement(1, 3, 5); //translate 5 in y
  matrix.SetElement(2, 3, 1); //translate 1 in z

  infoVME->SetMatrix(matrix);
  infoVME->Update();

  infoVME->SetPosShow(true, 0);
  infoVME->SetPosShow(true, 1);
  infoVME->SetPosShow(false, 2);

  result = infoVME->GetPosShow(0) == true &&
           infoVME->GetPosShow(1) == true &&
           infoVME->GetPosShow(2) == false;

  //warning : in this case it is append to the string the translation with 6 digits
  mafString control1 = infoVME->GetPosText(0);
  mafString control2 = infoVME->GetPosText(1);
  mafString control3 = infoVME->GetPosText(2);
  bool result1 = control1.Equals("PosLabel0 10.000000");
  bool result2 = control2.Equals("PosLabel1 5.000000");
  bool result3 = control3.Equals("");

  result = result && result1 && result2 && result3;

  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafVMEInfoTextTest::TestGetSetPosLabel()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMEInfoText> infoVME;
  
  infoVME->SetPosLabel("PosLabel_0", 0);
  infoVME->SetPosLabel("PosLabel_1", 1);
  infoVME->SetPosLabel("PosLabel_2", 2);

  result = mafString(infoVME->GetPosLabel(0)).Equals("PosLabel_0") &&
           mafString(infoVME->GetPosLabel(1)).Equals("PosLabel_1") &&
           mafString(infoVME->GetPosLabel(2)).Equals("PosLabel_2");

  TEST_RESULT;
}

//----------------------------------------------------------------------------
void mafVMEInfoTextTest::TestSetTimeStamp()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMEInfoText> infoVME;

  infoVME->SetPosLabel("PosLabel0 ", 0);
  infoVME->SetPosLabel("PosLabel1 ", 1);
  infoVME->SetPosLabel("PosLabel2 ", 2);

  infoVME->SetPosShow(true, 0);
  infoVME->SetPosShow(true, 1);
  infoVME->SetPosShow(false, 2); //hide completely the string assigning  ""

  mafMatrix matrix;
  matrix.SetElement(0, 3, 10); //translate 10 in x
  matrix.SetElement(1, 3, 5); //translate 5 in y
  matrix.SetElement(2, 3, 1); //translate 1 in z
  
  infoVME->SetMatrix(matrix);
  infoVME->Update();

  infoVME->SetTimeStamp(1.0);

  //warning : in this case it is append to the string the translation with 6 digits
  mafString control1 = infoVME->GetPosText(0);
  mafString control2 = infoVME->GetPosText(1);
  mafString control3 = infoVME->GetPosText(2);
  bool result1 = control1.Equals("PosLabel0 10.000000");
  bool result2 = control2.Equals("PosLabel1 5.000000");
  bool result3 = control3.Equals("");

  result = result1 && result2 && result3;

  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafVMEInfoTextTest::TestGetIcon()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMEInfoText> infoVME;
  char **icon = NULL;
  icon = infoVME->GetIcon();
  result = icon != NULL;
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafVMEInfoTextTest::TestDeepCopy()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMEInfoText> infoVME;

  enum
  {
    NUMBER_OF_TEST_STRING = 2,
  };

  mafString testString1 = "First Test DeepCopy String";
  mafString testString2 = "Second Test DeepCopy String";
  infoVME->AddString(testString1);
  infoVME->AddString(testString2);
  
  mafSmartPointer<mafVMEInfoText> infoVMECopy;
  infoVMECopy->DeepCopy(infoVME);

  result = infoVME->GetNumberOfStrings() == infoVMECopy->GetNumberOfStrings();

  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafVMEInfoTextTest::TestEquals()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMEInfoText> infoVME;

  enum
  {
    NUMBER_OF_TEST_STRING = 2,
  };

  mafString testString1 = "First Test Equals String";
  mafString testString2 = "Second Test Equals String";
  infoVME->AddString(testString1);
  infoVME->AddString(testString2);

  mafSmartPointer<mafVMEInfoText> infoVMECopy;
  infoVMECopy->DeepCopy(infoVME);

  result = infoVMECopy->Equals(infoVME);

  TEST_RESULT;

  //create a difference
  infoVMECopy->AddString("Third  Test Equals String");

  result = !infoVMECopy->Equals(infoVME);
  TEST_RESULT;
}