/*=========================================================================

 Program: MAF2
 Module: mafGizmoScale
 Authors: Stefano Perticoni
 
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

// maf includes
#include "mafMatrix.h"
#include "mafTransform.h"
#include "mafTransformFrame.h"
#include "mafVME.h"
#include "mafVMEOutput.h"
#include "mafGizmoScale.h"
#include "mafDecl.h"
#include "mafGizmoScaleAxis.h"
#include "mafGizmoScaleIsotropic.h"
#include "mafSmartPointer.h"
#include "mafInteractorGenericMouse.h"

// vtk includes
#include "vtkMAFSmartPointer.h"
#include "vtkTransform.h"
#include "vtkMath.h"

//----------------------------------------------------------------------------
mafGizmoScale::mafGizmoScale(mafVME* input, mafObserver *listener , bool buildGUI)
//----------------------------------------------------------------------------
{
  assert(input);

  m_InputVME = input;
  m_Listener = listener;
  m_BuildGUI = buildGUI;

  m_GSAxis[X] = m_GSAxis[Y] = m_GSAxis[Z] = NULL;
  m_GSIsotropic = NULL;
  
  //no gizmo component is active at construction ie no yellow or highlighted component at startup
  this->m_ActiveGizmoComponent = NONE;

  // default modality to local ie the gizmo is moving along with the input vme
  this->SetModalityToLocal();

  mafNEW(m_InitialGizmoPose);

  // initial gizmo pose is the input vme abs matrix
  m_InitialGizmoPose->DeepCopy(m_InputVME->GetOutput()->GetAbsMatrix());

  mafNEW(m_VmeMatrixRelativeToRefSysVME);
  mafNEW(m_RefSysVMEAbsMatrixAtMouseDown);

  // build the three scale gizmos, one for each axis
  for (int i = 0; i < 3; i++)
  {
    // Create mafGizmoScaleAxis and send events to this
    m_GSAxis[i] = new mafGizmoScaleAxis(input, this);
	  m_GSAxis[i]->SetAxis(i);
  }
  
  // build the anisotropic scale gizmo
  m_GSIsotropic = new mafGizmoScaleIsotropic(input, this);

  m_GuiGizmoScale = NULL;

  // should we build the gui?
  if (m_BuildGUI)
  {
    // create the gizmo gui
    // gui is sending events to this
    m_GuiGizmoScale = new mafGUIGizmoScale(this);

    // initialize gizmo gui
    m_GuiGizmoScale->SetAbsScaling(m_InputVME->GetOutput()->GetAbsMatrix());
    m_GuiGizmoScale->EnableWidgets(true);
  }

  // this gizmo will autoscale by default
  this->SetAutoscale(true);

  // and will be on the superimposed layer
  this->SetAlwaysVisible(true);
}
//----------------------------------------------------------------------------
mafGizmoScale::~mafGizmoScale() 
//----------------------------------------------------------------------------
{
  //Destroy:
  //3 gizmo scale axis 
  for (int i = 0; i < 3; i++)
  {
    cppDEL(m_GSAxis[i]);
  }
  // and 1 gizmo scale isotropic
  cppDEL(m_GSIsotropic);
  
  mafDEL(m_InitialGizmoPose);
  mafDEL(m_VmeMatrixRelativeToRefSysVME);
  mafDEL(m_RefSysVMEAbsMatrixAtMouseDown);
  cppDEL(m_GuiGizmoScale);
}
//----------------------------------------------------------------------------
void mafGizmoScale::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  // get the event sender
  void *sender = maf_event->GetSender();

  if (sender == m_GSAxis[X] || sender == m_GSAxis[Y] || sender == m_GSAxis[Z] || sender == m_GSIsotropic)
  {
    // process events from gizmo components
    OnEventGizmoComponents(maf_event); 
  }
  else if (sender == m_GuiGizmoScale)
  {
    // process events from the gui
    OnEventGizmoGui(maf_event); 
  }
  else
  {
    // otherwise send to the listener
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
        if (arg == mafInteractorGenericMouse::MOUSE_DOWN)
        {
          if (sender == m_GSAxis[X])
          {
		    // highlight the gizmo ie make it yellow 
            this->Highlight(X);
			// register the active gizmo component
            m_ActiveGizmoComponent = X_AXIS;
          }
          else if (sender == m_GSAxis[Y])
          {
		    // highlight the gizmo ie make it yellow 
            this->Highlight(Y);
			// register the active gizmo component
            m_ActiveGizmoComponent = Y_AXIS;
          }
          else if (sender == m_GSAxis[Z])
          {
		    // highlight the gizmo ie make it yellow 
            this->Highlight(Z);
			// register the active gizmo component
            m_ActiveGizmoComponent = Z_AXIS;
          }
          else if (sender == m_GSIsotropic)
          {
		    // highlight the gizmo ie make it yellow 
            this->Highlight(ISOTROPIC);
			// register the active gizmo component
            m_ActiveGizmoComponent = ISOTROPIC;
          }

          // Store initial gizmo pose
          m_InitialGizmoPose->DeepCopy(m_GSIsotropic->GetAbsPose());

          /* At MOUSE_DOWN
          1) Express VME matrix in RS refsys via mafTransform
          mflTr->SetInput(VME->GetAbsMatrix)
          mflTr->SetSourceRefSys(World ie Identity)
          mflTr->SetTargetRefSys(RS)

          VME_RS = mflTr->GetTransform();
          */

          // Express VME abs matrix in RefSysVME refsys via mafTransform
          mafSmartPointer<mafTransformFrame> tr;
          tr->SetInput(m_InputVME->GetOutput()->GetAbsMatrix());
          tr->SetTargetFrame(m_RefSysVME->GetOutput()->GetAbsMatrix());

          // update private ivar 
          m_VmeMatrixRelativeToRefSysVME->SetTimeStamp(m_InputVME->GetTimeStamp());
          m_VmeMatrixRelativeToRefSysVME->DeepCopy(&tr->GetMatrix());

          m_RefSysVMEAbsMatrixAtMouseDown->DeepCopy(m_RefSysVME->GetOutput()->GetAbsMatrix());
        }
        else if (arg == mafInteractorGenericMouse::MOUSE_MOVE)
        {               
          if (m_ActiveGizmoComponent == X_AXIS || m_ActiveGizmoComponent == Y_AXIS || m_ActiveGizmoComponent == Z_AXIS)
          {
            // matrix holding abs pose after mouse move event
            mafSmartPointer<mafMatrix> newAbsMatr;

            // gizmo working in global mode; only one axis/plane is moving in a single mouse move event

            // forward to active axis gizmo              
            vtkTransform *currTr = vtkTransform::New();
            currTr->PostMultiply();
            currTr->SetMatrix(m_GSAxis[m_ActiveGizmoComponent]->GetAbsPose()->GetVTKMatrix());
            currTr->Concatenate(e->GetMatrix()->GetVTKMatrix());
            currTr->Update();

            newAbsMatr->DeepCopy(currTr->GetMatrix());
            newAbsMatr->SetTimeStamp(GetAbsPose()->GetTimeStamp());

            // set the new pose to the gizmo
            m_GSAxis[m_ActiveGizmoComponent]->SetAbsPose(newAbsMatr);

            currTr->Delete();
          }
          else if (m_ActiveGizmoComponent == ISOTROPIC)
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
              currTr.GetPointer()->SetMatrix(*m_GSAxis[gizmoId]->GetAbsPose());
              currTr.GetPointer()->Concatenate(gizmoTr[gizmoId].GetPointer()->GetMatrix(),PRE_MULTIPLY);
              currTr.GetPointer()->Update();

              mafSmartPointer<mafMatrix> newAbsMatr;
              newAbsMatr->DeepCopy(currTr.GetPointer()->GetMatrixPointer());
              newAbsMatr->SetTimeStamp(GetAbsPose()->GetTimeStamp());

              // set the new pose to the gizmo
              m_GSAxis[gizmoId]->SetAbsPose(newAbsMatr);
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
          //            transform->PostMultiply()
          //            transform->SetMatrix(VME_RS)
          //            transform->Concatenate(S)

          vtkMAFSmartPointer<vtkTransform> scaleTrans;
          scaleTrans->PostMultiply();
          scaleTrans->SetMatrix(m_VmeMatrixRelativeToRefSysVME->GetVTKMatrix());

          vtkMAFSmartPointer<vtkTransform> absScaleTrans;
          if (m_ActiveGizmoComponent == X_AXIS)
          {
            absScaleTrans->Scale(scale, 1, 1); 
          }
          else if (m_ActiveGizmoComponent == Y_AXIS)
          {
            absScaleTrans->Scale(1, scale, 1);
          }
          else if (m_ActiveGizmoComponent == Z_AXIS)
          {
            absScaleTrans->Scale (1, 1, scale);
          }
          else if (m_ActiveGizmoComponent == ISOTROPIC)
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

          mafSmartPointer<mafMatrix> scaleTransMatrix;
          scaleTransMatrix->DeepCopy(scaleTrans.GetPointer()->GetMatrix());

          mafSmartPointer<mafTransformFrame> newVmeAbsPoseTr;
          newVmeAbsPoseTr.GetPointer()->SetInput(scaleTransMatrix);
          newVmeAbsPoseTr.GetPointer()->SetInputFrame(m_RefSysVMEAbsMatrixAtMouseDown);

          // Set VME Pose
          m_InputVME->SetAbsMatrix(newVmeAbsPoseTr.GetPointer()->GetMatrix());

          // notify the vme about changed vme abs pose due to scaling; also send new vme bs matrix
          mafEvent e2s;
          e2s.SetSender(this);
          e2s.SetMatrix((newVmeAbsPoseTr.GetPointer())->GetMatrixPointer());
          e2s.SetId(ID_TRANSFORM);
          mafEventMacro(e2s);

          // Update scale gizmo gui
          mafSmartPointer<mafMatrix> scaleMat;
          scaleMat->DeepCopy(absScaleTrans->GetMatrix());

          if (m_BuildGUI) m_GuiGizmoScale->SetAbsScaling(scaleMat);
        }
        else if (arg == mafInteractorGenericMouse::MOUSE_UP)
        {
          // put the gizmo back in the initial pose  
          SetAbsPose(m_InitialGizmoPose);

          // Update scale gizmo gui 
          mafSmartPointer<mafMatrix> identity;
          identity->Identity();
          if (m_BuildGUI) m_GuiGizmoScale->SetAbsScaling(identity);
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
    case (mafGUIGizmoScale::ID_SCALE_X):
    {
      // receiving abs scaling from gui
      SendTransformMatrixFromGui(maf_event);
    }
    break;
    case (mafGUIGizmoScale::ID_SCALE_Y):
    {
      SendTransformMatrixFromGui(maf_event);
    }
    break;
    case (mafGUIGizmoScale::ID_SCALE_Z):
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
   m_GSIsotropic->Highlight(false);
   for (int i = 0; i < 3; i++)
   {
     if (i != component)
     {
       m_GSAxis[i]->Highlight(false);
     }
   }

   m_GSAxis[component]->Highlight(true);
 
  }       
  else if (component == ISOTROPIC)
  {
    for (int i = X_AXIS; i < 3; i++)
    {
      m_GSAxis[i]->Highlight(false);
    }
    m_GSIsotropic->Highlight(true);
  }
  else if (component == NONE)
  {
    for (int i = 0; i < 3; i++)
    {
     // DeHighlight everything;
     m_GSAxis[i]->Highlight(false);
    }
    m_GSIsotropic->Highlight(false);
  }
}

//----------------------------------------------------------------------------  
void mafGizmoScale::Show(bool show)
//----------------------------------------------------------------------------
{
  // set visibility ivar
  m_Visibility = show;

  for (int i = 0; i < 3; i++)
  {
    m_GSAxis[i]->Show(show);
  }

  m_GSIsotropic->Show(show);

  // Gizmo scale text entry is not keyable to not confuse the user;
  /*

  // if auxiliary ref sys is different from vme its scaling cannot be changed
  // so gui must not be keyable. Otherwise set gui keyability to show.

  if (RefSysVME == InputVME)
  {
    m_GuiGizmoScale->EnableWidgets(show);
  }
  else
  {
    m_GuiGizmoScale->EnableWidgets(false);
  }
  */
  // update the camera
//  mafEventMacro(mafEvent(this, CAMERA_UPDATE));   // Paolo 20-07-2005
}
//----------------------------------------------------------------------------  
void mafGizmoScale::Show(bool showX, bool showY, bool showZ, bool showIso)
//----------------------------------------------------------------------------
{
	m_GSAxis[mafGizmoScale::X_AXIS]->Show(showX);
	m_GSAxis[mafGizmoScale::Y_AXIS]->Show(showY);
	m_GSAxis[mafGizmoScale::Z_AXIS]->Show(showZ);

	m_GSIsotropic->Show(showIso);
}
//----------------------------------------------------------------------------
mafMatrix *mafGizmoScale::GetAbsPose()
//----------------------------------------------------------------------------
{
  return m_GSAxis[0]->GetAbsPose();
}

//----------------------------------------------------------------------------  
void mafGizmoScale::SetInput(mafVME *input)
//----------------------------------------------------------------------------
{
  this->m_InputVME = input;
  for (int i = 0; i < 3; i++)
  {
    m_GSAxis[i]->SetInput(input);
  }
  m_GSIsotropic->SetInput(input);
}

//----------------------------------------------------------------------------
void mafGizmoScale::SendTransformMatrixFromGui(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  // Gizmo scale matrix is not keyable for the moment...

  mafEvent *e = mafEvent::SafeDownCast(maf_event);
  // send matrix to be postmultiplied to listener
  //                                                                  -1    
  // [NewAbsPose] = [M]*[OldAbsPose] => [M] = [NewAbsPose][OldAbsPose]

  // build objects
  mafMatrix *M = mafMatrix::New();
  mafMatrix *invOldAbsPose = mafMatrix::New();
  mafMatrix *newAbsPose = mafMatrix::New();

  // position from abs pose
  double position[3];
  mafTransform::GetPosition(*this->GetAbsPose(), position);

  // orientation from abs pose
  double orientation[3];
  mafTransform::GetOrientation(*this->GetAbsPose(), orientation);

  // incoming matrix is an abs scaling matrix
  newAbsPose->DeepCopy(e->GetMatrix()); // abs scaling from gui

  mafTransform::SetOrientation(*newAbsPose, orientation);
  mafTransform::SetPosition(*newAbsPose, position);

  // premultiply the scaling matrix coming from gui
  vtkMAFSmartPointer<vtkTransform> tr;
  tr->SetMatrix(newAbsPose->GetVTKMatrix());
  tr->Concatenate(e->GetMatrix()->GetVTKMatrix());

  newAbsPose->DeepCopy(tr->GetMatrix());

  invOldAbsPose->DeepCopy(this->GetAbsPose());
  invOldAbsPose->Invert();

  mafMatrix::Multiply4x4(*newAbsPose, *m_InputVME->GetOutput()->GetAbsMatrix(), *M);
  // update gizmo abs pose
  //this->SetAbsPose(newAbsPose, InputVME->GetTimeStamp());

  // update input vme abs pose
  
	m_InputVME->SetAbsMatrix(*M, m_InputVME->GetTimeStamp());
  m_InputVME->Modified();

  // notify the listener about changed vme pose
  SendTransformMatrix(M, ID_TRANSFORM, mafInteractorGenericMouse::MOUSE_MOVE);   

  mafSmartPointer<mafMatrix> identity;
  identity->Identity();
  if (m_BuildGUI) m_GuiGizmoScale->SetAbsScaling(identity);


  // clean up
  M->Delete();
  invOldAbsPose->Delete();
  newAbsPose->Delete();
}

//----------------------------------------------------------------------------  
void mafGizmoScale::SetAbsPose(mafMatrix *absPose, mafTimeStamp ts)
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafMatrix> tmpMatr;
  tmpMatr->DeepCopy(absPose);
  tmpMatr->SetTimeStamp(ts);
  
  // remove scaling part from gizmo abs pose; gizmo does not scale
//  double pos[3] = {0,0,0};
//  double orient[3] = {0,0,0};
 
//  mafTransform::GetPosition(absPose, pos);
//  mafTransform::GetOrientation(absPose, orient);

//  mafTransform::SetPosition(tmpMatr, pos);
//  mafTransform::SetOrientation(tmpMatr, orient);

  for (int i = 0; i < 3; i++)
  {
    m_GSAxis[i]->SetAbsPose(tmpMatr);
  }

  m_GSIsotropic->SetAbsPose(tmpMatr);
  if (m_BuildGUI) m_GuiGizmoScale->SetAbsScaling(tmpMatr);
}

//----------------------------------------------------------------------------
void mafGizmoScale::SetRefSys(mafVME *refSys)
//----------------------------------------------------------------------------
{
  assert(m_InputVME);  
  
  m_RefSysVME = refSys;
  SetAbsPose(m_RefSysVME->GetOutput()->GetAbsMatrix());

  // m_GuiGizmoScale not keyable for the moment to not confuse the user  
  /*

  assert(m_GuiGizmoScale);

  if (RefSysVME == InputVME)
  {
    // if the gizmo is visible set the widgets visibility to true
    // if the refsys is local
    if (Visibility == true)
    {
      m_GuiGizmoScale->EnableWidgets(true);
    }
  }
  else
  {
    // if the gizmo is visible set the widgets visibility to false
    // if the refsys is global since this refsys cannot be changed
    if (Visibility == true)
    {
      m_GuiGizmoScale->EnableWidgets(false);
    }
  }
  */
}

//----------------------------------------------------------------------------
double mafGizmoScale::GetScalingValue() const
//----------------------------------------------------------------------------
{
  int gizmoId = X_AXIS;
  if (X_AXIS <= m_ActiveGizmoComponent && m_ActiveGizmoComponent <= Z_AXIS)
  {
    gizmoId = m_ActiveGizmoComponent; 
  }
  else if (m_ActiveGizmoComponent == ISOTROPIC)
  {
    gizmoId = X_AXIS;
  }

  // get the current refsys versor
  double refSysVersor[3] = {0, 0, 0};
  mafTransform::GetVersor(gizmoId, *m_RefSysVME->GetOutput()->GetAbsMatrix(), refSysVersor);

  double gizmoLength = m_GSAxis[gizmoId]->GetCubeLength() + m_GSAxis[gizmoId]->GetCylinderLength();
  double dist = 0;
  double scale = 1;
  double p0[3] = {0, 0, 0};
  double p1[3] = {0, 0, 0};
  double p0p1[3] = {0, 0, 0};
 
  mafTransform::GetPosition(*m_InitialGizmoPose, p0);
  mafTransform::GetPosition(*m_GSAxis[gizmoId]->GetAbsPose(), p1);
  dist = sqrt(vtkMath::Distance2BetweenPoints(p0, p1));
  
  this->BuildVector(p0, p1, p0p1);
  
  // get the dist sign
  double sign = vtkMath::Dot(p0p1, refSysVersor) > 0 ? 1 : -1;
  dist *= sign;

  // scale to be applied
  scale =  fabs(1 + dist / gizmoLength);
  return scale;
}

mafVME* mafGizmoScale::GetRefSys()
{
  return m_RefSysVME;
}

void mafGizmoScale::SetRenderWindowHeightPercentage(double percentage)
{
	mafGizmoInterface::SetRenderWindowHeightPercentage(percentage);

	for (int i = 0; i < 3; i++)
	{
		m_GSAxis[i]->SetRenderWindowHeightPercentage(percentage);
	}

	m_GSIsotropic->SetRenderWindowHeightPercentage(percentage);

}

void mafGizmoScale::SetAutoscale( bool autoscale )
{
	mafGizmoInterface::SetAutoscale(autoscale);

	for (int i = 0; i < 3; i++)
	{
		m_GSAxis[i]->SetAutoscale(autoscale);
	}

	m_GSIsotropic->SetAutoscale(autoscale);

}

void mafGizmoScale::SetAlwaysVisible( bool alwaysVisible )
{
	mafGizmoInterface::SetAlwaysVisible(alwaysVisible);

	for (int i = 0; i < 3; i++)
	{
		m_GSAxis[i]->SetAlwaysVisible(alwaysVisible);
	}

	m_GSIsotropic->SetAlwaysVisible(alwaysVisible);
}

