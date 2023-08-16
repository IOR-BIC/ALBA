/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractorCameraMove
 Authors: Paolo Quadrani & Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaDefines.h"

#include "albaInteractorCameraMove.h"
#include "albaDeviceButtonsPadMouse.h"
#include "albaAvatar3D.h"
#include "albaInteractor.h"

#include "albaEventInteraction.h"
#include "vtkMath.h"
#include "vtkRendererCollection.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkCamera.h"
#include "vtkTransform.h"

#include <assert.h>
#include "albaView.h"

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaInteractorCameraMove)

//------------------------------------------------------------------------------
albaInteractorCameraMove::albaInteractorCameraMove()
{
  m_MotionFactor   = 10.0;
  m_State = MOUSE_CAMERA_NONE;

  m_StartButton = -1;
  m_InteractionFlag = 0;
  m_CurrentCamera = NULL;
  m_MousePose[0] = m_MousePose[1] = 0;
  m_AutoResetClippingRange = true;
}
//------------------------------------------------------------------------------
albaInteractorCameraMove::~albaInteractorCameraMove()
{
}
//------------------------------------------------------------------------------
void albaInteractorCameraMove::OnEvent(albaEventBase *event)
{
  assert(event);
  assert(event->GetSender());
  
  albaID id=event->GetId();
  albaID channel=event->GetChannel();

  if (channel == MCH_INPUT && m_InteractionFlag)
  {
    albaEventInteraction *e = (albaEventInteraction *)event;
    albaDeviceButtonsPadMouse *mouse = albaDeviceButtonsPadMouse::SafeDownCast(GetDevice());
    
    // if the event comes from tracker which started the interaction continue...
		if (id == albaDeviceButtonsPadMouse::GetMouse2DMoveId() && mouse)
		{
			if (!m_CurrentCamera)
				return;

			double pos2d[2];
			e->Get2DPosition(pos2d);
			m_MousePose[0] = (int)pos2d[0];
			m_MousePose[1] = (int)pos2d[1];

			OnMouseMove();

			m_LastMousePose[0] = m_MousePose[0];
			m_LastMousePose[1] = m_MousePose[1];

			return;
		}
  }
	else if (id == albaDeviceButtonsPadMouse::GetWheelId())
	{
		OnMouseWheel((albaEventInteraction *)event);
		return;
	}
    
  Superclass::OnEvent(event);
}
//------------------------------------------------------------------------------
int albaInteractorCameraMove::StartInteraction(albaDeviceButtonsPadMouse *mouse)
{
  SetRendererAndView(mouse->GetRenderer(),mouse->GetView());
  if (m_Renderer)
  {
    m_CurrentCamera = m_Renderer->GetActiveCamera();
    return true;
  }

  return false;
}
//----------------------------------------------------------------------------
void albaInteractorCameraMove::OnButtonDown(albaEventInteraction *e)
{
  m_ButtonPressed = e->GetButton();
  double pos[2];
  e->Get2DPosition(pos);
  m_LastMousePose[0] = m_MousePose[0] = (int)pos[0];
  m_LastMousePose[1] = m_MousePose[1] = (int)pos[1];

  albaDeviceButtonsPadMouse *mouse = (albaDeviceButtonsPadMouse *)e->GetSender();
  StartInteraction(mouse);

  switch(m_ButtonPressed) 
  {
    case ALBA_LEFT_BUTTON:
      OnLeftButtonDown(e);
  	break;
    case ALBA_MIDDLE_BUTTON:
      OnMiddleButtonDown(e);
  	break;
    case ALBA_RIGHT_BUTTON:
      OnRightButtonDown(e);
  	break;
  }
}

//----------------------------------------------------------------------------
void albaInteractorCameraMove::OnButtonUp(albaEventInteraction *e)
{
  m_ButtonPressed = e->GetButton();
  
  switch(m_ButtonPressed) 
  {
    case ALBA_LEFT_BUTTON:
      OnLeftButtonUp();
  	break;
    case ALBA_MIDDLE_BUTTON:
      OnMiddleButtonUp();
  	break;
    case ALBA_RIGHT_BUTTON:
      OnRightButtonUp();
  	break;
  }
  m_Renderer->GetRenderWindow()->SetDesiredUpdateRate(0.001);
  m_Renderer->GetRenderWindow()->Render();
}
//----------------------------------------------------------------------------
void albaInteractorCameraMove::OnMouseMove() 
{ 

  if (m_Renderer)
  {
		m_Renderer->GetRenderWindow()->SetDesiredUpdateRate(15.0);
  }

  switch (m_State) 
  {
    case MOUSE_CAMERA_ROTATE:
      Rotate();
    break;
    case MOUSE_CAMERA_PAN:
      Pan();
    break;
    case MOUSE_CAMERA_DOLLY:
      Dolly();
    break;
    case MOUSE_CAMERA_SPIN:
      Spin();
    break;
  }

  
}
//----------------------------------------------------------------------------
void albaInteractorCameraMove::OnLeftButtonDown(albaEventInteraction *e)
{ 
  if (e->GetModifier(ALBA_SHIFT_KEY)) 
  {
    if (e->GetModifier(ALBA_CTRL_KEY)) 
      StartDolly();
    else 
      StartPan();
  } 
  else 
  {
    if (e->GetModifier(ALBA_CTRL_KEY)) 
      StartSpin();
    else 
      StartRotate();
  }
}
//----------------------------------------------------------------------------
void albaInteractorCameraMove::OnMiddleButtonDown(albaEventInteraction *e) 
{
  StartPan();
}
//----------------------------------------------------------------------------
void albaInteractorCameraMove::OnRightButtonDown(albaEventInteraction *e) 
{
  StartDolly();
}
//----------------------------------------------------------------------------
void albaInteractorCameraMove::OnLeftButtonUp()
{
  switch (m_State) 
  {
    case MOUSE_CAMERA_DOLLY:
      EndDolly();
    break;
    case MOUSE_CAMERA_PAN:
      EndPan();
    break;
    case MOUSE_CAMERA_SPIN:
      EndSpin();
    break;
    case MOUSE_CAMERA_ROTATE:
      EndRotate();
    break;
  }
}
//----------------------------------------------------------------------------
void albaInteractorCameraMove::OnMiddleButtonUp()
{
  if (m_State == MOUSE_CAMERA_PAN) 
		EndPan();
}
//----------------------------------------------------------------------------
void albaInteractorCameraMove::OnRightButtonUp()
{
	if (m_State == MOUSE_CAMERA_DOLLY)
		EndDolly();
}
//----------------------------------------------------------------------------
void albaInteractorCameraMove::Rotate()
{
  if (m_Renderer == NULL)
    return;

	if (m_CurrentCamera->GetParallelProjection()) return; 

  int dx = m_MousePose[0] - m_LastMousePose[0];
  int dy = m_MousePose[1] - m_LastMousePose[1];
  
  int *size = m_Renderer->GetRenderWindow()->GetSize();

  double delta_elevation = -20.0 / size[1];
  double delta_azimuth = -20.0 / size[0];
  
  double rxf = (double)dx * delta_azimuth * m_MotionFactor;
  double ryf = (double)dy * delta_elevation * m_MotionFactor;
  
  m_CurrentCamera->Azimuth(rxf);
  m_CurrentCamera->Elevation(ryf);
  m_CurrentCamera->OrthogonalizeViewUp();
		
  if (m_AutoResetClippingRange) 
		ResetClippingRange();

	InvokeEvent(this, CAMERA_MOVED,MCH_UP,m_CurrentCamera);
  m_Renderer->GetRenderWindow()->Render();
}

#define MAGNET_ANGLE 2
//----------------------------------------------------------------------------
void albaInteractorCameraMove::Spin()
{
  if (m_Renderer == NULL || m_View == NULL || !m_View->GetCanSpin())
    return;

  double *center = m_Renderer->GetCenter();

  double newAngle = 
    atan2((double)m_MousePose[1] - (double)center[1],
          (double)m_MousePose[0] - (double)center[0]);

  double oldAngle = 
    atan2((double)m_LastMousePose[1] - (double)center[1],
          (double)m_LastMousePose[0] - (double)center[0]);
  
  newAngle *= vtkMath::RadiansToDegrees();
  oldAngle *= vtkMath::RadiansToDegrees();

	double rollAngle;
	double angleDiff = newAngle - oldAngle;

	double cameraRoll = m_CurrentCamera->GetRoll()+180.0;
	double predictedRoll = cameraRoll + angleDiff;
	if (predictedRoll > 360)
		predictedRoll = predictedRoll - 360;
	if (predictedRoll < 0)
		predictedRoll = predictedRoll + 360;

	//On "magnet" m_MousePose is set to m_LastMousePose in order to cumulate mouse movements 
	if (predictedRoll > 90 - MAGNET_ANGLE&&predictedRoll < 90 + MAGNET_ANGLE)
	{
		rollAngle = 90-cameraRoll;
		m_MousePose[0] = m_LastMousePose[0];
		m_MousePose[1] = m_LastMousePose[1];
	}
	else if (predictedRoll > 180 - MAGNET_ANGLE&&predictedRoll < 180 + MAGNET_ANGLE)
	{
		rollAngle = 180 - cameraRoll;
		m_MousePose[0] = m_LastMousePose[0];
		m_MousePose[1] = m_LastMousePose[1];
	}
	else if (predictedRoll > 270 - MAGNET_ANGLE&&predictedRoll < 270 + MAGNET_ANGLE)
	{
		rollAngle = 270-cameraRoll;
		m_MousePose[0] = m_LastMousePose[0];
		m_MousePose[1] = m_LastMousePose[1];
	}
	else if (predictedRoll > 360 - MAGNET_ANGLE)
	{
		rollAngle = 360- cameraRoll;
		m_MousePose[0] = m_LastMousePose[0];
		m_MousePose[1] = m_LastMousePose[1];
	}
	else if (predictedRoll < 0 + MAGNET_ANGLE)
	{
		rollAngle = -cameraRoll;
		m_MousePose[0] = m_LastMousePose[0];
		m_MousePose[1] = m_LastMousePose[1];
	}
	else
	{
		rollAngle = angleDiff;
	}

  m_CurrentCamera->Roll(rollAngle);
  m_CurrentCamera->OrthogonalizeViewUp();
     
  if (m_AutoResetClippingRange) 
		ResetClippingRange();

	InvokeEvent(this, CAMERA_MOVED,MCH_UP,m_CurrentCamera);
  m_Renderer->GetRenderWindow()->Render();
}
//----------------------------------------------------------------------------
void albaInteractorCameraMove::Pan()
{
  if (m_Renderer == NULL)
    return;

  double viewFocus[4], focalDepth, viewPoint[3];
  double newPickPoint[4], oldPickPoint[4], motionVector[3];
  
  // Calculate the focal depth since we'll be using it a lot

  m_CurrentCamera->GetFocalPoint(viewFocus);
  ComputeWorldToDisplay(viewFocus[0], viewFocus[1], viewFocus[2], viewFocus);
  focalDepth = viewFocus[2];

  ComputeDisplayToWorld((double)m_MousePose[0], (double)m_MousePose[1], focalDepth, newPickPoint);
    
  // Has to recalc old mouse point since the viewport has moved,
  // so can't move it outside the loop

  ComputeDisplayToWorld((double)m_LastMousePose[0],
                              (double)m_LastMousePose[1],
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
      
  if (m_AutoResetClippingRange)  
		ResetClippingRange();

	InvokeEvent(this, CAMERA_MOVED,MCH_UP,m_CurrentCamera);
  m_Renderer->GetRenderWindow()->Render();
}
//----------------------------------------------------------------------------
void albaInteractorCameraMove::Dolly()
{
  if (m_Renderer == NULL)
    return;
  
  double *center = m_Renderer->GetCenter();

  int dy = m_MousePose[1] - m_LastMousePose[1];
  double dyf = m_MotionFactor * (double)(dy) / (double)(center[1]);
  double zoomFactor = pow((double)1.1, dyf);
  
  if (m_CurrentCamera->GetParallelProjection())
    m_CurrentCamera->SetParallelScale(m_CurrentCamera->GetParallelScale()/zoomFactor);
  else
    m_CurrentCamera->Dolly(zoomFactor);
  
  if (m_AutoResetClippingRange)  
		ResetClippingRange();

	InvokeEvent(this, CAMERA_MOVED,MCH_UP,m_CurrentCamera);
	m_Renderer->GetRenderWindow()->Render();
}
//----------------------------------------------------------------------------
void albaInteractorCameraMove::ResetClippingRange()
{
  vtkRendererCollection *rc = m_Renderer->GetRenderWindow()->GetRenderers();
  rc->InitTraversal();

  // Layers can be ONE or TWO; the third layer ALWAYS_VISIBLE doesn't count, as implemented in albaRWI.
  int numberOfLayers = m_Renderer->GetRenderWindow()->GetNumberOfLayers();

  // The order of the renderers has been set in albaRWI.
  // This is ALWAYS VISIBLE RENDERER 
	vtkRenderer *rAV = rc->GetNextItem(); 	 
  // This is FRONT RENDERER
	vtkRenderer *rFR = rc->GetNextItem(); 	 
  // This can be BACK RENDERER or a renderer added by vtkALBAOrientationWidget.
  // It is BACK RENDERER only if  numberOfLayers==3.
  vtkRenderer *rBR = NULL;
  if (numberOfLayers==3)
  {
	  rBR = rc->GetNextItem(); 	 
  }

	double b1[6],b2[6],b3[6],b[6];
  if(rFR==NULL)
	{
	} 
	else if (rAV==NULL && rBR==NULL)
	{
     rFR->ResetCameraClippingRange(); 
	}
  else if (rAV)
  {
    // We have the tird layer (always visible), with default bounds (-1,1,-1,1,-1,1).
    // The clipping range must be computed with the bounds of the front renderer!
		rFR->ComputeVisiblePropBounds(b1);
		rAV->ComputeVisiblePropBounds(b2);

		if(b1[0] == VTK_LARGE_FLOAT && b2[0] == VTK_LARGE_FLOAT)
		{
			rFR->ResetCameraClippingRange();
		} 
		else if (b1[0] == VTK_LARGE_FLOAT )
		{
			rFR->ResetCameraClippingRange(b2);
		}
		else
		{
      // WORKAROUND 
      // The only actor shown is the Axis actor in the bottom left angle:
      // it must not be taken into account.
      if (b2[0]==-1 && b2[1]==1 && b2[2]==-1 && b2[3]==1 && b2[4]==-1 && b2[5]==1)
      {
        b[0] = b1[0];
			  b[2] = b1[2];
        b[4] = b1[4];
        b[1] = b1[1];
			  b[3] = b1[3];
			  b[5] = b1[5];
      }
      // WORKAROUND 
      // There are other actors (like the GIZMO): 
      // we must take them into account.
      else
      {
        b[0] = (b1[0]<b2[0]) ?	b1[0] : b2[0];    
			  b[2] = (b1[2]<b2[2]) ?	b1[2] : b2[2];    
			  b[4] = (b1[4]<b2[4]) ?	b1[4] : b2[4];    
			  b[1] = (b1[1]>b2[1]) ?	b1[1] : b2[1];    
			  b[3] = (b1[3]>b2[3]) ?	b1[3] : b2[3];    
			  b[5] = (b1[5]>b2[5]) ?	b1[5] : b2[5];  
      }
			rFR->ResetCameraClippingRange(b);
		}

    // We have also the back renderer.
    // The clipping range must be matched between the back renderer bounds and the already calculated one.	
    if (rBR)
  	{
	  	rBR->ComputeVisiblePropBounds(b3);

		  if (b3[0] == VTK_LARGE_FLOAT )
		  {
        // do nothing
			}
		  else
		  {
			  b[0] = (b[0]<b3[0]) ?	b[0] : b3[0];    
			  b[2] = (b[2]<b3[2]) ?	b[2] : b3[2];    
			  b[4] = (b[4]<b3[4]) ?	b[4] : b3[4];    
			  b[1] = (b[1]>b3[1]) ?	b[1] : b3[1];    
			  b[3] = (b[3]>b3[3]) ?	b[3] : b3[3];    
			  b[5] = (b[5]>b3[5]) ?	b[5] : b3[5];    
			  rFR->ResetCameraClippingRange(b);
		  }
	  }
  }
}
//----------------------------------------------------------------------------
void albaInteractorCameraMove::StartRotate() 
{
  if (m_State == MOUSE_CAMERA_NONE) 
	  StartState(MOUSE_CAMERA_ROTATE);
}
//----------------------------------------------------------------------------
void albaInteractorCameraMove::EndRotate() 
{
  if (m_State == MOUSE_CAMERA_ROTATE ) 
	  StopState();
}
//----------------------------------------------------------------------------
void albaInteractorCameraMove::StartZoom() 
{
  if (m_State == MOUSE_CAMERA_NONE) 
	  StartState(MOUSE_CAMERA_ZOOM);
}
//----------------------------------------------------------------------------
void albaInteractorCameraMove::EndZoom() 
{
  if (m_State == MOUSE_CAMERA_ZOOM) 
	  StopState();
}
//----------------------------------------------------------------------------
void albaInteractorCameraMove::StartPan() 
{
  if (m_State == MOUSE_CAMERA_NONE) 
   StartState(MOUSE_CAMERA_PAN);
}
//----------------------------------------------------------------------------
void albaInteractorCameraMove::EndPan() 
{
  if (m_State == MOUSE_CAMERA_PAN) 
		StopState();
}
//----------------------------------------------------------------------------
void albaInteractorCameraMove::StartSpin() 
{
  if (m_State == MOUSE_CAMERA_NONE) 
	  StartState(MOUSE_CAMERA_SPIN);
}
//----------------------------------------------------------------------------
void albaInteractorCameraMove::EndSpin() 
{
  if (m_State == MOUSE_CAMERA_SPIN) 
	  StopState();
}
//----------------------------------------------------------------------------
void albaInteractorCameraMove::StartDolly() 
{
  if (m_State == MOUSE_CAMERA_NONE) 
	  StartState(MOUSE_CAMERA_DOLLY);
}
//----------------------------------------------------------------------------
void albaInteractorCameraMove::EndDolly()
{
	if (m_State == MOUSE_CAMERA_DOLLY)
		StopState();
}
//----------------------------------------------------------------------------
void albaInteractorCameraMove::StartState(int newstate) 
{
  m_State = newstate;
}
//----------------------------------------------------------------------------
void albaInteractorCameraMove::StopState() 
{
  m_State = MOUSE_CAMERA_NONE;
}
//----------------------------------------------------------------------------
void albaInteractorCameraMove::AutoResetClippingRangeOff()
{
	m_AutoResetClippingRange = false;
}
//----------------------------------------------------------------------------
void albaInteractorCameraMove::AutoResetClippingRangeOn()
{
	m_AutoResetClippingRange = true;
}

void albaInteractorCameraMove::OnMouseWheel(albaEventInteraction *e)
{
	albaDeviceButtonsPadMouse *mouse;
	albaDevice *device = albaDevice::SafeDownCast((albaDevice*)e->GetSender());
	
	if(device)
		 mouse = albaDeviceButtonsPadMouse::SafeDownCast(device);
	
	if (mouse)
		SetRendererAndView(mouse->GetRenderer(),mouse->GetView());

	if(m_Renderer)
		m_CurrentCamera = m_Renderer->GetActiveCamera();

	if (m_Renderer == NULL)
		return;

	double *center = m_Renderer->GetCenter();

	double rotation = *(double *)e->GetData();
	double zoomFactor = 1.0 + (rotation / 1200.0);

	if (m_CurrentCamera->GetParallelProjection())
		m_CurrentCamera->SetParallelScale(m_CurrentCamera->GetParallelScale() / zoomFactor);
	else
		m_CurrentCamera->Dolly(zoomFactor);

	if (m_AutoResetClippingRange)
		ResetClippingRange();

	InvokeEvent(this, CAMERA_MOVED, MCH_UP, m_CurrentCamera);
	m_Renderer->GetRenderWindow()->Render();
}
