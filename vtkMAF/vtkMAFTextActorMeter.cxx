/*=========================================================================

 Program: MAF2
 Module: vtkMAFTextActorMeter
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkMAFTextActorMeter.h"

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



vtkCxxRevisionMacro(vtkMAFTextActorMeter, "$Revision: 1.1.2.1 $");
vtkStandardNewMacro(vtkMAFTextActorMeter);
//------------------------------------------------------------------------------
vtkMAFTextActorMeter::vtkMAFTextActorMeter()
//------------------------------------------------------------------------------
{
  TextActor = NULL;
  MeterCreate();
}
//------------------------------------------------------------------------------
vtkMAFTextActorMeter::~vtkMAFTextActorMeter()
//------------------------------------------------------------------------------
{
  if(TextActor) TextActor->Delete();
}
//------------------------------------------------------------------------------
void vtkMAFTextActorMeter::PrintSelf(ostream& os, vtkIndent indent)
//------------------------------------------------------------------------------
{
  this->Superclass::PrintSelf(os,indent);
}
//------------------------------------------------------------------------------
int vtkMAFTextActorMeter::RenderOverlay(vtkViewport *viewport)
//------------------------------------------------------------------------------
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
int vtkMAFTextActorMeter::RenderOpaqueGeometry(vtkViewport *viewport)
//------------------------------------------------------------------------------
{
	TextActor->RenderOpaqueGeometry(viewport);
	
	return 0;
}
//------------------------------------------------------------------------------
void vtkMAFTextActorMeter::MeterCreate()
//------------------------------------------------------------------------------
{
  if (TextActor == NULL) TextActor = vtkTextActor::New();
  TextActor->SetInput("");

  TextActor->SetPosition(-30 ,-30);
  TextActor->Modified();

}
//----------------------------------------------------------------------------
void vtkMAFTextActorMeter::MeterUpdate(vtkRenderer *ren)
//----------------------------------------------------------------------------
{
  double dp[3];
  
  ren->SetWorldPoint(TextPosition);
  ren->WorldToDisplay();
  ren->GetDisplayPoint(dp);
  
  TextActor->GetPositionCoordinate()->SetCoordinateSystemToDisplay();
  TextActor->SetPosition(dp[0] ,dp[1]);
  
}

//----------------------------------------------------------------------------
double *vtkMAFTextActorMeter::GetTextPosition()
//----------------------------------------------------------------------------
{
	return TextPosition;
}
//----------------------------------------------------------------------------
void vtkMAFTextActorMeter::SetColor(double r,double g,double b)
//----------------------------------------------------------------------------
{
  TextActor->GetTextProperty()->SetColor(r,g,b);
}
