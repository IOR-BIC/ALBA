/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUISaveRestorePoseTest
 Authors: Stefano Perticoni
 
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

#include "albaVMESurface.h"
#include "albaDecl.h"
#include "albaGUISaveRestorePoseTest.h"
#include "albaGUISaveRestorePose.h"
#include "albaTagArray.h"

#include "vtkSphereSource.h"

#include <iostream>


//----------------------------------------------------------------------------
void albaGUISaveRestorePoseTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaGUISaveRestorePoseTest::BeforeTest()
//----------------------------------------------------------------------------
{
  m_VMESphere = albaVMESurface::New();
  m_VMESphere->SetName("m_VMESphere");
  
  m_VTKSphere = vtkSphereSource::New();
  m_VMESphere->SetData(m_VTKSphere->GetOutput(), -1); 
}
//----------------------------------------------------------------------------
void albaGUISaveRestorePoseTest::AfterTest()
//----------------------------------------------------------------------------
{
  albaDEL(m_VMESphere);
  vtkDEL(m_VTKSphere);
}
//----------------------------------------------------------------------------
void albaGUISaveRestorePoseTest::TestConstructorDestructor()
//----------------------------------------------------------------------------
{
  albaGUISaveRestorePose *guiSRPPoseGUI = new albaGUISaveRestorePose(m_VMESphere, NULL, albaGUISaveRestorePose::ID_POSE_GUI, true);
  cppDEL(guiSRPPoseGUI);

  albaGUISaveRestorePose *guiSRPScaleGUI = new albaGUISaveRestorePose(m_VMESphere, NULL, albaGUISaveRestorePose::ID_SCALE_GUI, true);
  cppDEL(guiSRPScaleGUI);
}

//----------------------------------------------------------------------------
void albaGUISaveRestorePoseTest::TestStorePose()
//----------------------------------------------------------------------------
{
  albaGUISaveRestorePose *guiSaveRestorePose = new albaGUISaveRestorePose(m_VMESphere, NULL, albaGUISaveRestorePose::ID_POSE_GUI, true);

  albaString absMatrixTagName =  "STORED_ABS_POS_dummyPoseNameUsedForTesting";

  albaMatrix absPose;
  absPose.SetElement(0,0, 9999);
  absPose.SetElement(0,1, 10);
  absPose.SetElement(0,2, 20);

  m_VMESphere->SetAbsMatrix(absPose);
  
  albaTagItem *absMatrixTag = NULL;
  absMatrixTag = m_VMESphere->GetTagArray()->GetTag(absMatrixTagName.GetCStr());
  CPPUNIT_ASSERT(absMatrixTag == NULL);

  guiSaveRestorePose->StorePose();

  absMatrixTag = m_VMESphere->GetTagArray()->GetTag(absMatrixTagName.GetCStr());
  CPPUNIT_ASSERT(absMatrixTag != NULL);
  
  double component0 = absMatrixTag->GetComponentAsDouble(0);
  CPPUNIT_ASSERT(component0 == 9999);

  double component1 = absMatrixTag->GetComponentAsDouble(1);
  CPPUNIT_ASSERT(component1 == 10);

  double component2 = absMatrixTag->GetComponentAsDouble(2);
  CPPUNIT_ASSERT(component2 == 20);

  cppDEL(guiSaveRestorePose);

}

//----------------------------------------------------------------------------
void albaGUISaveRestorePoseTest::TestRestorePose()
//----------------------------------------------------------------------------
{
  albaGUISaveRestorePose *guiSaveRestorePose = new albaGUISaveRestorePose(m_VMESphere, NULL, albaGUISaveRestorePose::ID_POSE_GUI, true);

  albaString absMatrixTagName =  "STORED_ABS_POS_dummyPoseNameUsedForTesting";

  albaTagItem *absMatrixTag = NULL;

  albaMatrix absPose;
  absPose.SetElement(0,0, 9999);
  absPose.SetElement(0,1, 10);
  absPose.SetElement(0,2, 20);

  m_VMESphere->SetAbsMatrix(absPose);

  absMatrixTag = m_VMESphere->GetTagArray()->GetTag(absMatrixTagName.GetCStr());
  CPPUNIT_ASSERT(absMatrixTag == NULL);

  guiSaveRestorePose->StorePose();

  absMatrixTag = m_VMESphere->GetTagArray()->GetTag(absMatrixTagName.GetCStr());
  CPPUNIT_ASSERT(absMatrixTag != NULL);

  double component0 = absMatrixTag->GetComponentAsDouble(0);
  CPPUNIT_ASSERT(component0 == 9999);

  double component1 = absMatrixTag->GetComponentAsDouble(1);
  CPPUNIT_ASSERT(component1 == 10);

  double component2 = absMatrixTag->GetComponentAsDouble(2);
  CPPUNIT_ASSERT(component2 == 20);

  int uselessIntJustToCallRestorePose = -10;
  guiSaveRestorePose->RestorePose(uselessIntJustToCallRestorePose);
  
  albaMatrix *vmeABSMatrixPointer = m_VMESphere->GetOutput()->GetAbsMatrix();
  CPPUNIT_ASSERT(absPose.Equals(vmeABSMatrixPointer));

  cppDEL(guiSaveRestorePose);

}

//----------------------------------------------------------------------------
void albaGUISaveRestorePoseTest::TestRemovePose()
//----------------------------------------------------------------------------
{
  albaGUISaveRestorePose *guiSaveRestorePose = new albaGUISaveRestorePose(m_VMESphere, NULL, albaGUISaveRestorePose::ID_POSE_GUI, true);

  albaString absMatrixTagName =  "STORED_ABS_POS_dummyPoseNameUsedForTesting";

  albaMatrix absPose;
  absPose.SetElement(0,0, 9999);
  absPose.SetElement(0,1, 10);
  absPose.SetElement(0,2, 20);

  m_VMESphere->SetAbsMatrix(absPose);
  
  albaTagItem *absMatrixTag = NULL;
  absMatrixTag = m_VMESphere->GetTagArray()->GetTag(absMatrixTagName.GetCStr());
  CPPUNIT_ASSERT(absMatrixTag == NULL);

  guiSaveRestorePose->StorePose();

  absMatrixTag = m_VMESphere->GetTagArray()->GetTag(absMatrixTagName.GetCStr());
  CPPUNIT_ASSERT(absMatrixTag != NULL);

  double component0 = absMatrixTag->GetComponentAsDouble(0);
  CPPUNIT_ASSERT(component0 == 9999);

  double component1 = absMatrixTag->GetComponentAsDouble(1);
  CPPUNIT_ASSERT(component1 == 10);

  double component2 = absMatrixTag->GetComponentAsDouble(2);
  CPPUNIT_ASSERT(component2 == 20);

  const albaMatrix *vmeABSMatrixPointer = m_VMESphere->GetOutput()->GetAbsMatrix();

  int uselessIntJustToCallRestorePose = -10;
  guiSaveRestorePose->RestorePose(uselessIntJustToCallRestorePose);

  vmeABSMatrixPointer = m_VMESphere->GetOutput()->GetAbsMatrix();
  CPPUNIT_ASSERT(absPose.Equals(vmeABSMatrixPointer));
  
  int uselessIntJustToCallRemovePose = -20;
  guiSaveRestorePose->RemovePose(uselessIntJustToCallRemovePose);
  
  albaTagItem *absMatrixTagAfterRemoval = m_VMESphere->GetTagArray()->GetTag(absMatrixTagName);
  CPPUNIT_ASSERT(absMatrixTagAfterRemoval == NULL);

  cppDEL(guiSaveRestorePose);

}

void albaGUISaveRestorePoseTest::TestStorePoseHelper()
{
  albaGUISaveRestorePose *guiSaveRestorePose = new albaGUISaveRestorePose(m_VMESphere, NULL, albaGUISaveRestorePose::ID_POSE_GUI, true);

  albaString absMatrixTagName =  "Stored_Pose";

  albaMatrix absPose;
  absPose.SetElement(0,0, 9999);
  absPose.SetElement(0,1, 10);
  absPose.SetElement(0,2, 20);

  m_VMESphere->SetAbsMatrix(absPose);

  albaTagItem *absMatrixTag = NULL;
  absMatrixTag = m_VMESphere->GetTagArray()->GetTag(absMatrixTagName.GetCStr());
  CPPUNIT_ASSERT(absMatrixTag == NULL);

  guiSaveRestorePose->StorePoseHelper(absMatrixTagName);

  absMatrixTag = m_VMESphere->GetTagArray()->GetTag(absMatrixTagName.GetCStr());
  CPPUNIT_ASSERT(absMatrixTag != NULL);

  double component0 = absMatrixTag->GetComponentAsDouble(0);
  CPPUNIT_ASSERT(component0 == 9999);

  double component1 = absMatrixTag->GetComponentAsDouble(1);
  CPPUNIT_ASSERT(component1 == 10);

  double component2 = absMatrixTag->GetComponentAsDouble(2);
  CPPUNIT_ASSERT(component2 == 20);

  cppDEL(guiSaveRestorePose);

}

void albaGUISaveRestorePoseTest::TestRestorePoseHelper()
{
  albaGUISaveRestorePose *guiSaveRestorePose = new albaGUISaveRestorePose(m_VMESphere, NULL, albaGUISaveRestorePose::ID_POSE_GUI, true);

  albaString absMatrixTagName =  "Stored_Pose";

  albaTagItem *absMatrixTag = NULL;

  albaMatrix absPose;
  absPose.SetElement(0,0, 9999);
  absPose.SetElement(0,1, 10);
  absPose.SetElement(0,2, 20);

  m_VMESphere->SetAbsMatrix(absPose);

  absMatrixTag = m_VMESphere->GetTagArray()->GetTag(absMatrixTagName.GetCStr());
  CPPUNIT_ASSERT(absMatrixTag == NULL);

  guiSaveRestorePose->StorePoseHelper(absMatrixTagName);

  absMatrixTag = m_VMESphere->GetTagArray()->GetTag(absMatrixTagName.GetCStr());
  CPPUNIT_ASSERT(absMatrixTag != NULL);

  double component0 = absMatrixTag->GetComponentAsDouble(0);
  CPPUNIT_ASSERT(component0 == 9999);

  double component1 = absMatrixTag->GetComponentAsDouble(1);
  CPPUNIT_ASSERT(component1 == 10);

  double component2 = absMatrixTag->GetComponentAsDouble(2);
  CPPUNIT_ASSERT(component2 == 20);

  guiSaveRestorePose->RestorePoseHelper(absMatrixTagName);

  albaMatrix *vmeABSMatrixPointer = m_VMESphere->GetOutput()->GetAbsMatrix();
  CPPUNIT_ASSERT(absPose.Equals(vmeABSMatrixPointer));

  cppDEL(guiSaveRestorePose);

}

void albaGUISaveRestorePoseTest::TestRemovePoseHelper()
{
  albaGUISaveRestorePose *guiSaveRestorePose = new albaGUISaveRestorePose(m_VMESphere, NULL, albaGUISaveRestorePose::ID_POSE_GUI, true);

  albaString absMatrixTagName =  "Stored_Pose";

  albaMatrix absPose;
  absPose.SetElement(0,0, 9999);
  absPose.SetElement(0,1, 10);
  absPose.SetElement(0,2, 20);

  m_VMESphere->SetAbsMatrix(absPose);

  albaTagItem *absMatrixTag = NULL;
  absMatrixTag = m_VMESphere->GetTagArray()->GetTag(absMatrixTagName.GetCStr());
  CPPUNIT_ASSERT(absMatrixTag == NULL);

  guiSaveRestorePose->StorePoseHelper(absMatrixTagName);

  absMatrixTag = m_VMESphere->GetTagArray()->GetTag(absMatrixTagName.GetCStr());
  CPPUNIT_ASSERT(absMatrixTag != NULL);

  double component0 = absMatrixTag->GetComponentAsDouble(0);
  CPPUNIT_ASSERT(component0 == 9999);

  double component1 = absMatrixTag->GetComponentAsDouble(1);
  CPPUNIT_ASSERT(component1 == 10);

  double component2 = absMatrixTag->GetComponentAsDouble(2);
  CPPUNIT_ASSERT(component2 == 20);

  const albaMatrix *vmeABSMatrixPointer = m_VMESphere->GetOutput()->GetAbsMatrix();

  guiSaveRestorePose->RestorePoseHelper(absMatrixTagName);

  vmeABSMatrixPointer = m_VMESphere->GetOutput()->GetAbsMatrix();
  CPPUNIT_ASSERT(absPose.Equals(vmeABSMatrixPointer));

  guiSaveRestorePose->RemovePoseHelper(absMatrixTagName);

  albaTagItem *absMatrixTagAfterRemoval = m_VMESphere->GetTagArray()->GetTag(absMatrixTagName);
  CPPUNIT_ASSERT(absMatrixTagAfterRemoval == NULL);

  cppDEL(guiSaveRestorePose);

}