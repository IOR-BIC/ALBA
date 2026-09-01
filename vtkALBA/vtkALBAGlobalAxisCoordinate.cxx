/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAGlobalAxisCoordinate
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkALBAGlobalAxisCoordinate.h"
#include "vtkViewport.h"
#include "vtkObjectFactory.h"

#include "vtkCamera.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"

vtkStandardNewMacro(vtkALBAGlobalAxisCoordinate);

//----------------------------------------------------------------------------
vtkALBAGlobalAxisCoordinate::vtkALBAGlobalAxisCoordinate()
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
vtkALBAGlobalAxisCoordinate::~vtkALBAGlobalAxisCoordinate()
//----------------------------------------------------------------------------
{
	Camera->Delete();
  Renderer->Delete();
  Renderwindow->Delete();
}
//----------------------------------------------------------------------------
double *vtkALBAGlobalAxisCoordinate::GetComputedUserDefinedValue(vtkViewport *viewport)
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
