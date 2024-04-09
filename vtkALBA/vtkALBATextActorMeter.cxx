/*=========================================================================
Program:   ALBA
Module:    vtkALBATextActorMeter.cxx
Language:  C++
Date:      $Date: 2009-05-19 14:29:53 $
Version:   $Revision: 1.1 $
Authors:   Daniele Giunchi
==========================================================================
Copyright (c) BIC-IOR 2019 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#include "vtkALBATextActorMeter.h"

#include "vtkRenderer.h"
#include "vtkCamera.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkProperty.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"

#include "vtkActor.h"
#include "vtkTextActor.h"
#include "vtkTextProperty.h"



vtkStandardNewMacro(vtkALBATextActorMeter);
//------------------------------------------------------------------------------
vtkALBATextActorMeter::vtkALBATextActorMeter()
{
  TextActor = NULL;
  MeterCreate();
}
//------------------------------------------------------------------------------
vtkALBATextActorMeter::~vtkALBATextActorMeter()
{
  if(TextActor) TextActor->Delete();
}
//------------------------------------------------------------------------------
void vtkALBATextActorMeter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
//------------------------------------------------------------------------------
int vtkALBATextActorMeter::RenderOverlay(vtkViewport *viewport)
{
  vtkRenderer *ren = static_cast<vtkRenderer *>(viewport);

  vtkCamera *cam = ren->GetActiveCamera();
  if(!cam->GetParallelProjection()) return 0;

  MeterUpdate(ren);
  this->Modified();

  TextActor->RenderOverlay(viewport);
  return 1;  
}
//------------------------------------------------------------------------------
int vtkALBATextActorMeter::RenderOpaqueGeometry(vtkViewport *viewport)
{
	TextActor->RenderOpaqueGeometry(viewport);
	
	return 0;
}
//------------------------------------------------------------------------------
void vtkALBATextActorMeter::MeterCreate()
{
  if (TextActor == NULL) TextActor = vtkTextActor::New();
  TextActor->SetInput("");

  TextActor->SetPosition(-30 ,-30);
  TextActor->Modified();
}
//----------------------------------------------------------------------------
void vtkALBATextActorMeter::MeterUpdate(vtkRenderer *ren)
{
  double dp[3];
  
  ren->SetWorldPoint(TextPosition);
  ren->WorldToDisplay();
  ren->GetDisplayPoint(dp);
  
  TextActor->GetPositionCoordinate()->SetCoordinateSystemToDisplay();
  TextActor->SetPosition(dp[0] ,dp[1]);
}

//----------------------------------------------------------------------------
double *vtkALBATextActorMeter::GetTextPosition()
{
	return TextPosition;
}
//----------------------------------------------------------------------------
void vtkALBATextActorMeter::SetColor(double r,double g,double b)
{
  TextActor->GetTextProperty()->SetColor(r,g,b);
}

void vtkALBATextActorMeter::SetOpacity(double opacity)
{
	TextActor->GetTextProperty()->SetOpacity(opacity);
}
