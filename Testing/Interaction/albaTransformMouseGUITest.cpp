/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaTransformMouseGUITest
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
#include "albaTransformMouseGUITest.h"
#include "albaTransformMouseGUI.h"
#include "albaTransformMouseBehavior.h"
#include "albaRefSys.h"
#include "albaVMEGroup.h"
#include "albaGUIFrame.h"
#include "albaGenericMouseBehavior.h"
#include "albaCompositorMouseBehavior.h"
#include "wx/module.h"

//----------------------------------------------------------------------------
void albaTransformMouseGUITest::BeforeTest()
//----------------------------------------------------------------------------
{
  m_Win = new albaGUIFrame("testGui", wxDefaultPosition, wxSize(800, 600));
  wxModule::RegisterModules();
  wxModule::InitializeModules();
}
//----------------------------------------------------------------------------
void albaTransformMouseGUITest::AfterTest()
//----------------------------------------------------------------------------
{
  wxModule::CleanUpModules();
  delete m_Win;
}
//----------------------------------------------------------------------------
void albaTransformMouseGUITest::TestFixture()
//----------------------------------------------------------------------------
{

}
//----------------------------------------------------------------------------
void albaTransformMouseGUITest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEGroup> group;
  albaTransformMouseGUI *gui = new albaTransformMouseGUI(group,NULL,true);
  delete gui;
}
//----------------------------------------------------------------------------
void albaTransformMouseGUITest::TestEnableWidgets()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEGroup> group;
  albaTransformMouseGUI *gui = new albaTransformMouseGUI(group,NULL,true);

  gui->EnableWidgets(true);

  CPPUNIT_ASSERT( group->GetBehavior() == gui->GetBehavior() );

  gui->EnableWidgets(false);

  CPPUNIT_ASSERT( group->GetBehavior() == gui->m_OldBehavior );

  delete gui;
}
//----------------------------------------------------------------------------
void albaTransformMouseGUITest::TestAttachInteractorToVme()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEGroup> group;
  albaTransformMouseGUI *gui = new albaTransformMouseGUI(group,NULL,true);

  gui->AttachInteractorToVme();

  CPPUNIT_ASSERT( group->GetBehavior() == gui->GetBehavior() );

  delete gui;
}
//----------------------------------------------------------------------------
void albaTransformMouseGUITest::TestDetachInteractorFromVme()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEGroup> group;
  albaTransformMouseGUI *gui = new albaTransformMouseGUI(group,NULL,true);

  gui->DetachInteractorFromVme();

  CPPUNIT_ASSERT( group->GetBehavior() == gui->m_OldBehavior && group->GetBehavior() == NULL );

  delete gui;
}
//----------------------------------------------------------------------------
void albaTransformMouseGUITest::TestCreateBehavior()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEGroup> group;
  albaTransformMouseGUI *gui = new albaTransformMouseGUI(group,NULL,true);

  albaGenericMouseBehavior *mouse = NULL;
  mouse = gui->GetBehavior()->CreateBehavior(MOUSE_LEFT);
  CPPUNIT_ASSERT( mouse->GetStartButton() == ALBA_LEFT_BUTTON );

  mouse = gui->GetBehavior()->CreateBehavior(MOUSE_LEFT_CONTROL);
  CPPUNIT_ASSERT( mouse->GetStartButton() == ALBA_LEFT_BUTTON && mouse->GetModifiers() == ALBA_CTRL_KEY);

  mouse = gui->GetBehavior()->CreateBehavior(MOUSE_LEFT_SHIFT);
  CPPUNIT_ASSERT( mouse->GetStartButton() == ALBA_LEFT_BUTTON && mouse->GetModifiers() == ALBA_SHIFT_KEY);

  mouse = gui->GetBehavior()->CreateBehavior(MOUSE_RIGHT);
  CPPUNIT_ASSERT( mouse->GetStartButton() == ALBA_RIGHT_BUTTON );

  mouse = gui->GetBehavior()->CreateBehavior(MOUSE_RIGHT_CONTROL);
  CPPUNIT_ASSERT( mouse->GetStartButton() == ALBA_RIGHT_BUTTON && mouse->GetModifiers() == ALBA_CTRL_KEY);

  mouse = gui->GetBehavior()->CreateBehavior(MOUSE_RIGHT_SHIFT);
  CPPUNIT_ASSERT( mouse->GetStartButton() == ALBA_RIGHT_BUTTON && mouse->GetModifiers() == ALBA_SHIFT_KEY);

  mouse = gui->GetBehavior()->CreateBehavior(MOUSE_MIDDLE);
  CPPUNIT_ASSERT( mouse->GetStartButton() == ALBA_MIDDLE_BUTTON );

  mouse = gui->GetBehavior()->CreateBehavior(MOUSE_MIDDLE_CONTROL);
  CPPUNIT_ASSERT( mouse->GetStartButton() == ALBA_MIDDLE_BUTTON && mouse->GetModifiers() == ALBA_CTRL_KEY);

  mouse = gui->GetBehavior()->CreateBehavior(MOUSE_MIDDLE_SHIFT);
  CPPUNIT_ASSERT( mouse->GetStartButton() == ALBA_MIDDLE_BUTTON && mouse->GetModifiers() == ALBA_SHIFT_KEY);

  delete gui;
}
//----------------------------------------------------------------------------
void albaTransformMouseGUITest::TestOnEvent()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEGroup> group;
  albaTransformMouseGUI *gui = new albaTransformMouseGUI(group,NULL,true);

  albaEvent e;
  e.SetId(albaTransformMouseGUI::ID_ROTATION_AXES);
  gui->m_RotationConstraintId = albaTransformMouseBehavior::X_AXIS;
  gui->OnEvent(&e);

  CPPUNIT_ASSERT( gui->GetBehavior()->GetRotateBehavior()->GetPivotRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetRotateBehavior()->GetRotationConstraint()->GetRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetRotateBehavior()->GetRotationConstraint()->GetConstraintModality(albaInteractorConstraint::X) == albaInteractorConstraint::FREE );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetRotateBehavior()->GetRotationConstraint()->GetConstraintModality(albaInteractorConstraint::Y) == albaInteractorConstraint::LOCK );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetRotateBehavior()->GetRotationConstraint()->GetConstraintModality(albaInteractorConstraint::Z) == albaInteractorConstraint::LOCK );

  gui->m_RotationConstraintId = albaTransformMouseBehavior::Y_AXIS;
  gui->OnEvent(&e);

  CPPUNIT_ASSERT( gui->GetBehavior()->GetRotateBehavior()->GetPivotRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetRotateBehavior()->GetRotationConstraint()->GetRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetRotateBehavior()->GetRotationConstraint()->GetConstraintModality(albaInteractorConstraint::X) == albaInteractorConstraint::LOCK );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetRotateBehavior()->GetRotationConstraint()->GetConstraintModality(albaInteractorConstraint::Y) == albaInteractorConstraint::FREE );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetRotateBehavior()->GetRotationConstraint()->GetConstraintModality(albaInteractorConstraint::Z) == albaInteractorConstraint::LOCK );

  gui->m_RotationConstraintId = albaTransformMouseBehavior::Z_AXIS;
  gui->OnEvent(&e);

  CPPUNIT_ASSERT( gui->GetBehavior()->GetRotateBehavior()->GetPivotRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetRotateBehavior()->GetRotationConstraint()->GetRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetRotateBehavior()->GetRotationConstraint()->GetConstraintModality(albaInteractorConstraint::X) == albaInteractorConstraint::LOCK );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetRotateBehavior()->GetRotationConstraint()->GetConstraintModality(albaInteractorConstraint::Y) == albaInteractorConstraint::LOCK );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetRotateBehavior()->GetRotationConstraint()->GetConstraintModality(albaInteractorConstraint::Z) == albaInteractorConstraint::FREE );

  gui->m_RotationConstraintId = albaTransformMouseBehavior::VIEW_PLANE;
  gui->OnEvent(&e);

  CPPUNIT_ASSERT( gui->GetBehavior()->GetRotateBehavior()->GetPivotRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetRotateBehavior()->GetRotationConstraint()->GetRefSys()->GetType() == albaRefSys::VIEW );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetRotateBehavior()->GetRotationConstraint()->GetConstraintModality(albaInteractorConstraint::X) == albaInteractorConstraint::FREE );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetRotateBehavior()->GetRotationConstraint()->GetConstraintModality(albaInteractorConstraint::Y) == albaInteractorConstraint::FREE );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetRotateBehavior()->GetRotationConstraint()->GetConstraintModality(albaInteractorConstraint::Z) == albaInteractorConstraint::LOCK );

  gui->m_RotationConstraintId = albaTransformMouseBehavior::NORMAL_VIEW_PLANE;
  gui->OnEvent(&e);

  CPPUNIT_ASSERT( gui->GetBehavior()->GetRotateBehavior()->GetPivotRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetRotateBehavior()->GetRotationConstraint()->GetRefSys()->GetType() == albaRefSys::VIEW );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetRotateBehavior()->GetRotationConstraint()->GetConstraintModality(albaInteractorConstraint::X) == albaInteractorConstraint::LOCK );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetRotateBehavior()->GetRotationConstraint()->GetConstraintModality(albaInteractorConstraint::Y) == albaInteractorConstraint::LOCK );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetRotateBehavior()->GetRotationConstraint()->GetConstraintModality(albaInteractorConstraint::Z) == albaInteractorConstraint::FREE );
  //////////////////////////////////////////////////////////////////////////
  e.SetId(albaTransformMouseGUI::ID_TRASLATION_AXES);
  gui->m_TranslationConstraintId = albaTransformMouseBehavior::X_AXIS;
  gui->OnEvent(&e);
  
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslateBehavior()->GetPivotRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslateBehavior()->GetTranslationConstraint()->GetRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslateBehavior()->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::X) == albaInteractorConstraint::FREE );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslateBehavior()->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::Y) == albaInteractorConstraint::LOCK );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslateBehavior()->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::Z) == albaInteractorConstraint::LOCK );

  gui->m_TranslationConstraintId = albaTransformMouseBehavior::Y_AXIS;
  gui->OnEvent(&e);

  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslateBehavior()->GetPivotRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslateBehavior()->GetTranslationConstraint()->GetRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslateBehavior()->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::X) == albaInteractorConstraint::LOCK );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslateBehavior()->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::Y) == albaInteractorConstraint::FREE );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslateBehavior()->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::Z) == albaInteractorConstraint::LOCK );

  gui->m_TranslationConstraintId = albaTransformMouseBehavior::Z_AXIS;
  gui->OnEvent(&e);

  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslateBehavior()->GetPivotRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslateBehavior()->GetTranslationConstraint()->GetRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslateBehavior()->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::X) == albaInteractorConstraint::LOCK );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslateBehavior()->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::Y) == albaInteractorConstraint::LOCK );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslateBehavior()->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::Z) == albaInteractorConstraint::FREE );

  gui->m_TranslationConstraintId = albaTransformMouseBehavior::VIEW_PLANE;
  gui->OnEvent(&e);

  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslateBehavior()->GetPivotRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslateBehavior()->GetTranslationConstraint()->GetRefSys()->GetType() == albaRefSys::VIEW );

  gui->m_TranslationConstraintId = albaTransformMouseBehavior::NORMAL_VIEW_PLANE;
  gui->OnEvent(&e);

  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslateBehavior()->GetPivotRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslateBehavior()->GetTranslationConstraint()->GetRefSys()->GetType() == albaRefSys::VIEW );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslateBehavior()->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::X) == albaInteractorConstraint::FREE );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslateBehavior()->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::Y) == albaInteractorConstraint::FREE );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslateBehavior()->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::Z) == albaInteractorConstraint::LOCK );

  gui->m_TranslationConstraintId = albaTransformMouseBehavior::XY_PLANE;
  gui->OnEvent(&e);

  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslateBehavior()->GetPivotRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslateBehavior()->GetTranslationConstraint()->GetRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslateBehavior()->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::X) == albaInteractorConstraint::FREE );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslateBehavior()->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::Y) == albaInteractorConstraint::FREE );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslateBehavior()->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::Z) == albaInteractorConstraint::LOCK );

  gui->m_TranslationConstraintId = albaTransformMouseBehavior::XZ_PLANE;
  gui->OnEvent(&e);

  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslateBehavior()->GetPivotRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslateBehavior()->GetTranslationConstraint()->GetRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslateBehavior()->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::X) == albaInteractorConstraint::FREE );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslateBehavior()->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::Y) == albaInteractorConstraint::LOCK );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslateBehavior()->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::Z) == albaInteractorConstraint::FREE );

  gui->m_TranslationConstraintId = albaTransformMouseBehavior::YZ_PLANE;
  gui->OnEvent(&e);

  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslateBehavior()->GetPivotRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslateBehavior()->GetTranslationConstraint()->GetRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslateBehavior()->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::X) == albaInteractorConstraint::LOCK );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslateBehavior()->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::Y) == albaInteractorConstraint::FREE );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslateBehavior()->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::Z) == albaInteractorConstraint::FREE );

  gui->m_TranslationConstraintId = albaTransformMouseBehavior::SURFACE_SNAP;
  gui->OnEvent(&e);

  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslateBehavior()->GetSurfaceSnap() == true );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslateBehavior()->GetPivotRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslateBehavior()->GetTranslationConstraint()->GetRefSys()->GetType() == albaRefSys::VIEW );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslateBehavior()->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::X) == albaInteractorConstraint::FREE );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslateBehavior()->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::Y) == albaInteractorConstraint::FREE );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslateBehavior()->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::Z) == albaInteractorConstraint::LOCK );

  gui->m_TranslationConstraintId = albaTransformMouseBehavior::NORMAL_SURFACE;
  gui->OnEvent(&e);

  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslateBehavior()->GetSurfaceNormal() == true );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslateBehavior()->GetPivotRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslateBehavior()->GetTranslationConstraint()->GetRefSys()->GetType() == albaRefSys::VIEW );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslateBehavior()->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::X) == albaInteractorConstraint::FREE );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslateBehavior()->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::Y) == albaInteractorConstraint::FREE );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslateBehavior()->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::Z) == albaInteractorConstraint::LOCK );



  delete gui;
}