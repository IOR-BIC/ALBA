/*=========================================================================

  Program:   Multimod Fundation Library
  Module:    $RCSfile: vtkMAFProfilingActor.cxx,v $
  Language:  C++
  Date:      $Date: 2009-01-29 11:17:14 $
  Version:   $Revision: 1.1.2.1 $
  Authors:   Daniele Giunchi
  Project:   MultiMod Project (www.ior.it/multimod)

==========================================================================*/
#include "mafDefines.h"
#include "vtkMAFProfilingActor.h"

#include "vtkObjectFactory.h"
#include "vtkTimerLog.h"
#include "vtkRenderer.h"

vtkCxxRevisionMacro(vtkMAFProfilingActor, "$Revision: 1.1.2.1 $");
vtkStandardNewMacro(vtkMAFProfilingActor);

#define MAXIMUM_FPS 120.

//------------------------------------------------------------------------------
vtkMAFProfilingActor::vtkMAFProfilingActor()
//------------------------------------------------------------------------------
{
	TextFPS = NULL;
  Timer = NULL;

  UpdateTime = VTK_DOUBLE_MAX;
  UpdateFrequency = 0.5;
  FPSCreate();
}
//------------------------------------------------------------------------------
vtkMAFProfilingActor::~vtkMAFProfilingActor()
//------------------------------------------------------------------------------
{
  if(TextFPS)
  {
    TextFPS->Delete();
  }
  if(Timer)
  {
    Timer->Delete();
  }
}
//------------------------------------------------------------------------------
void vtkMAFProfilingActor::PrintSelf(ostream& os, vtkIndent indent)
//------------------------------------------------------------------------------
{
  this->Superclass::PrintSelf(os,indent);
}
//------------------------------------------------------------------------------
int vtkMAFProfilingActor::RenderOverlay(vtkViewport *viewport)
//------------------------------------------------------------------------------
{
  vtkRenderer *ren = static_cast<vtkRenderer *>(viewport);

  FPSUpdate(ren);
  this->Modified();

	TextFPS->RenderOverlay(viewport);
  return 1;
  
}
//------------------------------------------------------------------------------
int vtkMAFProfilingActor::RenderOpaqueGeometry(vtkViewport *viewport)
//------------------------------------------------------------------------------
{
	vtkRenderer *ren = static_cast<vtkRenderer *>(viewport);

	FPSUpdate(ren);
	this->Modified();

	TextFPS->RenderOpaqueGeometry(viewport);
	
	return 0;
}
//------------------------------------------------------------------------------
void vtkMAFProfilingActor::FPSCreate()
//------------------------------------------------------------------------------
{
  Timer = vtkTimerLog::New();
  if (TextFPS == NULL) 
    TextFPS = vtkTextActor::New();
}
//----------------------------------------------------------------------------
void vtkMAFProfilingActor::FPSUpdate(vtkRenderer *ren)
//----------------------------------------------------------------------------
{
  Timer->StopTimer();
  double render_time = ren->GetLastRenderTimeInSeconds();
  Timer->StartTimer();

  UpdateTime += render_time;

  double fps = 1.0/render_time;

  int *size = ren->GetSize();
  TextFPS->SetPosition(10,size[1]-40);
  TextFPS->Modified();

  if(UpdateTime < UpdateFrequency || fps > MAXIMUM_FPS) return;

  UpdateTime = 0.;
  sprintf(TextBuff,"fps: %.1f \nrender time: %.3f s",fps, render_time);
  TextFPS->SetInput(this->TextBuff);

  //mafLogMessage(TextBuff);

  // output to console
  /*static int i=0;
  ++i;
  cout << "Frame " << i << "\t" << "Render-Time = " << render_time << " sec"
        << "\t" << "Frame-Rate  = " << fps << " 1/sec" << endl;*/
}
