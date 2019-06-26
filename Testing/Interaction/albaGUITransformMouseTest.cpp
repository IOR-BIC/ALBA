/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUITransformMouseTest
 Authors: Matteo Giacomoni
 
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
#include "albaGUITransformMouseTest.h"
#include "albaGUITransformMouse.h"
#include "albaRefSys.h"
#include "albaVMEGroup.h"
#include "albaGUIFrame.h"
#include "albaInteractorGenericMouse.h"
#include "albaInteractorCompositorMouse.h"
#include "wx/module.h"

//----------------------------------------------------------------------------
void albaGUITransformMouseTest::BeforeTest()
//----------------------------------------------------------------------------
{
  m_Win = new albaGUIFrame("testGui", wxDefaultPosition, wxSize(800, 600));
  wxModule::RegisterModules();
  wxModule::InitializeModules();
}
//----------------------------------------------------------------------------
void albaGUITransformMouseTest::AfterTest()
//----------------------------------------------------------------------------
{
  wxModule::CleanUpModules();
  delete m_Win;
}
//----------------------------------------------------------------------------
void albaGUITransformMouseTest::TestFixture()
//----------------------------------------------------------------------------
{

}
//----------------------------------------------------------------------------
void albaGUITransformMouseTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEGroup> group;
  albaGUITransformMouse *gui = new albaGUITransformMouse(group,NULL,true);
  delete gui;
}
//----------------------------------------------------------------------------
void albaGUITransformMouseTest::TestEnableWidgets()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEGroup> group;
  albaGUITransformMouse *gui = new albaGUITransformMouse(group,NULL,true);

  gui->EnableWidgets(true);

  CPPUNIT_ASSERT( group->GetBehavior() == gui->m_IsaCompositor );

  gui->EnableWidgets(false);

  CPPUNIT_ASSERT( group->GetBehavior() == gui->m_OldInteractor );

  delete gui;
}
//----------------------------------------------------------------------------
void albaGUITransformMouseTest::TestAttachInteractorToVme()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEGroup> group;
  albaGUITransformMouse *gui = new albaGUITransformMouse(group,NULL,true);

  gui->AttachInteractorToVme();

  CPPUNIT_ASSERT( group->GetBehavior() == gui->m_IsaCompositor );

  delete gui;
}
//----------------------------------------------------------------------------
void albaGUITransformMouseTest::TestDetachInteractorFromVme()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEGroup> group;
  albaGUITransformMouse *gui = new albaGUITransformMouse(group,NULL,true);

  gui->DetachInteractorFromVme();

  CPPUNIT_ASSERT( group->GetBehavior() == gui->m_OldInteractor && group->GetBehavior() == NULL );

  delete gui;
}
//----------------------------------------------------------------------------
void albaGUITransformMouseTest::TestCreateBehavior()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEGroup> group;
  albaGUITransformMouse *gui = new albaGUITransformMouse(group,NULL,true);

  albaInteractorGenericMouse *mouse = NULL;
  mouse = gui->CreateBehavior(MOUSE_LEFT);
  CPPUNIT_ASSERT( mouse->GetStartButton() == ALBA_LEFT_BUTTON );

  mouse = gui->CreateBehavior(MOUSE_LEFT_CONTROL);
  CPPUNIT_ASSERT( mouse->GetStartButton() == ALBA_LEFT_BUTTON && mouse->GetModifiers() == ALBA_CTRL_KEY);

  mouse = gui->CreateBehavior(MOUSE_LEFT_SHIFT);
  CPPUNIT_ASSERT( mouse->GetStartButton() == ALBA_LEFT_BUTTON && mouse->GetModifiers() == ALBA_SHIFT_KEY);

  mouse = gui->CreateBehavior(MOUSE_RIGHT);
  CPPUNIT_ASSERT( mouse->GetStartButton() == ALBA_RIGHT_BUTTON );

  mouse = gui->CreateBehavior(MOUSE_RIGHT_CONTROL);
  CPPUNIT_ASSERT( mouse->GetStartButton() == ALBA_RIGHT_BUTTON && mouse->GetModifiers() == ALBA_CTRL_KEY);

  mouse = gui->CreateBehavior(MOUSE_RIGHT_SHIFT);
  CPPUNIT_ASSERT( mouse->GetStartButton() == ALBA_RIGHT_BUTTON && mouse->GetModifiers() == ALBA_SHIFT_KEY);

  mouse = gui->CreateBehavior(MOUSE_MIDDLE);
  CPPUNIT_ASSERT( mouse->GetStartButton() == ALBA_MIDDLE_BUTTON );

  mouse = gui->CreateBehavior(MOUSE_MIDDLE_CONTROL);
  CPPUNIT_ASSERT( mouse->GetStartButton() == ALBA_MIDDLE_BUTTON && mouse->GetModifiers() == ALBA_CTRL_KEY);

  mouse = gui->CreateBehavior(MOUSE_MIDDLE_SHIFT);
  CPPUNIT_ASSERT( mouse->GetStartButton() == ALBA_MIDDLE_BUTTON && mouse->GetModifiers() == ALBA_SHIFT_KEY);

  delete gui;
}
//----------------------------------------------------------------------------
void albaGUITransformMouseTest::TestOnEvent()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEGroup> group;
  albaGUITransformMouse *gui = new albaGUITransformMouse(group,NULL,true);

  albaEvent e;
  e.SetId(albaGUITransformMouse::ID_ROTATION_AXES);
  gui->m_RotationConstraintId = albaGUITransformMouse::X_AXIS;
  gui->OnEvent(&e);

  CPPUNIT_ASSERT( gui->m_IsaRotate->GetPivotRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->m_IsaRotate->GetRotationConstraint()->GetRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->m_IsaRotate->GetRotationConstraint()->GetConstraintModality(albaInteractorConstraint::X) == albaInteractorConstraint::FREE );
  CPPUNIT_ASSERT( gui->m_IsaRotate->GetRotationConstraint()->GetConstraintModality(albaInteractorConstraint::Y) == albaInteractorConstraint::LOCK );
  CPPUNIT_ASSERT( gui->m_IsaRotate->GetRotationConstraint()->GetConstraintModality(albaInteractorConstraint::Z) == albaInteractorConstraint::LOCK );

  gui->m_RotationConstraintId = albaGUITransformMouse::Y_AXIS;
  gui->OnEvent(&e);

  CPPUNIT_ASSERT( gui->m_IsaRotate->GetPivotRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->m_IsaRotate->GetRotationConstraint()->GetRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->m_IsaRotate->GetRotationConstraint()->GetConstraintModality(albaInteractorConstraint::X) == albaInteractorConstraint::LOCK );
  CPPUNIT_ASSERT( gui->m_IsaRotate->GetRotationConstraint()->GetConstraintModality(albaInteractorConstraint::Y) == albaInteractorConstraint::FREE );
  CPPUNIT_ASSERT( gui->m_IsaRotate->GetRotationConstraint()->GetConstraintModality(albaInteractorConstraint::Z) == albaInteractorConstraint::LOCK );

  gui->m_RotationConstraintId = albaGUITransformMouse::Z_AXIS;
  gui->OnEvent(&e);

  CPPUNIT_ASSERT( gui->m_IsaRotate->GetPivotRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->m_IsaRotate->GetRotationConstraint()->GetRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->m_IsaRotate->GetRotationConstraint()->GetConstraintModality(albaInteractorConstraint::X) == albaInteractorConstraint::LOCK );
  CPPUNIT_ASSERT( gui->m_IsaRotate->GetRotationConstraint()->GetConstraintModality(albaInteractorConstraint::Y) == albaInteractorConstraint::LOCK );
  CPPUNIT_ASSERT( gui->m_IsaRotate->GetRotationConstraint()->GetConstraintModality(albaInteractorConstraint::Z) == albaInteractorConstraint::FREE );

  gui->m_RotationConstraintId = albaGUITransformMouse::VIEW_PLANE;
  gui->OnEvent(&e);

  CPPUNIT_ASSERT( gui->m_IsaRotate->GetPivotRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->m_IsaRotate->GetRotationConstraint()->GetRefSys()->GetType() == albaRefSys::VIEW );
  CPPUNIT_ASSERT( gui->m_IsaRotate->GetRotationConstraint()->GetConstraintModality(albaInteractorConstraint::X) == albaInteractorConstraint::FREE );
  CPPUNIT_ASSERT( gui->m_IsaRotate->GetRotationConstraint()->GetConstraintModality(albaInteractorConstraint::Y) == albaInteractorConstraint::FREE );
  CPPUNIT_ASSERT( gui->m_IsaRotate->GetRotationConstraint()->GetConstraintModality(albaInteractorConstraint::Z) == albaInteractorConstraint::LOCK );

  gui->m_RotationConstraintId = albaGUITransformMouse::NORMAL_VIEW_PLANE;
  gui->OnEvent(&e);

  CPPUNIT_ASSERT( gui->m_IsaRotate->GetPivotRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->m_IsaRotate->GetRotationConstraint()->GetRefSys()->GetType() == albaRefSys::VIEW );
  CPPUNIT_ASSERT( gui->m_IsaRotate->GetRotationConstraint()->GetConstraintModality(albaInteractorConstraint::X) == albaInteractorConstraint::LOCK );
  CPPUNIT_ASSERT( gui->m_IsaRotate->GetRotationConstraint()->GetConstraintModality(albaInteractorConstraint::Y) == albaInteractorConstraint::LOCK );
  CPPUNIT_ASSERT( gui->m_IsaRotate->GetRotationConstraint()->GetConstraintModality(albaInteractorConstraint::Z) == albaInteractorConstraint::FREE );
  //////////////////////////////////////////////////////////////////////////
  e.SetId(albaGUITransformMouse::ID_TRASLATION_AXES);
  gui->m_TranslationConstraintId = albaGUITransformMouse::X_AXIS;
  gui->OnEvent(&e);
  
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetPivotRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetTranslationConstraint()->GetRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::X) == albaInteractorConstraint::FREE );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::Y) == albaInteractorConstraint::LOCK );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::Z) == albaInteractorConstraint::LOCK );

  gui->m_TranslationConstraintId = albaGUITransformMouse::Y_AXIS;
  gui->OnEvent(&e);

  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetPivotRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetTranslationConstraint()->GetRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::X) == albaInteractorConstraint::LOCK );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::Y) == albaInteractorConstraint::FREE );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::Z) == albaInteractorConstraint::LOCK );

  gui->m_TranslationConstraintId = albaGUITransformMouse::Z_AXIS;
  gui->OnEvent(&e);

  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetPivotRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetTranslationConstraint()->GetRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::X) == albaInteractorConstraint::LOCK );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::Y) == albaInteractorConstraint::LOCK );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::Z) == albaInteractorConstraint::FREE );

  gui->m_TranslationConstraintId = albaGUITransformMouse::VIEW_PLANE;
  gui->OnEvent(&e);

  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetPivotRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetTranslationConstraint()->GetRefSys()->GetType() == albaRefSys::VIEW );

  gui->m_TranslationConstraintId = albaGUITransformMouse::NORMAL_VIEW_PLANE;
  gui->OnEvent(&e);

  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetPivotRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetTranslationConstraint()->GetRefSys()->GetType() == albaRefSys::VIEW );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::X) == albaInteractorConstraint::FREE );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::Y) == albaInteractorConstraint::FREE );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::Z) == albaInteractorConstraint::LOCK );

  gui->m_TranslationConstraintId = albaGUITransformMouse::XY_PLANE;
  gui->OnEvent(&e);

  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetPivotRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetTranslationConstraint()->GetRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::X) == albaInteractorConstraint::FREE );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::Y) == albaInteractorConstraint::FREE );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::Z) == albaInteractorConstraint::LOCK );

  gui->m_TranslationConstraintId = albaGUITransformMouse::XZ_PLANE;
  gui->OnEvent(&e);

  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetPivotRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetTranslationConstraint()->GetRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::X) == albaInteractorConstraint::FREE );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::Y) == albaInteractorConstraint::LOCK );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::Z) == albaInteractorConstraint::FREE );

  gui->m_TranslationConstraintId = albaGUITransformMouse::YZ_PLANE;
  gui->OnEvent(&e);

  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetPivotRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetTranslationConstraint()->GetRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::X) == albaInteractorConstraint::LOCK );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::Y) == albaInteractorConstraint::FREE );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::Z) == albaInteractorConstraint::FREE );

  gui->m_TranslationConstraintId = albaGUITransformMouse::SURFACE_SNAP;
  gui->OnEvent(&e);

  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetSurfaceSnap() == true );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetPivotRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetTranslationConstraint()->GetRefSys()->GetType() == albaRefSys::VIEW );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::X) == albaInteractorConstraint::FREE );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::Y) == albaInteractorConstraint::FREE );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::Z) == albaInteractorConstraint::LOCK );

  gui->m_TranslationConstraintId = albaGUITransformMouse::NORMAL_SURFACE;
  gui->OnEvent(&e);

  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetSurfaceNormal() == true );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetPivotRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetTranslationConstraint()->GetRefSys()->GetType() == albaRefSys::VIEW );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::X) == albaInteractorConstraint::FREE );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::Y) == albaInteractorConstraint::FREE );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::Z) == albaInteractorConstraint::LOCK );



  delete gui;
}