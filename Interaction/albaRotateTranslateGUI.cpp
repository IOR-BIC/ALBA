/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaRotateTranslateGUI
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


#include "albaRotateTranslateGUI.h"
#include "albaDecl.h"
#include "albaGUI.h"
#include "albaRotateTranslateBehavior.h"
#include "albaVME.h"

//----------------------------------------------------------------------------
albaRotateTranslateGUI::albaRotateTranslateGUI(albaVME *input, albaObserver *listener /* = NULL */, bool testMode /* = false */)
//----------------------------------------------------------------------------
{
  assert(input);
  m_Listener = listener;
  m_InputVME = input;
  m_TestMode = testMode;
  m_Gui = NULL;
  m_RotationConstraintId = albaRotateTranslateBehavior::VIEW_PLANE;
  m_TranslationConstraintId = albaRotateTranslateBehavior::VIEW_PLANE;
  m_RefSysVME = m_InputVME;

  m_OldBehavior = m_InputVME->GetBehavior();
  albaNEW(m_Behavior);
  m_Behavior->SetListener(this);
  m_Behavior->SetVME(m_InputVME);

  if (!m_TestMode)
  	CreateGui();

  AttachInteractorToVme();
}
//----------------------------------------------------------------------------
albaRotateTranslateGUI::~albaRotateTranslateGUI()
//----------------------------------------------------------------------------
{
  DetachInteractorFromVme();
  albaDEL(m_Behavior);
}
//----------------------------------------------------------------------------
void albaRotateTranslateGUI::CreateGui()
//----------------------------------------------------------------------------
{
  m_Gui = new albaGUI(this);
  m_Gui->Divider(2);
  m_Gui->Label("mouse interaction", true);
  m_Gui->Label("left mouse: rotate");
  m_Gui->Label("middle mouse: translate");
  m_Gui->Label("left mouse + ctrl: rotate around view normal");
  m_Gui->Divider();

  // rotation axes
  wxString rot_axes[5] = {"x", "y", "z", "view","normal view"};

  // translation axes
	wxString translation_type[10] = {"x", "y", "z", "view","normal view", "xy", "xz", "yz", "surface snap","surface snap with normal || x"}; 

  // rotation constraints
  m_Gui->Label("rotation constraints");
	m_Gui->Combo(ID_ROTATION_AXES,"",&m_RotationConstraintId,5,rot_axes);

  // translation constraints
  m_Gui->Label("translation constraints");
	m_Gui->Combo(ID_TRASLATION_AXES,"",&m_TranslationConstraintId,10,translation_type);

	m_Gui->Divider();

  m_Gui->Update();
}
//----------------------------------------------------------------------------
void albaRotateTranslateGUI::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  switch(alba_event->GetId())
  {
    case ID_ROTATION_AXES:
      m_Behavior->SetRotationConstraint(m_RotationConstraintId);
    break;
    case ID_TRASLATION_AXES:
      m_Behavior->SetTranslationConstraint(m_TranslationConstraintId);
    break;
    case ID_TRANSFORM:
    {
      // forward transform events to listener operation; the operation will move the vme
      alba_event->SetSender(this);
      albaEventMacro(*alba_event);
    }
    break;
    default:
      albaEventMacro(*alba_event);
    break;
  }
}
//----------------------------------------------------------------------------
void albaRotateTranslateGUI::EnableWidgets(bool enable)
//----------------------------------------------------------------------------
{
  if (!m_TestMode)
  {
	  m_Gui->Enable(ID_ROTATION_AXES, enable);
	  m_Gui->Enable(ID_TRASLATION_AXES, enable);
  }

  if (enable)
    AttachInteractorToVme();
  else
    DetachInteractorFromVme();
}
//----------------------------------------------------------------------------
void albaRotateTranslateGUI::AttachInteractorToVme()
//----------------------------------------------------------------------------
{
  m_InputVME->SetBehavior(m_Behavior);
}
//----------------------------------------------------------------------------
void albaRotateTranslateGUI::DetachInteractorFromVme()
//----------------------------------------------------------------------------
{
  m_InputVME->SetBehavior(m_OldBehavior);
}
//----------------------------------------------------------------------------
void albaRotateTranslateGUI::RefSysVmeChanged()
//----------------------------------------------------------------------------
{
  m_Behavior->SetRefSysVME(m_RefSysVME);
}
