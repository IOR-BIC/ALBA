/*=========================================================================

 Program: MAF2
 Module: mafGizmoRotate
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


#include "mafGizmoRotate.h"
#include "mafDecl.h"
#include "mafGizmoRotateCircle.h"
#include "mafGizmoRotateFan.h"
#include "mafGUIGizmoRotate.h"
#include "mafSmartPointer.h"

#include "mafInteractorGenericMouse.h"

#include "mafMatrix.h"
#include "mafTransform.h"
#include "mafVME.h"
#include "mafVMEOutput.h"

#include "vtkTransform.h"

//----------------------------------------------------------------------------
mafGizmoRotate::mafGizmoRotate(mafVME* input, mafObserver *listener, bool buildGUI)
//----------------------------------------------------------------------------
{
  assert(input);
  m_BuildGUI = buildGUI;
  m_InputVME    = input;
  m_Listener  = listener;

  m_GRFan[X]    = m_GRFan[Y]    = m_GRFan[Z]    = NULL;
  m_GRCircle[X] = m_GRCircle[Y] = m_GRCircle[Z] = NULL;
  m_GuiGizmoRotate = NULL;
  m_CircleFanRadius = -1;

  for (int i = 0; i < 3; i++)
  {

    // create the fan and send events to this
    m_GRFan[i] = new mafGizmoRotateFan(input, this);
    m_GRFan[i]->SetAxis(i);
	m_GRFan[i]->SetMediator(this);

    // Create mafGizmoRotateCircle and send events to the corresponding fan
	
	mafString name("Circle");
	if(i == 0) name.Append("X");
	else if(i == 1) name.Append("Y");
	else if(i == 2) name.Append("Z");
    m_GRCircle[i] = new mafGizmoRotateCircle(input, m_GRFan[i], name);
	m_GRCircle[i]->SetAxis(i);
	m_GRCircle[i]->SetMediator(this);
  }

  SetAlwaysVisible(true);
  SetAutoscale(true);

  if (m_BuildGUI)
  {
    // create the gizmo gui
    // gui is sending events to this
    m_GuiGizmoRotate = new mafGUIGizmoRotate(this);

    // initialize gizmo gui
    m_GuiGizmoRotate->SetAbsOrientation(m_InputVME->GetOutput()->GetAbsMatrix());
  }
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
    cppDEL(m_GRCircle[i]);
    cppDEL(m_GRFan[i]);
  }
  cppDEL(m_GuiGizmoRotate);
}

//----------------------------------------------------------------------------
void mafGizmoRotate::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  // get the sender
  void *sender = maf_event->GetSender();
    
  if (sender == m_GRFan[X] || sender == m_GRFan[Y] || sender == m_GRFan[Z])
  {
    OnEventGizmoComponents(maf_event); // process events from fans
  }
  else if (sender == m_GuiGizmoRotate)
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
        if (arg == mafInteractorGenericMouse::MOUSE_DOWN)
        {
          if (sender == m_GRFan[X])
          {
            this->Highlight(X);
          }
          else if (sender == m_GRFan[Y])
          {
            this->Highlight(Y);
          }
          else if (sender == m_GRFan[Z])
          {
            this->Highlight(Z);
          }
        }  
        else if (arg == mafInteractorGenericMouse::MOUSE_MOVE)
        {
          // gizmo mode == local; gizmo is rotating during mouse move events
          if (m_Modality == G_LOCAL)
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
        else if (arg == mafInteractorGenericMouse::MOUSE_UP)
        {
          // gizmo mode == local
          if (m_Modality == G_LOCAL)
          {
            SetAbsPose(GetAbsPose());
          }
					this->Highlight(NONE);
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
    case (mafGUIGizmoRotate::ID_ROTATE_X):
    {
      // receiving abs orientation from gui
      SendTransformMatrixFromGui(maf_event);
    }
    break;
    case (mafGUIGizmoRotate::ID_ROTATE_Y):
    {
      SendTransformMatrixFromGui(maf_event);     
    }
    break;
    case (mafGUIGizmoRotate::ID_ROTATE_Z):
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
  for (int i = 0; i < NUM_COMPONENTS; i++)
  {
		bool hilight = (i == component);
    m_GRCircle[i]->Highlight(hilight);
    m_GRFan[i]->Show(hilight);
  }
}

//----------------------------------------------------------------------------  
void mafGizmoRotate::Show(bool show)
//----------------------------------------------------------------------------
{
  // set visibility ivar
  m_Visibility = show;

  for (int i = 0; i < 3; i++)
  {
    m_GRCircle[i]->Show(show);
    m_GRFan[i]->Show(show);
  }

  // if auxiliary ref sys is different from vme its orientation cannot be changed
  // so gui must not be keyable. Otherwise set gui keyability to show.
  if (m_BuildGUI)
  {
    if (m_RefSysVME == m_InputVME)
    {
      m_GuiGizmoRotate->EnableWidgets(show);
    }
    else
    {
      m_GuiGizmoRotate->EnableWidgets(false);
    }
  }
}
//----------------------------------------------------------------------------
void mafGizmoRotate::Show( bool showX, bool showY, bool showZ )
//----------------------------------------------------------------------------
{
  // set visibility ivar
  m_Visibility = showX || showY || showZ;

  m_GRCircle[mafGizmoRotate::X_AXIS]->Show(showX);
  m_GRFan[mafGizmoRotate::X_AXIS]->Show(showX);
  m_GRCircle[mafGizmoRotate::Y_AXIS]->Show(showY);
  m_GRFan[mafGizmoRotate::Y_AXIS]->Show(showY);
  m_GRCircle[mafGizmoRotate::Z_AXIS]->Show(showZ);
  m_GRFan[mafGizmoRotate::Z_AXIS]->Show(showZ);
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
    m_GRCircle[i]->SetAbsPose(tmpMatr);
    if (applyPoseToFans == true)
    {
      m_GRFan[i]->SetAbsPose(tmpMatr);
    }
  }
  if (m_BuildGUI) m_GuiGizmoRotate->SetAbsOrientation(tmpMatr);
}

//----------------------------------------------------------------------------
mafMatrix *mafGizmoRotate::GetAbsPose()
//----------------------------------------------------------------------------
{
  return m_GRCircle[0]->GetAbsPose();
}

//----------------------------------------------------------------------------  
void mafGizmoRotate::SetInput(mafVME *input)
//----------------------------------------------------------------------------
{
  m_InputVME = input;
  for (int i = 0; i < 3; i++)
  {
    m_GRCircle[i]->SetInput(input);
    m_GRFan[i]->SetInput(input);
  }
}

//----------------------------------------------------------------------------  
mafInteractorGenericInterface *mafGizmoRotate::GetInteractor(int axis)
//----------------------------------------------------------------------------  
{
  return m_GRCircle[axis]->GetInteractor();
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
    SendTransformMatrix(M, ID_TRANSFORM, mafInteractorGenericMouse::MOUSE_MOVE);
  }
}

//----------------------------------------------------------------------------
void mafGizmoRotate::SetRefSys(mafVME *refSys)
//----------------------------------------------------------------------------
{
  assert(m_InputVME);  
  
  m_RefSysVME = refSys;
  SetAbsPose(m_RefSysVME->GetOutput()->GetAbsMatrix());

  if (m_RefSysVME == m_InputVME)
  {
    SetModalityToLocal();

    // if the gizmo is visible set the widgets visibility to true
    // if the refsys is local
    if (m_Visibility == true && m_BuildGUI == true)
    {
      m_GuiGizmoRotate->EnableWidgets(true);
    }
  }
  else
  {
    SetModalityToGlobal();    

    // if the gizmo is visible set the widgets visibility to false
    // if the refsys is global since this refsys cannot be changed
    if (m_Visibility == true && m_BuildGUI == true)
    {
      m_GuiGizmoRotate->EnableWidgets(false);
    }
  }
}
//----------------------------------------------------------------------------
void mafGizmoRotate::SetCircleFanRadius(double radius)
//----------------------------------------------------------------------------
{
  short circleNumber;
  for(circleNumber = 0; circleNumber < 3; circleNumber++)
  {
    if(m_GRCircle[circleNumber]) m_GRCircle[circleNumber]->SetRadius(radius);
    if(m_GRFan[circleNumber]) m_GRFan[circleNumber]->SetRadius(radius);
  }

  m_CircleFanRadius = radius;
}

mafVME* mafGizmoRotate::GetRefSys()
{
  return m_RefSysVME;
}

double mafGizmoRotate::GetCircleFanRadius()
{
  return m_CircleFanRadius;
}

void mafGizmoRotate::SetAutoscale( bool autoscale )
{
	mafGizmoInterface::SetAutoscale(autoscale);

	for (int i = 0; i < 3; i++)
	{
		m_GRFan[i]->SetAutoscale(autoscale);
		m_GRCircle[i]->SetAutoscale(autoscale);
	}
}

void mafGizmoRotate::SetAlwaysVisible( bool alwaysVisible )
{
	mafGizmoInterface::SetAlwaysVisible(alwaysVisible);

	for (int i = 0; i < 3; i++)
	{
		m_GRFan[i]->SetAlwaysVisible(alwaysVisible);
		m_GRCircle[i]->SetAlwaysVisible(alwaysVisible);
	}
}

void mafGizmoRotate::SetRenderWindowHeightPercentage(double percentage)
{
	for (int i = 0; i < 3; i++)
	{
		m_GRFan[i]->SetRenderWindowHeightPercentage(percentage);
		m_GRCircle[i]->SetRenderWindowHeightPercentage(percentage);
	}
}
