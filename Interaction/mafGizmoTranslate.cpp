/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGizmoTranslate.cpp,v $
  Language:  C++
  Date:      $Date: 2008-10-21 15:11:45 $
  Version:   $Revision: 1.8.2.1 $
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


#include "mafGizmoTranslate.h"
#include "mafDecl.h"
#include "mmiGenericMouse.h"
#include "mafGizmoTranslateAxis.h"
#include "mafGizmoTranslatePlane.h"
#include "mafGUIGizmoTranslate.h"
#include "mafSmartPointer.h"

#include "mmiGenericMouse.h"

#include "mafMatrix.h"
#include "mafTransform.h"
#include "mafVME.h"
#include "mafVMEOutput.h"

#include "vtkTransform.h"
#include "vtkMatrix4x4.h"

//----------------------------------------------------------------------------
mafGizmoTranslate::mafGizmoTranslate(mafVME* input, mafObserver *listener, bool buildGUI)
//----------------------------------------------------------------------------
{
  assert(input);
  m_InputVME = input;
  m_Listener = listener;
  m_BuildGUI = buildGUI;
  m_GTAxis[X] = m_GTAxis[Y] = m_GTAxis[Z] = NULL;
  m_GTPlane[XNORMAL] = m_GTPlane[YNORMAL] = m_GTPlane[ZNORMAL] = NULL;
  m_GuiGizmoTranslate = NULL;
  
  m_ConstraintModality[X] = mmiConstraint::FREE;
  m_ConstraintModality[Y] = mmiConstraint::FREE;
  m_ConstraintModality[Z] = mmiConstraint::FREE;

  m_Step[X] = 1;
  m_Step[Y] = 1;
  m_Step[Z] = 1;


  //no gizmo component is active at construction
  this->m_ActiveGizmoComponent = -1;
  this->SetModalityToLocal();

  mafMatrix *absInputMatrix = m_InputVME->GetOutput()->GetAbsMatrix();
  mafNEW(m_PivotPose);
  m_PivotPose->DeepCopy(absInputMatrix);

  for (int i = 0; i < 3; i++)
  {
    // Create mafGizmoTranslateAxis and send events to this
    m_GTAxis[i] = new mafGizmoTranslateAxis(input, this);
	  m_GTAxis[i]->SetAxis(i);
    // Create mafGTranslateAPlane 
    m_GTPlane[i] = new mafGizmoTranslatePlane(input, this);
    m_GTPlane[i]->SetPlane(i);
  }

  if (m_BuildGUI)
  {
    // create the gizmo gui
    // gui is sending events to this
    m_GuiGizmoTranslate = new mafGUIGizmoTranslate(this);
    // initialize gizmo gui
    m_GuiGizmoTranslate->SetAbsPosition(absInputMatrix);
  }
}
//----------------------------------------------------------------------------
mafGizmoTranslate::~mafGizmoTranslate() 
//----------------------------------------------------------------------------
{
  //Destroy:
  //3 mafGizmoTranslateAxis 
  //3 mafGizmoTranslatePlane
  for (int i = 0; i < 3; i++)
  {
    cppDEL(m_GTAxis[i]);
    cppDEL(m_GTPlane[i]);
  }
  mafDEL(m_PivotPose);
  cppDEL(m_GuiGizmoTranslate);
}
//----------------------------------------------------------------------------
void mafGizmoTranslate::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  void *sender = maf_event->GetSender();

  if (sender == m_GTAxis[X] || sender == m_GTAxis[Y] || sender == m_GTAxis[Z] || 
      sender == m_GTPlane[XNORMAL] || sender == m_GTPlane[YNORMAL] || sender == m_GTPlane[ZNORMAL])
  {
    OnEventGizmoComponents(maf_event); // process events from gizmo components
  }
  else if (sender == m_GuiGizmoTranslate)
  {
    OnEventGizmoGui(maf_event); // process events from the gui
  }
  else
  {
    mafEventMacro(*maf_event);
  }
}
//----------------------------------------------------------------------------
void mafGizmoTranslate::OnEventGizmoComponents(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    void *sender = e->GetSender();
    long arg = e->GetArg();

    switch (e->GetId())
    {
      case ID_TRANSFORM:
      {
        // if a gizmo has been picked register the active component; the sender is the component
        // to be activated
        if (arg == mmiGenericMouse::MOUSE_DOWN)
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
        else if (arg == mmiGenericMouse::MOUSE_MOVE)
        {     
          // matrix holding abs pose after mouse move event
          mafSmartPointer<mafMatrix> newAbsMatr;
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
        else if (arg == mmiGenericMouse::MOUSE_UP)
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
        mafEventMacro(*e);
      }
      break;

      default:
      {
        mafEventMacro(*e);
      }
      break;
    }
  }
}
//----------------------------------------------------------------------------
void mafGizmoTranslate::OnEventGizmoGui(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  switch(maf_event->GetId())
  {
    // process events from the gui   
    case (mafGUIGizmoTranslate::ID_TRANSLATE_X):
    {
      // receiving abs position from gui
      SendTransformMatrixFromGui(maf_event);     
    }
    break;
    case (mafGUIGizmoTranslate::ID_TRANSLATE_Y):
    {
      SendTransformMatrixFromGui(maf_event);     
    }
    break;
    case (mafGUIGizmoTranslate::ID_TRANSLATE_Z):
    {
      SendTransformMatrixFromGui(maf_event);     
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
void mafGizmoTranslate::Highlight (int component) 
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
void mafGizmoTranslate::Show(bool show)
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
mafMatrix *mafGizmoTranslate::GetAbsPose()
//----------------------------------------------------------------------------
{
  return m_GTAxis[0]->GetAbsPose();
}

//----------------------------------------------------------------------------  
void mafGizmoTranslate::SetInput(mafVME *input)
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
void mafGizmoTranslate::SendTransformMatrixFromGui(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    // send matrix to be postmultiplied to listener
    // [NewAbsPose] = [M]*[OldAbsPose] => [M] = [NewAbsPose][OldAbsPose]

    // build objects
    mafSmartPointer<mafMatrix> M;
    mafMatrix invOldAbsPose;
    mafSmartPointer<mafMatrix> newAbsPose;

    // incoming matrix is a translation matrix
    newAbsPose->DeepCopy(e->GetMatrix()); // abs position from gui
    // copy rotation part from OldAbsPose into NewAbsPose
    mafTransform::CopyRotation(*this->GetAbsPose(), *newAbsPose.GetPointer()); // abs orientation from old pose
    invOldAbsPose.DeepCopy(this->GetAbsPose());
    invOldAbsPose.Invert();
    mafMatrix::Multiply4x4(*newAbsPose.GetPointer(), invOldAbsPose, *M.GetPointer());
    // update gizmo abs pose
    this->SetAbsPose(newAbsPose, m_InputVME->GetTimeStamp());
    // send transfrom to postmultiply to the listener. Events is sent as a transform event
    SendTransformMatrix(M, ID_TRANSFORM, mmiGenericMouse::MOUSE_MOVE);
  }
}
//----------------------------------------------------------------------------  
void mafGizmoTranslate::SetAbsPose(mafMatrix *absPose, mafTimeStamp ts)
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafMatrix> tmpMatr;
  tmpMatr->DeepCopy(absPose);
  tmpMatr->SetTimeStamp(ts);
  // remove scaling part from gizmo abs pose; gizmo not scale
//  double pos[3] = {0,0,0};
//  double orient[3] = {0,0,0};
 
  //mafTransform::GetPosition(*absPose, pos);
  //mafTransform::GetOrientation(*absPose, orient);

  //mafTransform::SetPosition(*tmpMatr, pos);
  //mafTransform::SetOrientation(*tmpMatr, orient);

  for (int i = 0; i < 3; i++)
  {
    m_GTPlane[i]->SetAbsPose(tmpMatr);
    m_GTAxis[i]->SetAbsPose(tmpMatr);
  }
  if (m_BuildGUI) m_GuiGizmoTranslate->SetAbsPosition(tmpMatr);
}
//----------------------------------------------------------------------------
void mafGizmoTranslate::SetRefSys(mafVME *refSys)
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
void mafGizmoTranslate::SetConstraintModality(int axis, int constrainModality)
//----------------------------------------------------------------------------
{
  m_GTAxis[axis]->SetConstraintModality(axis,constrainModality);
  m_GTPlane[axis]->SetConstraintModality(axis,constrainModality);
  m_ConstraintModality[axis] = constrainModality;
}
//----------------------------------------------------------------------------
void mafGizmoTranslate::SetStep(int axis, double step)
//----------------------------------------------------------------------------
{
  m_GTAxis[axis]->SetStep(axis,step);
  m_GTPlane[axis]->SetStep(axis,step);
  m_Step[axis] = step;
}

int mafGizmoTranslate::GetConstraintModality( int axis )
{
  return m_ConstraintModality[axis];
}

int mafGizmoTranslate::GetStep( int axis )
{
  return m_Step[axis];
}

mafVME* mafGizmoTranslate::GetRefSys()
{
  return m_RefSysVME;
}