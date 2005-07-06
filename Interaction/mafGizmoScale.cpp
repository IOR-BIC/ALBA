/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGizmoScale.cpp,v $
  Language:  C++
  Date:      $Date: 2005-07-06 13:50:25 $
  Version:   $Revision: 1.1 $
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


#include "mafGizmoScale.h"
#include "mafDecl.h"
#include "mafGizmoScaleAxis.h"
#include "mafGizmoScaleIsotropic.h"
#include "mafSmartPointer.h"
#include "vtkMAFSmartPointer.h"

#include "mmiGenericMouse.h"

#include "mafMatrix.h"
#include "mafTransform.h"
#include "mafTransformFrame.h"
#include "mafVME.h"
#include "mafVMEOutput.h"

#include "vtkTransform.h"
#include "vtkMath.h"

//----------------------------------------------------------------------------
mafGizmoScale::mafGizmoScale(mafVME* input, mafObserver *listener)
//----------------------------------------------------------------------------
{
  assert(input);
  InputVME = input;
  m_Listener = listener;
  
  GSAxis[X] = GSAxis[Y] = GSAxis[Z] = NULL;
  GSIsotropic = NULL;
  GuiGizmoScale = NULL;

  //no gizmo component is active at construction
  this->ActiveGizmoComponent = NONE;
  this->SetModalityToLocal();

  InitialGizmoPose = mafMatrix::New();
  InitialGizmoPose->DeepCopy(InputVME->GetOutput()->GetAbsMatrix());

  VmeMatrixRelativeToRefSysVME = mafMatrix::New();
  RefSysVMEAbsMatrixAtMouseDown = mafMatrix::New();

  for (int i = 0; i < 3; i++)
  {
    // Create mafGizmoScaleAxis and send events to this
    GSAxis[i] = new mafGizmoScaleAxis(input, this);
	  GSAxis[i]->SetAxis(i);
  }
  
  GSIsotropic = new mafGizmoScaleIsotropic(input, this);

  // create the gizmo gui
  // gui is sending events to this
  GuiGizmoScale = new mafGuiGizmoScale(this);
  // initialize gizmo gui
  GuiGizmoScale->SetAbsScaling(InputVME->GetOutput()->GetAbsMatrix());
  GuiGizmoScale->EnableWidgets(false);

  Show(false);
}
//----------------------------------------------------------------------------
mafGizmoScale::~mafGizmoScale() 
//----------------------------------------------------------------------------
{
  //Destroy:
  //3 mafGizmoScaleAxis 
  for (int i = 0; i < 3; i++)
  {
    cppDEL(GSAxis[i]);
  }
  cppDEL(GSIsotropic);
  InitialGizmoPose->Delete();  
  VmeMatrixRelativeToRefSysVME->Delete();
  RefSysVMEAbsMatrixAtMouseDown->Delete();
  cppDEL(GuiGizmoScale);
}
//----------------------------------------------------------------------------
void mafGizmoScale::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  void *sender = maf_event->GetSender();

  if (sender == GSAxis[X] || sender == GSAxis[Y] || sender == GSAxis[Z] || sender == GSIsotropic)
  {
    OnEventGizmoComponents(maf_event); // process events from gizmo components
  }
  else if (sender == GuiGizmoScale)
  {
    OnEventGizmoGui(maf_event); // process events from the gui
  }
  else
  {
    mafEventMacro(*maf_event);
  }
}

//----------------------------------------------------------------------------
void mafGizmoScale::OnEventGizmoComponents(mafEventBase *maf_event)
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
          if (sender == GSAxis[X])
          {
            this->Highlight(X);
            ActiveGizmoComponent = X_AXIS;
          }
          else if (sender == GSAxis[Y])
          {
            this->Highlight(Y);
            ActiveGizmoComponent = Y_AXIS;
          }
          else if (sender == GSAxis[Z])
          {
            this->Highlight(Z);
            ActiveGizmoComponent = Z_AXIS;
          }
          else if (sender == GSIsotropic)
          {
            this->Highlight(ISOTROPIC);
            ActiveGizmoComponent = ISOTROPIC;
          }

          // Store initial gizmo pose
          InitialGizmoPose->DeepCopy(GSIsotropic->GetAbsPose());


          /* At MOUSE_DOWN
          1) Express VME matrix in RS refsys via mafTransform
          mflTr->SetInput(VME->GetAbsMatrix)
          mflTr->SetSourceRefSys(World ie Identity)
          mflTr->SetTargetRefSys(RS)

          VME_RS = mflTr->GetTransform();
          */

          // Express VME abs matrix in RefSysVME refsys via mafTransform
          mafSmartPointer<mafTransformFrame> tr;
          tr->SetInput(InputVME->GetOutput()->GetAbsMatrix());
          tr->SetTargetFrame(RefSysVME->GetOutput()->GetAbsMatrix());

          // update private ivar 
          VmeMatrixRelativeToRefSysVME->SetTimeStamp(InputVME->GetTimeStamp());
          VmeMatrixRelativeToRefSysVME->DeepCopy(&tr->GetMatrix());

          RefSysVMEAbsMatrixAtMouseDown->DeepCopy(RefSysVME->GetOutput()->GetAbsMatrix());
        }
        else if (arg == mmiGenericMouse::MOUSE_MOVE)
        {               
          if (ActiveGizmoComponent == X_AXIS || ActiveGizmoComponent == Y_AXIS || ActiveGizmoComponent == Z_AXIS)
          {
            // matrix holding abs pose after mouse move event
            mafMatrix newAbsMatr;

            // gizmo working in global mode; only one axis/plane is moving in a single mouse move event

            // forward to active axis gizmo              
            vtkTransform *currTr = vtkTransform::New();
            currTr->PostMultiply();
            currTr->SetMatrix(GSAxis[ActiveGizmoComponent]->GetAbsPose()->GetVTKMatrix());
            currTr->Concatenate(e->GetMatrix()->GetVTKMatrix());
            currTr->Update();

            newAbsMatr.DeepCopy(currTr->GetMatrix());
            newAbsMatr.SetTimeStamp(GetAbsPose()->GetTimeStamp());

            // set the new pose to the gizmo
            GSAxis[ActiveGizmoComponent]->SetAbsPose(&newAbsMatr);

            currTr->Delete();
          }
          else if (ActiveGizmoComponent == ISOTROPIC)
          {
            // get the translation value
            double translationValue = e->GetMatrix()->GetElement(0,3);

            // build translation matrices to premultiply to the three gizmos
            mafSmartPointer<mafTransform> gizmoTr[3];
            gizmoTr[0].GetPointer()->Translate(translationValue, 0, 0,PRE_MULTIPLY);
            gizmoTr[1].GetPointer()->Translate(0, translationValue, 0,PRE_MULTIPLY);
            gizmoTr[2].GetPointer()->Translate(0, 0, translationValue,PRE_MULTIPLY);

            // premultiply translation matrix to each gizmo abs pose
            for (int gizmoId = X_AXIS; gizmoId < ISOTROPIC; gizmoId++)
            {        
              mafSmartPointer<mafTransform> currTr;
              currTr.GetPointer()->SetMatrix(*GSAxis[gizmoId]->GetAbsPose());
              currTr.GetPointer()->Concatenate(gizmoTr[gizmoId].GetPointer()->GetMatrix(),PRE_MULTIPLY);
              currTr.GetPointer()->Update();

              mafMatrix newAbsMatr;
              newAbsMatr.DeepCopy(currTr.GetPointer()->GetMatrixPointer());
              newAbsMatr.SetTimeStamp(GetAbsPose()->GetTimeStamp());

              // set the new pose to the gizmo
              GSAxis[gizmoId]->SetAbsPose(&newAbsMatr);
            }
          }
          ////////////////////////////////////////
          // update the scaling value
          double scale = GetScalingValue();
          ////////////////////////////////////////

          /*----------------------------------------------------------------------------
          // Build matrix to be set to InputVME given the scale matrix S
          // to be applied relative to RS refsys to the vme
          //----------------------------------------------------------------------------


          ^   
          ^      |
          |      |s
          |       --->
          ^             --->  VME 
          |            RS
          |
          --->
          W

          */        
          //        // At MOUSE_MOVE
          //        1) Generate scale matrix        
          //        2) Postmultiply scale matrix to VME_RS matrix
          //            vtkTr *tr;
          //            tr->PostMultiply()
          //            tr->SetMatrix(VME_RS)
          //            tr->Concatenate(S)

          vtkMAFSmartPointer<vtkTransform> scaleTrans;
          scaleTrans->PostMultiply();
          scaleTrans->SetMatrix(VmeMatrixRelativeToRefSysVME->GetVTKMatrix());

          vtkMAFSmartPointer<vtkTransform> absScaleTrans;
          if (ActiveGizmoComponent == X_AXIS)
          {
            absScaleTrans->Scale(scale, 1, 1); 
          }
          else if (ActiveGizmoComponent == Y_AXIS)
          {
            absScaleTrans->Scale(1, scale, 1);
          }
          else if (ActiveGizmoComponent == Z_AXIS)
          {
            absScaleTrans->Scale (1, 1, scale);
          }
          else if (ActiveGizmoComponent == ISOTROPIC)
          {
            absScaleTrans->Scale(scale, scale, scale);
          }

          scaleTrans->Concatenate(absScaleTrans);

          //        3) Express S * VME_RS in W refsys: this is new vme abs pose
          //      
          //            mflTr->SetInput(S_x_VME_RS)
          //            mflTr->SetSourceRefSys(RS)
          //            mflTr->SetTargetRS(W)
          //
          //            NEW_VME_ABS_pose = mflTr->GetTransform();
          //  

          mafMatrix scaleTransMatrix;
          scaleTransMatrix.DeepCopy(scaleTrans.GetPointer()->GetMatrix());

          mafSmartPointer<mafTransformFrame> newVmeAbsPoseTr;
          newVmeAbsPoseTr.GetPointer()->SetInput(&scaleTransMatrix);
          newVmeAbsPoseTr.GetPointer()->SetInputFrame(RefSysVMEAbsMatrixAtMouseDown);

          // Set VME Pose
          InputVME->SetAbsMatrix(newVmeAbsPoseTr.GetPointer()->GetMatrix());

          // notify the vme about changed vme abs pose due to scaling; also send new vme bs matrix
          mafEvent e2s;
          e2s.SetSender(this);
          e2s.SetMatrix((newVmeAbsPoseTr.GetPointer())->GetMatrixPointer());
          e2s.SetId(ID_TRANSFORM);
          mafEventMacro(e2s);

          // Update scale gizmo gui
          mafMatrix scaleMat;
          scaleMat.DeepCopy(absScaleTrans->GetMatrix());

          GuiGizmoScale->SetAbsScaling(&scaleMat);
        }
        else if (arg == mmiGenericMouse::MOUSE_UP)
        {
          // put the gizmo back in the initial pose  
          SetAbsPose(InitialGizmoPose);

          // Update scale gizmo gui 
          mafMatrix identity;
          identity.Identity();
          GuiGizmoScale->SetAbsScaling(&identity);
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
void mafGizmoScale::OnEventGizmoGui(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  switch(maf_event->GetId())
  {
    // process events from the gui   
    case (mafGuiGizmoScale::ID_SCALE_X):
    {
      // receiving abs scaling from gui
      SendTransformMatrixFromGui(maf_event);
    }
    break;
    case (mafGuiGizmoScale::ID_SCALE_Y):
    {
      SendTransformMatrixFromGui(maf_event);
    }
    break;
    case (mafGuiGizmoScale::ID_SCALE_Z):
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
void mafGizmoScale::Highlight (int component) 
//----------------------------------------------------------------------------
{
  if (X_AXIS <= component && component < ISOTROPIC)
  {
   GSIsotropic->Highlight(false);
   for (int i = 0; i < 3; i++)
   {
     if (i != component)
     {
       GSAxis[i]->Highlight(false);
     }
   }

   GSAxis[component]->Highlight(true);
 
  }       
  else if (component == ISOTROPIC)
  {
    for (int i = X_AXIS; i < 3; i++)
    {
      GSAxis[i]->Highlight(false);
    }
    GSIsotropic->Highlight(true);
  }
  else if (component == NONE)
  {
    for (int i = 0; i < 3; i++)
    {
     // DeHighlight everything;
     GSAxis[i]->Highlight(false);
    }
    GSIsotropic->Highlight(false);
  }
}

//----------------------------------------------------------------------------  
void mafGizmoScale::Show(bool show)
//----------------------------------------------------------------------------
{
  // set visibility ivar
  Visibility = show;

  for (int i = 0; i < 3; i++)
  {
    GSAxis[i]->Show(show);
  }

  GSIsotropic->Show(show);

  // Gizmo scale text entry is not keyable to not confuse the user;
  /*

  // if auxiliary ref sys is different from vme its scaling cannot be changed
  // so gui must not be keyable. Otherwise set gui keyability to show.

  if (RefSysVME == InputVME)
  {
    GuiGizmoScale->EnableWidgets(show);
  }
  else
  {
    GuiGizmoScale->EnableWidgets(false);
  }
  */
  // update the camera
  mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}

//----------------------------------------------------------------------------
mafMatrix *mafGizmoScale::GetAbsPose()
//----------------------------------------------------------------------------
{
  return GSAxis[0]->GetAbsPose();
}

//----------------------------------------------------------------------------  
void mafGizmoScale::SetInput(mafVME *input)
//----------------------------------------------------------------------------
{
  this->InputVME = input;
  for (int i = 0; i < 3; i++)
  {
    GSAxis[i]->SetInput(input);
  }
  GSIsotropic->SetInput(input);
}

//----------------------------------------------------------------------------
void mafGizmoScale::SendTransformMatrixFromGui(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  // Gizmo scale matrix is not keyable for the moment...

/*
  // send matrix to be postmultiplied to listener
  //                                                                  -1    
  // [NewAbsPose] = [M]*[OldAbsPose] => [M] = [NewAbsPose][OldAbsPose]

  // build objects
  vtkMatrix4x4 *M = vtkMatrix4x4::New();
  vtkMatrix4x4 *invOldAbsPose = vtkMatrix4x4::New();
  vtkMatrix4x4 *newAbsPose = vtkMatrix4x4::New();

  // position from abs pose
  double position[3];
  mafTransform::GetPosition(this->GetAbsPose(), position);

  // orientation from abs pose
  double orientation[3];
  mafTransform::GetOrientation(this->GetAbsPose(), orientation);

  // incoming matrix is an abs scaling matrix
  newAbsPose->DeepCopy(e->GetMatrix()); // abs scaling from gui

  mafTransform::SetOrientation(newAbsPose, orientation);
  mafTransform::SetPosition(newAbsPose, position);

  // premultiply the scaling matrix coming from gui
  vtkMAFSmartPointer<vtkTransform> tr;
  tr->SetMatrix(newAbsPose);
  tr->Concatenate(e->GetMatrix());

  newAbsPose->DeepCopy(tr->GetMatrix());

  invOldAbsPose->DeepCopy(this->GetAbsPose());
  invOldAbsPose->Invert();

  vtkMatrix4x4::Multiply4x4(newAbsPose, invOldAbsPose, M);

  // update gizmo abs pose
  this->SetAbsPose(newAbsPose, InputVME->GetTimeStamp());

  // update input vme abs pose
  InputVME->SetAbsPose(newAbsPose);

  // notify the listener about changed vme pose
  SendTransformMatrix(M, ID_TRANSFORM, mmiGenericMouse::MOUSE_MOVE);   

  // clean up
  M->Delete();
  invOldAbsPose->Delete();
  newAbsPose->Delete();*/
}

//----------------------------------------------------------------------------  
void mafGizmoScale::SetAbsPose(mafMatrix *absPose, mafTimeStamp ts)
//----------------------------------------------------------------------------
{
  mafMatrix tmpMatr;
  tmpMatr.DeepCopy(absPose);
  tmpMatr.SetTimeStamp(ts);
  
  // remove scaling part from gizmo abs pose; gizmo does not scale
//  double pos[3] = {0,0,0};
//  double orient[3] = {0,0,0};
 
//  mafTransform::GetPosition(absPose, pos);
//  mafTransform::GetOrientation(absPose, orient);

//  mafTransform::SetPosition(tmpMatr, pos);
//  mafTransform::SetOrientation(tmpMatr, orient);

  for (int i = 0; i < 3; i++)
  {
    GSAxis[i]->SetAbsPose(&tmpMatr);
  }

  GSIsotropic->SetAbsPose(&tmpMatr);
  GuiGizmoScale->SetAbsScaling(&tmpMatr);
}

//----------------------------------------------------------------------------
void mafGizmoScale::SetRefSys(mafVME *refSys)
//----------------------------------------------------------------------------
{
  assert(InputVME);  
  assert(GuiGizmoScale);

  RefSysVME = refSys;
  SetAbsPose(RefSysVME->GetOutput()->GetAbsMatrix());

  // GuiGizmoScale not keyable for the moment to not confuse the user  
  /*

  if (RefSysVME == InputVME)
  {
    // if the gizmo is visible set the widgets visibility to true
    // if the refsys is local
    if (Visibility == true)
    {
      GuiGizmoScale->EnableWidgets(true);
    }
  }
  else
  {
    // if the gizmo is visible set the widgets visibility to false
    // if the refsys is global since this refsys cannot be changed
    if (Visibility == true)
    {
      GuiGizmoScale->EnableWidgets(false);
    }
  }
  */
}

//----------------------------------------------------------------------------
double mafGizmoScale::GetScalingValue() const
//----------------------------------------------------------------------------
{
  int gizmoId = X_AXIS;
  if (X_AXIS <= ActiveGizmoComponent && ActiveGizmoComponent <= Z_AXIS)
  {
    gizmoId = ActiveGizmoComponent; 
  }
  else if (ActiveGizmoComponent == ISOTROPIC)
  {
    gizmoId = X_AXIS;
  }

  // get the current refsys versor
  double refSysVersor[3] = {0, 0, 0};
  mafTransform::GetVersor(gizmoId, *RefSysVME->GetOutput()->GetAbsMatrix(), refSysVersor);

  double gizmoLength = GSAxis[gizmoId]->GetCubeLength() + GSAxis[gizmoId]->GetCylinderLength();
  double dist = 0;
  double scale = 1;
  double p0[3] = {0, 0, 0};
  double p1[3] = {0, 0, 0};
  double p0p1[3] = {0, 0, 0};
 
  mafTransform::GetPosition(*InitialGizmoPose, p0);
  mafTransform::GetPosition(*GSAxis[gizmoId]->GetAbsPose(), p1);
  dist = sqrt(vtkMath::Distance2BetweenPoints(p0, p1));
  
  this->BuildVector(p0, p1, p0p1);
  
  // get the dist sign
  double sign = vtkMath::Dot(p0p1, refSysVersor) > 0 ? 1 : -1;
  dist *= sign;

  // scale to be applied
  scale =  fabs(1 + dist / gizmoLength);
  return scale;
}
