/*=========================================================================

  Program:   Multimod Fundation Library
  Module:    $RCSfile: vtkMAFTextOrientator.cxx,v $
  Language:  C++
  Date:      $Date: 2007-12-17 10:01:51 $
  Version:   $Revision: 1.3 $
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


vtkCxxRevisionMacro(vtkMAFTextOrientator, "$Revision: 1.3 $");
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
  
  //left
  m_TextSourceLeftActor->GetPositionCoordinate()->SetCoordinateSystemToDisplay();
  m_TextSourceLeftActor->SetPosition(m_Dimension/4 , middleY-m_Dimension/2);

  //down
  m_TextSourceDownActor->GetPositionCoordinate()->SetCoordinateSystemToDisplay();
  m_TextSourceDownActor->SetPosition(middleX - m_Dimension/4 , m_Dimension/4);

  //right
  m_TextSourceRightActor->GetPositionCoordinate()->SetCoordinateSystemToDisplay();
  m_TextSourceRightActor->SetPosition(renderSize[0] - 1.5*m_Dimension , middleY-m_Dimension/2);

  //up
  m_TextSourceUpActor->GetPositionCoordinate()->SetCoordinateSystemToDisplay();
  m_TextSourceUpActor->SetPosition(middleX - m_Dimension/4 , renderSize[1]-2*m_Dimension);

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