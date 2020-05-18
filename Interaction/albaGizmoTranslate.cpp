/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGizmoTranslate
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


#include "albaGizmoTranslate.h"
#include "albaDecl.h"
#include "albaInteractorGenericMouse.h"
#include "albaGizmoTranslateAxis.h"
#include "albaGizmoTranslatePlane.h"
#include "albaGUIGizmoTranslate.h"
#include "albaSmartPointer.h"

#include "albaInteractorGenericMouse.h"

#include "albaMatrix.h"
#include "albaTransform.h"
#include "albaVME.h"
#include "albaVMEOutput.h"

#include "vtkTransform.h"
#include "vtkMatrix4x4.h"

//----------------------------------------------------------------------------
albaGizmoTranslate::albaGizmoTranslate(albaVME* input, albaObserver *listener, bool buildGUI)
//----------------------------------------------------------------------------
{
  assert(input);
  m_InputVME = input;
  m_Listener = listener;
  m_BuildGUI = buildGUI;
  m_GTAxis[X] = m_GTAxis[Y] = m_GTAxis[Z] = NULL;
  m_GTPlane[XNORMAL] = m_GTPlane[YNORMAL] = m_GTPlane[ZNORMAL] = NULL;
  m_GuiGizmoTranslate = NULL;
  
  m_ConstraintModality[X] = albaInteractorConstraint::FREE;
  m_ConstraintModality[Y] = albaInteractorConstraint::FREE;
  m_ConstraintModality[Z] = albaInteractorConstraint::FREE;

  m_Step[X] = 1;
  m_Step[Y] = 1;
  m_Step[Z] = 1;


  //no gizmo component is active at construction
  this->m_ActiveGizmoComponent = -1;
  this->SetModalityToLocal();

  albaMatrix *absInputMatrix = m_InputVME->GetOutput()->GetAbsMatrix();
  albaNEW(m_PivotPose);
  m_PivotPose->DeepCopy(absInputMatrix);

	for (int i = 0; i < 3; i++)
	{
		// Create albaGizmoTranslateAxis and send events to this
		albaString name("Arrow");
		if (i == 0) name.Append("X");
		else if (i == 1) name.Append("Y");
		else if (i == 2) name.Append("Z");
		m_GTAxis[i] = new albaGizmoTranslateAxis(input, this, name);
		m_GTAxis[i]->SetAxis(i);

		// Create albaGTranslateAPlane 
		m_GTPlane[i] = new albaGizmoTranslatePlane(input, this);
		m_GTPlane[i]->SetPlane(i);
	}

  SetAlwaysVisible(true);
  SetAutoscale(true);

  if (m_BuildGUI)
  {
    // create the gizmo gui
    // gui is sending events to this
    m_GuiGizmoTranslate = new albaGUIGizmoTranslate(this);
    // initialize gizmo gui
    m_GuiGizmoTranslate->SetAbsPosition(absInputMatrix);
  }
}
//----------------------------------------------------------------------------
albaGizmoTranslate::~albaGizmoTranslate() 
//----------------------------------------------------------------------------
{
  //Destroy:
  //3 albaGizmoTranslateAxis 
  //3 albaGizmoTranslatePlane
  for (int i = 0; i < 3; i++)
  {
    cppDEL(m_GTAxis[i]);
    cppDEL(m_GTPlane[i]);
  }
  albaDEL(m_PivotPose);
  cppDEL(m_GuiGizmoTranslate);
}
//----------------------------------------------------------------------------
void albaGizmoTranslate::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  void *sender = alba_event->GetSender();

  if (sender == m_GTAxis[X] || sender == m_GTAxis[Y] || sender == m_GTAxis[Z] || 
      sender == m_GTPlane[XNORMAL] || sender == m_GTPlane[YNORMAL] || sender == m_GTPlane[ZNORMAL])
  {
    OnEventGizmoComponents(alba_event); // process events from gizmo components
  }
  else if (sender == m_GuiGizmoTranslate)
  {
    OnEventGizmoGui(alba_event); // process events from the gui
  }
  else
  {
    albaEventMacro(*alba_event);
  }
}
//----------------------------------------------------------------------------
void albaGizmoTranslate::OnEventGizmoComponents(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    void *sender = e->GetSender();
    long arg = e->GetArg();

    switch (e->GetId())
    {
      case ID_TRANSFORM:
      {
        // if a gizmo has been picked register the active component; the sender is the component
        // to be activated
        if (arg == albaInteractorGenericMouse::MOUSE_DOWN)
        {
          if (sender == m_GTAxis[X])
          {
            this->Highlight(X);
            m_ActiveGizmoComponent = X_AXIS;
          }
          else if (sender == m_GTAxis[Y])
          {
            this->Highlight(Y);
            m_ActiveGizmoComponent = Y_AXIS;
          }
          else if (sender == m_GTAxis[Z])
          {
            this->Highlight(Z);
            m_ActiveGizmoComponent = Z_AXIS;
          }
          else if (sender == m_GTPlane[XNORMAL])
          {
            this->Highlight(XN_PLANE);
            m_ActiveGizmoComponent = XN_PLANE;
          }
          else if (sender == m_GTPlane[YNORMAL])
          {
            this->Highlight(YN_PLANE);
            m_ActiveGizmoComponent = YN_PLANE;
          }
          else if (sender == m_GTPlane[ZNORMAL])
          {
            this->Highlight(ZN_PLANE);
            m_ActiveGizmoComponent = ZN_PLANE;
          }
          // Store pivot position
          m_PivotPose->DeepCopy(m_GTAxis[m_ActiveGizmoComponent]->GetAbsPose());
        }
        else if (arg == albaInteractorGenericMouse::MOUSE_MOVE)
        {     
          // matrix holding abs pose after mouse move event
          albaSmartPointer<albaMatrix> newAbsMatr;
          if (this->m_Modality == G_LOCAL) // gizmo working in local mode; all its components are moving
                                         // in a single mouse move event
          {
            // local mode
            // forward to all gizmo
            vtkTransform *currTr = vtkTransform::New();
            currTr->PostMultiply();
            currTr->SetMatrix(GetAbsPose()->GetVTKMatrix());
            currTr->Concatenate(e->GetMatrix()->GetVTKMatrix());
            currTr->Update();

            newAbsMatr->DeepCopy(currTr->GetMatrix());
            newAbsMatr->SetTimeStamp(GetAbsPose()->GetTimeStamp());

            // set the new pose to the gizmo
            SetAbsPose(newAbsMatr);
            currTr->Delete();
          }          
          else
          {
            // gizmo working in global mode; only one axis/plane is moving in a single mouse move event
            if (m_ActiveGizmoComponent == X ||  m_ActiveGizmoComponent == Y || m_ActiveGizmoComponent == Z)
            {
              // forward to active axis gizmo              
              vtkTransform *currTr = vtkTransform::New();
              currTr->PostMultiply();
              currTr->SetMatrix(m_GTAxis[m_ActiveGizmoComponent]->GetAbsPose()->GetVTKMatrix());
              currTr->Concatenate(e->GetMatrix()->GetVTKMatrix());
              currTr->Update();

              newAbsMatr->DeepCopy(currTr->GetMatrix());
              newAbsMatr->SetTimeStamp(GetAbsPose()->GetTimeStamp());

              // set the new pose to the gizmo
              m_GTAxis[m_ActiveGizmoComponent]->SetAbsPose(newAbsMatr);
              currTr->Delete();
            }
            else if (m_ActiveGizmoComponent == XN_PLANE ||  m_ActiveGizmoComponent == YN_PLANE || m_ActiveGizmoComponent == ZN_PLANE)
            {
              // forward to active plane gizmo
              vtkTransform *currTr = vtkTransform::New();
              currTr->PostMultiply();
              currTr->SetMatrix(m_GTPlane[m_ActiveGizmoComponent - 3]->GetAbsPose()->GetVTKMatrix());
              currTr->Concatenate(e->GetMatrix()->GetVTKMatrix());
              currTr->Update();

              newAbsMatr->DeepCopy(currTr->GetMatrix());
              newAbsMatr->SetTimeStamp(GetAbsPose()->GetTimeStamp());

              // set the new pose to the gizmo
              m_GTPlane[m_ActiveGizmoComponent - 3]->SetAbsPose(newAbsMatr);
              currTr->Delete();
            }     
          }
          // update only gui with gizmo abs position
          if (m_BuildGUI) m_GuiGizmoTranslate->SetAbsPosition(newAbsMatr);
        }
        else if (arg == albaInteractorGenericMouse::MOUSE_UP)
        {
          if (this->m_Modality == G_GLOBAL)
          {
            // put the gizmo back in the start position       
            SetAbsPose(m_PivotPose);
          }
        }
        // forward event to the listener ie the operation
        // instanciating the gizmo; the sender is changed to "this" so that the operation can check for
        // gizmo sending events
        e->SetSender(this);
        albaEventMacro(*e);
      }
      break;

      default:
      {
        albaEventMacro(*e);
      }
      break;
    }
  }
}
//----------------------------------------------------------------------------
void albaGizmoTranslate::OnEventGizmoGui(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  switch(alba_event->GetId())
  {
    // process events from the gui   
    case (albaGUIGizmoTranslate::ID_TRANSLATE_X):
    {
      // receiving abs position from gui
      SendTransformMatrixFromGui(alba_event);     
    }
    break;
    case (albaGUIGizmoTranslate::ID_TRANSLATE_Y):
    {
      SendTransformMatrixFromGui(alba_event);     
    }
    break;
    case (albaGUIGizmoTranslate::ID_TRANSLATE_Z):
    {
      SendTransformMatrixFromGui(alba_event);     
    }
    break;
    default:
    {
      albaEventMacro(*alba_event);
    }
    break;
  }
}
//----------------------------------------------------------------------------
void albaGizmoTranslate::Highlight (int component) 
//----------------------------------------------------------------------------
{
  int offset = 3;
  if (X_AXIS <= component && component <= Z_AXIS)
  {
    m_GTAxis[component]->Highlight(true);

    for (int i = 0; i < 3; i++)
    {
      m_GTPlane[i]->Highlight(false);
      if (i != component)
      {
        m_GTAxis[i]->Highlight(false);
      }
    }
  }       
  else if (XN_PLANE <= component && component <= ZN_PLANE)
  {
    int PlaneToHighlight = component - offset; 
    m_GTPlane[PlaneToHighlight]->Highlight(true);
    m_GTAxis[PlaneToHighlight]->Highlight(false);
    for (int i = 0; i < 3; i++)
    {
      if (i != PlaneToHighlight)
      {
        m_GTAxis[i]->Highlight(true);
        m_GTPlane[i]->Highlight(false);
      }
    }
  }
  else if (component == NONE)
  {
    for (int i = 0; i < 3; i++)
    {
      // DeHighlight everything;
      m_GTAxis[i]->Highlight(false);
      m_GTPlane[i]->Highlight(false);
    }
  }
}
//----------------------------------------------------------------------------  
void albaGizmoTranslate::Show(bool show)
//----------------------------------------------------------------------------
{
  // set visibility ivar
  m_Visibility = show;
  for (int i = 0; i < 3; i++)
  {
    m_GTAxis[i]->Show(show);
    m_GTPlane[i]->Show(show);
  }

  if (m_BuildGUI)
  {
    // if auxiliary ref sys is different from vme its orientation cannot be changed
    // so gui must not be keyable. Otherwise set gui keyability to show.
    if (m_RefSysVME == m_InputVME)
    {
      m_GuiGizmoTranslate->EnableWidgets(show);
    }
    else
    {
      m_GuiGizmoTranslate->EnableWidgets(false);
    }
  }
}
//----------------------------------------------------------------------------
void albaGizmoTranslate::Show( bool showX,bool showY,bool showZ )
//----------------------------------------------------------------------------
{
  // set visibility ivar
  m_Visibility = showX || showY || showZ;

  m_GTAxis[albaGizmoTranslate::X_AXIS]->Show(showX);
  m_GTPlane[albaGizmoTranslate::X_AXIS]->Show(showX&&showY&&showZ);
  m_GTAxis[albaGizmoTranslate::Y_AXIS]->Show(showY);
  m_GTPlane[albaGizmoTranslate::Y_AXIS]->Show(showX&&showY&&showZ);
  m_GTAxis[albaGizmoTranslate::Z_AXIS]->Show(showZ);
  m_GTPlane[albaGizmoTranslate::Z_AXIS]->Show(showX&&showY&&showZ);

  if (m_BuildGUI)
  {
    // if auxiliary ref sys is different from vme its orientation cannot be changed
    // so gui must not be keyable. Otherwise set gui keyability to show.
    if (m_RefSysVME == m_InputVME)
    {
      m_GuiGizmoTranslate->EnableWidgets(m_Visibility);
    }
    else
    {
      m_GuiGizmoTranslate->EnableWidgets(false);
    }
  }
}
//----------------------------------------------------------------------------
albaMatrix *albaGizmoTranslate::GetAbsPose()
//----------------------------------------------------------------------------
{
  return m_GTAxis[0]->GetAbsPose();
}

//----------------------------------------------------------------------------  
void albaGizmoTranslate::SetInput(albaVME *input)
//----------------------------------------------------------------------------
{
  this->m_InputVME = input;
  for (int i = 0; i < 3; i++)
  {
    m_GTAxis[i]->SetInput(input);
    m_GTPlane[i]->SetInput(input);
  }
}
//----------------------------------------------------------------------------
void albaGizmoTranslate::SendTransformMatrixFromGui(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    // send matrix to be postmultiplied to listener
    // [NewAbsPose] = [M]*[OldAbsPose] => [M] = [NewAbsPose][OldAbsPose]

    // build objects
    albaSmartPointer<albaMatrix> M;
    albaMatrix invOldAbsPose;
    albaSmartPointer<albaMatrix> newAbsPose;

    // incoming matrix is a translation matrix
    newAbsPose->DeepCopy(e->GetMatrix()); // abs position from gui
    // copy rotation part from OldAbsPose into NewAbsPose
    albaTransform::CopyRotation(*this->GetAbsPose(), *newAbsPose.GetPointer()); // abs orientation from old pose
    invOldAbsPose.DeepCopy(this->GetAbsPose());
    invOldAbsPose.Invert();
    albaMatrix::Multiply4x4(*newAbsPose.GetPointer(), invOldAbsPose, *M.GetPointer());
    // update gizmo abs pose
    this->SetAbsPose(newAbsPose, m_InputVME->GetTimeStamp());
    // send transfrom to postmultiply to the listener. Events is sent as a transform event
    SendTransformMatrix(M, ID_TRANSFORM, albaInteractorGenericMouse::MOUSE_MOVE);
  }
}
//----------------------------------------------------------------------------  
void albaGizmoTranslate::SetAbsPose(albaMatrix *absPose, albaTimeStamp ts)
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaMatrix> tmpMatr;
  tmpMatr->DeepCopy(absPose);
  tmpMatr->SetTimeStamp(ts);
  // remove scaling part from gizmo abs pose; gizmo not scale
//  double pos[3] = {0,0,0};
//  double orient[3] = {0,0,0};
 
  //albaTransform::GetPosition(*absPose, pos);
  //albaTransform::GetOrientation(*absPose, orient);

  //albaTransform::SetPosition(*tmpMatr, pos);
  //albaTransform::SetOrientation(*tmpMatr, orient);

  for (int i = 0; i < 3; i++)
  {
    m_GTPlane[i]->SetAbsPose(tmpMatr);
    m_GTAxis[i]->SetAbsPose(tmpMatr);
  }
  if (m_BuildGUI) m_GuiGizmoTranslate->SetAbsPosition(tmpMatr);
}
//----------------------------------------------------------------------------
void albaGizmoTranslate::SetRefSys(albaVME *refSys)
//----------------------------------------------------------------------------
{
  assert(m_InputVME);  
  if (m_BuildGUI) assert(m_GuiGizmoTranslate);

  m_RefSysVME = refSys;
  SetAbsPose(m_RefSysVME->GetOutput()->GetAbsMatrix());
  
	if (m_RefSysVME == m_InputVME)
    SetModalityToLocal();
  else
    SetModalityToGlobal();
	
	// if the gizmo is visible set the widgets visibility to false
	// if the ref-sys is global since this ref-sys cannot be changed
	if (m_Visibility == true && m_BuildGUI)
		m_GuiGizmoTranslate->EnableWidgets(true);
	
}
//----------------------------------------------------------------------------
void albaGizmoTranslate::SetConstraintModality(int axis, int constrainModality)
//----------------------------------------------------------------------------
{
  m_GTAxis[axis]->SetConstraintModality(axis,constrainModality);
  m_GTPlane[axis]->SetConstraintModality(axis,constrainModality);
  m_ConstraintModality[axis] = constrainModality;
}
//----------------------------------------------------------------------------
void albaGizmoTranslate::SetStep(int axis, double step)
//----------------------------------------------------------------------------
{
  m_GTAxis[axis]->SetStep(axis,step);
  m_GTPlane[axis]->SetStep(axis,step);
  m_Step[axis] = step;
}

int albaGizmoTranslate::GetConstraintModality( int axis )
{
  return m_ConstraintModality[axis];
}

int albaGizmoTranslate::GetStep( int axis )
{
  return m_Step[axis];
}

albaVME* albaGizmoTranslate::GetRefSys()
{
  return m_RefSysVME;
}

void albaGizmoTranslate::SetAutoscale( bool autoscale )
{
	albaGizmoInterface::SetAutoscale(autoscale);

	for (int i = 0; i < 3; i++)
	{
		m_GTAxis[i]->SetAutoscale(autoscale);
		m_GTPlane[i]->SetAutoscale(autoscale);
	}
}

void albaGizmoTranslate::SetAlwaysVisible( bool alwaysVisible )
{
	albaGizmoInterface::SetAlwaysVisible(alwaysVisible);

	for (int i = 0; i < 3; i++)
	{
		m_GTAxis[i]->SetAlwaysVisible(alwaysVisible);
		m_GTPlane[i]->SetAlwaysVisible(alwaysVisible);
	}
}

void albaGizmoTranslate::SetRenderWindowHeightPercentage(double percentage)
{
	for (int i = 0; i < 3; i++)
	{
		m_GTAxis[i]->SetRenderWindowHeightPercentage(percentage);
		m_GTPlane[i]->SetRenderWindowHeightPercentage(percentage);
	}
}
