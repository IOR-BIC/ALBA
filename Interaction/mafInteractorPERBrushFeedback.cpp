/*=========================================================================

 Program: MAF2
 Module: mafInteractorPERBrushFeedback
 Authors: Eleonora Mambrini, Gianluigi Crimi
 
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


#include "mafInteractorPERBrushFeedback.h"

#include "mafDeviceButtonsPadTracker.h"
#include "mafDeviceButtonsPadMouse.h"
#include "mafEventBase.h"
#include "mafEventInteraction.h"
#include "mafInteractorCameraMove.h"
#include "mafRWIBase.h"
#include "mafView.h"
#include "mafViewCompound.h"
#include "mafVME.h"
#include "mafVMEOutput.h"

#include "vtkActor2D.h"
#include "vtkCamera.h"
#include "vtkCellPicker.h"
#include "vtkCoordinate.h"
#include "vtkCubeSource.h"
#include "vtkDataSet.h"
#include "vtkImageMapper.h"
#include "vtkMapper2D.h"
#include "vtkPointData.h"
#include "vtkPoints.h"
#include "vtkPolyDataMapper2D.h"
#include "vtkProperty2D.h"
#include "vtkRenderer.h"
#include "vtkRendererCollection.h"
#include "vtkRenderWindow.h"
#include "vtkSphereSource.h"

#include <assert.h>
//------------------------------------------------------------------------------
mafCxxTypeMacro(mafInteractorPERBrushFeedback)
//------------------------------------------------------------------------------
mafInteractorPERBrushFeedback::mafInteractorPERBrushFeedback()
//------------------------------------------------------------------------------
{
  m_Radius = 0.5;
  m_CurrentShape = CIRCLE_BRUSH_SHAPE;

  m_Coordinate = vtkCoordinate::New();
  m_Coordinate->SetCoordinateSystemToWorld();

  // Measure tools
  vtkNEW(m_BrushMapper);
  vtkNEW(m_BrushActor);

  m_SphereSource = vtkSphereSource::New();
  m_SphereSource->SetCenter(10, 10, 10);
  m_SphereSource->SetRadius(m_Radius);
  m_SphereSource->SetPhiResolution
    (20);
  m_SphereSource->SetThetaResolution(20);
  m_SphereSource->Update();

  m_CubeSource = vtkCubeSource::New();
  m_CubeSource->SetCenter(10, 10, 10);
  m_CubeSource->SetXLength(int(m_Radius*2));
  m_CubeSource->SetYLength(int(m_Radius*2));
  m_CubeSource->SetZLength(1);
  m_CubeSource->Update();

  m_BrushMapper->SetInput(m_SphereSource->GetOutput());
  m_BrushMapper->SetTransformCoordinate(m_Coordinate);
  m_BrushActor->SetMapper(m_BrushMapper);
  m_BrushActor->GetProperty()->SetColor(0.0,0.0,1.0);
  m_BrushActor->GetProperty()->SetOpacity(0.025);

  m_IsActorAdded = false;

  m_EnableDrawing = true;

  m_Count = 0;

  m_TargetVolumeSpacing = 1;
}

//------------------------------------------------------------------------------
mafInteractorPERBrushFeedback::~mafInteractorPERBrushFeedback()
//------------------------------------------------------------------------------
{
  m_Renderer->RemoveActor2D(m_BrushActor);

  vtkDEL(m_BrushMapper);
  vtkDEL(m_BrushActor);
  vtkDEL(m_SphereSource);
  vtkDEL(m_CubeSource);
  vtkDEL(m_Coordinate);

}

//------------------------------------------------------------------------------
void mafInteractorPERBrushFeedback::OnEvent(mafEventBase *event)
//------------------------------------------------------------------------------
{
  // Make the superclass to manage StartInteractionEvent
  // and StopInteractionEvent: this will make OnStart/StopInteraction()
  // to be called, or eventually the event to be forwarded.
  Superclass::OnEvent(event);

  mafID ch = event->GetChannel();

  if (ch == MCH_INPUT)
  {
    mafID id = event->GetId();
    mafDevice *device = (mafDevice *)event->GetSender();
    assert(device);

    if (id == mafDeviceButtonsPadMouse::GetMouseCharEventId() && !IsInteracting(device))
    {
      mafEventInteraction *e = mafEventInteraction::SafeDownCast(event);
      OnChar(e);
    }

    // Draw the brush feedback in the right position
    mafDeviceButtonsPadMouse *mouse = mafDeviceButtonsPadMouse::SafeDownCast(device);
		if (mouse)
		{

			int parallelView = m_Renderer->GetActiveCamera()->GetParallelProjection() != 0;
			if (parallelView)
			{
				//TODO: draw a box

				double pos_2d[2];
				mafEventInteraction *e = mafEventInteraction::SafeDownCast(event);
				e->Get2DPosition(pos_2d);

				mafEventMacro(mafEvent(this, CAMERA_UPDATE));

				if (m_EnableDrawing)
				{
					if (m_CurrentShape == CIRCLE_BRUSH_SHAPE)
						DrawEllipse(pos_2d[0], pos_2d[1]);
					else
						DrawBox(pos_2d[0], pos_2d[1]);
				}
			}
		}

    // find if this device is one of those currently interacting
    if (IsInteracting(device))
    {
      /*m_Count = 0;*/
      // process the Move event
      if (id == mafDeviceButtonsPadTracker::GetTracker3DMoveId() || id == mafDeviceButtonsPadMouse::GetMouse2DMoveId())
      {
        mafEventInteraction *e = mafEventInteraction::SafeDownCast(event);
        OnMove(e);
      }
      // In any case, forward the event to the right behavior
      mafVME *vme = GetPickedVME(device);
      if(vme)
      {
        mafInteractor *bh = vme->GetBehavior(); //can be NULL
        if (bh)
        {
          bh->OnEvent(event); // forward to VME behavior
        }
      }
      // if no picked behavior...
      if (m_CameraBehavior && m_CameraBehavior->IsInteracting(device))
      {
        m_CameraBehavior->OnEvent(event); // forward to Camera behavior
      }
      else if (m_CameraMouseBehavior && m_CameraMouseBehavior->IsInteracting(device))
      {
        m_CameraMouseBehavior->OnEvent(event); // forward to Camera behavior
      }
    }
    else
    {
      double mouse_pos[2];
      mafEventInteraction *e = mafEventInteraction::SafeDownCast(event);
      e->Get2DPosition(mouse_pos);
      bool picked_something = false;

      vtkCellPicker *cellPicker;
      vtkNEW(cellPicker);
      cellPicker->SetTolerance(0.001);
      mafDeviceButtonsPadMouse *mouse = mafDeviceButtonsPadMouse::SafeDownCast(device);
      mafView *v = mouse->GetView();
      if (v)
      {
        mafViewCompound *vc = mafViewCompound::SafeDownCast(v);
        if (vc)
        {
          v = vc->GetSubView();
        }
      }
      if (v)
      {
        vtkRendererCollection *rc = v->GetRWI()->GetRenderWindow()->GetRenderers();
        vtkRenderer *r = NULL;
        rc->InitTraversal();
        while(r = rc->GetNextItem())
        {
          if(cellPicker->Pick(mouse_pos[0],mouse_pos[1],0,r))
          {
            picked_something = true;
          }
        }
        if (picked_something)
        {
          vtkPoints *p = vtkPoints::New();
          double pos_picked[3];
          cellPicker->GetPickPosition(pos_picked);
          p->SetNumberOfPoints(1);
          p->SetPoint(0,pos_picked);
          v->Pick(mouse_pos[0],mouse_pos[1]);
          double scalar_value = 0;
          mafVME *pickedVME = v->GetPickedVme();
          if(pickedVME)
          {
            vtkDataSet *vtk_data = pickedVME->GetOutput()->GetVTKData();
            //GetPickPosition calculate the picking position with matrix multiplication 
            //the return value can be affected of some approximation errors, if the value
            //is outside the bounds FindPoint will return -1;
            double bounds[6];
            vtk_data->GetBounds(bounds);
            if (pos_picked[0]<bounds[0]) pos_picked[0]=bounds[0];
            if (pos_picked[0]>bounds[1]) pos_picked[0]=bounds[1];
            if (pos_picked[1]<bounds[2]) pos_picked[1]=bounds[2];
            if (pos_picked[1]>bounds[3]) pos_picked[1]=bounds[3];
            if (pos_picked[2]<bounds[4]) pos_picked[2]=bounds[4];
            if (pos_picked[2]>bounds[5]) pos_picked[2]=bounds[5];
            int pid = vtk_data->FindPoint(pos_picked);
            vtkDataArray *scalars = vtk_data->GetPointData()->GetScalars();
            if (scalars)
              scalars->GetTuple(pid,&scalar_value);

            //add a patch otherwise some old events will be rise at the end of mouse move
            m_Count++;

            mafEvent pick_event(this,MOUSE_MOVE,p);
            pick_event.SetArg(pid);
            pick_event.SetDouble(m_Count);
            pick_event.SetBool(e->GetModifier(MAF_CTRL_KEY) == true);
            mafEventMacro(pick_event);
            p->Delete();
          }
        }
      }
      vtkDEL(cellPicker);
    }
  }
}

//------------------------------------------------------------------------------
void mafInteractorPERBrushFeedback::DrawEllipse(double x, double y)
//------------------------------------------------------------------------------
{
  static double dx, dy, dz;

  //RemoveActor();

  double wp[4], p[3];
  m_Renderer->SetDisplayPoint(x,y,0);
  m_Renderer->DisplayToWorld();
  m_Renderer->GetWorldPoint(wp);
  p[0] = wp[0];
  p[1] = wp[1];
  p[2] = wp[2];

  if(!m_IsActorAdded)
    AddActor();

  m_SphereSource->SetCenter(p);
  m_SphereSource->Update();

  m_Renderer->GetRenderWindow()->Render();

}

//------------------------------------------------------------------------------
void mafInteractorPERBrushFeedback::DrawBox(double x, double y)
//------------------------------------------------------------------------------
{
  static double dx, dy, dz;


  double wp[4], p[3];
  m_Renderer->SetDisplayPoint(x,y,0);
  m_Renderer->DisplayToWorld();
  m_Renderer->GetWorldPoint(wp);
  p[0] = wp[0];
  p[1] = wp[1];
  p[2] = wp[2];

  if(!m_IsActorAdded)
    AddActor();

  m_CubeSource->SetCenter(p);
  m_CubeSource->Update();

  m_Renderer->GetRenderWindow()->Render();

}

//------------------------------------------------------------------------------
void mafInteractorPERBrushFeedback::RemoveActor()
//------------------------------------------------------------------------------
{
  if (m_Renderer && m_BrushActor)
  {
    // Uncomment this to render brush shadow
// 	  m_Renderer->RemoveActor2D(m_BrushActor);
// 	  m_Renderer->Render();

    m_IsActorAdded = false;
  }
}
//------------------------------------------------------------------------------
void mafInteractorPERBrushFeedback::AddActor()
//------------------------------------------------------------------------------
{
  if (m_Renderer && m_BrushActor)
  {
    // Uncomment this to render brush shadow
// 	  m_Renderer->AddActor2D(m_BrushActor);
// 	  m_Renderer->Render();

    m_IsActorAdded = true;
  }
}
//------------------------------------------------------------------------------
void mafInteractorPERBrushFeedback::SetRadius(double radius)
//------------------------------------------------------------------------------
{
  m_Radius = radius;

  if (m_SphereSource && m_CubeSource && m_Renderer)
  {
    m_SphereSource->SetRadius(m_Radius * m_TargetVolumeSpacing);
    m_SphereSource->Update();

    m_CubeSource->SetXLength(double(int(2*m_Radius) * m_TargetVolumeSpacing));
    m_CubeSource->SetYLength(double(int(2*m_Radius) * m_TargetVolumeSpacing));
    m_CubeSource->SetZLength(double(int(2*m_Radius) * m_TargetVolumeSpacing));
    m_CubeSource->Update();

    m_Renderer->Render();
  }
}

//------------------------------------------------------------------------------
void mafInteractorPERBrushFeedback::SetBrushShape(int shape)
//------------------------------------------------------------------------------
{
  if(shape == CIRCLE_BRUSH_SHAPE)
  {
    m_CurrentShape = CIRCLE_BRUSH_SHAPE;

    m_BrushMapper->SetInput(m_SphereSource->GetOutput());
    m_BrushMapper->SetTransformCoordinate(m_Coordinate);
    m_BrushActor->SetMapper(m_BrushMapper);
    m_BrushActor->GetProperty()->SetColor(0.0,0.0,1.0);
    m_BrushActor->GetProperty()->SetOpacity(0.025);
  }
  else
  {
    m_CurrentShape = SQUARE_BRUSH_SHAPE;

    m_BrushMapper->SetInput(m_CubeSource->GetOutput());
    m_BrushMapper->SetTransformCoordinate(m_Coordinate);
    m_BrushActor->SetMapper(m_BrushMapper);
    m_BrushActor->GetProperty()->SetColor(0.0,0.0,1.0);
    m_BrushActor->GetProperty()->SetOpacity(0.05);
  }
}