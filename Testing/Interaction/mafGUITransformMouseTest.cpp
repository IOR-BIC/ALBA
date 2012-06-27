/*=========================================================================

 Program: MAF2
 Module: mafGUITransformMouseTest
 Authors: Matteo Giacomoni
 
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
#include "mafGUITransformMouseTest.h"
#include "mafGUITransformMouse.h"
#include "mafRefSys.h"
#include "mafVMEGroup.h"
#include "mafGUIFrame.h"
#include "mafInteractorGenericMouse.h"
#include "mafInteractorCompositorMouse.h"
#include "wx/module.h"

//----------------------------------------------------------------------------
void mafGUITransformMouseTest::setUp()
//----------------------------------------------------------------------------
{
  m_Win = new mafGUIFrame("testGui", wxDefaultPosition, wxSize(800, 600));
  wxModule::RegisterModules();
  wxModule::InitializeModules();
}
//----------------------------------------------------------------------------
void mafGUITransformMouseTest::tearDown()
//----------------------------------------------------------------------------
{
  wxModule::CleanUpModules();

  delete m_Win;

  delete wxLog::SetActiveTarget(NULL);
}
//----------------------------------------------------------------------------
void mafGUITransformMouseTest::TestFixture()
//----------------------------------------------------------------------------
{

}
//----------------------------------------------------------------------------
void mafGUITransformMouseTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMEGroup> group;
  mafGUITransformMouse *gui = new mafGUITransformMouse(group,NULL,true);
  delete gui;
}
//----------------------------------------------------------------------------
void mafGUITransformMouseTest::TestEnableWidgets()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMEGroup> group;
  mafGUITransformMouse *gui = new mafGUITransformMouse(group,NULL,true);

  gui->EnableWidgets(true);

  CPPUNIT_ASSERT( group->GetBehavior() == gui->m_IsaCompositor );

  gui->EnableWidgets(false);

  CPPUNIT_ASSERT( group->GetBehavior() == gui->m_OldInteractor );

  delete gui;
}
//----------------------------------------------------------------------------
void mafGUITransformMouseTest::TestAttachInteractorToVme()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMEGroup> group;
  mafGUITransformMouse *gui = new mafGUITransformMouse(group,NULL,true);

  gui->AttachInteractorToVme();

  CPPUNIT_ASSERT( group->GetBehavior() == gui->m_IsaCompositor );

  delete gui;
}
//----------------------------------------------------------------------------
void mafGUITransformMouseTest::TestDetachInteractorFromVme()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMEGroup> group;
  mafGUITransformMouse *gui = new mafGUITransformMouse(group,NULL,true);

  gui->DetachInteractorFromVme();

  CPPUNIT_ASSERT( group->GetBehavior() == gui->m_OldInteractor && group->GetBehavior() == NULL );

  delete gui;
}
//----------------------------------------------------------------------------
void mafGUITransformMouseTest::TestCreateBehavior()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMEGroup> group;
  mafGUITransformMouse *gui = new mafGUITransformMouse(group,NULL,true);

  mafInteractorGenericMouse *mouse = NULL;
  mouse = gui->CreateBehavior(MOUSE_LEFT);
  CPPUNIT_ASSERT( mouse->GetStartButton() == MAF_LEFT_BUTTON );

  mouse = gui->CreateBehavior(MOUSE_LEFT_CONTROL);
  CPPUNIT_ASSERT( mouse->GetStartButton() == MAF_LEFT_BUTTON && mouse->GetModifiers() == MAF_CTRL_KEY);

  mouse = gui->CreateBehavior(MOUSE_LEFT_SHIFT);
  CPPUNIT_ASSERT( mouse->GetStartButton() == MAF_LEFT_BUTTON && mouse->GetModifiers() == MAF_SHIFT_KEY);

  mouse = gui->CreateBehavior(MOUSE_RIGHT);
  CPPUNIT_ASSERT( mouse->GetStartButton() == MAF_RIGHT_BUTTON );

  mouse = gui->CreateBehavior(MOUSE_RIGHT_CONTROL);
  CPPUNIT_ASSERT( mouse->GetStartButton() == MAF_RIGHT_BUTTON && mouse->GetModifiers() == MAF_CTRL_KEY);

  mouse = gui->CreateBehavior(MOUSE_RIGHT_SHIFT);
  CPPUNIT_ASSERT( mouse->GetStartButton() == MAF_RIGHT_BUTTON && mouse->GetModifiers() == MAF_SHIFT_KEY);

  mouse = gui->CreateBehavior(MOUSE_MIDDLE);
  CPPUNIT_ASSERT( mouse->GetStartButton() == MAF_MIDDLE_BUTTON );

  mouse = gui->CreateBehavior(MOUSE_MIDDLE_CONTROL);
  CPPUNIT_ASSERT( mouse->GetStartButton() == MAF_MIDDLE_BUTTON && mouse->GetModifiers() == MAF_CTRL_KEY);

  mouse = gui->CreateBehavior(MOUSE_MIDDLE_SHIFT);
  CPPUNIT_ASSERT( mouse->GetStartButton() == MAF_MIDDLE_BUTTON && mouse->GetModifiers() == MAF_SHIFT_KEY);

  delete gui;
}
//----------------------------------------------------------------------------
void mafGUITransformMouseTest::TestOnEvent()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMEGroup> group;
  mafGUITransformMouse *gui = new mafGUITransformMouse(group,NULL,true);

  mafEvent e;
  e.SetId(mafGUITransformMouse::ID_ROTATION_AXES);
  gui->m_RotationConstraintId = mafGUITransformMouse::X_AXIS;
  gui->OnEvent(&e);

  CPPUNIT_ASSERT( gui->m_IsaRotate->GetPivotRefSys()->GetType() == mafRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->m_IsaRotate->GetRotationConstraint()->GetRefSys()->GetType() == mafRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->m_IsaRotate->GetRotationConstraint()->GetConstraintModality(mafInteractorConstraint::X) == mafInteractorConstraint::FREE );
  CPPUNIT_ASSERT( gui->m_IsaRotate->GetRotationConstraint()->GetConstraintModality(mafInteractorConstraint::Y) == mafInteractorConstraint::LOCK );
  CPPUNIT_ASSERT( gui->m_IsaRotate->GetRotationConstraint()->GetConstraintModality(mafInteractorConstraint::Z) == mafInteractorConstraint::LOCK );

  gui->m_RotationConstraintId = mafGUITransformMouse::Y_AXIS;
  gui->OnEvent(&e);

  CPPUNIT_ASSERT( gui->m_IsaRotate->GetPivotRefSys()->GetType() == mafRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->m_IsaRotate->GetRotationConstraint()->GetRefSys()->GetType() == mafRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->m_IsaRotate->GetRotationConstraint()->GetConstraintModality(mafInteractorConstraint::X) == mafInteractorConstraint::LOCK );
  CPPUNIT_ASSERT( gui->m_IsaRotate->GetRotationConstraint()->GetConstraintModality(mafInteractorConstraint::Y) == mafInteractorConstraint::FREE );
  CPPUNIT_ASSERT( gui->m_IsaRotate->GetRotationConstraint()->GetConstraintModality(mafInteractorConstraint::Z) == mafInteractorConstraint::LOCK );

  gui->m_RotationConstraintId = mafGUITransformMouse::Z_AXIS;
  gui->OnEvent(&e);

  CPPUNIT_ASSERT( gui->m_IsaRotate->GetPivotRefSys()->GetType() == mafRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->m_IsaRotate->GetRotationConstraint()->GetRefSys()->GetType() == mafRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->m_IsaRotate->GetRotationConstraint()->GetConstraintModality(mafInteractorConstraint::X) == mafInteractorConstraint::LOCK );
  CPPUNIT_ASSERT( gui->m_IsaRotate->GetRotationConstraint()->GetConstraintModality(mafInteractorConstraint::Y) == mafInteractorConstraint::LOCK );
  CPPUNIT_ASSERT( gui->m_IsaRotate->GetRotationConstraint()->GetConstraintModality(mafInteractorConstraint::Z) == mafInteractorConstraint::FREE );

  gui->m_RotationConstraintId = mafGUITransformMouse::VIEW_PLANE;
  gui->OnEvent(&e);

  CPPUNIT_ASSERT( gui->m_IsaRotate->GetPivotRefSys()->GetType() == mafRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->m_IsaRotate->GetRotationConstraint()->GetRefSys()->GetType() == mafRefSys::VIEW );
  CPPUNIT_ASSERT( gui->m_IsaRotate->GetRotationConstraint()->GetConstraintModality(mafInteractorConstraint::X) == mafInteractorConstraint::FREE );
  CPPUNIT_ASSERT( gui->m_IsaRotate->GetRotationConstraint()->GetConstraintModality(mafInteractorConstraint::Y) == mafInteractorConstraint::FREE );
  CPPUNIT_ASSERT( gui->m_IsaRotate->GetRotationConstraint()->GetConstraintModality(mafInteractorConstraint::Z) == mafInteractorConstraint::LOCK );

  gui->m_RotationConstraintId = mafGUITransformMouse::NORMAL_VIEW_PLANE;
  gui->OnEvent(&e);

  CPPUNIT_ASSERT( gui->m_IsaRotate->GetPivotRefSys()->GetType() == mafRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->m_IsaRotate->GetRotationConstraint()->GetRefSys()->GetType() == mafRefSys::VIEW );
  CPPUNIT_ASSERT( gui->m_IsaRotate->GetRotationConstraint()->GetConstraintModality(mafInteractorConstraint::X) == mafInteractorConstraint::LOCK );
  CPPUNIT_ASSERT( gui->m_IsaRotate->GetRotationConstraint()->GetConstraintModality(mafInteractorConstraint::Y) == mafInteractorConstraint::LOCK );
  CPPUNIT_ASSERT( gui->m_IsaRotate->GetRotationConstraint()->GetConstraintModality(mafInteractorConstraint::Z) == mafInteractorConstraint::FREE );
  //////////////////////////////////////////////////////////////////////////
  e.SetId(mafGUITransformMouse::ID_TRASLATION_AXES);
  gui->m_TranslationConstraintId = mafGUITransformMouse::X_AXIS;
  gui->OnEvent(&e);
  
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetPivotRefSys()->GetType() == mafRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetTranslationConstraint()->GetRefSys()->GetType() == mafRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetTranslationConstraint()->GetConstraintModality(mafInteractorConstraint::X) == mafInteractorConstraint::FREE );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetTranslationConstraint()->GetConstraintModality(mafInteractorConstraint::Y) == mafInteractorConstraint::LOCK );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetTranslationConstraint()->GetConstraintModality(mafInteractorConstraint::Z) == mafInteractorConstraint::LOCK );

  gui->m_TranslationConstraintId = mafGUITransformMouse::Y_AXIS;
  gui->OnEvent(&e);

  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetPivotRefSys()->GetType() == mafRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetTranslationConstraint()->GetRefSys()->GetType() == mafRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetTranslationConstraint()->GetConstraintModality(mafInteractorConstraint::X) == mafInteractorConstraint::LOCK );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetTranslationConstraint()->GetConstraintModality(mafInteractorConstraint::Y) == mafInteractorConstraint::FREE );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetTranslationConstraint()->GetConstraintModality(mafInteractorConstraint::Z) == mafInteractorConstraint::LOCK );

  gui->m_TranslationConstraintId = mafGUITransformMouse::Z_AXIS;
  gui->OnEvent(&e);

  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetPivotRefSys()->GetType() == mafRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetTranslationConstraint()->GetRefSys()->GetType() == mafRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetTranslationConstraint()->GetConstraintModality(mafInteractorConstraint::X) == mafInteractorConstraint::LOCK );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetTranslationConstraint()->GetConstraintModality(mafInteractorConstraint::Y) == mafInteractorConstraint::LOCK );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetTranslationConstraint()->GetConstraintModality(mafInteractorConstraint::Z) == mafInteractorConstraint::FREE );

  gui->m_TranslationConstraintId = mafGUITransformMouse::VIEW_PLANE;
  gui->OnEvent(&e);

  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetPivotRefSys()->GetType() == mafRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetTranslationConstraint()->GetRefSys()->GetType() == mafRefSys::VIEW );

  gui->m_TranslationConstraintId = mafGUITransformMouse::NORMAL_VIEW_PLANE;
  gui->OnEvent(&e);

  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetPivotRefSys()->GetType() == mafRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetTranslationConstraint()->GetRefSys()->GetType() == mafRefSys::VIEW );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetTranslationConstraint()->GetConstraintModality(mafInteractorConstraint::X) == mafInteractorConstraint::FREE );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetTranslationConstraint()->GetConstraintModality(mafInteractorConstraint::Y) == mafInteractorConstraint::FREE );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetTranslationConstraint()->GetConstraintModality(mafInteractorConstraint::Z) == mafInteractorConstraint::LOCK );

  gui->m_TranslationConstraintId = mafGUITransformMouse::XY_PLANE;
  gui->OnEvent(&e);

  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetPivotRefSys()->GetType() == mafRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetTranslationConstraint()->GetRefSys()->GetType() == mafRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetTranslationConstraint()->GetConstraintModality(mafInteractorConstraint::X) == mafInteractorConstraint::FREE );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetTranslationConstraint()->GetConstraintModality(mafInteractorConstraint::Y) == mafInteractorConstraint::FREE );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetTranslationConstraint()->GetConstraintModality(mafInteractorConstraint::Z) == mafInteractorConstraint::LOCK );

  gui->m_TranslationConstraintId = mafGUITransformMouse::XZ_PLANE;
  gui->OnEvent(&e);

  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetPivotRefSys()->GetType() == mafRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetTranslationConstraint()->GetRefSys()->GetType() == mafRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetTranslationConstraint()->GetConstraintModality(mafInteractorConstraint::X) == mafInteractorConstraint::FREE );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetTranslationConstraint()->GetConstraintModality(mafInteractorConstraint::Y) == mafInteractorConstraint::LOCK );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetTranslationConstraint()->GetConstraintModality(mafInteractorConstraint::Z) == mafInteractorConstraint::FREE );

  gui->m_TranslationConstraintId = mafGUITransformMouse::YZ_PLANE;
  gui->OnEvent(&e);

  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetPivotRefSys()->GetType() == mafRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetTranslationConstraint()->GetRefSys()->GetType() == mafRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetTranslationConstraint()->GetConstraintModality(mafInteractorConstraint::X) == mafInteractorConstraint::LOCK );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetTranslationConstraint()->GetConstraintModality(mafInteractorConstraint::Y) == mafInteractorConstraint::FREE );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetTranslationConstraint()->GetConstraintModality(mafInteractorConstraint::Z) == mafInteractorConstraint::FREE );

  gui->m_TranslationConstraintId = mafGUITransformMouse::SURFACE_SNAP;
  gui->OnEvent(&e);

  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetSurfaceSnap() == true );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetPivotRefSys()->GetType() == mafRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetTranslationConstraint()->GetRefSys()->GetType() == mafRefSys::VIEW );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetTranslationConstraint()->GetConstraintModality(mafInteractorConstraint::X) == mafInteractorConstraint::FREE );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetTranslationConstraint()->GetConstraintModality(mafInteractorConstraint::Y) == mafInteractorConstraint::FREE );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetTranslationConstraint()->GetConstraintModality(mafInteractorConstraint::Z) == mafInteractorConstraint::LOCK );

  gui->m_TranslationConstraintId = mafGUITransformMouse::NORMAL_SURFACE;
  gui->OnEvent(&e);

  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetSurfaceNormal() == true );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetPivotRefSys()->GetType() == mafRefSys::CUSTOM );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetTranslationConstraint()->GetRefSys()->GetType() == mafRefSys::VIEW );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetTranslationConstraint()->GetConstraintModality(mafInteractorConstraint::X) == mafInteractorConstraint::FREE );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetTranslationConstraint()->GetConstraintModality(mafInteractorConstraint::Y) == mafInteractorConstraint::FREE );
  CPPUNIT_ASSERT( gui->m_IsaTranslate->GetTranslationConstraint()->GetConstraintModality(mafInteractorConstraint::Z) == mafInteractorConstraint::LOCK );



  delete gui;
}