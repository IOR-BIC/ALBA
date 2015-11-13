/*=========================================================================

 Program: MAF2
 Module: mafVMERefSysTest
 Authors: Alberto Losi
 
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
#include "mafVMERefSysTest.h"

#include "mafMatrix.h"
#include "mafTransform.h"
#include "mafSmartPointer.h"
#include "mafVMERefSys.h"
#include "mafVMEOutputSurface.h"
#include "mafVMELandmark.h"
#include "mafVMEGeneric.h"
#include "mafMatrixPipe.h"
#include "mafAbsMatrixPipe.h"
#include "mafVMERoot.h"

#define TEST_RESULT CPPUNIT_ASSERT(result);

//----------------------------------------------------------------------------
void mafVMERefSysTest::TestFixture()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void mafVMERefSysTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMERefSys> vme;

  mafVMERefSys *vme2;
  mafNEW(vme2);
  vme2->Delete();
}
//----------------------------------------------------------------------------
void mafVMERefSysTest::TestAllConstructor()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMERefSys> vme;
  result = vme->GetReferenceCount() == 1; // mafSmartPointer increase the reference count
  TEST_RESULT;

  mafVMERefSys *vme2 = NULL;
  vme2 = mafVMERefSys::New();
  result = vme2->GetReferenceCount() == 0; // default reference count on New
  TEST_RESULT;
  mafDEL(vme2);

  mafVMERefSys *vme3 = NULL;
  mafNEW(vme3);
  result = vme3->GetReferenceCount() == 1; // mafNEW macro increase the reference count
  TEST_RESULT;
  mafDEL(vme3);
}
//----------------------------------------------------------------------------
void mafVMERefSysTest::TestLandmarkAccept()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMERefSys> vme;
  mafSmartPointer<mafVMELandmark> landmark;
  mafSmartPointer<mafVMEGeneric> vmegeneric;

  result = vme->LandmarkAccept(landmark);
  TEST_RESULT;

  result = !vme->LandmarkAccept(vmegeneric);
  TEST_RESULT;

}
//----------------------------------------------------------------------------
void mafVMERefSysTest::TestEquals()
//----------------------------------------------------------------------------
{
  mafMatrix m;
  m.SetElement(0,3,3.0);
  // Matrix pose for VME at the same time of the VME timestamp
  m.SetTimeStamp(1.0);

  mafSmartPointer<mafVMERefSys> vme1;
  vme1->SetName("vme test");
  vme1->SetTimeStamp(1.0);
  vme1->SetAbsMatrix(m);
  vme1->SetScaleFactor(0.1);

  mafSmartPointer<mafVMERefSys> vme2;

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
void mafVMERefSysTest::TestDeepCopy()
//----------------------------------------------------------------------------
{
  mafMatrix m;
  m.SetElement(0,3,3.0);

  // Matrix pose for VME at the same time of the VME timestamp
  m.SetTimeStamp(1.0);

  mafSmartPointer<mafVMERefSys> vme1;
  vme1->SetName("vme test");
  vme1->SetTimeStamp(1.0);
  vme1->SetAbsMatrix(m);
  vme1->SetScaleFactor(0.1);

  mafSmartPointer<mafVMERefSys> vme2;
  vme2->DeepCopy(vme1);

  result = vme1->Equals(vme2);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafVMERefSysTest::TestSetGetScaleFactor()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMERefSys> vme;
  vme->SetScaleFactor(0.1);
  result = (0.1 == vme->GetScaleFactor());
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafVMERefSysTest::TestGetSurfaceOutput()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMERefSys> vme;
  mafVMEOutputSurface* vmeoutput = vme->GetSurfaceOutput();
  result = (NULL != vmeoutput);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafVMERefSysTest::TestSetMatrix()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMERefSys> vme;
  mafMatrix m;
  m.SetElement(0,3,3.0);
  vme->SetMatrix(m);
  mafMatrixPipe *pipe = vme->GetAbsMatrixPipe();
  result = (vme->GetAbsMatrixPipe() ->GetMatrix().Equals(&m));
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafVMERefSysTest::TestGetLocalTimeStamps()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMERefSys> vme;
  std::vector<mafTimeStamp> kframe;
  vme->GetLocalTimeStamps(kframe);
  result = (kframe.size() == 0);
  TEST_RESULT;

//   This method always returns an empty vector
//   mafMatrix m;
//   m.SetElement(0,3,3.0);
// 
//   std::vector<mafTimeStamp> kframe;
//   kframe.push_back(0.0);
//   kframe.push_back(0.1);
//   kframe.push_back(0.2);
//   kframe.push_back(0.3);
//   
//   for(std::vector<mafTimeStamp>::iterator it = kframe.begin(); it != kframe.end(); it++)
//   {
//     vme->SetTimeStamp((*it));
//     vme->SetMatrix(m);
//   }
//   
//   std::vector<mafTimeStamp> kframe2;
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
void mafVMERefSysTest::TestIsAnimated()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMERefSys> vme;
  result = !vme->IsAnimated();
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafVMERefSysTest::TestGetMaterial()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMERefSys> vme;
  mmaMaterial *material = vme->GetMaterial();
  result = (material != NULL);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafVMERefSysTest::TestGetIcon()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMERefSys> vme;
  char **icon = vme->GetIcon();
  result = (icon != NULL);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafVMERefSysTest::TestGetVisualPipe()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMERefSys> vme;
  result = (vme->GetVisualPipe() == "mafPipeRefSys");
  TEST_RESULT;
}
//----------------------------------------------------------------------------
//void mafVMERefSysTest::TestSetRefSysLink()
//----------------------------------------------------------------------------
// {
// 
// }
//----------------------------------------------------------------------------
void mafVMERefSysTest::TestGetOriginVME()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMERefSys> vme;

  mafSmartPointer<mafVMEGeneric> vmeorigin;
  mafSmartPointer<mafVMERoot> root;

  vme->ReparentTo(root);
  vmeorigin->ReparentTo(root);

  vme->SetRefSysLink("OriginVME",vmeorigin);

  result = (vme->GetOriginVME() == mafVME::SafeDownCast(vmeorigin));
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafVMERefSysTest::TestGetPoint1VME()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMERefSys> vme;

  mafSmartPointer<mafVMEGeneric> vmepoint1;
  mafSmartPointer<mafVMERoot> root;

  vme->ReparentTo(root);
  vmepoint1->ReparentTo(root);

  vme->SetRefSysLink("Point1VME",vmepoint1);

  result = (vme->GetPoint1VME() == mafVME::SafeDownCast(vmepoint1));
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafVMERefSysTest::TestGetPoint2VME()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMERefSys> vme;

  mafSmartPointer<mafVMEGeneric> vmepoint2;
  mafSmartPointer<mafVMERoot> root;

  vme->ReparentTo(root);
  vmepoint2->ReparentTo(root);

  vme->SetRefSysLink("Point2VME",vmepoint2);

  result = (vme->GetPoint2VME() == mafVME::SafeDownCast(vmepoint2));
  TEST_RESULT;
}