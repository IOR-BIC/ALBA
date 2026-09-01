/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAProfilingActor
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
//#include "albaDefines.h"
#include "vtkALBAProfilingActor.h"

#include "vtkObjectFactory.h"
#include "vtkTimerLog.h"
#include "vtkRenderer.h"

vtkStandardNewMacro(vtkALBAProfilingActor);

#define MAXIMUM_FPS 120.

//------------------------------------------------------------------------------
vtkALBAProfilingActor::vtkALBAProfilingActor()
//------------------------------------------------------------------------------
{
	TextFPS = NULL;
  Timer = NULL;

  UpdateTime = VTK_DOUBLE_MAX;
  UpdateFrequency = 0.5;
  FPSCreate();
}
//------------------------------------------------------------------------------
vtkALBAProfilingActor::~vtkALBAProfilingActor()
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
void vtkALBAProfilingActor::PrintSelf(ostream& os, vtkIndent indent)
//------------------------------------------------------------------------------
{
  this->Superclass::PrintSelf(os,indent);
}
//------------------------------------------------------------------------------
int vtkALBAProfilingActor::RenderOverlay(vtkViewport *viewport)
//------------------------------------------------------------------------------
{
  vtkRenderer *ren = static_cast<vtkRenderer *>(viewport);

  FPSUpdate(ren);
  this->Modified();

	TextFPS->RenderOverlay(viewport);
  return 1;
  
}
//------------------------------------------------------------------------------
int vtkALBAProfilingActor::RenderOpaqueGeometry(vtkViewport *viewport)
//------------------------------------------------------------------------------
{
	vtkRenderer *ren = static_cast<vtkRenderer *>(viewport);

	FPSUpdate(ren);
	this->Modified();

	TextFPS->RenderOpaqueGeometry(viewport);
	
	return 0;
}
//------------------------------------------------------------------------------
void vtkALBAProfilingActor::FPSCreate()
//------------------------------------------------------------------------------
{
  Timer = vtkTimerLog::New();
  if (TextFPS == NULL) 
    TextFPS = vtkTextActor::New();
}
//----------------------------------------------------------------------------
void vtkALBAProfilingActor::FPSUpdate(vtkRenderer *ren)
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

  //albaLogMessage(TextBuff);

  // output to console
  /*static int i=0;
  ++i;
  cout << "Frame " << i << "\t" << "Render-Time = " << render_time << " sec"
        << "\t" << "Frame-Rate  = " << fps << " 1/sec" << endl;*/
}
