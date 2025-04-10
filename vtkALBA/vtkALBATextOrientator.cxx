/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBATextOrientator
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkALBATextOrientator.h"

#include "vtkRenderer.h"
#include "vtkCamera.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkProperty.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"

#include "vtkPolyDataMapper2D.h"
#include "vtkProperty2D.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkPolyData.h"


vtkStandardNewMacro(vtkALBATextOrientator);
//------------------------------------------------------------------------------
vtkALBATextOrientator::vtkALBATextOrientator()
//------------------------------------------------------------------------------
{
  TextSourceLeftActor = NULL;
  TextSourceLeftMapper = NULL;
  TextSourceLeft = NULL;

  TextSourceDownActor = NULL;
  TextSourceDownMapper = NULL;
  TextSourceDown = NULL;

  TextSourceRightActor = NULL;
  TextSourceRightMapper = NULL;
  TextSourceRight = NULL;

  TextSourceUpActor = NULL;
  TextSourceUpMapper = NULL;
  TextSourceUp = NULL;


  Dimension = 10;
  AttachPositionFlag = false;

  DisplayOffsetUp[0] = DisplayOffsetUp[1] = 0;  
  DisplayOffsetRight[0] = DisplayOffsetRight[1] = 0;
  DisplayOffsetDown[0] = DisplayOffsetDown[1] = 0;
  DisplayOffsetLeft[0] = DisplayOffsetLeft[1] = 0;

  OrientatorCreate();
}
//------------------------------------------------------------------------------
vtkALBATextOrientator::~vtkALBATextOrientator()
//------------------------------------------------------------------------------
{
  if(TextSourceLeftActor) TextSourceLeftActor->Delete();
  if(TextSourceLeftMapper) TextSourceLeftMapper->Delete();
  if(TextSourceLeft) TextSourceLeft->Delete();

  if(TextSourceDownActor) TextSourceDownActor->Delete();
  if(TextSourceDownMapper) TextSourceDownMapper->Delete();
  if(TextSourceDown) TextSourceDown->Delete();

  if(TextSourceRightActor) TextSourceRightActor->Delete();
  if(TextSourceRightMapper) TextSourceRightMapper->Delete();
  if(TextSourceRight) TextSourceRight->Delete();

  if(TextSourceUpActor) TextSourceUpActor->Delete();
  if(TextSourceUpMapper) TextSourceUpMapper->Delete();
  if(TextSourceUp) TextSourceUp->Delete();
	
}
//------------------------------------------------------------------------------
void vtkALBATextOrientator::PrintSelf(ostream& os, vtkIndent indent)
//------------------------------------------------------------------------------
{
  this->Superclass::PrintSelf(os,indent);
}
//------------------------------------------------------------------------------
int vtkALBATextOrientator::RenderOverlay(vtkViewport *viewport)
//------------------------------------------------------------------------------
{
  vtkRenderer *ren = static_cast<vtkRenderer *>(viewport);

  /*vtkCamera *cam = ren->GetActiveCamera();
  if(!cam->GetParallelProjection()) return 0;*/


  OrientatorUpdate(ren);
  this->Modified();

  if(TextSourceLeftActor->GetVisibility())
    TextSourceLeftActor->RenderOverlay(viewport);
  if(TextSourceDownActor->GetVisibility())
    TextSourceDownActor->RenderOverlay(viewport);
  if(TextSourceRightActor->GetVisibility())
    TextSourceRightActor->RenderOverlay(viewport);
  if(TextSourceUpActor->GetVisibility())
    TextSourceUpActor->RenderOverlay(viewport);

  return 1;
  
}
//------------------------------------------------------------------------------
int vtkALBATextOrientator::RenderOpaqueGeometry(vtkViewport *viewport)
//------------------------------------------------------------------------------
{
	vtkRenderer *ren = static_cast<vtkRenderer *>(viewport);

	OrientatorUpdate(ren);

  if(TextSourceLeftActor->GetVisibility())
    TextSourceLeftActor->RenderOpaqueGeometry(viewport);
  if(TextSourceDownActor->GetVisibility())
    TextSourceDownActor->RenderOpaqueGeometry(viewport);
  if(TextSourceRightActor->GetVisibility())
    TextSourceRightActor->RenderOpaqueGeometry(viewport);
  if(TextSourceUpActor->GetVisibility())
    TextSourceUpActor->RenderOpaqueGeometry(viewport);
   
	return 0;
}
//------------------------------------------------------------------------------
void vtkALBATextOrientator::OrientatorCreate()
//------------------------------------------------------------------------------
{
  //left
  TextSourceLeft = vtkTextSource::New();
  TextSourceLeft->BackingOn();
  TextSourceLeft->SetText("");
  TextSourceLeft->SetForegroundColor(1.0,1.0,1.0);
  TextSourceLeft->SetBackgroundColor(0.0,0.0,0.0);
  TextSourceLeft->Update();

  TextSourceLeftMapper = vtkPolyDataMapper2D::New();
  TextSourceLeftMapper->SetInputConnection(TextSourceLeft->GetOutputPort());
  
  TextSourceLeftActor = vtkActor2D::New();
  TextSourceLeftActor->SetMapper(TextSourceLeftMapper);

  //down
  TextSourceDown = vtkTextSource::New();
  TextSourceDown->BackingOn();
  TextSourceDown->SetText("");
  TextSourceDown->SetForegroundColor(1.0,1.0,1.0);
  TextSourceDown->SetBackgroundColor(0.0,0.0,0.0);
  TextSourceDown->Update();

  TextSourceDownMapper = vtkPolyDataMapper2D::New();
  TextSourceDownMapper->SetInputConnection(TextSourceDown->GetOutputPort());

  TextSourceDownActor = vtkActor2D::New();
  TextSourceDownActor->SetMapper(TextSourceDownMapper);

  //right
  TextSourceRight = vtkTextSource::New();
  TextSourceRight->BackingOn();
  TextSourceRight->SetText("");
  TextSourceRight->SetForegroundColor(1.0,1.0,1.0);
  TextSourceRight->SetBackgroundColor(0.0,0.0,0.0);
  TextSourceRight->Update();

  TextSourceRightMapper = vtkPolyDataMapper2D::New();
  TextSourceRightMapper->SetInputConnection(TextSourceRight->GetOutputPort());

  TextSourceRightActor = vtkActor2D::New();
  TextSourceRightActor->SetMapper(TextSourceRightMapper);

  //up
  TextSourceUp = vtkTextSource::New();
  TextSourceUp->BackingOn();
  TextSourceUp->SetText("");
  TextSourceUp->SetForegroundColor(1.0,1.0,1.0);
  TextSourceUp->SetBackgroundColor(0.0,0.0,0.0);
  TextSourceUp->Update();

  TextSourceUpMapper = vtkPolyDataMapper2D::New();
  TextSourceUpMapper->SetInputConnection(TextSourceUp->GetOutputPort());

  TextSourceUpActor = vtkActor2D::New();
  TextSourceUpActor->SetMapper(TextSourceUpMapper);
}
//----------------------------------------------------------------------------
void vtkALBATextOrientator::OrientatorUpdate(vtkRenderer *ren)
//----------------------------------------------------------------------------
{
  int *renderSize;
  renderSize = ren->GetSize();
  
  int middleX = renderSize[0]/2;
  int middleY = renderSize[1]/2;
  
  if(AttachPositionFlag == false)
  {
    //left
    TextSourceLeftActor->GetPositionCoordinate()->SetCoordinateSystemToDisplay();
    TextSourceLeftActor->SetPosition(Dimension/4 + DisplayOffsetLeft[0], middleY-Dimension/2 + DisplayOffsetLeft[1]);

    //down
    TextSourceDownActor->GetPositionCoordinate()->SetCoordinateSystemToDisplay();
    TextSourceDownActor->SetPosition(middleX - Dimension/4 + DisplayOffsetDown[0], Dimension/4 + DisplayOffsetDown[1]);

    //right
    TextSourceRightActor->GetPositionCoordinate()->SetCoordinateSystemToDisplay();
    TextSourceRightActor->SetPosition(renderSize[0] - 1.5*Dimension + DisplayOffsetRight[0] , middleY-Dimension/2 + DisplayOffsetRight[1]);

    //up
    TextSourceUpActor->GetPositionCoordinate()->SetCoordinateSystemToDisplay();
    TextSourceUpActor->SetPosition(middleX - Dimension/4 + DisplayOffsetUp[0], renderSize[1]-2*Dimension + DisplayOffsetUp[1]);
  }
  else
  {
    

    double displayUp[3];
    ren->SetWorldPoint(AttachPositionUp[0],AttachPositionUp[1],AttachPositionUp[2],1.);
    ren->WorldToDisplay();
    ren->GetDisplayPoint(displayUp);
    int temporaryXUp,temporaryYUp;
    temporaryXUp = displayUp[0] + DisplayOffsetUp[0];
    temporaryYUp = displayUp[1]+ DisplayOffsetUp[1];

    if(temporaryXUp < 10)
      temporaryXUp = 10;
    if(temporaryXUp > renderSize[0] - 10)
      temporaryXUp = renderSize[0] - 10;

    if(temporaryYUp < 15)
      temporaryYUp = 15;
    if(temporaryYUp > renderSize[1] - 15)
      temporaryYUp = renderSize[1] - 15;

    TextSourceUpActor->SetPosition(temporaryXUp, temporaryYUp);


    double displayRight[3];
    ren->SetWorldPoint(AttachPositionRight[0],AttachPositionRight[1],AttachPositionRight[2],1.);
    ren->WorldToDisplay();
    ren->GetDisplayPoint(displayRight);
    int temporaryXRight,temporaryYRight;
    temporaryXRight = displayRight[0] + DisplayOffsetRight[0];
    temporaryYRight = displayRight[1]+ DisplayOffsetRight[1];

    if(temporaryXRight < 10)
      temporaryXRight = 10;
    if(temporaryXRight > renderSize[0] - 10)
      temporaryXRight = renderSize[0] - 10;

    if(temporaryYRight < 15)
      temporaryYRight = 15;
    if(temporaryYRight > renderSize[1] - 15)
      temporaryYRight = renderSize[1] - 15;


    TextSourceRightActor->SetPosition(temporaryXRight, temporaryYRight);

    double displayDown[3];
    ren->SetWorldPoint(AttachPositionDown[0],AttachPositionDown[1],AttachPositionDown[2],1.);
    ren->WorldToDisplay();
    ren->GetDisplayPoint(displayDown);
    int temporaryXDown,temporaryYDown;
    temporaryXDown = displayDown[0] + DisplayOffsetDown[0];
    temporaryYDown = displayDown[1] + DisplayOffsetDown[1];

    if(temporaryXDown < 10)
      temporaryXDown = 10;
    if(temporaryXDown > renderSize[0] - 10)
      temporaryXDown = renderSize[0] - 10;

    if(temporaryYDown < 15)
      temporaryYDown = 15;
    if(temporaryYDown > renderSize[1] - 15)
      temporaryYDown = renderSize[1] - 15;

    TextSourceDownActor->SetPosition(temporaryXDown, temporaryYDown);

    double displayLeft[3];
    ren->SetWorldPoint(AttachPositionLeft[0],AttachPositionLeft[1],AttachPositionLeft[2],1.);
    ren->WorldToDisplay();
    ren->GetDisplayPoint(displayLeft);
    int temporaryXLeft,temporaryYLeft;
    temporaryXLeft = displayLeft[0] + DisplayOffsetLeft[0];
    temporaryYLeft = displayLeft[1] + DisplayOffsetLeft[1];

    if(temporaryXLeft < 10)
      temporaryXLeft = 10;
    if(temporaryXLeft > renderSize[0] - 10)
      temporaryXLeft = renderSize[0] - 10;

    if(temporaryYLeft < 15)
      temporaryYLeft = 15;
    if(temporaryYLeft > renderSize[1] - 15)
      temporaryYLeft = renderSize[1] - 15;


    TextSourceLeftActor->SetPosition(temporaryXLeft, temporaryYLeft);
  }
  
}
//----------------------------------------------------------------------------
void vtkALBATextOrientator::SetBackgroundVisibility(bool show)
//----------------------------------------------------------------------------
{
  TextSourceLeft->SetBacking(show);
  TextSourceLeft->Update();
  TextSourceDown->SetBacking(show);
  TextSourceDown->Update();
  TextSourceRight->SetBacking(show);
  TextSourceRight->Update();
  TextSourceUp->SetBacking(show);
  TextSourceUp->Update();
}


//----------------------------------------------------------------------------
void vtkALBATextOrientator::SetTextColor(double red, double green, double blue)
//----------------------------------------------------------------------------
{
  TextSourceLeft->SetForegroundColor(red,green,blue);
  TextSourceLeft->Update();
  TextSourceDown->SetForegroundColor(red,green,blue);
  TextSourceDown->Update();
  TextSourceRight->SetForegroundColor(red,green,blue);
  TextSourceRight->Update();
  TextSourceUp->SetForegroundColor(red,green,blue);
  TextSourceUp->Update();
}
//----------------------------------------------------------------------------
void vtkALBATextOrientator::SetBackgroundColor(double red, double green, double blue)
//----------------------------------------------------------------------------
{
  TextSourceLeft->SetBackgroundColor(red,green,blue);
  TextSourceLeft->Update();
  TextSourceDown->SetBackgroundColor(red,green,blue);
  TextSourceDown->Update();
  TextSourceRight->SetBackgroundColor(red,green,blue);
  TextSourceRight->Update();
  TextSourceUp->SetBackgroundColor(red,green,blue);
  TextSourceUp->Update();
}
//----------------------------------------------------------------------------
void vtkALBATextOrientator::SetScale(double multiple)
//----------------------------------------------------------------------------
{
  Dimension *= multiple;
  vtkTransform *transform = vtkTransform::New();
  transform->Scale(multiple,multiple,multiple);
  transform->Update();

  vtkTransformPolyDataFilter *tpdf = vtkTransformPolyDataFilter::New();
  tpdf->SetTransform(transform);
  //left
  tpdf->SetInputConnection(TextSourceLeft->GetOutputPort());
  tpdf->Update();
  
  TextSourceLeft->GetOutput()->DeepCopy(tpdf->GetOutput());
  
  //down
  tpdf->SetInputConnection(TextSourceDown->GetOutputPort());
  tpdf->Update();
  
  TextSourceDown->GetOutput()->DeepCopy(tpdf->GetOutput());
  
  //right
  tpdf->SetInputConnection(TextSourceRight->GetOutputPort());
  tpdf->Update();

  TextSourceRight->GetOutput()->DeepCopy(tpdf->GetOutput());
  
  //up
  tpdf->SetInputConnection(TextSourceUp->GetOutputPort());
  tpdf->Update();

  TextSourceUp->GetOutput()->DeepCopy(tpdf->GetOutput());

  tpdf->Delete();
  transform->Delete();
}
//----------------------------------------------------------------------------
void vtkALBATextOrientator::SetSingleActorVisibility(int actor, bool show)
//----------------------------------------------------------------------------
{
  switch(actor)
  {
  case ID_ACTOR_LEFT:
    TextSourceLeftActor->SetVisibility(show);
    TextSourceLeftActor->Modified();
    break;
  case ID_ACTOR_DOWN:
    TextSourceDownActor->SetVisibility(show);
    TextSourceDownActor->Modified();
    break;
  case ID_ACTOR_RIGHT:
    TextSourceRightActor->SetVisibility(show);
    TextSourceRightActor->Modified();
    break;
  case ID_ACTOR_UP:
    TextSourceUpActor->SetVisibility(show);
    TextSourceUpActor->Modified();
    break;
  }
}
//----------------------------------------------------------------------------
void vtkALBATextOrientator::SetAttachPositions(double up[3], double right[3], double Down[3], double left[3])
//----------------------------------------------------------------------------
{
   AttachPositionUp[0] = up[0];
   AttachPositionUp[1] = up[1];
   AttachPositionUp[2] = up[2];

   AttachPositionRight[0] = right[0];
   AttachPositionRight[1] = right[1];
   AttachPositionRight[2] = right[2];

   AttachPositionDown[0] = Down[0];
   AttachPositionDown[1] = Down[1];
   AttachPositionDown[2] = Down[2];

   AttachPositionLeft[0] = left[0];
   AttachPositionLeft[1] = left[1];
   AttachPositionLeft[2] = left[2];
}