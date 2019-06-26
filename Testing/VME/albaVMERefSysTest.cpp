/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMERefSysTest
 Authors: Alberto Losi
 
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
#include "albaVMERefSysTest.h"

#include "albaMatrix.h"
#include "albaTransform.h"
#include "albaSmartPointer.h"
#include "albaVMERefSys.h"
#include "albaVMEOutputSurface.h"
#include "albaVMELandmark.h"
#include "albaVMEGeneric.h"
#include "albaMatrixPipe.h"
#include "albaAbsMatrixPipe.h"
#include "albaVMERoot.h"

#define TEST_RESULT CPPUNIT_ASSERT(result);

//----------------------------------------------------------------------------
void albaVMERefSysTest::TestFixture()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void albaVMERefSysTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMERefSys> vme;

  albaVMERefSys *vme2;
  albaNEW(vme2);
  vme2->Delete();
}
//----------------------------------------------------------------------------
void albaVMERefSysTest::TestAllConstructor()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMERefSys> vme;
  result = vme->GetReferenceCount() == 1; // albaSmartPointer increase the reference count
  TEST_RESULT;

  albaVMERefSys *vme2 = NULL;
  vme2 = albaVMERefSys::New();
  result = vme2->GetReferenceCount() == 0; // default reference count on New
  TEST_RESULT;
  albaDEL(vme2);

  albaVMERefSys *vme3 = NULL;
  albaNEW(vme3);
  result = vme3->GetReferenceCount() == 1; // albaNEW macro increase the reference count
  TEST_RESULT;
  albaDEL(vme3);
}
//----------------------------------------------------------------------------
void albaVMERefSysTest::TestLandmarkAccept()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMERefSys> vme;
  albaSmartPointer<albaVMELandmark> landmark;
  albaSmartPointer<albaVMEGeneric> vmegeneric;

  result = vme->LandmarkAccept(landmark);
  TEST_RESULT;

  result = !vme->LandmarkAccept(vmegeneric);
  TEST_RESULT;

}
//----------------------------------------------------------------------------
void albaVMERefSysTest::TestEquals()
//----------------------------------------------------------------------------
{
  albaMatrix m;
  m.SetElement(0,3,3.0);
  // Matrix pose for VME at the same time of the VME timestamp
  m.SetTimeStamp(1.0);

  albaSmartPointer<albaVMERefSys> vme1;
  vme1->SetName("vme test");
  vme1->SetTimeStamp(1.0);
  vme1->SetAbsMatrix(m);
  vme1->SetScaleFactor(0.1);

  albaSmartPointer<albaVMERefSys> vme2;

  // different name, matrix, timestamp and scale factor
  result = !vme1->Equals(vme2);
  TEST_RESULT;

  // same name but different matrix, timestamp and scale factor
  vme2->SetName("vme test");
  result = !vme1->Equals(vme2);
  TEST_RESULT;

  // same name and timestamp but different matrix and scale factor
  vme2->SetTimeStamp(1.0);
  result = !vme1->Equals(vme2);
  TEST_RESULT;

  // same name and timestamp and matrix but different scale factor
  vme2->SetAbsMatrix(m);
  result = !vme1->Equals(vme2);
  TEST_RESULT;

  // vmes equals
  vme2->SetScaleFactor(0.1);
  result = vme1->Equals(vme2);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMERefSysTest::TestDeepCopy()
//----------------------------------------------------------------------------
{
  albaMatrix m;
  m.SetElement(0,3,3.0);

  // Matrix pose for VME at the same time of the VME timestamp
  m.SetTimeStamp(1.0);

  albaSmartPointer<albaVMERefSys> vme1;
  vme1->SetName("vme test");
  vme1->SetTimeStamp(1.0);
  vme1->SetAbsMatrix(m);
  vme1->SetScaleFactor(0.1);

  albaSmartPointer<albaVMERefSys> vme2;
  vme2->DeepCopy(vme1);

  result = vme1->Equals(vme2);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMERefSysTest::TestSetGetScaleFactor()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMERefSys> vme;
  vme->SetScaleFactor(0.1);
  result = (0.1 == vme->GetScaleFactor());
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMERefSysTest::TestGetSurfaceOutput()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMERefSys> vme;
  albaVMEOutputSurface* vmeoutput = vme->GetSurfaceOutput();
  result = (NULL != vmeoutput);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMERefSysTest::TestSetMatrix()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMERefSys> vme;
  albaMatrix m;
  m.SetElement(0,3,3.0);
  vme->SetMatrix(m);
  albaMatrixPipe *pipe = vme->GetAbsMatrixPipe();
  result = (vme->GetAbsMatrixPipe() ->GetMatrix().Equals(&m));
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMERefSysTest::TestGetLocalTimeStamps()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMERefSys> vme;
  std::vector<albaTimeStamp> kframe;
  vme->GetLocalTimeStamps(kframe);
  result = (kframe.size() == 0);
  TEST_RESULT;

//   This method always returns an empty vector
//   albaMatrix m;
//   m.SetElement(0,3,3.0);
// 
//   std::vector<albaTimeStamp> kframe;
//   kframe.push_back(0.0);
//   kframe.push_back(0.1);
//   kframe.push_back(0.2);
//   kframe.push_back(0.3);
//   
//   for(std::vector<albaTimeStamp>::iterator it = kframe.begin(); it != kframe.end(); it++)
//   {
//     vme->SetTimeStamp((*it));
//     vme->SetMatrix(m);
//   }
//   
//   std::vector<albaTimeStamp> kframe2;
//   vme->GetLocalTimeStamps(kframe2);
// 
//   result = (kframe.size() == kframe2.size());
//   TEST_RESULT;
// 
//   result = true;
//   for(int i = 0; i < kframe.size(); i ++)
//   {
//     result = result && (kframe.at(i) == kframe2.at(i));
//   }
//   TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMERefSysTest::TestIsAnimated()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMERefSys> vme;
  result = !vme->IsAnimated();
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMERefSysTest::TestGetMaterial()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMERefSys> vme;
  mmaMaterial *material = vme->GetMaterial();
  result = (material != NULL);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMERefSysTest::TestGetIcon()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMERefSys> vme;
  char **icon = vme->GetIcon();
  result = (icon != NULL);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMERefSysTest::TestGetVisualPipe()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMERefSys> vme;
  result = (vme->GetVisualPipe() == "albaPipeRefSys");
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMERefSysTest::TestGetOriginVME()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMERefSys> vme;

  albaSmartPointer<albaVMEGeneric> vmeorigin;
  albaSmartPointer<albaVMERoot> root;

  vme->ReparentTo(root);
  vmeorigin->ReparentTo(root);

  vme->SetLink("OriginVME",vmeorigin);

  result = (vme->GetOriginVME() == albaVME::SafeDownCast(vmeorigin));
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMERefSysTest::TestGetPoint1VME()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMERefSys> vme;

  albaSmartPointer<albaVMEGeneric> vmepoint1;
  albaSmartPointer<albaVMERoot> root;

  vme->ReparentTo(root);
  vmepoint1->ReparentTo(root);

  vme->SetLink("Point1VME",vmepoint1);

  result = (vme->GetPoint1VME() == albaVME::SafeDownCast(vmepoint1));
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMERefSysTest::TestGetPoint2VME()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMERefSys> vme;

  albaSmartPointer<albaVMEGeneric> vmepoint2;
  albaSmartPointer<albaVMERoot> root;

  vme->ReparentTo(root);
  vmepoint2->ReparentTo(root);

  vme->SetLink("Point2VME",vmepoint2);

  result = (vme->GetPoint2VME() == albaVME::SafeDownCast(vmepoint2));
  TEST_RESULT;
}