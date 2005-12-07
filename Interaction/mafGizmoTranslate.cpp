/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGizmoTranslate.cpp,v $
  Language:  C++
  Date:      $Date: 2005-12-07 11:21:31 $
  Version:   $Revision: 1.4 $
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
#include "mafGuiGizmoTranslate.h"
#include "mafSmartPointer.h"

#include "mmiGenericMouse.h"

#include "mafMatrix.h"
#include "mafTransform.h"
#include "mafVME.h"
#include "mafVMEOutput.h"

#include "vtkTransform.h"
#include "vtkMatrix4x4.h"

//----------------------------------------------------------------------------
mafGizmoTranslate::mafGizmoTranslate(mafVME* input, mafObserver *listener)
//----------------------------------------------------------------------------
{
  assert(input);
  InputVME = input;
  m_Listener = listener;
  
  GTAxis[X] = GTAxis[Y] = GTAxis[Z] = NULL;
  GTPlane[XNORMAL] = GTPlane[YNORMAL] = GTPlane[ZNORMAL] = NULL;
  GuiGizmoTranslate = NULL;

  //no gizmo component is active at construction
  this->ActiveGizmoComponent = -1;
  this->SetModalityToLocal();

  PivotPose = mafMatrix::New();
  PivotPose->DeepCopy(InputVME->GetOutput()->GetAbsMatrix());

  for (int i = 0; i < 3; i++)
  {
    // Create mafGizmoTranslateAxis and send events to this
    GTAxis[i] = new mafGizmoTranslateAxis(input, this);
	  GTAxis[i]->SetAxis(i);

    // Create mafGTranslateAPlane 
    GTPlane[i] = new mafGizmoTranslatePlane(input, this);
    GTPlane[i]->SetPlane(i);
  }
  
  // create the gizmo gui
  // gui is sending events to this
  GuiGizmoTranslate = new mafGuiGizmoTranslate(this);
  // initialize gizmo gui
  GuiGizmoTranslate->SetAbsPosition(InputVME->GetOutput()->GetAbsMatrix());

  Show(false);
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
    cppDEL(GTAxis[i]);
    cppDEL(GTPlane[i]);
  }

  PivotPose->Delete();  
  cppDEL(GuiGizmoTranslate);
}
//----------------------------------------------------------------------------
void mafGizmoTranslate::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  void *sender = maf_event->GetSender();

  if (sender == GTAxis[X] || sender == GTAxis[Y] || sender == GTAxis[Z] || 
      sender == GTPlane[XNORMAL] || sender == GTPlane[YNORMAL] || sender == GTPlane[ZNORMAL])
  {
    OnEventGizmoComponents(maf_event); // process events from gizmo components
  }
  else if (sender == GuiGizmoTranslate)
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
          if (sender == GTAxis[X])
          {
            this->Highlight(X);
            ActiveGizmoComponent = X_AXIS;
          }
          else if (sender == GTAxis[Y])
          {
            this->Highlight(Y);
            ActiveGizmoComponent = Y_AXIS;
          }
          else if (sender == GTAxis[Z])
          {
            this->Highlight(Z);
            ActiveGizmoComponent = Z_AXIS;
          }
          else if (sender == GTPlane[XNORMAL])
          {
            this->Highlight(XN_PLANE);
            ActiveGizmoComponent = XN_PLANE;
          }
          else if (sender == GTPlane[YNORMAL])
          {
            this->Highlight(YN_PLANE);
            ActiveGizmoComponent = YN_PLANE;
          }
          else if (sender == GTPlane[ZNORMAL])
          {
            this->Highlight(ZN_PLANE);
            ActiveGizmoComponent = ZN_PLANE;
          }

          // Store pivot position
          PivotPose->DeepCopy(GTAxis[ActiveGizmoComponent]->GetAbsPose());

        }
        else if (arg == mmiGenericMouse::MOUSE_MOVE)
        {     
          // matrix holding abs pose after mouse move event
          mafSmartPointer<mafMatrix> newAbsMatr;
          if (this->Modality == G_LOCAL) // gizmo working in local mode; all its components are moving
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
            if (ActiveGizmoComponent == X ||  ActiveGizmoComponent == Y || ActiveGizmoComponent == Z)
            {

              // forward to active axis gizmo              
              vtkTransform *currTr = vtkTransform::New();
              currTr->PostMultiply();
              currTr->SetMatrix(GTAxis[ActiveGizmoComponent]->GetAbsPose()->GetVTKMatrix());
              currTr->Concatenate(e->GetMatrix()->GetVTKMatrix());
              currTr->Update();

              newAbsMatr->DeepCopy(currTr->GetMatrix());
              newAbsMatr->SetTimeStamp(GetAbsPose()->GetTimeStamp());

              // set the new pose to the gizmo
              GTAxis[ActiveGizmoComponent]->SetAbsPose(newAbsMatr);

              currTr->Delete();
            }
            else if (ActiveGizmoComponent == XN_PLANE ||  ActiveGizmoComponent == YN_PLANE || ActiveGizmoComponent == ZN_PLANE)
            {
              // forward to active plane gizmo
              vtkTransform *currTr = vtkTransform::New();
              currTr->PostMultiply();
              currTr->SetMatrix(GTPlane[ActiveGizmoComponent - 3]->GetAbsPose()->GetVTKMatrix());
              currTr->Concatenate(e->GetMatrix()->GetVTKMatrix());
              currTr->Update();

              newAbsMatr->DeepCopy(currTr->GetMatrix());
              newAbsMatr->SetTimeStamp(GetAbsPose()->GetTimeStamp());

              // set the new pose to the gizmo
              GTPlane[ActiveGizmoComponent - 3]->SetAbsPose(newAbsMatr);

              currTr->Delete();
            }     
          }

          // update only gui with gizmo abs position
          GuiGizmoTranslate->SetAbsPosition(newAbsMatr);
        }
        else if (arg == mmiGenericMouse::MOUSE_UP)
        {
          if (this->Modality == G_GLOBAL)
          {
            // put the gizmo back in the start position       
            SetAbsPose(PivotPose);
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
    case (mafGuiGizmoTranslate::ID_TRANSLATE_X):
    {
      // receiving abs position from gui
      SendTransformMatrixFromGui(maf_event);     
    }
    break;
    case (mafGuiGizmoTranslate::ID_TRANSLATE_Y):
    {
      SendTransformMatrixFromGui(maf_event);     
    }
    break;
    case (mafGuiGizmoTranslate::ID_TRANSLATE_Z):
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
    GTAxis[component]->Highlight(true);

    for (int i = 0; i < 3; i++)
    {
      GTPlane[i]->Highlight(false);
      if (i != component)
      {
        GTAxis[i]->Highlight(false);
      }
    }
  }       
  else if (XN_PLANE <= component && component <= ZN_PLANE)
  {

    int PlaneToHighlight = component - offset; 
    GTPlane[PlaneToHighlight]->Highlight(true);
    GTAxis[PlaneToHighlight]->Highlight(false);

    for (int i = 0; i < 3; i++)
    {
      if (i != PlaneToHighlight)
      {
        GTAxis[i]->Highlight(true);
        GTPlane[i]->Highlight(false);
      }
    }
  }
  else if (component == NONE)
  {
    for (int i = 0; i < 3; i++)
    {
        // DeHighlight everything;
        GTAxis[i]->Highlight(false);
        GTPlane[i]->Highlight(false);
    }
  }
}

//----------------------------------------------------------------------------  
void mafGizmoTranslate::Show(bool show)
//----------------------------------------------------------------------------
{
  // set visibility ivar
  Visibility = show;

  for (int i = 0; i < 3; i++)
  {
    GTAxis[i]->Show(show);
    GTPlane[i]->Show(show);
  }

  // if auxiliary ref sys is different from vme its orientation cannot be changed
  // so gui must not be keyable. Otherwise set gui keyability to show.

  if (RefSysVME == InputVME)
  {
    GuiGizmoTranslate->EnableWidgets(show);
  }
  else
  {
    GuiGizmoTranslate->EnableWidgets(false);
  }

  // update the camera
  //mafEventMacro(mafEvent(this, CAMERA_UPDATE));  // Paolo 20-07-2005
}

//----------------------------------------------------------------------------
mafMatrix *mafGizmoTranslate::GetAbsPose()
//----------------------------------------------------------------------------
{
  return GTAxis[0]->GetAbsPose();
}

//----------------------------------------------------------------------------  
void mafGizmoTranslate::SetInput(mafVME *input)
//----------------------------------------------------------------------------
{
  this->InputVME = input;
  for (int i = 0; i < 3; i++)
  {
    GTAxis[i]->SetInput(input);
    GTPlane[i]->SetInput(input);
  }
}

//----------------------------------------------------------------------------
void mafGizmoTranslate::SendTransformMatrixFromGui(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    // send matrix to be postmultiplied to listener
    //                                                                  -1    
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
    this->SetAbsPose(newAbsPose, InputVME->GetTimeStamp());

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
    GTPlane[i]->SetAbsPose(tmpMatr);
    GTAxis[i]->SetAbsPose(tmpMatr);
  }

  GuiGizmoTranslate->SetAbsPosition(tmpMatr);
}

//----------------------------------------------------------------------------
void mafGizmoTranslate::SetRefSys(mafVME *refSys)
//----------------------------------------------------------------------------
{
  assert(InputVME);  
  assert(GuiGizmoTranslate);

  RefSysVME = refSys;
  SetAbsPose(RefSysVME->GetOutput()->GetAbsMatrix());

  if (RefSysVME == InputVME)
  {
    SetModalityToLocal();

    // if the gizmo is visible set the widgets visibility to true
    // if the ref-sys is local
    if (Visibility == true)
    {
      GuiGizmoTranslate->EnableWidgets(true);
    }
  }
  else
  {
    SetModalityToGlobal();

    // if the gizmo is visible set the widgets visibility to false
    // if the ref-sys is global since this ref-sys cannot be changed
    if (Visibility == true)
    {
      GuiGizmoTranslate->EnableWidgets(false);
    }
  }
}
//----------------------------------------------------------------------------
void mafGizmoTranslate::SetConstraintModality(int axis, int constrainModality)
//----------------------------------------------------------------------------
{
  GTAxis[axis]->SetConstraintModality(axis,constrainModality);
  GTPlane[axis]->SetConstraintModality(axis,constrainModality);
}
//----------------------------------------------------------------------------
void mafGizmoTranslate::SetStep(int axis, double step)
//----------------------------------------------------------------------------
{
  GTAxis[axis]->SetStep(axis,step);
  GTPlane[axis]->SetStep(axis,step);
}
