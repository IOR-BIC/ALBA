/*=========================================================================

  Program:   Multimod Fundation Library
  Module:    $RCSfile: vtkTextActorMeter.cxx,v $
  Language:  C++
  Date:      $Date: 2006-03-28 08:35:13 $
  Version:   $Revision: 1.1 $
  Authors:   Daniele Giunchi
  Project:   MultiMod Project (www.ior.it/multimod)

==========================================================================*/

#include "vtkTextActorMeter.h"

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



vtkCxxRevisionMacro(vtkTextActorMeter, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkTextActorMeter);
//------------------------------------------------------------------------------
vtkTextActorMeter::vtkTextActorMeter()
//------------------------------------------------------------------------------
{
  TextActor = NULL;
  MeterCreate();
}
//------------------------------------------------------------------------------
vtkTextActorMeter::~vtkTextActorMeter()
//------------------------------------------------------------------------------
{
  if(TextActor) TextActor->Delete();
}
//------------------------------------------------------------------------------
void vtkTextActorMeter::PrintSelf(ostream& os, vtkIndent indent)
//------------------------------------------------------------------------------
{
  this->Superclass::PrintSelf(os,indent);
}
//------------------------------------------------------------------------------
int vtkTextActorMeter::RenderOverlay(vtkViewport *viewport)
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
int vtkTextActorMeter::RenderOpaqueGeometry(vtkViewport *viewport)
//------------------------------------------------------------------------------
{
	TextActor->RenderOpaqueGeometry(viewport);
	
	return 0;
}
//------------------------------------------------------------------------------
void vtkTextActorMeter::MeterCreate()
//------------------------------------------------------------------------------
{
  if (TextActor == NULL) TextActor = vtkTextActor::New();
  TextActor->SetInput("");

  TextActor->SetPosition(-30 ,-30);
  TextActor->Modified();

}
//----------------------------------------------------------------------------
void vtkTextActorMeter::MeterUpdate(vtkRenderer *ren)
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
double *vtkTextActorMeter::GetTextPosition()
//----------------------------------------------------------------------------
{
	return TextPosition;
}