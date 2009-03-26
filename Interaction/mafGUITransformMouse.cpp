/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUITransformMouse.cpp,v $
  Language:  C++
  Date:      $Date: 2009-03-26 16:52:57 $
  Version:   $Revision: 1.1.2.1 $
  Authors:   Stefano Perticoni
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "mafGUITransformMouse.h"
#include "mafDecl.h"

#include "mafGUI.h"
#include "mafGUIButton.h"

#include "mmiGenericMouse.h"
#include "mmiCompositorMouse.h"

#include "mafMatrix.h"
#include "mafAbsMatrixPipe.h"
#include "mafTransform.h"
#include "mafVME.h"
#include "mafVMEOutput.h"

#include "vtkMatrix4x4.h"
#include "vtkRenderer.h"

//----------------------------------------------------------------------------
mafGUITransformMouse::mafGUITransformMouse(mafVME *input, mafObserver *listener)
//----------------------------------------------------------------------------
{
  assert(input);

  m_Listener = listener;
  m_InputVME = input;
  m_Gui = NULL;
  
  m_IsaRotate = NULL;
  m_IsaTranslate = NULL;
  m_IsaRoll = NULL;

  m_RotationConstraintId = VIEW_PLANE;
  m_TranslationConstraintId = VIEW_PLANE;
  
  m_RefSysVME = m_InputVME;
  OldInteractor = NULL;

  CreateISA();
  CreateGui();
  
  AttachInteractorToVme();
}
//----------------------------------------------------------------------------
mafGUITransformMouse::~mafGUITransformMouse() 
//----------------------------------------------------------------------------
{ 
  DetachInteractorFromVme();    
  mafDEL(m_IsaCompositor); 
}

//----------------------------------------------------------------------------
void mafGUITransformMouse::CreateGui()
//----------------------------------------------------------------------------
{
  m_Gui = new mafGUI(this);
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
void mafGUITransformMouse::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  switch(maf_event->GetId())
  {
    case ID_ROTATION_AXES:
    {
      // set rotation constraint;
      if (m_RotationConstraintId == X_AXIS)
      {
        m_IsaRotate->GetPivotRefSys()->SetTypeToCustom(m_RefSysVME->GetOutput()->GetAbsMatrix());
        m_IsaRotate->GetRotationConstraint()->GetRefSys()->SetTypeToCustom(m_RefSysVME->GetOutput()->GetAbsMatrix());
        m_IsaRotate->GetRotationConstraint()->SetConstraintModality(mmiConstraint::FREE, mmiConstraint::LOCK, mmiConstraint::LOCK);
      }
      else if (m_RotationConstraintId == Y_AXIS)
      {
        m_IsaRotate->GetPivotRefSys()->SetTypeToCustom(m_RefSysVME->GetOutput()->GetAbsMatrix());
        m_IsaRotate->GetRotationConstraint()->GetRefSys()->SetTypeToCustom(m_RefSysVME->GetOutput()->GetAbsMatrix());
        m_IsaRotate->GetRotationConstraint()->SetConstraintModality(mmiConstraint::LOCK, mmiConstraint::FREE, mmiConstraint::LOCK);
      }
      else if (m_RotationConstraintId == Z_AXIS)
      {
        m_IsaRotate->GetPivotRefSys()->SetTypeToCustom(m_RefSysVME->GetOutput()->GetAbsMatrix());
        m_IsaRotate->GetRotationConstraint()->GetRefSys()->SetTypeToCustom(m_RefSysVME->GetOutput()->GetAbsMatrix());
        m_IsaRotate->GetRotationConstraint()->SetConstraintModality(mmiConstraint::LOCK, mmiConstraint::LOCK, mmiConstraint::FREE);
      }
      else if (m_RotationConstraintId == VIEW_PLANE)
      {
        m_IsaRotate->GetPivotRefSys()->SetTypeToCustom(m_RefSysVME->GetOutput()->GetAbsMatrix());
        m_IsaRotate->GetRotationConstraint()->SetConstraintModality(mmiConstraint::FREE, mmiConstraint::FREE, mmiConstraint::LOCK);
				//Modified by Matteo 30-05-2006
				//m_IsaRotate->GetRotationConstraint()->SetConstraintModality(mmiConstraint::LOCK, mmiConstraint::LOCK, mmiConstraint::FREE);
        //End Matteo
				m_IsaRotate->GetRotationConstraint()->GetRefSys()->SetTypeToView();
      }
			else if (m_RotationConstraintId == NORMAL_VIEW_PLANE)
			{
				m_IsaRotate->GetPivotRefSys()->SetTypeToCustom(m_RefSysVME->GetOutput()->GetAbsMatrix());
        //m_IsaRotate->GetRotationConstraint()->SetConstraintModality(mmiConstraint::FREE, mmiConstraint::FREE, mmiConstraint::LOCK);
				//Modified by Matteo 30-05-2006
				m_IsaRotate->GetRotationConstraint()->SetConstraintModality(mmiConstraint::LOCK, mmiConstraint::LOCK, mmiConstraint::FREE);
        //End Matteo
				m_IsaRotate->GetRotationConstraint()->GetRefSys()->SetTypeToView();
			}
    }
    break;
    case ID_TRASLATION_AXES:
    {
      // set translation constraint;
      if (m_TranslationConstraintId == X_AXIS)
      {
        m_IsaTranslate->SurfaceSnapOff();
        m_IsaTranslate->SurfaceNormalOff();
        m_IsaTranslate->GetTranslationConstraint()->GetRefSys()->SetTypeToCustom(m_RefSysVME->GetOutput()->GetAbsMatrix());
        m_IsaTranslate->GetPivotRefSys()->SetTypeToCustom(m_RefSysVME->GetOutput()->GetAbsMatrix());
        m_IsaTranslate->GetTranslationConstraint()->SetConstraintModality(mmiConstraint::FREE, mmiConstraint::LOCK, mmiConstraint::LOCK);   
      }
      else if (m_TranslationConstraintId == Y_AXIS)
      {
        m_IsaTranslate->SurfaceSnapOff();
        m_IsaTranslate->SurfaceNormalOff();
        m_IsaTranslate->GetTranslationConstraint()->GetRefSys()->SetTypeToCustom(m_RefSysVME->GetOutput()->GetAbsMatrix());
        m_IsaTranslate->GetPivotRefSys()->SetTypeToCustom(m_RefSysVME->GetOutput()->GetAbsMatrix());
        m_IsaTranslate->GetTranslationConstraint()->SetConstraintModality(mmiConstraint::LOCK, mmiConstraint::FREE, mmiConstraint::LOCK);        
      }
      else if (m_TranslationConstraintId == Z_AXIS)
      {                                             
        m_IsaTranslate->SurfaceSnapOff();
        m_IsaTranslate->SurfaceNormalOff();
        m_IsaTranslate->GetTranslationConstraint()->GetRefSys()->SetTypeToCustom(m_RefSysVME->GetOutput()->GetAbsMatrix());
        m_IsaTranslate->GetPivotRefSys()->SetTypeToCustom(m_RefSysVME->GetOutput()->GetAbsMatrix());
        m_IsaTranslate->GetTranslationConstraint()->SetConstraintModality(mmiConstraint::LOCK, mmiConstraint::LOCK, mmiConstraint::FREE);        
      }      
      else if (m_TranslationConstraintId == VIEW_PLANE)
      {
        m_IsaTranslate->SurfaceSnapOff();
        m_IsaTranslate->SurfaceNormalOff();
        m_IsaTranslate->GetTranslationConstraint()->GetRefSys()->SetTypeToView();
				//Modified by Matteo 30-05-2006
				//m_IsaTranslate->GetTranslationConstraint()->GetRefSys()->SetMatrix(m_RefSysVME->GetOutput()->GetAbsMatrix());
				//m_IsaTranslate->GetPivotRefSys()->SetTypeToCustom(m_RefSysVME->GetOutput()->GetAbsMatrix());
				//m_IsaTranslate->GetTranslationConstraint()->SetConstraintModality(mmiConstraint::FREE, mmiConstraint::FREE, mmiConstraint::LOCK);
				//End Matteo
			}
			else if (m_TranslationConstraintId == NORMAL_VIEW_PLANE)
      {
        m_IsaTranslate->SurfaceSnapOff();
        m_IsaTranslate->SurfaceNormalOff();
        m_IsaTranslate->GetTranslationConstraint()->GetRefSys()->SetTypeToView();
				//Modified by Matteo 30-05-2006
				m_IsaTranslate->GetTranslationConstraint()->GetRefSys()->SetMatrix(m_RefSysVME->GetOutput()->GetAbsMatrix());
				m_IsaTranslate->GetPivotRefSys()->SetTypeToCustom(m_RefSysVME->GetOutput()->GetAbsMatrix());
				m_IsaTranslate->GetTranslationConstraint()->SetConstraintModality(mmiConstraint::FREE, mmiConstraint::FREE, mmiConstraint::LOCK);
				//End Matteo
			}

      if (m_TranslationConstraintId == XY_PLANE)
      {
        m_IsaTranslate->SurfaceSnapOff();
        m_IsaTranslate->SurfaceNormalOff();
        m_IsaTranslate->GetTranslationConstraint()->GetRefSys()->SetTypeToCustom(m_RefSysVME->GetOutput()->GetAbsMatrix());
        m_IsaTranslate->GetPivotRefSys()->SetTypeToCustom(m_RefSysVME->GetOutput()->GetAbsMatrix());
        m_IsaTranslate->GetTranslationConstraint()->SetConstraintModality(mmiConstraint::FREE, mmiConstraint::FREE, mmiConstraint::LOCK);
      }
      else if (m_TranslationConstraintId == XZ_PLANE)  
      {
        m_IsaTranslate->SurfaceSnapOff();
        m_IsaTranslate->SurfaceNormalOff();
        m_IsaTranslate->GetTranslationConstraint()->GetRefSys()->SetTypeToCustom(m_RefSysVME->GetOutput()->GetAbsMatrix());
        m_IsaTranslate->GetPivotRefSys()->SetTypeToCustom(m_RefSysVME->GetOutput()->GetAbsMatrix());
        m_IsaTranslate->GetTranslationConstraint()->SetConstraintModality(mmiConstraint::FREE, mmiConstraint::LOCK, mmiConstraint::FREE);       
      }
      else if (m_TranslationConstraintId == YZ_PLANE)
      {               
        m_IsaTranslate->SurfaceSnapOff();
        m_IsaTranslate->SurfaceNormalOff();
        m_IsaTranslate->GetTranslationConstraint()->GetRefSys()->SetTypeToCustom(m_RefSysVME->GetOutput()->GetAbsMatrix());
        m_IsaTranslate->GetPivotRefSys()->SetTypeToCustom(m_RefSysVME->GetOutput()->GetAbsMatrix());
        m_IsaTranslate->GetTranslationConstraint()->SetConstraintModality(mmiConstraint::LOCK, mmiConstraint::FREE, mmiConstraint::FREE);   
      }
      // isa gen is sending matrix to the operation
      else if (m_TranslationConstraintId == SURFACE_SNAP)
      {
        m_IsaTranslate->SurfaceSnapOn();
        m_IsaTranslate->SurfaceNormalOff();
        m_IsaTranslate->GetTranslationConstraint()->GetRefSys()->SetTypeToView();
				//Modified by Matteo 30-05-2006
				m_IsaTranslate->GetTranslationConstraint()->GetRefSys()->SetMatrix(m_RefSysVME->GetOutput()->GetAbsMatrix());
				m_IsaTranslate->GetPivotRefSys()->SetTypeToCustom(m_RefSysVME->GetOutput()->GetAbsMatrix());
				m_IsaTranslate->GetTranslationConstraint()->SetConstraintModality(mmiConstraint::FREE, mmiConstraint::FREE, mmiConstraint::LOCK);
				//End Matteo
      }
      else if (m_TranslationConstraintId == NORMAL_SURFACE)
      {
        m_IsaTranslate->SurfaceSnapOff();
        m_IsaTranslate->SurfaceNormalOn();
       
        m_IsaTranslate->GetTranslationConstraint()->GetRefSys()->SetTypeToView();
        //Modified by Matteo 30-05-2006
        m_IsaTranslate->GetTranslationConstraint()->GetRefSys()->SetMatrix(m_RefSysVME->GetOutput()->GetAbsMatrix());
        m_IsaTranslate->GetPivotRefSys()->SetTypeToCustom(m_RefSysVME->GetOutput()->GetAbsMatrix());
        m_IsaTranslate->GetTranslationConstraint()->SetConstraintModality(mmiConstraint::FREE, mmiConstraint::FREE, mmiConstraint::LOCK);
        //End Matteo
      }
    }
    break;

    case ID_TRANSFORM:
    {
      // forward transform events to listener operation; the operation will move the vme
      maf_event->SetSender(this);
      mafEventMacro(*maf_event);
    }
    break;

    default:
    {
      mafEventMacro(*maf_event);
    }
    break;
  }
}

//----------------------------------------------------------------------------
void mafGUITransformMouse::CreateISA()
//----------------------------------------------------------------------------
{
  OldInteractor = m_InputVME->GetBehavior();

  // Create the isa compositor:
  m_IsaCompositor = mmiCompositorMouse::New();

  // default aux ref sys is the vme ref sys
  m_RefSysVME = m_InputVME;

  mafMatrix *absMatrix;
  absMatrix = m_RefSysVME->GetOutput()->GetAbsMatrix();
  //----------------------------------------------------------------------------
	// create the rotate behavior
	//----------------------------------------------------------------------------
  
  m_IsaRotate = m_IsaCompositor->CreateBehavior(MOUSE_LEFT);
  
  m_IsaRotate->SetListener(this); 
  m_IsaRotate->SetVME(m_InputVME);
  m_IsaRotate->GetRotationConstraint()->GetRefSys()->SetTypeToView();
  m_IsaRotate->GetRotationConstraint()->GetRefSys()->SetMatrix(absMatrix);
  
  m_IsaRotate->GetPivotRefSys()->SetTypeToView();
  m_IsaRotate->GetPivotRefSys()->SetMatrix(m_RefSysVME->GetOutput()->GetMatrix());
  
  //m_IsaRotate->GetRotationConstraint()->SetConstraintModality(mmiConstraint::FREE, mmiConstraint::FREE, mmiConstraint::LOCK);
	//Modified by Matteo 30-05-2006
	m_IsaRotate->GetRotationConstraint()->SetConstraintModality(mmiConstraint::LOCK, mmiConstraint::LOCK, mmiConstraint::FREE);
  //End Matteo
  m_IsaRotate->EnableRotation(true);
  
	//----------------------------------------------------------------------------
	// create the translate behavior
	//----------------------------------------------------------------------------
  
  m_IsaTranslate = m_IsaCompositor->CreateBehavior(MOUSE_MIDDLE);
  
  m_IsaTranslate->SetListener(this);
  m_IsaTranslate->SetVME(m_InputVME);
  m_IsaTranslate->GetTranslationConstraint()->GetRefSys()->SetTypeToView();
  // set the pivot point
  m_IsaTranslate->GetTranslationConstraint()->GetRefSys()->SetMatrix(absMatrix);
  m_IsaTranslate->GetPivotRefSys()->SetTypeToCustom(absMatrix);

  m_IsaTranslate->GetTranslationConstraint()->SetConstraintModality(mmiConstraint::FREE, mmiConstraint::FREE, mmiConstraint::LOCK);
  m_IsaTranslate->EnableTranslation(true);

  //----------------------------------------------------------------------------
	// create the roll behavior
	//----------------------------------------------------------------------------
  
  m_IsaRoll = m_IsaCompositor->CreateBehavior(MOUSE_LEFT_CONTROL);
  
  // isa gen is sending matrix to the operation
  m_IsaRoll->SetListener(this);
  m_IsaRoll->SetVME(m_InputVME);
  m_IsaRoll->GetRotationConstraint()->GetRefSys()->SetTypeToView();
  m_IsaRoll->GetRotationConstraint()->GetRefSys()->SetMatrix(absMatrix);
  
  m_IsaRoll->GetPivotRefSys()->SetTypeToView();
  m_IsaRoll->GetPivotRefSys()->SetMatrix(absMatrix);
  
  m_IsaRoll->GetRotationConstraint()->SetConstraintModality(mmiConstraint::LOCK, mmiConstraint::LOCK, mmiConstraint::FREE);
  m_IsaRoll->EnableRotation(true);
}

//----------------------------------------------------------------------------
void mafGUITransformMouse::EnableWidgets(bool enable)
//----------------------------------------------------------------------------
{
  m_Gui->Enable(ID_ROTATION_AXES, enable);
  m_Gui->Enable(ID_TRASLATION_AXES, enable);

  if (enable == true)
  {
    AttachInteractorToVme();
  }
  else
  {
    DetachInteractorFromVme();
  }
}

//----------------------------------------------------------------------------
void mafGUITransformMouse::AttachInteractorToVme()
//----------------------------------------------------------------------------
{
  m_InputVME->SetBehavior(m_IsaCompositor);
}

//----------------------------------------------------------------------------
void mafGUITransformMouse::DetachInteractorFromVme()
//----------------------------------------------------------------------------
{
  m_InputVME->SetBehavior(OldInteractor);
}

//----------------------------------------------------------------------------
void mafGUITransformMouse::RefSysVmeChanged()
//----------------------------------------------------------------------------
{
  m_IsaTranslate->GetTranslationConstraint()->GetRefSys()->SetMatrix(m_RefSysVME->GetOutput()->GetAbsMatrix());
  m_IsaTranslate->GetPivotRefSys()->SetMatrix(m_RefSysVME->GetOutput()->GetAbsMatrix());

  m_IsaRotate->GetRotationConstraint()->GetRefSys()->SetMatrix(m_RefSysVME->GetOutput()->GetAbsMatrix());
  m_IsaRotate->GetPivotRefSys()->SetMatrix(m_RefSysVME->GetOutput()->GetAbsMatrix());

  m_IsaRoll->GetRotationConstraint()->GetRefSys()->SetMatrix(m_RefSysVME->GetOutput()->GetAbsMatrix());
  m_IsaRoll->GetPivotRefSys()->SetMatrix(m_RefSysVME->GetOutput()->GetAbsMatrix());
}
