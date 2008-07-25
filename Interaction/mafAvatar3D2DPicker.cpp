/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafAvatar3D2DPicker.cpp,v $
  Language:  C++
  Date:      $Date: 2008-07-25 07:03:38 $
  Version:   $Revision: 1.3 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafAvatar3D2DPicker.h"

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
#include "vtkProperty2D.h"
#include "vtkRenderer.h"
#include "vtkRendererCollection.h"
#include "vtkRenderWindow.h"
//#include "vtkCaptionActor2D.h"
#include "vtkConeSource.h"
#include "vtkPolyDataMapper2D.h"
#include "vtkActor2D.h"
#include "vtkCellPicker.h"

float const CONE_HEIGHT = .1;

//------------------------------------------------------------------------------
// Events
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
vtkStandardNewMacro(mafAvatar3D2DPicker)
//------------------------------------------------------------------------------
mafAvatar3D2DPicker::mafAvatar3D2DPicker()
//------------------------------------------------------------------------------
{
  // Create 3D cursor
  vtkNEW(ConeCursor);
  vtkNEW(CursorMapper);
  vtkNEW(CursorActor);
  vtkNEW(CursorAxes);
  vtkNEW(CursorAxesMapper);
  vtkNEW(CursorAxesActor);

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

  mflSmartPointer<vtkConeSource> cone;
  cone->SetResolution(6);

  Canvas2DExtent[0]=Canvas2DExtent[1]=Canvas2DExtent[2]=Canvas2DExtent[3]=0;

  //  Create 2D cursor
/*  vtkNEW(Cursor2D);
  Cursor2D->SetCaption("2D Cursor!!!");
  Cursor2D->GetProperty()->SetColor(1,0,0);
  Cursor2D->SetAttachmentPoint(0,0,0.5);
  Cursor2D->SetHeight(0.05);
  Cursor2D->BorderOff();
  Cursor2D->SetPosition(25,10);
  Cursor2D->ThreeDimensionalLeaderOff();
  Cursor2D->SetLeaderGlyph(cone->GetOutput());
  Cursor2D->SetWidth(0.35);
  Cursor2D->SetHeight(0.10);
  Cursor2D->SetMaximumLeaderGlyphSize(10);
  Cursor2D->SetLeaderGlyphSize(0.25);*/
  vtkNEW(Cursor2D);
  Cursor2D->SetHeight(10.0);
  Cursor2D->SetRadius(10.0);
  Cursor2D->SetResolution(3);
  Cursor2D->Update();
  vtkNEW(CursorMapper2D);
  CursorMapper2D->SetInput(Cursor2D->GetOutput());
  vtkNEW(CursorActor2D);
  CursorActor2D->SetMapper(CursorMapper2D);
  CursorActor2D->GetProperty()->SetColor(1,0,0);

//  SetActor2D(Cursor2D);
  SetActor2D(CursorActor2D);

  vtkNEW(Picker2D);
}

//------------------------------------------------------------------------------
mafAvatar3D2DPicker::~mafAvatar3D2DPicker()
//------------------------------------------------------------------------------
{
  vtkDEL(ConeCursor);
  vtkDEL(CursorMapper);
  vtkDEL(CursorActor);
  vtkDEL(CursorAxes);
  vtkDEL(CursorAxesMapper);
  vtkDEL(CursorAxesActor);
  vtkDEL(Cursor2D);
  vtkDEL(CursorMapper2D);
  vtkDEL(CursorActor2D);
  vtkDEL(Picker2D);
  // Cursor 3D is removed by parent class after forced shutdown
}

//------------------------------------------------------------------------------
int mafAvatar3D2DPicker::InternalStore(mflXMLWriter *writer)
//------------------------------------------------------------------------------
{
  if(Superclass::InternalStore(writer))
    return -1;
  
  double canvas2d[4];
  GetCanvas2DExtent(canvas2d);
  mafAttribute::StoreVectorN(writer,canvas2d,4,"Canvas2DExtent");
  return 0;
}

//------------------------------------------------------------------------------
int mafAvatar3D2DPicker::InternalRestore(vtkXMLDataElement *node,vtkXMLDataParser *parser)
//------------------------------------------------------------------------------
{
  if(Superclass::InternalRestore(node,parser))
    return -1;

  double canvas2d[4]={0,0,0,0};
  mafAttribute::RestoreVectorN(node,parser,canvas2d,4,"Canvas2DExtent");
  SetCanvas2DExtent(canvas2d);
  return 0;
  
}

//------------------------------------------------------------------------------
void mafAvatar3D2DPicker::CreateSettings()
//------------------------------------------------------------------------------
{
  Settings = new mafGUIAvatar3D2DPickerSettings(this); 
}
//------------------------------------------------------------------------------
int mafAvatar3D2DPicker::Pick(mflMatrix &tracker_pose)
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
      if (Mode!=MODE_2D)
      {
        if( Picker3D->Pick(world_p1,world_p2,r) )
        {
          Show();
          return true;
        }
      }
      else
      {
        float xy[2];
        WorldToDisplay(world_pose,xy);
        if( Picker2D->Pick(xy[0],xy[1],0,r) )
        {
          return true;
        }
      }
    }
  }

  Show();
  return false;
}

//------------------------------------------------------------------------------
void mafAvatar3D2DPicker::OnMove3DEvent(mflEventInteraction *e)
//------------------------------------------------------------------------------
{
  Superclass::OnMove3DEvent(e);

  if (Renderer)
  {    
    float xy[2],xy_norm[2];
   
    WorldToDisplay(LastPoseMatrix,xy);
    
    xy_norm[0]=xy[0];
    xy_norm[1]=xy[1];
    Renderer->ViewportToNormalizedViewport(xy_norm[0],xy_norm[1]);

    // check if 2D coords fall in 2D canvas extent
    if (xy_norm[0]>=Canvas2DExtent[0]&&xy_norm[0]<Canvas2DExtent[1]&& \
        xy_norm[1]>=Canvas2DExtent[2]&&xy_norm[1]<Canvas2DExtent[3])
    {
      if (Mode!=MODE_2D)
      {
        // Enable 2D behavior
        SetModeTo2D();
        HideCursor3D();
        ShowCursor2D();
      }
      
      CursorActor2D->SetDisplayPosition(xy[0],xy[1]);
    }
    else
    {
      if (Mode!=MODE_3D)
      {
        // Enable 3D behavior
        SetModeTo3D();
        ShowCursor3D();
        HideCursor2D();
      }
    }
  }  
}
