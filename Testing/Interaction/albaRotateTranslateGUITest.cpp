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
#include "albaBehaviorRotateTranslate.h"
#include "albaRefSys.h"
#include "albaVMEGroup.h"
#include "albaGUIFrame.h"
#include "albaBehaviorTransform.h"
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
albaBehaviorRotateTranslate *albaRotateTranslateGUITest::CreateBehavior(albaVME *vme)
//----------------------------------------------------------------------------
{
  albaBehaviorRotateTranslate *behavior;
  albaNEW(behavior);
  behavior->SetVME(vme);
  return behavior;
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
  albaBehaviorRotateTranslate *behavior = CreateBehavior(group);
  albaRotateTranslateGUI *gui = new albaRotateTranslateGUI(behavior,NULL,true);
  delete gui;
  albaDEL(behavior);
}
//----------------------------------------------------------------------------
void albaRotateTranslateGUITest::TestEnableWidgets()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEGroup> group;
  albaBehaviorRotateTranslate *behavior = CreateBehavior(group);
  albaRotateTranslateGUI *gui = new albaRotateTranslateGUI(behavior,NULL,true);

  // the GUI never assigns the behavior to the VME: the operation does it
  gui->EnableWidgets(true);
  CPPUNIT_ASSERT( group->GetBehavior() == NULL );

  gui->EnableWidgets(false);
  CPPUNIT_ASSERT( group->GetBehavior() == NULL );

  delete gui;
  albaDEL(behavior);
}
//----------------------------------------------------------------------------
void albaRotateTranslateGUITest::TestTransformBehaviors()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEGroup> group;
  albaBehaviorRotateTranslate *behavior = CreateBehavior(group);
  albaRotateTranslateGUI *gui = new albaRotateTranslateGUI(behavior,NULL,true);
  CPPUNIT_ASSERT(gui->GetBehavior() == behavior);

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
  albaDEL(behavior);
}
//----------------------------------------------------------------------------
void albaRotateTranslateGUITest::TestOnEvent()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEGroup> group;
  albaBehaviorRotateTranslate *behavior = CreateBehavior(group);
  albaRotateTranslateGUI *gui = new albaRotateTranslateGUI(behavior,NULL,true);

  albaEvent e;
  e.SetId(albaRotateTranslateGUI::ID_ROTATION_AXES);
  gui->m_RotationConstraintId = albaBehaviorRotateTranslate::X_AXIS;
  gui->OnEvent(&e);

  CPPUNIT_ASSERT( gui->GetBehavior()->GetPivotRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetRotationConstraint()->GetRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetRotationConstraint()->GetConstraintModality(albaTransformConstraint::X) == albaTransformConstraint::FREE );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetRotationConstraint()->GetConstraintModality(albaTransformConstraint::Y) == albaTransformConstraint::LOCK );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetRotationConstraint()->GetConstraintModality(albaTransformConstraint::Z) == albaTransformConstraint::LOCK );

  gui->m_RotationConstraintId = albaBehaviorRotateTranslate::Y_AXIS;
  gui->OnEvent(&e);

  CPPUNIT_ASSERT( gui->GetBehavior()->GetPivotRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetRotationConstraint()->GetRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetRotationConstraint()->GetConstraintModality(albaTransformConstraint::X) == albaTransformConstraint::LOCK );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetRotationConstraint()->GetConstraintModality(albaTransformConstraint::Y) == albaTransformConstraint::FREE );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetRotationConstraint()->GetConstraintModality(albaTransformConstraint::Z) == albaTransformConstraint::LOCK );

  gui->m_RotationConstraintId = albaBehaviorRotateTranslate::Z_AXIS;
  gui->OnEvent(&e);

  CPPUNIT_ASSERT( gui->GetBehavior()->GetPivotRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetRotationConstraint()->GetRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetRotationConstraint()->GetConstraintModality(albaTransformConstraint::X) == albaTransformConstraint::LOCK );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetRotationConstraint()->GetConstraintModality(albaTransformConstraint::Y) == albaTransformConstraint::LOCK );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetRotationConstraint()->GetConstraintModality(albaTransformConstraint::Z) == albaTransformConstraint::FREE );

  gui->m_RotationConstraintId = albaBehaviorRotateTranslate::VIEW_PLANE;
  gui->OnEvent(&e);

  CPPUNIT_ASSERT( gui->GetBehavior()->GetPivotRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetRotationConstraint()->GetRefSys()->GetType() == albaRefSys::VIEW );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetRotationConstraint()->GetConstraintModality(albaTransformConstraint::X) == albaTransformConstraint::FREE );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetRotationConstraint()->GetConstraintModality(albaTransformConstraint::Y) == albaTransformConstraint::FREE );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetRotationConstraint()->GetConstraintModality(albaTransformConstraint::Z) == albaTransformConstraint::LOCK );

  gui->m_RotationConstraintId = albaBehaviorRotateTranslate::NORMAL_VIEW_PLANE;
  gui->OnEvent(&e);

  CPPUNIT_ASSERT( gui->GetBehavior()->GetPivotRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetRotationConstraint()->GetRefSys()->GetType() == albaRefSys::VIEW );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetRotationConstraint()->GetConstraintModality(albaTransformConstraint::X) == albaTransformConstraint::LOCK );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetRotationConstraint()->GetConstraintModality(albaTransformConstraint::Y) == albaTransformConstraint::LOCK );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetRotationConstraint()->GetConstraintModality(albaTransformConstraint::Z) == albaTransformConstraint::FREE );
  //////////////////////////////////////////////////////////////////////////
  e.SetId(albaRotateTranslateGUI::ID_TRASLATION_AXES);
  gui->m_TranslationConstraintId = albaBehaviorRotateTranslate::X_AXIS;
  gui->OnEvent(&e);
  
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslationConstraint()->GetRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslationConstraint()->GetConstraintModality(albaTransformConstraint::X) == albaTransformConstraint::FREE );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslationConstraint()->GetConstraintModality(albaTransformConstraint::Y) == albaTransformConstraint::LOCK );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslationConstraint()->GetConstraintModality(albaTransformConstraint::Z) == albaTransformConstraint::LOCK );

  gui->m_TranslationConstraintId = albaBehaviorRotateTranslate::Y_AXIS;
  gui->OnEvent(&e);

  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslationConstraint()->GetRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslationConstraint()->GetConstraintModality(albaTransformConstraint::X) == albaTransformConstraint::LOCK );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslationConstraint()->GetConstraintModality(albaTransformConstraint::Y) == albaTransformConstraint::FREE );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslationConstraint()->GetConstraintModality(albaTransformConstraint::Z) == albaTransformConstraint::LOCK );

  gui->m_TranslationConstraintId = albaBehaviorRotateTranslate::Z_AXIS;
  gui->OnEvent(&e);

  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslationConstraint()->GetRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslationConstraint()->GetConstraintModality(albaTransformConstraint::X) == albaTransformConstraint::LOCK );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslationConstraint()->GetConstraintModality(albaTransformConstraint::Y) == albaTransformConstraint::LOCK );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslationConstraint()->GetConstraintModality(albaTransformConstraint::Z) == albaTransformConstraint::FREE );

  gui->m_TranslationConstraintId = albaBehaviorRotateTranslate::VIEW_PLANE;
  gui->OnEvent(&e);

  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslationConstraint()->GetRefSys()->GetType() == albaRefSys::VIEW );

  gui->m_TranslationConstraintId = albaBehaviorRotateTranslate::NORMAL_VIEW_PLANE;
  gui->OnEvent(&e);

  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslationConstraint()->GetRefSys()->GetType() == albaRefSys::VIEW );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslationConstraint()->GetConstraintModality(albaTransformConstraint::X) == albaTransformConstraint::FREE );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslationConstraint()->GetConstraintModality(albaTransformConstraint::Y) == albaTransformConstraint::FREE );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslationConstraint()->GetConstraintModality(albaTransformConstraint::Z) == albaTransformConstraint::LOCK );

  gui->m_TranslationConstraintId = albaBehaviorRotateTranslate::XY_PLANE;
  gui->OnEvent(&e);

  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslationConstraint()->GetRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslationConstraint()->GetConstraintModality(albaTransformConstraint::X) == albaTransformConstraint::FREE );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslationConstraint()->GetConstraintModality(albaTransformConstraint::Y) == albaTransformConstraint::FREE );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslationConstraint()->GetConstraintModality(albaTransformConstraint::Z) == albaTransformConstraint::LOCK );

  gui->m_TranslationConstraintId = albaBehaviorRotateTranslate::XZ_PLANE;
  gui->OnEvent(&e);

  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslationConstraint()->GetRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslationConstraint()->GetConstraintModality(albaTransformConstraint::X) == albaTransformConstraint::FREE );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslationConstraint()->GetConstraintModality(albaTransformConstraint::Y) == albaTransformConstraint::LOCK );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslationConstraint()->GetConstraintModality(albaTransformConstraint::Z) == albaTransformConstraint::FREE );

  gui->m_TranslationConstraintId = albaBehaviorRotateTranslate::YZ_PLANE;
  gui->OnEvent(&e);

  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslationConstraint()->GetRefSys()->GetType() == albaRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslationConstraint()->GetConstraintModality(albaTransformConstraint::X) == albaTransformConstraint::LOCK );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslationConstraint()->GetConstraintModality(albaTransformConstraint::Y) == albaTransformConstraint::FREE );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslationConstraint()->GetConstraintModality(albaTransformConstraint::Z) == albaTransformConstraint::FREE );

  gui->m_TranslationConstraintId = albaBehaviorRotateTranslate::SURFACE_SNAP;
  gui->OnEvent(&e);

  CPPUNIT_ASSERT( gui->GetBehavior()->GetSurfaceSnap() == true );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslationConstraint()->GetRefSys()->GetType() == albaRefSys::VIEW );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslationConstraint()->GetConstraintModality(albaTransformConstraint::X) == albaTransformConstraint::FREE );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslationConstraint()->GetConstraintModality(albaTransformConstraint::Y) == albaTransformConstraint::FREE );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslationConstraint()->GetConstraintModality(albaTransformConstraint::Z) == albaTransformConstraint::LOCK );

  gui->m_TranslationConstraintId = albaBehaviorRotateTranslate::NORMAL_SURFACE;
  gui->OnEvent(&e);

  CPPUNIT_ASSERT( gui->GetBehavior()->GetSurfaceNormal() == true );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslationConstraint()->GetRefSys()->GetType() == albaRefSys::VIEW );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslationConstraint()->GetConstraintModality(albaTransformConstraint::X) == albaTransformConstraint::FREE );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslationConstraint()->GetConstraintModality(albaTransformConstraint::Y) == albaTransformConstraint::FREE );
  CPPUNIT_ASSERT( gui->GetBehavior()->GetTranslationConstraint()->GetConstraintModality(albaTransformConstraint::Z) == albaTransformConstraint::LOCK );



  delete gui;
  albaDEL(behavior);
}