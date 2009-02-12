/*=========================================================================

  Program:   Multimod Fundation Library
  Module:    $RCSfile: vtkMAFGlobalAxisCoordinate.cxx,v $
  Language:  C++
  Date:      $Date: 2009-02-12 10:53:24 $
  Version:   $Revision: 1.1.2.1 $
  Authors:   Silvano Imboden 
  Project:   MultiMod Project (www.ior.it/multimod)

==========================================================================*/

#include "vtkMAFGlobalAxisCoordinate.h"
#include "vtkViewport.h"
#include "vtkObjectFactory.h"

#include "vtkCamera.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"

//vtkCxxRevisionMacro(vtkMAFGlobalAxisCoordinate, "$Revision: 1.1.2.1 $");
vtkStandardNewMacro(vtkMAFGlobalAxisCoordinate);

//----------------------------------------------------------------------------
vtkMAFGlobalAxisCoordinate::vtkMAFGlobalAxisCoordinate()
//----------------------------------------------------------------------------
{
  this->CoordinateSystem = VTK_USERDEFINED;

	Camera = vtkCamera::New();
	Renderer = vtkRenderer::New();
  Renderer->SetActiveCamera(Camera);
	Renderwindow = vtkRenderWindow::New();
  Renderwindow->AddRenderer(Renderer);
  Renderwindow->SetSize(40,40);
}
//----------------------------------------------------------------------------
vtkMAFGlobalAxisCoordinate::~vtkMAFGlobalAxisCoordinate()
//----------------------------------------------------------------------------
{
	Camera->Delete();
  Renderer->Delete();
  Renderwindow->Delete();
}
//----------------------------------------------------------------------------
double *vtkMAFGlobalAxisCoordinate::GetComputedUserDefinedValue(vtkViewport *viewport)
//----------------------------------------------------------------------------
{
	vtkRenderer *r = (vtkRenderer *)viewport;
  vtkCamera *c = r->GetActiveCamera();

  Camera->SetViewAngle(c->GetViewAngle());
  Camera->SetPosition(c->GetPosition());
  Camera->SetFocalPoint(c->GetFocalPoint());
  Camera->SetViewUp(c->GetViewUp());
  Camera->SetParallelProjection(c->GetParallelProjection());
  Camera->SetParallelScale(c->GetParallelScale());

	double b = 0.5;
	Renderer->ResetCamera(-b,b,-b,b,-b,b);

  double w[4];
	w[0] = this->Value[0];
	w[1] = this->Value[1];
	w[2] = this->Value[2];
	w[3] = 1;

	Renderer->SetWorldPoint(w);
	Renderer->WorldToDisplay();
	Renderer->GetDisplayPoint(ComputedUserDefinedValue);  

  return this->ComputedUserDefinedValue;
}
