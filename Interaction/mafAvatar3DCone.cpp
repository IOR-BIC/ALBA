/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafAvatar3DCone.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-30 14:34:55 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
// To be included first because of wxWindows
#ifdef __GNUG__
    #pragma implementation "mafAvatar3DCone.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "mafAvatar3DCone.h"

// factory
#include "vtkObjectFactory.h"

// VTK classes
#include "mflTransform.h"
#include "mflMatrix.h"
#include "vtkMatrix4x4.h"
#include "vtkConeSource.h"
#include "vtkAxes.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkPolyDataMapper.h"
#include "vtkAssembly.h"
#include "vtkProperty.h"
#include "vtkRayCast3DPicker.h"
#include "vtkLineSource.h"
#include "vtkTubeFilter.h"
#include "vtkRenderer.h"
#include "vtkRendererCollection.h"
#include "vtkRenderWindow.h"

float const CONE_HEIGHT = .1;

//------------------------------------------------------------------------------
// Events
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
vtkStandardNewMacro(mafAvatar3DCone)
//------------------------------------------------------------------------------
mafAvatar3DCone::mafAvatar3DCone()
//------------------------------------------------------------------------------
{
  vtkNEW(ConeCursor);
  vtkNEW(CursorMapper);
  vtkNEW(CursorActor);
  vtkNEW(CursorAxes);
  vtkNEW(CursorAxesMapper);
  vtkNEW(CursorAxesActor);
  //mflSmartPointer<mafActiveAssembly> cursor3D;
  mflSmartPointer<vtkAssembly> cursor3D;
  
  SetActor3D(cursor3D);

  ConeCursor->SetResolution(8);
  //ConeCursor->SetRadius(CONE_HEIGHT/3.5);
  ConeCursor->SetRadius(CONE_HEIGHT/10);
  ConeCursor->SetHeight(CONE_HEIGHT);
  ConeCursor->SetDirection(0,0,-1);


  CursorMapper->SetInput(ConeCursor->GetOutput());
  CursorActor->SetMapper(CursorMapper);
  CursorActor->PickableOff();
  CursorActor->DragableOff();
  //CursorActor->SetOrientation(0,90,0);
 
  CursorAxes->SetScaleFactor(CONE_HEIGHT/2);
  CursorAxesMapper->SetInput(CursorAxes->GetOutput());
  CursorAxesActor->SetMapper(CursorAxesMapper);
  CursorAxesActor->PickableOff();
  CursorAxesActor->DragableOff();
  CursorAxesActor->GetProperty()->SetAmbient(1);
  CursorAxesActor->GetProperty()->SetDiffuse(0);

  cursor3D->SetPickable(0);
  CursorAxesActor->SetPickable(0);
  CursorActor->SetPickable(0);

  cursor3D->AddPart(CursorActor);
  cursor3D->AddPart(CursorAxesActor);

  //cursor3D->SetScale(CONE_HEIGHT); // scale down to 10% of canonical volume

  mflSmartPointer<vtkMatrix4x4> pose;
  mflTransform::SetPosition(pose,0,0,0);
  cursor3D->SetUserMatrix(pose);

  //cursor3D->SetVisibility(0);
}

//------------------------------------------------------------------------------
mafAvatar3DCone::~mafAvatar3DCone()
//------------------------------------------------------------------------------
{
  vtkDEL(ConeCursor);
  vtkDEL(CursorMapper);
  vtkDEL(CursorActor);
  vtkDEL(CursorAxes);
  vtkDEL(CursorAxesMapper);
  vtkDEL(CursorAxesActor);
  // Cursor 3D is removed by parent class after forced shutdown
}

//------------------------------------------------------------------------------
int mafAvatar3DCone::Pick(mflMatrix *tracker_pose)
//------------------------------------------------------------------------------
{
  if (Renderer)
  {
    // compute pose in the world frame
    mflSmartPointer<mflMatrix> world_pose;
    this->TrackerToWorld(tracker_pose,world_pose);
    
      
    // here should call the picker Pick() function

    // Compute intersection ray:
    float p2[4]={ 0,0,-CONE_HEIGHT/2,1}; // canonical homogenous coordinate
    float p1[4]={ 0,0,CONE_HEIGHT/2,1}; // canonical homogenous coordinate
    
    // points in world coordinates
    float world_p1[4],world_p2[4];
    world_pose->MultiplyPoint(p1,world_p1);
    world_pose->MultiplyPoint(p2,world_p2);

    Hide();

    vtkRendererCollection *rc = Renderer->GetRenderWindow()->GetRenderers();
    vtkRenderer *r = NULL;
    assert(rc);

    rc->InitTraversal();
    while(r = rc->GetNextItem())
    {
      if( Picker3D->Pick(world_p1,world_p2,r) )
      {
        Show();
        return true;
      }
    }
  }

  Show();
  return false;
}