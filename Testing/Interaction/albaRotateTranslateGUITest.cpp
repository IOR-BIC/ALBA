/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaRotateTranslateGUITest
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
#include "albaRotateTranslateGUITest.h"
#include "albaRotateTranslateGUI.h"
#include "albaRotateTranslateBehavior.h"
#include "albaRefSys.h"
#include "albaVMEGroup.h"
#include "albaGUIFrame.h"
#include "albaTransformBehavior.h"
#include "wx/module.h"

//----------------------------------------------------------------------------
void albaRotateTranslateGUITest::BeforeTest()
//----------------------------------------------------------------------------
{
  m_Win = new albaGUIFrame("testGui", wxDefaultPosition, wxSize(800, 600));
  wxModule::RegisterModules();
  wxModule::InitializeModules();
}
//----------------------------------------------------------------------------
void albaRotateTranslateGUITest::AfterTest()
//----------------------------------------------------------------------------
{
  wxModule::CleanUpModules();
  delete m_Win;
}
//----------------------------------------------------------------------------
void albaRotateTranslateGUITest::TestFixture()
//----------------------------------------------------------------------------
{

}
//----------------------------------------------------------------------------
void albaRotateTranslateGUITest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEGroup> group;
  albaRotateTranslateGUI *gui = new albaRotateTranslateGUI(group,NULL,true);
  delete gui;
}
//----------------------------------------------------------------------------
void albaRotateTranslateGUITest::TestEnableWidgets()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEGroup> group;
  albaRotateTranslateGUI *gui = new albaRotateTranslateGUI(group,NULL,true);

  gui->EnableWidgets(true);

  CPPUNIT_ASSERT( group->GetBehavior() == gui->GetBehavior() );

  gui->EnableWidgets(false);

  CPPUNIT_ASSERT( group->GetBehavior() == gui->m_OldBehavior );

  delete gui;
}
//----------------------------------------------------------------------------
void albaRotateTranslateGUITest::TestAttachInteractorToVme()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEGroup> group;
  albaRotateTranslateGUI *gui = new albaRotateTranslateGUI(group,NULL,true);

  gui->AttachInteractorToVme();

  CPPUNIT_ASSERT( group->GetBehavior() == gui->GetBehavior() );

  delete gui;
}
//----------------------------------------------------------------------------
void albaRotateTranslateGUITest::TestDetachInteractorFromVme()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEGroup> group;
  albaRotateTranslateGUI *gui = new albaRotateTranslateGUI(group,NULL,true);

  gui->DetachInteractorFromVme();

  CPPUNIT_ASSERT( group->GetBehavior() == gui->m_OldBehavior && group->GetBehavior() == NULL );

  delete gui;
}
//----------------------------------------------------------------------------
void albaRotateTranslateGUITest::TestTransformBehaviors()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEGroup> group;
  albaRotateTranslateGUI *gui = new albaRotateTranslateGUI(group,NULL,true);
  albaRotateTranslateBehavior *behavior = gui->GetBehavior();

  // the behavior moves the input VME, with the default transforms in the view plane
  CPPUNIT_ASSERT(behavior->GetRotationConstraint()->GetRefSys()->GetType() == albaRefSys::VIEW);
  CPPUNIT_ASSERT(behavior->GetTranslationConstraint()->GetRefSys()->GetType() == albaRefSys::VIEW);
  CPPUNIT_ASSERT(behavior->GetPivotRefSys()->GetType() == albaRefSys::CUSTOM);

  // without a renderer the buttons are not used; the right button is never used
  double pos[2] = {10, 20};
  CPPUNIT_ASSERT(!behavior->OnLeftButtonDown(NULL, NULL, pos));
  CPPUNIT_ASSERT(!behavior->OnMiddleButtonDown(NULL, NULL, pos));
  CPPUNIT_ASSERT(!behavior->OnRightButtonDown(NULL, NULL, pos));
  CPPUNIT_ASSERT(!behavior->OnDrag(NULL, NULL, pos));
  CPPUNIT_ASSERT(!behavior->OnLeftButtonUp(NULL, NULL, pos));

  delete gui;
}
//----------------------------------------------------------------------------
void albaRotateTranslateGUITest::TestOnEvent()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEGroup> group;
  albaRotateTranslateGUI *gui = new albaRotateTranslateGUI(group,NULL,true);

  albaEvent e;
  e.SetId(albaRotateTranslateGUI::ID_ROTATION_AXES);
  gui->m_RotationConstraintId = albaRotateTranslateBehavior::X_AXIS;
  gui->OnEvent(&e);

  CPPUNIT_ASSERT( gui->GetBehavior()->GetPivotRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetRotationConstraint()->GetRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetRotationConstraint()->GetConstraintModality(albaInteractorConstraint::X) == albaInteractorConstraint::FREE );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetRotationConstraint()->GetConstraintModality(albaInteractorConstraint::Y) == albaInteractorConstraint::LOCK );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetRotationConstraint()->GetConstraintModality(albaInteractorConstraint::Z) == albaInteractorConstraint::LOCK );

  gui->m_RotationConstraintId = albaRotateTranslateBehavior::Y_AXIS;
  gui->OnEvent(&e);

  CPPUNIT_ASSERT( gui->GetBehavior()->GetPivotRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetRotationConstraint()->GetRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetRotationConstraint()->GetConstraintModality(albaInteractorConstraint::X) == albaInteractorConstraint::LOCK );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetRotationConstraint()->GetConstraintModality(albaInteractorConstraint::Y) == albaInteractorConstraint::FREE );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetRotationConstraint()->GetConstraintModality(albaInteractorConstraint::Z) == albaInteractorConstraint::LOCK );

  gui->m_RotationConstraintId = albaRotateTranslateBehavior::Z_AXIS;
  gui->OnEvent(&e);

  CPPUNIT_ASSERT( gui->GetBehavior()->GetPivotRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetRotationConstraint()->GetRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetRotationConstraint()->GetConstraintModality(albaInteractorConstraint::X) == albaInteractorConstraint::LOCK );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetRotationConstraint()->GetConstraintModality(albaInteractorConstraint::Y) == albaInteractorConstraint::LOCK );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetRotationConstraint()->GetConstraintModality(albaInteractorConstraint::Z) == albaInteractorConstraint::FREE );

  gui->m_RotationConstraintId = albaRotateTranslateBehavior::VIEW_PLANE;
  gui->OnEvent(&e);

  CPPUNIT_ASSERT( gui->GetBehavior()->GetPivotRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetRotationConstraint()->GetRefSys()->GetType() == albaRefSys::VIEW );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetRotationConstraint()->GetConstraintModality(albaInteractorConstraint::X) == albaInteractorConstraint::FREE );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetRotationConstraint()->GetConstraintModality(albaInteractorConstraint::Y) == albaInteractorConstraint::FREE );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetRotationConstraint()->GetConstraintModality(albaInteractorConstraint::Z) == albaInteractorConstraint::LOCK );

  gui->m_RotationConstraintId = albaRotateTranslateBehavior::NORMAL_VIEW_PLANE;
  gui->OnEvent(&e);

  CPPUNIT_ASSERT( gui->GetBehavior()->GetPivotRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetRotationConstraint()->GetRefSys()->GetType() == albaRefSys::VIEW );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetRotationConstraint()->GetConstraintModality(albaInteractorConstraint::X) == albaInteractorConstraint::LOCK );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetRotationConstraint()->GetConstraintModality(albaInteractorConstraint::Y) == albaInteractorConstraint::LOCK );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetRotationConstraint()->GetConstraintModality(albaInteractorConstraint::Z) == albaInteractorConstraint::FREE );
  //////////////////////////////////////////////////////////////////////////
  e.SetId(albaRotateTranslateGUI::ID_TRASLATION_AXES);
  gui->m_TranslationConstraintId = albaRotateTranslateBehavior::X_AXIS;
  gui->OnEvent(&e);
  
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslationConstraint()->GetRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::X) == albaInteractorConstraint::FREE );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::Y) == albaInteractorConstraint::LOCK );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::Z) == albaInteractorConstraint::LOCK );

  gui->m_TranslationConstraintId = albaRotateTranslateBehavior::Y_AXIS;
  gui->OnEvent(&e);

  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslationConstraint()->GetRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::X) == albaInteractorConstraint::LOCK );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::Y) == albaInteractorConstraint::FREE );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::Z) == albaInteractorConstraint::LOCK );

  gui->m_TranslationConstraintId = albaRotateTranslateBehavior::Z_AXIS;
  gui->OnEvent(&e);

  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslationConstraint()->GetRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::X) == albaInteractorConstraint::LOCK );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::Y) == albaInteractorConstraint::LOCK );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::Z) == albaInteractorConstraint::FREE );

  gui->m_TranslationConstraintId = albaRotateTranslateBehavior::VIEW_PLANE;
  gui->OnEvent(&e);

  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslationConstraint()->GetRefSys()->GetType() == albaRefSys::VIEW );

  gui->m_TranslationConstraintId = albaRotateTranslateBehavior::NORMAL_VIEW_PLANE;
  gui->OnEvent(&e);

  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslationConstraint()->GetRefSys()->GetType() == albaRefSys::VIEW );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::X) == albaInteractorConstraint::FREE );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::Y) == albaInteractorConstraint::FREE );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::Z) == albaInteractorConstraint::LOCK );

  gui->m_TranslationConstraintId = albaRotateTranslateBehavior::XY_PLANE;
  gui->OnEvent(&e);

  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslationConstraint()->GetRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::X) == albaInteractorConstraint::FREE );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::Y) == albaInteractorConstraint::FREE );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::Z) == albaInteractorConstraint::LOCK );

  gui->m_TranslationConstraintId = albaRotateTranslateBehavior::XZ_PLANE;
  gui->OnEvent(&e);

  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslationConstraint()->GetRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::X) == albaInteractorConstraint::FREE );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::Y) == albaInteractorConstraint::LOCK );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::Z) == albaInteractorConstraint::FREE );

  gui->m_TranslationConstraintId = albaRotateTranslateBehavior::YZ_PLANE;
  gui->OnEvent(&e);

  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslationConstraint()->GetRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::X) == albaInteractorConstraint::LOCK );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::Y) == albaInteractorConstraint::FREE );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::Z) == albaInteractorConstraint::FREE );

  gui->m_TranslationConstraintId = albaRotateTranslateBehavior::SURFACE_SNAP;
  gui->OnEvent(&e);

  CPPUNIT_ASSERT( gui->GetBehavior()->GetSurfaceSnap() == true );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslationConstraint()->GetRefSys()->GetType() == albaRefSys::VIEW );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::X) == albaInteractorConstraint::FREE );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::Y) == albaInteractorConstraint::FREE );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::Z) == albaInteractorConstraint::LOCK );

  gui->m_TranslationConstraintId = albaRotateTranslateBehavior::NORMAL_SURFACE;
  gui->OnEvent(&e);

  CPPUNIT_ASSERT( gui->GetBehavior()->GetSurfaceNormal() == true );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslationConstraint()->GetRefSys()->GetType() == albaRefSys::VIEW );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::X) == albaInteractorConstraint::FREE );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::Y) == albaInteractorConstraint::FREE );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::Z) == albaInteractorConstraint::LOCK );



  delete gui;
}