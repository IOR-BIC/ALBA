/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmiCameraMove.cpp,v $
  Language:  C++
  Date:      $Date: 2005-05-03 15:42:35 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani & Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mmiCameraMove.h"
#include "mmdMouse.h"
#include "mafAvatar3D.h"
#include "mafInteractor.h"

#include "mafEventInteraction.h"
#include "mafDefines.h"
#include "vtkMath.h"
#include "vtkRendererCollection.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkCamera.h"
#include "vtkTransform.h"

#include <assert.h>

//------------------------------------------------------------------------------
mafCxxTypeMacro(mmiCameraMove)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mmiCameraMove::mmiCameraMove()
//------------------------------------------------------------------------------
{
  m_MotionFactor   = 10.0;
  m_State = MOUSE_CAMERA_NONE;

  m_StartButton = -1;
  m_InteractionFlag = 0;
  m_CurrentCamera = NULL;
  m_MousePose[0] = m_MousePose[1] = 0;
}

//------------------------------------------------------------------------------
mmiCameraMove::~mmiCameraMove()
//------------------------------------------------------------------------------
{
}

//------------------------------------------------------------------------------
void mmiCameraMove::OnEvent(mafEventBase *event)
//------------------------------------------------------------------------------
{
  assert(event);
  assert(event->GetSender());
  
  mafID id=event->GetId();
  mafID channel=event->GetChannel();

  if (channel == mafDevice::DeviceInputChannel && m_InteractionFlag)
  {
    mafEventInteraction *e = (mafEventInteraction *)event;
    mmdMouse *mouse = mmdMouse::SafeDownCast(GetDevice());
    
    // if the event comes from tracker which started the interaction continue...
    if (id == mmdMouse::Move2DEvent && mouse)
    {
      if (!m_CurrentCamera)
        return;

	    double pos2d[2];
      e->Get2DPosition(pos2d);
      m_MousePose[0] = (int)pos2d[0];
      m_MousePose[1] = (int)pos2d[1];
      
      OnMouseMove();

      Lastm_MousePose[0] = m_MousePose[0];
      Lastm_MousePose[1] = m_MousePose[1];
      
      return;

    }

  }
    
  Superclass::ProcessEvent(event,channel);
}
//------------------------------------------------------------------------------
int mmiCameraMove::StartInteraction(mmdMouse *mouse)
//------------------------------------------------------------------------------
{
  SetRenderer(mouse->GetRenderer());
  if (Renderer)
  {
    m_CurrentCamera = Renderer->GetActiveCamera();
    return true;
  }

  return false;
}

//----------------------------------------------------------------------------
void mmiCameraMove::OnButtonDown(mafEventInteraction *e)
//----------------------------------------------------------------------------
{
  ButtonPressed = e->GetButton();
  double pos[2];
  e->Get2DPosition(pos);
  Lastm_MousePose[0] = m_MousePose[0] = (int)pos[0];
  Lastm_MousePose[1] = m_MousePose[1] = (int)pos[1];

  mmdMouse *mouse = (mmdMouse *)e->GetSender();
  StartInteraction(mouse);

  switch(ButtonPressed) 
  {
    case MAF_LEFT_BUTTON:
      OnLeftButtonDown(e);
  	break;
    case MAF_MIDDLE_BUTTON:
      OnMiddleButtonDown();
  	break;
    case MAF_RIGHT_BUTTON:
      OnRightButtonDown();
  	break;
  }
}

//----------------------------------------------------------------------------
void mmiCameraMove::OnButtonUp(mafEventInteraction *e)
//----------------------------------------------------------------------------
{
  ButtonPressed = e->GetButton();
  
  switch(ButtonPressed) 
  {
    case MAF_LEFT_BUTTON:
      OnLeftButtonUp();
  	break;
    case MAF_MIDDLE_BUTTON:
      OnMiddleButtonUp();
  	break;
    case MAF_RIGHT_BUTTON:
      OnRightButtonUp();
  	break;
  }
}

//----------------------------------------------------------------------------
void mmiCameraMove::OnMouseMove() 
//----------------------------------------------------------------------------
{ 
  switch (this->m_State) 
  {
    case MOUSE_CAMERA_ROTATE:
      this->Rotate();
    break;
    case MOUSE_CAMERA_PAN:
      this->Pan();
    break;
    case MOUSE_CAMERA_DOLLY:
      this->Dolly();
    break;
    case MOUSE_CAMERA_SPIN:
      this->Spin();
    break;
  }
}
//----------------------------------------------------------------------------
void mmiCameraMove::OnLeftButtonDown(mafEventInteraction *e) 
//----------------------------------------------------------------------------
{ 
  if (e->GetModifier(MAF_SHIFT_KEY)) 
  {
    if (e->GetModifier(MAF_CTRL_KEY)) 
      this->StartDolly();
    else 
      this->StartPan();
  } 
  else 
  {
    if (e->GetModifier(MAF_CTRL_KEY)) 
      this->StartSpin();
    else 
      this->StartRotate();
  }
}
//----------------------------------------------------------------------------
void mmiCameraMove::OnMiddleButtonDown() 
//----------------------------------------------------------------------------
{
  this->StartPan();
}
//----------------------------------------------------------------------------
void mmiCameraMove::OnRightButtonDown() 
//----------------------------------------------------------------------------
{
  this->StartDolly();
}
//----------------------------------------------------------------------------
void mmiCameraMove::OnLeftButtonUp()
//----------------------------------------------------------------------------
{
  switch (this->m_State) 
    {
    case MOUSE_CAMERA_DOLLY:
      this->EndDolly();
      break;

    case MOUSE_CAMERA_PAN:
      this->EndPan();
      break;

    case MOUSE_CAMERA_SPIN:
      this->EndSpin();
      break;

    case MOUSE_CAMERA_ROTATE:
      this->EndRotate();
      break;
    }
}
//----------------------------------------------------------------------------
void mmiCameraMove::OnMiddleButtonUp()
//----------------------------------------------------------------------------
{
  switch (this->m_State) 
    {
    case MOUSE_CAMERA_PAN:
      this->EndPan();
      break;
    }
}
//----------------------------------------------------------------------------
void mmiCameraMove::OnRightButtonUp()
//----------------------------------------------------------------------------
{
  switch (this->m_State) 
    {
    case MOUSE_CAMERA_DOLLY:
      this->EndDolly();
      break;
    }
}
//----------------------------------------------------------------------------
void mmiCameraMove::Rotate()
//----------------------------------------------------------------------------
{
  if (Renderer == NULL)
    return;

	if (m_CurrentCamera->GetParallelProjection()) return; 

  int dx = m_MousePose[0] - Lastm_MousePose[0];
  int dy = m_MousePose[1] - Lastm_MousePose[1];
  
  int *size = Renderer->GetRenderWindow()->GetSize();

  float delta_elevation = -20.0 / size[1];
  float delta_azimuth = -20.0 / size[0];
  
  double rxf = (double)dx * delta_azimuth * this->m_MotionFactor;
  double ryf = (double)dy * delta_elevation * this->m_MotionFactor;
  
  m_CurrentCamera->Azimuth(rxf);
  m_CurrentCamera->Elevation(ryf);
  m_CurrentCamera->OrthogonalizeViewUp();

  ResetClippingRange();
  Renderer->GetRenderWindow()->Render();
}
//----------------------------------------------------------------------------
void mmiCameraMove::Spin()
//----------------------------------------------------------------------------
{
  if (Renderer == NULL)
    return;

	if (m_CurrentCamera->GetParallelProjection()) return; 

  float *center = Renderer->GetCenter();

  double newAngle = 
    atan2((double)m_MousePose[1] - (double)center[1],
          (double)m_MousePose[0] - (double)center[0]);

  double oldAngle = 
    atan2((double)Lastm_MousePose[1] - (double)center[1],
          (double)Lastm_MousePose[0] - (double)center[0]);
  
  newAngle *= vtkMath::RadiansToDegrees();
  oldAngle *= vtkMath::RadiansToDegrees();

  m_CurrentCamera->Roll(newAngle - oldAngle);
  m_CurrentCamera->OrthogonalizeViewUp();
      
  ResetClippingRange();
  Renderer->GetRenderWindow()->Render();
}
//----------------------------------------------------------------------------
void mmiCameraMove::Pan()
//----------------------------------------------------------------------------
{
  if (this->Renderer == NULL)
    return;

  double viewFocus[4], focalDepth, viewPoint[3];
  float newPickPoint[4], oldPickPoint[4], motionVector[3];
  
  // Calculate the focal depth since we'll be using it a lot

  m_CurrentCamera->GetFocalPoint(viewFocus);
  ComputeWorldToDisplay(viewFocus[0], viewFocus[1], viewFocus[2], viewFocus);
  focalDepth = viewFocus[2];

  ComputeDisplayToWorld((double)m_MousePose[0], (double)m_MousePose[1], focalDepth, newPickPoint);
    
  // Has to recalc old mouse point since the viewport has moved,
  // so can't move it outside the loop

  ComputeDisplayToWorld((double)Lastm_MousePose[0],
                              (double)Lastm_MousePose[1],
                              focalDepth, 
                              oldPickPoint);
  
  // Camera motion is reversed

  motionVector[0] = oldPickPoint[0] - newPickPoint[0];
  motionVector[1] = oldPickPoint[1] - newPickPoint[1];
  motionVector[2] = oldPickPoint[2] - newPickPoint[2];
  
  m_CurrentCamera->GetFocalPoint(viewFocus);
  m_CurrentCamera->GetPosition(viewPoint);
  m_CurrentCamera->SetFocalPoint(motionVector[0] + viewFocus[0],
                        motionVector[1] + viewFocus[1],
                        motionVector[2] + viewFocus[2]);

  m_CurrentCamera->SetPosition(motionVector[0] + viewPoint[0],
                      motionVector[1] + viewPoint[1],
                      motionVector[2] + viewPoint[2]);
      
  ResetClippingRange();
  Renderer->GetRenderWindow()->Render();
}
//----------------------------------------------------------------------------
void mmiCameraMove::Dolly()
//----------------------------------------------------------------------------
{
  if (Renderer == NULL)
    return;
  
  float *center = Renderer->GetCenter();

  int dy = m_MousePose[1] - Lastm_MousePose[1];
  double dyf = this->m_MotionFactor * (double)(dy) / (double)(center[1]);
  double zoomFactor = pow((double)1.1, dyf);
  
  if (m_CurrentCamera->GetParallelProjection())
    m_CurrentCamera->SetParallelScale(m_CurrentCamera->GetParallelScale()/zoomFactor);
  else
    m_CurrentCamera->Dolly(zoomFactor);
  
  ResetClippingRange();
	Renderer->GetRenderWindow()->Render();
}
//----------------------------------------------------------------------------
void mmiCameraMove::ResetClippingRange()
//----------------------------------------------------------------------------
{
  vtkRendererCollection *rc = Renderer->GetRenderWindow()->GetRenderers();
  rc->InitTraversal();
	vtkRenderer *r1 = rc->GetNextItem(); 	 
	vtkRenderer *r2 = rc->GetNextItem(); 	 

	if(r1==NULL)
	{
	} 
	else if (r2==NULL)
	{
     r1->ResetCameraClippingRange(); 
	}
  else 
	{
     // abbiamo anche il background renderer 
		float b1[6],b2[6];
		r1->ComputeVisiblePropBounds(b1);
		r2->ComputeVisiblePropBounds(b2);

		if(b1[0] == VTK_LARGE_FLOAT && b2[0] == VTK_LARGE_FLOAT)
		{
			r1->ResetCameraClippingRange();
		} 
		else if (b1[0] == VTK_LARGE_FLOAT )
		{
			r1->ResetCameraClippingRange(b2);
		}
		else if (b2[0] == VTK_LARGE_FLOAT )
		{
			r1->ResetCameraClippingRange(b1);
		}
		else
		{
			b1[0] = (b1[0]<b2[0]) ?	b1[0] : b2[0];    
			b1[2] = (b1[2]<b2[2]) ?	b1[2] : b2[2];    
			b1[4] = (b1[4]<b2[4]) ?	b1[4] : b2[4];    
			b1[1] = (b1[1]>b2[1]) ?	b1[1] : b2[1];    
			b1[3] = (b1[3]>b2[3]) ?	b1[3] : b2[3];    
			b1[5] = (b1[5]>b2[5]) ?	b1[5] : b2[5];    
			r1->ResetCameraClippingRange(b1);
		}
	}
}


//----------------------------------------------------------------------------
void mmiCameraMove::StartRotate() 
//----------------------------------------------------------------------------
{
  if (this->m_State != MOUSE_CAMERA_NONE) 
    {
    return;
    }
  this->Startm_State(MOUSE_CAMERA_ROTATE);
}

//----------------------------------------------------------------------------
void mmiCameraMove::EndRotate() 
//----------------------------------------------------------------------------
{
  if (this->m_State != MOUSE_CAMERA_ROTATE) 
    {
    return;
    }
  this->Stopm_State();
}

//----------------------------------------------------------------------------
void mmiCameraMove::StartZoom() 
//----------------------------------------------------------------------------
{
  if (this->m_State != MOUSE_CAMERA_NONE) 
    {
    return;
    }
  this->Startm_State(MOUSE_CAMERA_ZOOM);
}

//----------------------------------------------------------------------------
void mmiCameraMove::EndZoom() 
//----------------------------------------------------------------------------
{
  if (this->m_State != MOUSE_CAMERA_ZOOM) 
    {
    return;
    }
  this->Stopm_State();
}

//----------------------------------------------------------------------------
void mmiCameraMove::StartPan() 
//----------------------------------------------------------------------------
{
  if (this->m_State != MOUSE_CAMERA_NONE) 
    {
    return;
    }
  this->Startm_State(MOUSE_CAMERA_PAN);
}

//----------------------------------------------------------------------------
void mmiCameraMove::EndPan() 
//----------------------------------------------------------------------------
{
  if (this->m_State != MOUSE_CAMERA_PAN) 
    {
    return;
    }
  this->Stopm_State();
}

//----------------------------------------------------------------------------
void mmiCameraMove::StartSpin() 
//----------------------------------------------------------------------------
{
  if (this->m_State != MOUSE_CAMERA_NONE) 
    {
    return;
    }
  this->Startm_State(MOUSE_CAMERA_SPIN);
}

//----------------------------------------------------------------------------
void mmiCameraMove::EndSpin() 
//----------------------------------------------------------------------------
{
  if (this->m_State != MOUSE_CAMERA_SPIN) 
    {
    return;
    }
  this->Stopm_State();
}

//----------------------------------------------------------------------------
void mmiCameraMove::StartDolly() 
//----------------------------------------------------------------------------
{
  if (this->m_State != MOUSE_CAMERA_NONE) 
    {
    return;
    }
  this->Startm_State(MOUSE_CAMERA_DOLLY);
}

//----------------------------------------------------------------------------
void mmiCameraMove::EndDolly() 
//----------------------------------------------------------------------------
{
    if (this->m_State != MOUSE_CAMERA_DOLLY) 
      {
      return;
      }
    this->Stopm_State();
}

//----------------------------------------------------------------------------
void mmiCameraMove::Startm_State(int newstate) 
//----------------------------------------------------------------------------
{
  this->m_State = newstate;
}

//----------------------------------------------------------------------------
void mmiCameraMove::Stopm_State() 
//----------------------------------------------------------------------------
{
  this->m_State = MOUSE_CAMERA_NONE;
}
