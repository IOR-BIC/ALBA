/*=========================================================================

 Program: MAF2
 Module: mafGUISaveRestorePoseTest
 Authors: Stefano Perticoni
 
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

#include "mafVMESurface.h"
#include "mafDecl.h"
#include "mafGUISaveRestorePoseTest.h"
#include "mafGUISaveRestorePose.h"
#include "mafTagArray.h"

#include "vtkSphereSource.h"

#include <iostream>


//----------------------------------------------------------------------------
void mafGUISaveRestorePoseTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafGUISaveRestorePoseTest::BeforeTest()
//----------------------------------------------------------------------------
{
  m_VMESphere = mafVMESurface::New();
  m_VMESphere->SetName("m_VMESphere");
  
  m_VTKSphere = vtkSphereSource::New();
  m_VMESphere->SetData(m_VTKSphere->GetOutput(), -1); 
}
//----------------------------------------------------------------------------
void mafGUISaveRestorePoseTest::AfterTest()
//----------------------------------------------------------------------------
{
  mafDEL(m_VMESphere);
  vtkDEL(m_VTKSphere);
}
//----------------------------------------------------------------------------
void mafGUISaveRestorePoseTest::TestConstructorDestructor()
//----------------------------------------------------------------------------
{
  mafGUISaveRestorePose *guiSRPPoseGUI = new mafGUISaveRestorePose(m_VMESphere, NULL, mafGUISaveRestorePose::ID_POSE_GUI, true);
  cppDEL(guiSRPPoseGUI);

  mafGUISaveRestorePose *guiSRPScaleGUI = new mafGUISaveRestorePose(m_VMESphere, NULL, mafGUISaveRestorePose::ID_SCALE_GUI, true);
  cppDEL(guiSRPScaleGUI);
}

//----------------------------------------------------------------------------
void mafGUISaveRestorePoseTest::TestStorePose()
//----------------------------------------------------------------------------
{
  mafGUISaveRestorePose *guiSaveRestorePose = new mafGUISaveRestorePose(m_VMESphere, NULL, mafGUISaveRestorePose::ID_POSE_GUI, true);

  mafString absMatrixTagName =  "STORED_ABS_POS_dummyPoseNameUsedForTesting";

  mafMatrix absPose;
  absPose.SetElement(0,0, 9999);
  absPose.SetElement(0,1, 10);
  absPose.SetElement(0,2, 20);

  m_VMESphere->SetAbsMatrix(absPose);
  
  mafTagItem *absMatrixTag = NULL;
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
void mafGUISaveRestorePoseTest::TestRestorePose()
//----------------------------------------------------------------------------
{
  mafGUISaveRestorePose *guiSaveRestorePose = new mafGUISaveRestorePose(m_VMESphere, NULL, mafGUISaveRestorePose::ID_POSE_GUI, true);

  mafString absMatrixTagName =  "STORED_ABS_POS_dummyPoseNameUsedForTesting";

  mafTagItem *absMatrixTag = NULL;

  mafMatrix absPose;
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
  
  mafMatrix *vmeABSMatrixPointer = m_VMESphere->GetOutput()->GetAbsMatrix();
  CPPUNIT_ASSERT(absPose.Equals(vmeABSMatrixPointer));

  cppDEL(guiSaveRestorePose);

}

//----------------------------------------------------------------------------
void mafGUISaveRestorePoseTest::TestRemovePose()
//----------------------------------------------------------------------------
{
  mafGUISaveRestorePose *guiSaveRestorePose = new mafGUISaveRestorePose(m_VMESphere, NULL, mafGUISaveRestorePose::ID_POSE_GUI, true);

  mafString absMatrixTagName =  "STORED_ABS_POS_dummyPoseNameUsedForTesting";

  mafMatrix absPose;
  absPose.SetElement(0,0, 9999);
  absPose.SetElement(0,1, 10);
  absPose.SetElement(0,2, 20);

  m_VMESphere->SetAbsMatrix(absPose);
  
  mafTagItem *absMatrixTag = NULL;
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

  const mafMatrix *vmeABSMatrixPointer = m_VMESphere->GetOutput()->GetAbsMatrix();

  int uselessIntJustToCallRestorePose = -10;
  guiSaveRestorePose->RestorePose(uselessIntJustToCallRestorePose);

  vmeABSMatrixPointer = m_VMESphere->GetOutput()->GetAbsMatrix();
  CPPUNIT_ASSERT(absPose.Equals(vmeABSMatrixPointer));
  
  int uselessIntJustToCallRemovePose = -20;
  guiSaveRestorePose->RemovePose(uselessIntJustToCallRemovePose);
  
  mafTagItem *absMatrixTagAfterRemoval = m_VMESphere->GetTagArray()->GetTag(absMatrixTagName);
  CPPUNIT_ASSERT(absMatrixTagAfterRemoval == NULL);

  cppDEL(guiSaveRestorePose);

}

void mafGUISaveRestorePoseTest::TestStorePoseHelper()
{
  mafGUISaveRestorePose *guiSaveRestorePose = new mafGUISaveRestorePose(m_VMESphere, NULL, mafGUISaveRestorePose::ID_POSE_GUI, true);

  mafString absMatrixTagName =  "Stored_Pose";

  mafMatrix absPose;
  absPose.SetElement(0,0, 9999);
  absPose.SetElement(0,1, 10);
  absPose.SetElement(0,2, 20);

  m_VMESphere->SetAbsMatrix(absPose);

  mafTagItem *absMatrixTag = NULL;
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

void mafGUISaveRestorePoseTest::TestRestorePoseHelper()
{
  mafGUISaveRestorePose *guiSaveRestorePose = new mafGUISaveRestorePose(m_VMESphere, NULL, mafGUISaveRestorePose::ID_POSE_GUI, true);

  mafString absMatrixTagName =  "Stored_Pose";

  mafTagItem *absMatrixTag = NULL;

  mafMatrix absPose;
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

  mafMatrix *vmeABSMatrixPointer = m_VMESphere->GetOutput()->GetAbsMatrix();
  CPPUNIT_ASSERT(absPose.Equals(vmeABSMatrixPointer));

  cppDEL(guiSaveRestorePose);

}

void mafGUISaveRestorePoseTest::TestRemovePoseHelper()
{
  mafGUISaveRestorePose *guiSaveRestorePose = new mafGUISaveRestorePose(m_VMESphere, NULL, mafGUISaveRestorePose::ID_POSE_GUI, true);

  mafString absMatrixTagName =  "Stored_Pose";

  mafMatrix absPose;
  absPose.SetElement(0,0, 9999);
  absPose.SetElement(0,1, 10);
  absPose.SetElement(0,2, 20);

  m_VMESphere->SetAbsMatrix(absPose);

  mafTagItem *absMatrixTag = NULL;
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

  const mafMatrix *vmeABSMatrixPointer = m_VMESphere->GetOutput()->GetAbsMatrix();

  guiSaveRestorePose->RestorePoseHelper(absMatrixTagName);

  vmeABSMatrixPointer = m_VMESphere->GetOutput()->GetAbsMatrix();
  CPPUNIT_ASSERT(absPose.Equals(vmeABSMatrixPointer));

  guiSaveRestorePose->RemovePoseHelper(absMatrixTagName);

  mafTagItem *absMatrixTagAfterRemoval = m_VMESphere->GetTagArray()->GetTag(absMatrixTagName);
  CPPUNIT_ASSERT(absMatrixTagAfterRemoval == NULL);

  cppDEL(guiSaveRestorePose);

}