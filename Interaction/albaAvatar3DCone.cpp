/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaAvatar3DCone
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaAvatar3DCone.h"

// VTK classes
#include "albaTransform.h"
#include "albaMatrix.h"
#include "vtkMatrix4x4.h"
#include "vtkConeSource.h"
#include "vtkAxes.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkPolyDataMapper.h"
#include "vtkAssembly.h"
#include "vtkProperty.h"
#include "vtkALBARayCast3DPicker.h"
#include "vtkLineSource.h"
#include "vtkTubeFilter.h"
#include "vtkRenderer.h"
#include "vtkRendererCollection.h"
#include "vtkRenderWindow.h"

float const CONE_HEIGHT = .1;

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaAvatar3DCone)
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
albaAvatar3DCone::albaAvatar3DCone()
//------------------------------------------------------------------------------
{
  vtkNEW(m_ConeCursor);
  vtkNEW(m_CursorMapper);
  vtkNEW(m_CursorActor);
  vtkNEW(m_CursorAxes);
  vtkNEW(m_CursorAxesMapper);
  vtkNEW(m_CursorAxesActor);
  //vtkALBASmartPointer<albaActiveAssembly> cursor3D;
  vtkALBASmartPointer<vtkAssembly> cursor3D;
  
  SetActor3D(cursor3D);

  m_ConeCursor->SetResolution(8);
  //m_ConeCursor->SetRadius(CONE_HEIGHT/3.5);
  m_ConeCursor->SetRadius(CONE_HEIGHT/10);
  m_ConeCursor->SetHeight(CONE_HEIGHT);
  m_ConeCursor->SetDirection(0,0,-1);


  m_CursorMapper->SetInput(m_ConeCursor->GetOutput());
  m_CursorActor->SetMapper(m_CursorMapper);
  m_CursorActor->PickableOff();
  m_CursorActor->DragableOff();
  //m_CursorActor->SetOrientation(0,90,0);
 
  m_CursorAxes->SetScaleFactor(CONE_HEIGHT/2);
  m_CursorAxesMapper->SetInput(m_CursorAxes->GetOutput());
  m_CursorAxesActor->SetMapper(m_CursorAxesMapper);
  m_CursorAxesActor->PickableOff();
  m_CursorAxesActor->DragableOff();
  m_CursorAxesActor->GetProperty()->SetAmbient(1);
  m_CursorAxesActor->GetProperty()->SetDiffuse(0);

  cursor3D->SetPickable(0);
  m_CursorAxesActor->SetPickable(0);
  m_CursorActor->SetPickable(0);

  cursor3D->AddPart(m_CursorActor);
  cursor3D->AddPart(m_CursorAxesActor);

  //cursor3D->SetScale(CONE_HEIGHT); // scale down to 10% of canonical volume

  albaTransform pose;
  
  pose.SetPosition(0,0,0);
  cursor3D->SetUserMatrix(pose.GetMatrix().GetVTKMatrix());

  //cursor3D->SetVisibility(0);
}

//------------------------------------------------------------------------------
albaAvatar3DCone::~albaAvatar3DCone()
//------------------------------------------------------------------------------
{
  vtkDEL(m_ConeCursor);
  vtkDEL(m_CursorMapper);
  vtkDEL(m_CursorActor);
  vtkDEL(m_CursorAxes);
  vtkDEL(m_CursorAxesMapper);
  vtkDEL(m_CursorAxesActor);
  // Cursor 3D is removed by parent class after forced shutdown
}

//------------------------------------------------------------------------------
int albaAvatar3DCone::Pick(albaMatrix &tracker_pose)
//------------------------------------------------------------------------------
{
  if (m_Renderer)
  {
    // compute pose in the world frame
    albaMatrix world_pose;
    TrackerToWorld(tracker_pose,world_pose,CANONICAL_TO_WORLD_SCALE);
      
    // here should call the picker Pick() function

    // Compute intersection ray:
    double p2[4]={ 0,0,-CONE_HEIGHT/2,1}; // canonical homogenous coordinate
    double p1[4]={ 0,0,CONE_HEIGHT/2,1}; // canonical homogenous coordinate
    
    // points in world coordinates
    double world_p1[4],world_p2[4];
    world_pose.MultiplyPoint(p1,world_p1);
    world_pose.MultiplyPoint(p2,world_p2);

    Hide();

    vtkRendererCollection *rc = m_Renderer->GetRenderWindow()->GetRenderers();
    vtkRenderer *r = NULL;
    assert(rc);

    rc->InitTraversal();
    while(r = rc->GetNextItem())
    {
      if( m_Picker3D->Pick(world_p1,world_p2,r) )
      {
        Show();
        return true;
      }
    }
  }

  Show();
  return false;
}
