/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGizmoCrossTranslate
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


#include "albaGizmoCrossTranslate.h"
#include "albaDecl.h"
#include "albaInteractorGenericMouse.h"
#include "albaGizmoCrossTranslateAxis.h"
#include "albaGizmoCrossTranslatePlane.h"
#include "albaGUIGizmoTranslate.h"
#include "albaSmartPointer.h"

#include "albaInteractorGenericMouse.h"

#include "albaMatrix.h"
#include "albaTransform.h"
#include "albaVME.h"
#include "albaVMEOutput.h"

#include "vtkTransform.h"
#include "vtkMatrix4x4.h"
#include "vtkDataSet.h"
#include "albaVMESlicer.h"
#include "albaVMEVolumeGray.h"

//----------------------------------------------------------------------------
albaGizmoCrossTranslate::albaGizmoCrossTranslate(albaVME* input, albaObserver *listener, bool buildGUI, int normal)
//----------------------------------------------------------------------------
{
  assert(input);

  m_InputVME = input;
  m_Listener = listener;
  m_BuildGUI = buildGUI;
  m_GTUpDown = NULL;
  m_GTLeftRight = NULL;
  m_GTPlane = NULL;
  m_GuiGizmoTranslate = NULL;

  m_ConstrainTranslationToInputVMEBounds = true;

  m_Normal = normal;

  m_ConstraintModality = albaInteractorConstraint::FREE;

  m_Step = 1;

  this->m_ActiveGizmoComponent = -1;
  this->SetModalityToLocal();

  albaMatrix *absInputMatrix = m_InputVME->GetOutput()->GetAbsMatrix();
  albaNEW(m_PivotPose);
  m_PivotPose->DeepCopy(absInputMatrix);


  // Create albaGizmoTranslateAxis and send events to this

  m_GTUpDown = new albaGizmoCrossTranslateAxis(input, this);
  m_GTLeftRight = new albaGizmoCrossTranslateAxis(input, this);

  int axis0 = -1;
  int axis1 = -1;

  int plane = -1;

  axis0 = X;
  axis1 = Y;
  plane = albaGizmoCrossTranslatePlane::Z_NORMAL;

  m_GTUpDown->SetAxis(axis0);  
  m_GTLeftRight->SetAxis(axis1);

  // Create albaGTranslateAPlane 
  m_GTPlane = new albaGizmoCrossTranslatePlane(input, this);
  m_GTPlane->SetPlane(plane);
  
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
albaGizmoCrossTranslate::~albaGizmoCrossTranslate() 
//----------------------------------------------------------------------------
{
  //Destroy:
  //3 albaGizmoTranslateAxis 
  //3 albaGizmoTranslatePlane

  cppDEL(m_GTUpDown);
  cppDEL(m_GTLeftRight);
  cppDEL(m_GTPlane);

  albaDEL(m_PivotPose);
  cppDEL(m_GuiGizmoTranslate);
}
//----------------------------------------------------------------------------
void albaGizmoCrossTranslate::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  void *sender = alba_event->GetSender();

  if (sender == m_GTUpDown || sender == m_GTLeftRight || sender == m_GTPlane )
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
void albaGizmoCrossTranslate::OnEventGizmoComponents(albaEventBase *alba_event)
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
          if (sender == m_GTUpDown)
          {
            this->Highlight(TRANSLATE_UP_DOWN);
            m_ActiveGizmoComponent = TRANSLATE_UP_DOWN;

            // show arrow source up down 
            m_GTUpDown->ShowTranslationFeedbackArrows(true);
          }
          else if (sender == m_GTLeftRight)
          {
            this->Highlight(TRANSLATE_LEFT_RIGHT);
            m_ActiveGizmoComponent = TRANSLATE_LEFT_RIGHT;
            
            // show arrow source left right
            m_GTLeftRight->ShowTranslationFeedbackArrows(true);
          }
          else if (sender == m_GTPlane)
          {
            this->Highlight(TRANSLATE_ON_PLANE);
            m_ActiveGizmoComponent = TRANSLATE_ON_PLANE;

            // show arrow source plane
            m_GTPlane->ShowTranslationFeedbackArrows(true);
          }

          // Store pivot position
          m_PivotPose->DeepCopy(m_GTUpDown->GetAbsPose());
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

            if (m_ConstrainTranslationToInputVMEBounds)
            {
              double slicedVolumeBounds[6];

              albaVMESlicer *slicer = NULL;
              slicer = albaVMESlicer::SafeDownCast(m_InputVME);

              assert(slicer);

              albaVMEVolumeGray *slicedVolume = NULL;

              slicedVolume = albaVMEVolumeGray::SafeDownCast(slicer->GetSlicedVMELink());
              assert(slicedVolume);

              slicedVolume->GetOutput()->GetVTKData()->GetBounds(slicedVolumeBounds);

              double gizmoCrossCenter[3] = {0,0,0};
              albaTransform::GetPosition(currTr->GetMatrix(), gizmoCrossCenter);

              enum 
              {
                X,
                Y,
                Z,
              };


              enum 
              {
                XMIN,
                XMAX,
                YMIN,
                YMAX,
                ZMIN,
                ZMAX,
              };

              bool outOfX = (gizmoCrossCenter[X] < slicedVolumeBounds[XMIN] ||
                gizmoCrossCenter[X] > slicedVolumeBounds[XMAX] );

              bool outOfY = (gizmoCrossCenter[Y] < slicedVolumeBounds[YMIN] ||
                gizmoCrossCenter[Y] > slicedVolumeBounds[YMAX] );

              bool outOfZ = (gizmoCrossCenter[Z] < slicedVolumeBounds[ZMIN] ||
                gizmoCrossCenter[Z] > slicedVolumeBounds[ZMAX] );

              bool outOfInputVME = outOfX || outOfY || outOfZ;

              if (outOfInputVME)
              {
                return;
              }
            }
            // set the new pose to the gizmo
            SetAbsPose(newAbsMatr);
            currTr->Delete();
          }          
          else
          {
            // gizmo working in global mode; only one axis/plane is moving in a single mouse move event
            if (m_ActiveGizmoComponent == TRANSLATE_UP_DOWN)
            {
              // forward to active axis gizmo              
              vtkTransform *currTr = vtkTransform::New();
              currTr->PostMultiply();
              currTr->SetMatrix(m_GTUpDown->GetAbsPose()->GetVTKMatrix());
              currTr->Concatenate(e->GetMatrix()->GetVTKMatrix());
              currTr->Update();

              newAbsMatr->DeepCopy(currTr->GetMatrix());
              newAbsMatr->SetTimeStamp(GetAbsPose()->GetTimeStamp());

              // set the new pose to the gizmo
              m_GTUpDown->SetAbsPose(newAbsMatr);
              currTr->Delete();
            }
            else if (m_ActiveGizmoComponent == TRANSLATE_LEFT_RIGHT)
            {
              // forward to active axis gizmo              
              vtkTransform *currTr = vtkTransform::New();
              currTr->PostMultiply();
              currTr->SetMatrix(m_GTLeftRight->GetAbsPose()->GetVTKMatrix());
              currTr->Concatenate(e->GetMatrix()->GetVTKMatrix());
              currTr->Update();

              newAbsMatr->DeepCopy(currTr->GetMatrix());
              newAbsMatr->SetTimeStamp(GetAbsPose()->GetTimeStamp());

              // set the new pose to the gizmo
              m_GTLeftRight->SetAbsPose(newAbsMatr);
              currTr->Delete();
            }
            else if (m_ActiveGizmoComponent == TRANSLATE_ON_PLANE)
            {
              // forward to active plane gizmo
              vtkTransform *currTr = vtkTransform::New();
              currTr->PostMultiply();
              currTr->SetMatrix(m_GTPlane->GetAbsPose()->GetVTKMatrix());
              currTr->Concatenate(e->GetMatrix()->GetVTKMatrix());
              currTr->Update();

              newAbsMatr->DeepCopy(currTr->GetMatrix());
              newAbsMatr->SetTimeStamp(GetAbsPose()->GetTimeStamp());

              // set the new pose to the gizmo
              m_GTPlane->SetAbsPose(newAbsMatr);
              currTr->Delete();
            }     
          }
          // update only gui with gizmo abs position
          if (m_BuildGUI) m_GuiGizmoTranslate->SetAbsPosition(newAbsMatr);
        }
        else if (arg == albaInteractorGenericMouse::MOUSE_UP)
        {
          if (sender == m_GTUpDown || sender == m_GTLeftRight || sender == m_GTPlane)
          {
            this->Highlight(NONE);
            m_GTUpDown->ShowTranslationFeedbackArrows(false);
            m_GTLeftRight->ShowTranslationFeedbackArrows(false);
            m_GTPlane->ShowTranslationFeedbackArrows(false);
          }

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
void albaGizmoCrossTranslate::OnEventGizmoGui(albaEventBase *alba_event)
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
void albaGizmoCrossTranslate::Highlight (int component) 
//----------------------------------------------------------------------------
{
  if (component == TRANSLATE_UP_DOWN)
  {
    m_GTUpDown->Highlight(true);
    m_GTLeftRight->Highlight(false);
    m_GTPlane->Highlight(false);
  } 
  else if (component == TRANSLATE_LEFT_RIGHT)
  {
    m_GTUpDown->Highlight(false);
    m_GTLeftRight->Highlight(true);
    m_GTPlane->Highlight(false);
  }
  else if (component == TRANSLATE_ON_PLANE)
  {
    m_GTUpDown->Highlight(false);
    m_GTLeftRight->Highlight(false);
    m_GTPlane->Highlight(true);
  }
  else if (component == NONE)
  {
    // DeHighlight everything;
    m_GTUpDown->Highlight(false);
    m_GTLeftRight->Highlight(false);
    m_GTPlane->Highlight(false);
  }
}
//----------------------------------------------------------------------------  
void albaGizmoCrossTranslate::Show(bool show)
//----------------------------------------------------------------------------
{
  // set visibility ivar
  m_Visibility = show;

  m_GTUpDown->Show(show);
  m_GTLeftRight->Show(show);
  m_GTPlane->Show(show);

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
albaMatrix *albaGizmoCrossTranslate::GetAbsPose()
//----------------------------------------------------------------------------
{
  return m_GTUpDown->GetAbsPose();
}

//----------------------------------------------------------------------------  
void albaGizmoCrossTranslate::SetInput(albaVME *input)
//----------------------------------------------------------------------------
{
  this->m_InputVME = input;

  m_GTUpDown->SetInput(input);
  m_GTLeftRight->SetInput(input);
  m_GTPlane->SetInput(input);

}
//----------------------------------------------------------------------------
void albaGizmoCrossTranslate::SendTransformMatrixFromGui(albaEventBase *alba_event)
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
void albaGizmoCrossTranslate::SetAbsPose(albaMatrix *absPose, albaTimeStamp ts)
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaMatrix> tmpMatr;
  tmpMatr->DeepCopy(absPose);
  tmpMatr->SetTimeStamp(ts);

  m_GTPlane->SetAbsPose(tmpMatr);
  m_GTUpDown->SetAbsPose(tmpMatr);
  m_GTLeftRight->SetAbsPose(tmpMatr);

  if (m_BuildGUI) m_GuiGizmoTranslate->SetAbsPosition(tmpMatr);

}
//----------------------------------------------------------------------------
void albaGizmoCrossTranslate::SetRefSys(albaVME *refSys)
//----------------------------------------------------------------------------
{
  assert(m_InputVME);  
  if (m_BuildGUI) assert(m_GuiGizmoTranslate);

  m_RefSysVME = refSys;
  SetAbsPose(m_RefSysVME->GetOutput()->GetAbsMatrix());
  if (m_RefSysVME == m_InputVME)
  {
    SetModalityToLocal();
    // if the gizmo is visible set the widgets visibility to true
    // if the ref-sys is local
    if (m_Visibility == true)
    {
      if (m_BuildGUI) m_GuiGizmoTranslate->EnableWidgets(true);
    }
  }
  else
  {
	// TODO REFACTOR THIS:
	// SetModalityToGlobal and SetModalityToLocal API should be renamed to something more meaningful
	SetModalityToGlobal();

    // if the gizmo is visible set the widgets visibility to false
    // if the ref-sys is global since this ref-sys cannot be changed
    if (m_Visibility == true)
    {
      if (m_BuildGUI) m_GuiGizmoTranslate->EnableWidgets(false);
    }
  }
}
//----------------------------------------------------------------------------
void albaGizmoCrossTranslate::SetConstraintModality(int constrainModality)
//----------------------------------------------------------------------------
{

  if (m_Normal == X)
  {
    m_GTUpDown->SetConstraintModality(Y,constrainModality);
    m_GTLeftRight->SetConstraintModality(Z, constrainModality);
    m_GTPlane->SetConstraintModality(m_Normal,constrainModality);
  }
  else if (m_Normal == Y)
  {
    m_GTUpDown->SetConstraintModality(X,constrainModality);
    m_GTLeftRight->SetConstraintModality(Z, constrainModality);
    m_GTPlane->SetConstraintModality(m_Normal,constrainModality);
  }
  else if (m_Normal == Z)
  {
    m_GTUpDown->SetConstraintModality(X,constrainModality);
    m_GTLeftRight->SetConstraintModality(Y, constrainModality);
    m_GTPlane->SetConstraintModality(m_Normal,constrainModality);
  }

  m_ConstraintModality = constrainModality;
}
//----------------------------------------------------------------------------
void albaGizmoCrossTranslate::SetStep(double step)
//----------------------------------------------------------------------------
{
  if (m_Normal == X)
  {
    m_GTUpDown->SetStep(Y,step);
    m_GTLeftRight->SetStep(Z,step);
    m_GTPlane->SetStep(m_Normal,step);
  }
  else if (m_Normal == Y)
  {
    m_GTUpDown->SetStep(X,step);
    m_GTLeftRight->SetStep(Z,step);
    m_GTPlane->SetStep(m_Normal,step);
  }
  else if (m_Normal == Z)
  {
    m_GTUpDown->SetStep(X,step);
    m_GTLeftRight->SetStep(Y,step);
    m_GTPlane->SetStep(m_Normal,step);
  }

  m_Step = step;
}

int albaGizmoCrossTranslate::GetConstraintModality()
{
  return m_ConstraintModality;
}

int albaGizmoCrossTranslate::GetStep()
{
  return m_Step;
}

albaVME* albaGizmoCrossTranslate::GetRefSys()
{
  return m_RefSysVME;
}

void albaGizmoCrossTranslate::SetAutoscale( bool autoscale )
{
  albaGizmoInterface::SetAutoscale(autoscale);

  m_GTUpDown->SetAutoscale(autoscale);
  m_GTLeftRight->SetAutoscale(autoscale);
  m_GTPlane->SetAutoscale(autoscale);	
}

void albaGizmoCrossTranslate::SetAlwaysVisible( bool alwaysVisible )
{
  albaGizmoInterface::SetAlwaysVisible(alwaysVisible);

  m_GTUpDown->SetAutoscale(alwaysVisible);
  m_GTLeftRight->SetAutoscale(alwaysVisible);
  m_GTPlane->SetAutoscale(alwaysVisible);	
}

void albaGizmoCrossTranslate::SetRenderWindowHeightPercentage(double percentage)
{
  albaGizmoInterface::SetRenderWindowHeightPercentage(percentage);

  m_GTUpDown->SetAutoscale(percentage);
  m_GTLeftRight->SetAutoscale(percentage);
  m_GTPlane->SetAutoscale(percentage);		
}
