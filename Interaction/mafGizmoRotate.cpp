/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGizmoRotate.cpp,v $
  Language:  C++
  Date:      $Date: 2005-07-07 15:13:26 $
  Version:   $Revision: 1.2 $
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


#include "mafGizmoRotate.h"
#include "mafDecl.h"
#include "mafGizmoRotateCircle.h"
#include "mafGizmoRotateFan.h"
#include "mafGuiGizmoRotate.h"
#include "mafSmartPointer.h"

#include "mmiGenericMouse.h"

#include "mafMatrix.h"
#include "mafTransform.h"
#include "mafVME.h"
#include "mafVMEOutput.h"

#include "vtkTransform.h"

//----------------------------------------------------------------------------
mafGizmoRotate::mafGizmoRotate(mafVME* input, mafObserver *listener)
//----------------------------------------------------------------------------
{
  assert(input);
  InputVME    = input;
  m_Listener  = listener;

  GRFan[X]    = GRFan[Y]    = GRFan[Z]    = NULL;
  GRCircle[X] = GRCircle[Y] = GRCircle[Z] = NULL;
  GuiGizmoRotate = NULL;

  for (int i = 0; i < 3; i++)
  {
    // create the fan and send events to this
    GRFan[i] = new mafGizmoRotateFan(input, this);
    GRFan[i]->SetAxis(i);

    // Create mafGizmoRotateCircle and send events to the corresponding fan
    GRCircle[i] = new mafGizmoRotateCircle(input, GRFan[i]);
	  GRCircle[i]->SetAxis(i);
  }
  

  // create the gizmo gui
  // gui is sending events to this
  GuiGizmoRotate = new mafGuiGizmoRotate(this);
  
  // initialize gizmo gui
  GuiGizmoRotate->SetAbsOrientation(InputVME->GetOutput()->GetAbsMatrix());

  Show(false);
}
//----------------------------------------------------------------------------
mafGizmoRotate::~mafGizmoRotate() 
//----------------------------------------------------------------------------
{
  //Destroy:
  //3 mafGizmoRotateCircle 
  //1 mafGizmoRotateFan
  for (int i = 0; i < 3; i++)
  {
    cppDEL(GRCircle[i]);
    cppDEL(GRFan[i]);
  }
  cppDEL(GuiGizmoRotate);
}

//----------------------------------------------------------------------------
void mafGizmoRotate::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  // get the sender
  void *sender = maf_event->GetSender();
    
  if (sender == GRFan[X] || sender == GRFan[Y] || sender == GRFan[Z])
  {
    OnEventGizmoComponents(maf_event); // process events from fans
  }
  else if (sender == GuiGizmoRotate)
  {
    OnEventGizmoGui(maf_event); // process events from gui
  }
  else
  {
    // forward to the listener
    mafEventMacro(*maf_event);
  }
}

//----------------------------------------------------------------------------
void mafGizmoRotate::OnEventGizmoComponents(mafEventBase *maf_event) 
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch (e->GetId())
    {
      //receiving pose matrices from the fan
      case ID_TRANSFORM:
      {
        void *sender = e->GetSender();
        long arg = e->GetArg();

        // if a gizmo circle has been picked
        if (arg == mmiGenericMouse::MOUSE_DOWN)
        {
          if (sender == GRFan[X])
          {
            this->Highlight(X);
          }
          else if (sender == GRFan[Y])
          {
            this->Highlight(Y);
          }
          else if (sender == GRFan[Z])
          {
            this->Highlight(Z);
          }
        }  
        else if (arg == mmiGenericMouse::MOUSE_MOVE)
        {
          // gizmo mode == local; gizmo is rotating during mouse move events
          if (Modality == G_LOCAL)
          {
            // get the old abs pose
            vtkTransform *currTr = vtkTransform::New();
            currTr->PostMultiply();
            currTr->SetMatrix(GetAbsPose()->GetVTKMatrix());
            currTr->Concatenate(e->GetMatrix()->GetVTKMatrix());
            currTr->Update();

            mafMatrix newAbsMatr;
            newAbsMatr.DeepCopy(currTr->GetMatrix());
            newAbsMatr.SetTimeStamp(GetAbsPose()->GetTimeStamp());

            // set the new pose to the gizmo
            SetAbsPose(&newAbsMatr, false);
            currTr->Delete();
          }
        }
        else if (arg == mmiGenericMouse::MOUSE_UP)
        {
          // gizmo mode == local
          if (Modality == G_LOCAL)
          {
            SetAbsPose(GetAbsPose());
          }
        }

        // forward event to the listener ie the operation
        // instanciating the gizmo; the sender is changed to "this" so that the operation can check for
        // the gizmo sending events
        e->SetSender(this);
        mafEventMacro(*e);
      }
      break;
      default:
      {
        mafEventMacro(*e);
      }
    }
  }
}

//----------------------------------------------------------------------------
void mafGizmoRotate::OnEventGizmoGui(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  // process events from the gui
  switch (maf_event->GetId())
  {
    case (mafGuiGizmoRotate::ID_ROTATE_X):
    {
      // receiving abs orientation from gui
      SendTransformMatrixFromGui(maf_event);
    }
    break;
    case (mafGuiGizmoRotate::ID_ROTATE_Y):
    {
      SendTransformMatrixFromGui(maf_event);     
    }
    break;
    case (mafGuiGizmoRotate::ID_ROTATE_Z):
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
void mafGizmoRotate::Highlight (int component) 
//----------------------------------------------------------------------------
{
  if (X_AXIS <= component && component < NUM_COMPONENTS)
  {
    for (int i = 0; i < NUM_COMPONENTS; i++)
    {
      if (i != component)
      {
        GRCircle[i]->Highlight(false);
        GRFan[i]->Show(false);
      }
      GRCircle[component]->Highlight(true);
      GRFan[component]->Show(true);
    }
  }
  else if (component == NONE)
  {
    for (int i = 0; i < NUM_COMPONENTS; i++)
    {
        GRCircle[i]->Highlight(false);
        GRFan[i]->Show(false);
    }
  }
}

//----------------------------------------------------------------------------  
void mafGizmoRotate::Show(bool show)
//----------------------------------------------------------------------------
{
  // set visibility ivar
  Visibility = show;

  for (int i = 0; i < 3; i++)
  {
    GRCircle[i]->Show(show);
    GRFan[i]->Show(show);
  }

  // if auxiliary ref sys is different from vme its orientation cannot be changed
  // so gui must not be keyable. Otherwise set gui keyability to show.

  if (RefSysVME == InputVME)
  {
    GuiGizmoRotate->EnableWidgets(show);
  }
  else
  {
    GuiGizmoRotate->EnableWidgets(false);
  }
  // update the camera
  mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}

//----------------------------------------------------------------------------  
void mafGizmoRotate::SetAbsPose(mafMatrix *absPose, bool applyPoseToFans)
//----------------------------------------------------------------------------
{
  // remove scaling part from gizmo abs pose; gizmo not scale
  double pos[3] = {0,0,0};
  double orient[3] = {0,0,0};
 
  mafTransform::GetPosition(*absPose, pos);
  mafTransform::GetOrientation(*absPose, orient);

  mafSmartPointer<mafMatrix> tmpMatr;
  tmpMatr->SetTimeStamp(absPose->GetTimeStamp());
  mafTransform::SetPosition(*tmpMatr.GetPointer(), pos);
  mafTransform::SetOrientation(*tmpMatr.GetPointer(), orient);

  for (int i = 0; i < 3; i++)
  {
    GRCircle[i]->SetAbsPose(tmpMatr);
    if (applyPoseToFans == true)
    {
      GRFan[i]->SetAbsPose(tmpMatr);
    }
  }
  GuiGizmoRotate->SetAbsOrientation(tmpMatr);
}

//----------------------------------------------------------------------------
mafMatrix *mafGizmoRotate::GetAbsPose()
//----------------------------------------------------------------------------
{
  return GRCircle[0]->GetAbsPose();
}

//----------------------------------------------------------------------------  
void mafGizmoRotate::SetInput(mafVME *input)
//----------------------------------------------------------------------------
{
  InputVME = input;
  for (int i = 0; i < 3; i++)
  {
    GRCircle[i]->SetInput(input);
    GRFan[i]->SetInput(input);
  }
}

//----------------------------------------------------------------------------  
mmiGenericInterface *mafGizmoRotate::GetInteractor(int axis)
//----------------------------------------------------------------------------  
{
  return GRCircle[axis]->GetInteractor();
}

//----------------------------------------------------------------------------
void mafGizmoRotate::SendTransformMatrixFromGui(mafEventBase *maf_event)
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

    // incoming matrix is a rotation matrix
    newAbsPose->DeepCopy(GetAbsPose());
    // copy rotation from incoming matrix
    mafTransform::CopyRotation(*e->GetMatrix(), *newAbsPose.GetPointer());

    invOldAbsPose.DeepCopy(this->GetAbsPose());
    invOldAbsPose.Invert();

    mafMatrix::Multiply4x4(*newAbsPose.GetPointer(),invOldAbsPose,*M.GetPointer());

    // update gizmo abs pose
    this->SetAbsPose(newAbsPose, true);

    // send transfrom to postmultiply to the listener. Events is sent as a transform event
    SendTransformMatrix(M, ID_TRANSFORM, mmiGenericMouse::MOUSE_MOVE);
  }
}

//----------------------------------------------------------------------------
void mafGizmoRotate::SetRefSys(mafVME *refSys)
//----------------------------------------------------------------------------
{
  assert(InputVME);  
  assert(GuiGizmoRotate);

  RefSysVME = refSys;
  SetAbsPose(RefSysVME->GetOutput()->GetAbsMatrix());

  if (RefSysVME == InputVME)
  {
    SetModalityToLocal();

    // if the gizmo is visible set the widgets visibility to true
    // if the refsys is local
    if (Visibility == true)
    {
      GuiGizmoRotate->EnableWidgets(true);
    }
  }
  else
  {
    SetModalityToGlobal();    

    // if the gizmo is visible set the widgets visibility to false
    // if the refsys is global since this refsys cannot be changed
    if (Visibility == true)
    {
      GuiGizmoRotate->EnableWidgets(false);
    }
  }
}
