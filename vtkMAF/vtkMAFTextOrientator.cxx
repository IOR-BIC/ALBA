/*=========================================================================

  Program:   Multimod Fundation Library
  Module:    $RCSfile: vtkMAFTextOrientator.cxx,v $
  Language:  C++
  Date:      $Date: 2007-11-23 10:18:37 $
  Version:   $Revision: 1.1 $
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


vtkCxxRevisionMacro(vtkMAFTextOrientator, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkMAFTextOrientator);
//------------------------------------------------------------------------------
vtkMAFTextOrientator::vtkMAFTextOrientator()
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


  m_Dimension = 10;

  OrientatorCreate();
}
//------------------------------------------------------------------------------
vtkMAFTextOrientator::~vtkMAFTextOrientator()
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
int vtkMAFTextOrientator::RenderOpaqueGeometry(vtkViewport *viewport)
//------------------------------------------------------------------------------
{
	vtkRenderer *ren = static_cast<vtkRenderer *>(viewport);

	OrientatorUpdate(ren);
	this->Modified();

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
void vtkMAFTextOrientator::OrientatorCreate()
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
  TextSourceLeftMapper->SetInput(TextSourceLeft->GetOutput());
  
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
  TextSourceDownMapper->SetInput(TextSourceDown->GetOutput());

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
  TextSourceRightMapper->SetInput(TextSourceRight->GetOutput());

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
  TextSourceUpMapper->SetInput(TextSourceUp->GetOutput());

  TextSourceUpActor = vtkActor2D::New();
  TextSourceUpActor->SetMapper(TextSourceUpMapper);
}
//----------------------------------------------------------------------------
void vtkMAFTextOrientator::OrientatorUpdate(vtkRenderer *ren)
//----------------------------------------------------------------------------
{
  int *renderSize;
  renderSize = ren->GetSize();
  
  int middleX = renderSize[0]/2;
  int middleY = renderSize[1]/2;
  
  //left
  TextSourceLeftActor->GetPositionCoordinate()->SetCoordinateSystemToDisplay();
  TextSourceLeftActor->SetPosition(m_Dimension/4 , middleY-m_Dimension/2);

  //down
  TextSourceDownActor->GetPositionCoordinate()->SetCoordinateSystemToDisplay();
  TextSourceDownActor->SetPosition(middleX - m_Dimension/4 , m_Dimension/4);

  //right
  TextSourceRightActor->GetPositionCoordinate()->SetCoordinateSystemToDisplay();
  TextSourceRightActor->SetPosition(renderSize[0] - 1.5*m_Dimension , middleY-m_Dimension/2);

  //up
  TextSourceUpActor->GetPositionCoordinate()->SetCoordinateSystemToDisplay();
  TextSourceUpActor->SetPosition(middleX - m_Dimension/4 , renderSize[1]-2*m_Dimension);

}
//----------------------------------------------------------------------------
void vtkMAFTextOrientator::SetBackgroundVisibility(bool show)
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
void vtkMAFTextOrientator::SetTextColor(double red, double green, double blue)
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
void vtkMAFTextOrientator::SetBackgroundColor(double red, double green, double blue)
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
  tpdf->SetInput(TextSourceLeft->GetOutput());
  tpdf->Update();
  
  TextSourceLeft->GetOutput()->DeepCopy(tpdf->GetOutput());
  TextSourceLeft->GetOutput()->Update();
  //down
  tpdf->SetInput(TextSourceDown->GetOutput());
  tpdf->Update();
  
  TextSourceDown->GetOutput()->DeepCopy(tpdf->GetOutput());
  TextSourceDown->GetOutput()->Update();
  //right
  tpdf->SetInput(TextSourceRight->GetOutput());
  tpdf->Update();

  TextSourceRight->GetOutput()->DeepCopy(tpdf->GetOutput());
  TextSourceRight->GetOutput()->Update();
  //up
  tpdf->SetInput(TextSourceUp->GetOutput());
  tpdf->Update();

  TextSourceUp->GetOutput()->DeepCopy(tpdf->GetOutput());
  TextSourceUp->GetOutput()->Update();
}
//----------------------------------------------------------------------------
void vtkMAFTextOrientator::SetSingleActorVisibility(int actor, bool show)
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