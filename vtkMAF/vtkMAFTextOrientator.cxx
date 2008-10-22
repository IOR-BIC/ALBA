/*=========================================================================

  Program:   Multimod Fundation Library
  Module:    $RCSfile: vtkMAFTextOrientator.cxx,v $
  Language:  C++
  Date:      $Date: 2008-10-22 08:45:51 $
  Version:   $Revision: 1.3.2.1 $
  Authors:   Daniele Giunchi
  Project:   MultiMod Project (www.ior.it/multimod)

==========================================================================*/

#include "vtkMAFTextOrientator.h"

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


vtkCxxRevisionMacro(vtkMAFTextOrientator, "$Revision: 1.3.2.1 $");
vtkStandardNewMacro(vtkMAFTextOrientator);
//------------------------------------------------------------------------------
vtkMAFTextOrientator::vtkMAFTextOrientator()
//------------------------------------------------------------------------------
{
  m_TextSourceLeftActor = NULL;
  m_TextSourceLeftMapper = NULL;
  m_TextSourceLeft = NULL;

  m_TextSourceDownActor = NULL;
  m_TextSourceDownMapper = NULL;
  m_TextSourceDown = NULL;

  m_TextSourceRightActor = NULL;
  m_TextSourceRightMapper = NULL;
  m_TextSourceRight = NULL;

  m_TextSourceUpActor = NULL;
  m_TextSourceUpMapper = NULL;
  m_TextSourceUp = NULL;


  m_Dimension = 10;
  m_AttachPositionFlag = false;

  m_DisplayOffsetUp[0] = m_DisplayOffsetUp[1] = 0;  
  m_DisplayOffsetRight[0] = m_DisplayOffsetRight[1] = 0;
  m_DisplayOffsetDown[0] = m_DisplayOffsetDown[1] = 0;
  m_DisplayOffsetLeft[0] = m_DisplayOffsetLeft[1] = 0;

  OrientatorCreate();
}
//------------------------------------------------------------------------------
vtkMAFTextOrientator::~vtkMAFTextOrientator()
//------------------------------------------------------------------------------
{
  if(m_TextSourceLeftActor) m_TextSourceLeftActor->Delete();
  if(m_TextSourceLeftMapper) m_TextSourceLeftMapper->Delete();
  if(m_TextSourceLeft) m_TextSourceLeft->Delete();

  if(m_TextSourceDownActor) m_TextSourceDownActor->Delete();
  if(m_TextSourceDownMapper) m_TextSourceDownMapper->Delete();
  if(m_TextSourceDown) m_TextSourceDown->Delete();

  if(m_TextSourceRightActor) m_TextSourceRightActor->Delete();
  if(m_TextSourceRightMapper) m_TextSourceRightMapper->Delete();
  if(m_TextSourceRight) m_TextSourceRight->Delete();

  if(m_TextSourceUpActor) m_TextSourceUpActor->Delete();
  if(m_TextSourceUpMapper) m_TextSourceUpMapper->Delete();
  if(m_TextSourceUp) m_TextSourceUp->Delete();
	
}
//------------------------------------------------------------------------------
void vtkMAFTextOrientator::PrintSelf(ostream& os, vtkIndent indent)
//------------------------------------------------------------------------------
{
  this->Superclass::PrintSelf(os,indent);
}
//------------------------------------------------------------------------------
int vtkMAFTextOrientator::RenderOverlay(vtkViewport *viewport)
//------------------------------------------------------------------------------
{
  vtkRenderer *ren = static_cast<vtkRenderer *>(viewport);

  /*vtkCamera *cam = ren->GetActiveCamera();
  if(!cam->GetParallelProjection()) return 0;*/


  OrientatorUpdate(ren);
  this->Modified();

  if(m_TextSourceLeftActor->GetVisibility())
    m_TextSourceLeftActor->RenderOverlay(viewport);
  if(m_TextSourceDownActor->GetVisibility())
    m_TextSourceDownActor->RenderOverlay(viewport);
  if(m_TextSourceRightActor->GetVisibility())
    m_TextSourceRightActor->RenderOverlay(viewport);
  if(m_TextSourceUpActor->GetVisibility())
    m_TextSourceUpActor->RenderOverlay(viewport);

  return 1;
  
}
//------------------------------------------------------------------------------
int vtkMAFTextOrientator::RenderOpaqueGeometry(vtkViewport *viewport)
//------------------------------------------------------------------------------
{
	vtkRenderer *ren = static_cast<vtkRenderer *>(viewport);

	OrientatorUpdate(ren);
	this->Modified();

  if(m_TextSourceLeftActor->GetVisibility())
    m_TextSourceLeftActor->RenderOpaqueGeometry(viewport);
  if(m_TextSourceDownActor->GetVisibility())
    m_TextSourceDownActor->RenderOpaqueGeometry(viewport);
  if(m_TextSourceRightActor->GetVisibility())
    m_TextSourceRightActor->RenderOpaqueGeometry(viewport);
  if(m_TextSourceUpActor->GetVisibility())
    m_TextSourceUpActor->RenderOpaqueGeometry(viewport);
   
	return 0;
}
//------------------------------------------------------------------------------
void vtkMAFTextOrientator::OrientatorCreate()
//------------------------------------------------------------------------------
{
  //left
  m_TextSourceLeft = vtkTextSource::New();
  m_TextSourceLeft->BackingOn();
  m_TextSourceLeft->SetText("");
  m_TextSourceLeft->SetForegroundColor(1.0,1.0,1.0);
  m_TextSourceLeft->SetBackgroundColor(0.0,0.0,0.0);
  m_TextSourceLeft->Update();

  m_TextSourceLeftMapper = vtkPolyDataMapper2D::New();
  m_TextSourceLeftMapper->SetInput(m_TextSourceLeft->GetOutput());
  
  m_TextSourceLeftActor = vtkActor2D::New();
  m_TextSourceLeftActor->SetMapper(m_TextSourceLeftMapper);

  //down
  m_TextSourceDown = vtkTextSource::New();
  m_TextSourceDown->BackingOn();
  m_TextSourceDown->SetText("");
  m_TextSourceDown->SetForegroundColor(1.0,1.0,1.0);
  m_TextSourceDown->SetBackgroundColor(0.0,0.0,0.0);
  m_TextSourceDown->Update();

  m_TextSourceDownMapper = vtkPolyDataMapper2D::New();
  m_TextSourceDownMapper->SetInput(m_TextSourceDown->GetOutput());

  m_TextSourceDownActor = vtkActor2D::New();
  m_TextSourceDownActor->SetMapper(m_TextSourceDownMapper);

  //right
  m_TextSourceRight = vtkTextSource::New();
  m_TextSourceRight->BackingOn();
  m_TextSourceRight->SetText("");
  m_TextSourceRight->SetForegroundColor(1.0,1.0,1.0);
  m_TextSourceRight->SetBackgroundColor(0.0,0.0,0.0);
  m_TextSourceRight->Update();

  m_TextSourceRightMapper = vtkPolyDataMapper2D::New();
  m_TextSourceRightMapper->SetInput(m_TextSourceRight->GetOutput());

  m_TextSourceRightActor = vtkActor2D::New();
  m_TextSourceRightActor->SetMapper(m_TextSourceRightMapper);

  //up
  m_TextSourceUp = vtkTextSource::New();
  m_TextSourceUp->BackingOn();
  m_TextSourceUp->SetText("");
  m_TextSourceUp->SetForegroundColor(1.0,1.0,1.0);
  m_TextSourceUp->SetBackgroundColor(0.0,0.0,0.0);
  m_TextSourceUp->Update();

  m_TextSourceUpMapper = vtkPolyDataMapper2D::New();
  m_TextSourceUpMapper->SetInput(m_TextSourceUp->GetOutput());

  m_TextSourceUpActor = vtkActor2D::New();
  m_TextSourceUpActor->SetMapper(m_TextSourceUpMapper);
}
//----------------------------------------------------------------------------
void vtkMAFTextOrientator::OrientatorUpdate(vtkRenderer *ren)
//----------------------------------------------------------------------------
{
  int *renderSize;
  renderSize = ren->GetSize();
  
  int middleX = renderSize[0]/2;
  int middleY = renderSize[1]/2;
  
  if(m_AttachPositionFlag == false)
  {
    //left
    m_TextSourceLeftActor->GetPositionCoordinate()->SetCoordinateSystemToDisplay();
    m_TextSourceLeftActor->SetPosition(m_Dimension/4 + m_DisplayOffsetLeft[0], middleY-m_Dimension/2 + m_DisplayOffsetLeft[1]);

    //down
    m_TextSourceDownActor->GetPositionCoordinate()->SetCoordinateSystemToDisplay();
    m_TextSourceDownActor->SetPosition(middleX - m_Dimension/4 + m_DisplayOffsetDown[0], m_Dimension/4 + m_DisplayOffsetDown[1]);

    //right
    m_TextSourceRightActor->GetPositionCoordinate()->SetCoordinateSystemToDisplay();
    m_TextSourceRightActor->SetPosition(renderSize[0] - 1.5*m_Dimension + m_DisplayOffsetRight[0] , middleY-m_Dimension/2 + m_DisplayOffsetRight[1]);

    //up
    m_TextSourceUpActor->GetPositionCoordinate()->SetCoordinateSystemToDisplay();
    m_TextSourceUpActor->SetPosition(middleX - m_Dimension/4 + m_DisplayOffsetUp[0], renderSize[1]-2*m_Dimension + m_DisplayOffsetUp[1]);
  }
  else
  {
    

    double displayUp[3];
    ren->SetWorldPoint(m_AttachPositionUp[0],m_AttachPositionUp[1],m_AttachPositionUp[2],1.);
    ren->WorldToDisplay();
    ren->GetDisplayPoint(displayUp);
    int temporaryXUp,temporaryYUp;
    temporaryXUp = displayUp[0] + m_DisplayOffsetUp[0];
    temporaryYUp = displayUp[1]+ m_DisplayOffsetUp[1];

    if(temporaryXUp < 10)
      temporaryXUp = 10;
    if(temporaryXUp > renderSize[0] - 10)
      temporaryXUp = renderSize[0] - 10;

    if(temporaryYUp < 15)
      temporaryYUp = 15;
    if(temporaryYUp > renderSize[1] - 15)
      temporaryYUp = renderSize[1] - 15;

    m_TextSourceUpActor->SetPosition(temporaryXUp, temporaryYUp);


    double displayRight[3];
    ren->SetWorldPoint(m_AttachPositionRight[0],m_AttachPositionRight[1],m_AttachPositionRight[2],1.);
    ren->WorldToDisplay();
    ren->GetDisplayPoint(displayRight);
    int temporaryXRight,temporaryYRight;
    temporaryXRight = displayRight[0] + m_DisplayOffsetRight[0];
    temporaryYRight = displayRight[1]+ m_DisplayOffsetRight[1];

    if(temporaryXRight < 10)
      temporaryXRight = 10;
    if(temporaryXRight > renderSize[0] - 10)
      temporaryXRight = renderSize[0] - 10;

    if(temporaryYRight < 15)
      temporaryYRight = 15;
    if(temporaryYRight > renderSize[1] - 15)
      temporaryYRight = renderSize[1] - 15;


    m_TextSourceRightActor->SetPosition(temporaryXRight, temporaryYRight);

    double displayDown[3];
    ren->SetWorldPoint(m_AttachPositionDown[0],m_AttachPositionDown[1],m_AttachPositionDown[2],1.);
    ren->WorldToDisplay();
    ren->GetDisplayPoint(displayDown);
    int temporaryXDown,temporaryYDown;
    temporaryXDown = displayDown[0] + m_DisplayOffsetDown[0];
    temporaryYDown = displayDown[1] + m_DisplayOffsetDown[1];

    if(temporaryXDown < 10)
      temporaryXDown = 10;
    if(temporaryXDown > renderSize[0] - 10)
      temporaryXDown = renderSize[0] - 10;

    if(temporaryYDown < 15)
      temporaryYDown = 15;
    if(temporaryYDown > renderSize[1] - 15)
      temporaryYDown = renderSize[1] - 15;

    m_TextSourceDownActor->SetPosition(temporaryXDown, temporaryYDown);

    double displayLeft[3];
    ren->SetWorldPoint(m_AttachPositionLeft[0],m_AttachPositionLeft[1],m_AttachPositionLeft[2],1.);
    ren->WorldToDisplay();
    ren->GetDisplayPoint(displayLeft);
    int temporaryXLeft,temporaryYLeft;
    temporaryXLeft = displayLeft[0] + m_DisplayOffsetLeft[0];
    temporaryYLeft = displayLeft[1] + m_DisplayOffsetLeft[1];

    if(temporaryXLeft < 10)
      temporaryXLeft = 10;
    if(temporaryXLeft > renderSize[0] - 10)
      temporaryXLeft = renderSize[0] - 10;

    if(temporaryYLeft < 15)
      temporaryYLeft = 15;
    if(temporaryYLeft > renderSize[1] - 15)
      temporaryYLeft = renderSize[1] - 15;


    m_TextSourceLeftActor->SetPosition(temporaryXLeft, temporaryYLeft);
  }
  
}
//----------------------------------------------------------------------------
void vtkMAFTextOrientator::SetBackgroundVisibility(bool show)
//----------------------------------------------------------------------------
{
  m_TextSourceLeft->SetBacking(show);
  m_TextSourceLeft->Update();
  m_TextSourceDown->SetBacking(show);
  m_TextSourceDown->Update();
  m_TextSourceRight->SetBacking(show);
  m_TextSourceRight->Update();
  m_TextSourceUp->SetBacking(show);
  m_TextSourceUp->Update();
}


//----------------------------------------------------------------------------
void vtkMAFTextOrientator::SetTextColor(double red, double green, double blue)
//----------------------------------------------------------------------------
{
  m_TextSourceLeft->SetForegroundColor(red,green,blue);
  m_TextSourceLeft->Update();
  m_TextSourceDown->SetForegroundColor(red,green,blue);
  m_TextSourceDown->Update();
  m_TextSourceRight->SetForegroundColor(red,green,blue);
  m_TextSourceRight->Update();
  m_TextSourceUp->SetForegroundColor(red,green,blue);
  m_TextSourceUp->Update();
}
//----------------------------------------------------------------------------
void vtkMAFTextOrientator::SetBackgroundColor(double red, double green, double blue)
//----------------------------------------------------------------------------
{
  m_TextSourceLeft->SetBackgroundColor(red,green,blue);
  m_TextSourceLeft->Update();
  m_TextSourceDown->SetBackgroundColor(red,green,blue);
  m_TextSourceDown->Update();
  m_TextSourceRight->SetBackgroundColor(red,green,blue);
  m_TextSourceRight->Update();
  m_TextSourceUp->SetBackgroundColor(red,green,blue);
  m_TextSourceUp->Update();
}
//----------------------------------------------------------------------------
void vtkMAFTextOrientator::SetScale(double multiple)
//----------------------------------------------------------------------------
{
  m_Dimension *= multiple;
  vtkTransform *transform = vtkTransform::New();
  transform->Scale(multiple,multiple,multiple);
  transform->Update();

  vtkTransformPolyDataFilter *tpdf = vtkTransformPolyDataFilter::New();
  tpdf->SetTransform(transform);
  //left
  tpdf->SetInput(m_TextSourceLeft->GetOutput());
  tpdf->Update();
  
  m_TextSourceLeft->GetOutput()->DeepCopy(tpdf->GetOutput());
  m_TextSourceLeft->GetOutput()->Update();
  //down
  tpdf->SetInput(m_TextSourceDown->GetOutput());
  tpdf->Update();
  
  m_TextSourceDown->GetOutput()->DeepCopy(tpdf->GetOutput());
  m_TextSourceDown->GetOutput()->Update();
  //right
  tpdf->SetInput(m_TextSourceRight->GetOutput());
  tpdf->Update();

  m_TextSourceRight->GetOutput()->DeepCopy(tpdf->GetOutput());
  m_TextSourceRight->GetOutput()->Update();
  //up
  tpdf->SetInput(m_TextSourceUp->GetOutput());
  tpdf->Update();

  m_TextSourceUp->GetOutput()->DeepCopy(tpdf->GetOutput());
  m_TextSourceUp->GetOutput()->Update();

  tpdf->Delete();
  transform->Delete();
}
//----------------------------------------------------------------------------
void vtkMAFTextOrientator::SetSingleActorVisibility(int actor, bool show)
//----------------------------------------------------------------------------
{
  switch(actor)
  {
  case ID_ACTOR_LEFT:
    m_TextSourceLeftActor->SetVisibility(show);
    m_TextSourceLeftActor->Modified();
    break;
  case ID_ACTOR_DOWN:
    m_TextSourceDownActor->SetVisibility(show);
    m_TextSourceDownActor->Modified();
    break;
  case ID_ACTOR_RIGHT:
    m_TextSourceRightActor->SetVisibility(show);
    m_TextSourceRightActor->Modified();
    break;
  case ID_ACTOR_UP:
    m_TextSourceUpActor->SetVisibility(show);
    m_TextSourceUpActor->Modified();
    break;
  }
}
//----------------------------------------------------------------------------
void vtkMAFTextOrientator::SetAttachPositions(double up[3], double right[3], double Down[3], double left[3])
//----------------------------------------------------------------------------
{
   m_AttachPositionUp[0] = up[0];
   m_AttachPositionUp[1] = up[1];
   m_AttachPositionUp[2] = up[2];

   m_AttachPositionRight[0] = right[0];
   m_AttachPositionRight[1] = right[1];
   m_AttachPositionRight[2] = right[2];

   m_AttachPositionDown[0] = Down[0];
   m_AttachPositionDown[1] = Down[1];
   m_AttachPositionDown[2] = Down[2];

   m_AttachPositionLeft[0] = left[0];
   m_AttachPositionLeft[1] = left[1];
   m_AttachPositionLeft[2] = left[2];
}