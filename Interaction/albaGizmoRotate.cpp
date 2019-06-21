/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGizmoRotate
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


#include "albaGizmoRotate.h"
#include "albaDecl.h"
#include "albaGizmoRotateCircle.h"
#include "albaGizmoRotateFan.h"
#include "albaGUIGizmoRotate.h"
#include "albaSmartPointer.h"

#include "albaInteractorGenericMouse.h"

#include "albaMatrix.h"
#include "albaTransform.h"
#include "albaVME.h"
#include "albaVMEOutput.h"

#include "vtkTransform.h"

//----------------------------------------------------------------------------
albaGizmoRotate::albaGizmoRotate(albaVME* input, albaObserver *listener, bool buildGUI)
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
    m_GRFan[i] = new albaGizmoRotateFan(input, this);
    m_GRFan[i]->SetAxis(i);
	m_GRFan[i]->SetMediator(this);

    // Create albaGizmoRotateCircle and send events to the corresponding fan
	
	albaString name("Circle");
	if(i == 0) name.Append("X");
	else if(i == 1) name.Append("Y");
	else if(i == 2) name.Append("Z");
    m_GRCircle[i] = new albaGizmoRotateCircle(input, m_GRFan[i], name);
	m_GRCircle[i]->SetAxis(i);
	m_GRCircle[i]->SetMediator(this);
  }

  SetAlwaysVisible(true);
  SetAutoscale(true);

  if (m_BuildGUI)
  {
    // create the gizmo gui
    // gui is sending events to this
    m_GuiGizmoRotate = new albaGUIGizmoRotate(this);

    // initialize gizmo gui
    m_GuiGizmoRotate->SetAbsOrientation(m_InputVME->GetOutput()->GetAbsMatrix());
  }
}
//----------------------------------------------------------------------------
albaGizmoRotate::~albaGizmoRotate() 
//----------------------------------------------------------------------------
{
  //Destroy:
  //3 albaGizmoRotateCircle 
  //1 albaGizmoRotateFan
  for (int i = 0; i < 3; i++)
  {
    cppDEL(m_GRCircle[i]);
    cppDEL(m_GRFan[i]);
  }
  cppDEL(m_GuiGizmoRotate);
}

//----------------------------------------------------------------------------
void albaGizmoRotate::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  // get the sender
  void *sender = alba_event->GetSender();
    
  if (sender == m_GRFan[X] || sender == m_GRFan[Y] || sender == m_GRFan[Z])
  {
    OnEventGizmoComponents(alba_event); // process events from fans
  }
  else if (sender == m_GuiGizmoRotate)
  {
    OnEventGizmoGui(alba_event); // process events from gui
  }
  else
  {
    // forward to the listener
    albaEventMacro(*alba_event);
  }
}

//----------------------------------------------------------------------------
void albaGizmoRotate::OnEventGizmoComponents(albaEventBase *alba_event) 
//----------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch (e->GetId())
    {
      //receiving pose matrices from the fan
      case ID_TRANSFORM:
      {
        void *sender = e->GetSender();
        long arg = e->GetArg();

        // if a gizmo circle has been picked
        if (arg == albaInteractorGenericMouse::MOUSE_DOWN)
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
        else if (arg == albaInteractorGenericMouse::MOUSE_MOVE)
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

            albaMatrix newAbsMatr;
            newAbsMatr.DeepCopy(currTr->GetMatrix());
            newAbsMatr.SetTimeStamp(GetAbsPose()->GetTimeStamp());

            // set the new pose to the gizmo
            SetAbsPose(&newAbsMatr, false);
            currTr->Delete();
          }
        }
        else if (arg == albaInteractorGenericMouse::MOUSE_UP)
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
        albaEventMacro(*e);
      }
      break;
      default:
      {
        albaEventMacro(*e);
      }
    }
  }
}

//----------------------------------------------------------------------------
void albaGizmoRotate::OnEventGizmoGui(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  // process events from the gui
  switch (alba_event->GetId())
  {
    case (albaGUIGizmoRotate::ID_ROTATE_X):
    {
      // receiving abs orientation from gui
      SendTransformMatrixFromGui(alba_event);
    }
    break;
    case (albaGUIGizmoRotate::ID_ROTATE_Y):
    {
      SendTransformMatrixFromGui(alba_event);     
    }
    break;
    case (albaGUIGizmoRotate::ID_ROTATE_Z):
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
void albaGizmoRotate::Highlight (int component) 
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
void albaGizmoRotate::Show(bool show)
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
void albaGizmoRotate::Show( bool showX, bool showY, bool showZ )
//----------------------------------------------------------------------------
{
  // set visibility ivar
  m_Visibility = showX || showY || showZ;

  m_GRCircle[albaGizmoRotate::X_AXIS]->Show(showX);
  m_GRFan[albaGizmoRotate::X_AXIS]->Show(showX);
  m_GRCircle[albaGizmoRotate::Y_AXIS]->Show(showY);
  m_GRFan[albaGizmoRotate::Y_AXIS]->Show(showY);
  m_GRCircle[albaGizmoRotate::Z_AXIS]->Show(showZ);
  m_GRFan[albaGizmoRotate::Z_AXIS]->Show(showZ);
}

//----------------------------------------------------------------------------  
void albaGizmoRotate::SetAbsPose(albaMatrix *absPose, bool applyPoseToFans)
//----------------------------------------------------------------------------
{
  // remove scaling part from gizmo abs pose; gizmo not scale
  double pos[3] = {0,0,0};
  double orient[3] = {0,0,0};
 
  albaTransform::GetPosition(*absPose, pos);
  albaTransform::GetOrientation(*absPose, orient);

  albaSmartPointer<albaMatrix> tmpMatr;
  tmpMatr->SetTimeStamp(absPose->GetTimeStamp());
  albaTransform::SetPosition(*tmpMatr.GetPointer(), pos);
  albaTransform::SetOrientation(*tmpMatr.GetPointer(), orient);

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
albaMatrix *albaGizmoRotate::GetAbsPose()
//----------------------------------------------------------------------------
{
  return m_GRCircle[0]->GetAbsPose();
}

//----------------------------------------------------------------------------  
void albaGizmoRotate::SetInput(albaVME *input)
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
albaInteractorGenericInterface *albaGizmoRotate::GetInteractor(int axis)
//----------------------------------------------------------------------------  
{
  return m_GRCircle[axis]->GetInteractor();
}

//----------------------------------------------------------------------------
void albaGizmoRotate::SendTransformMatrixFromGui(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    // send matrix to be postmultiplied to listener
    //                                                                  -1    
    // [NewAbsPose] = [M]*[OldAbsPose] => [M] = [NewAbsPose][OldAbsPose]

    // build objects
    albaSmartPointer<albaMatrix> M;
    albaMatrix invOldAbsPose;
    albaSmartPointer<albaMatrix> newAbsPose;

    // incoming matrix is a rotation matrix
    newAbsPose->DeepCopy(GetAbsPose());
    // copy rotation from incoming matrix
    albaTransform::CopyRotation(*e->GetMatrix(), *newAbsPose.GetPointer());

    invOldAbsPose.DeepCopy(this->GetAbsPose());
    invOldAbsPose.Invert();

    albaMatrix::Multiply4x4(*newAbsPose.GetPointer(),invOldAbsPose,*M.GetPointer());

    // update gizmo abs pose
    this->SetAbsPose(newAbsPose, true);

    // send transfrom to postmultiply to the listener. Events is sent as a transform event
    SendTransformMatrix(M, ID_TRANSFORM, albaInteractorGenericMouse::MOUSE_MOVE);
  }
}

//----------------------------------------------------------------------------
void albaGizmoRotate::SetRefSys(albaVME *refSys)
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
void albaGizmoRotate::SetCircleFanRadius(double radius)
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

albaVME* albaGizmoRotate::GetRefSys()
{
  return m_RefSysVME;
}

double albaGizmoRotate::GetCircleFanRadius()
{
  return m_CircleFanRadius;
}

void albaGizmoRotate::SetAutoscale( bool autoscale )
{
	albaGizmoInterface::SetAutoscale(autoscale);

	for (int i = 0; i < 3; i++)
	{
		m_GRFan[i]->SetAutoscale(autoscale);
		m_GRCircle[i]->SetAutoscale(autoscale);
	}
}

void albaGizmoRotate::SetAlwaysVisible( bool alwaysVisible )
{
	albaGizmoInterface::SetAlwaysVisible(alwaysVisible);

	for (int i = 0; i < 3; i++)
	{
		m_GRFan[i]->SetAlwaysVisible(alwaysVisible);
		m_GRCircle[i]->SetAlwaysVisible(alwaysVisible);
	}
}

void albaGizmoRotate::SetRenderWindowHeightPercentage(double percentage)
{
	for (int i = 0; i < 3; i++)
	{
		m_GRFan[i]->SetRenderWindowHeightPercentage(percentage);
		m_GRCircle[i]->SetRenderWindowHeightPercentage(percentage);
	}
}
