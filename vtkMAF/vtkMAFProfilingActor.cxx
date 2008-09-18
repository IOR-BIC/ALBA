/*=========================================================================

  Program:   Multimod Fundation Library
  Module:    $RCSfile: vtkMAFProfilingActor.cxx,v $
  Language:  C++
  Date:      $Date: 2008-09-18 10:25:19 $
  Version:   $Revision: 1.1 $
  Authors:   Daniele Giunchi
  Project:   MultiMod Project (www.ior.it/multimod)

==========================================================================*/
#include "mafDefines.h"
#include "vtkMAFProfilingActor.h"

#include "vtkObjectFactory.h"
#include "vtkTimerLog.h"
#include "vtkRenderer.h"

vtkCxxRevisionMacro(vtkMAFProfilingActor, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkMAFProfilingActor);

#define MAXIMUM_FPS 120.

//------------------------------------------------------------------------------
vtkMAFProfilingActor::vtkMAFProfilingActor()
//------------------------------------------------------------------------------
{
	m_TextFPS = NULL;
  m_Timer = NULL;

  m_UpdateTime = VTK_DOUBLE_MAX;
  m_UpdateFrequency = 0.5;
  FPSCreate();
}
//------------------------------------------------------------------------------
vtkMAFProfilingActor::~vtkMAFProfilingActor()
//------------------------------------------------------------------------------
{
  if(m_TextFPS)
  {
    m_TextFPS->Delete();
  }
  if(m_Timer)
  {
    m_Timer->Delete();
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

	m_TextFPS->RenderOverlay(viewport);
  return 1;
  
}
//------------------------------------------------------------------------------
int vtkMAFProfilingActor::RenderOpaqueGeometry(vtkViewport *viewport)
//------------------------------------------------------------------------------
{
	vtkRenderer *ren = static_cast<vtkRenderer *>(viewport);

	FPSUpdate(ren);
	this->Modified();

	m_TextFPS->RenderOpaqueGeometry(viewport);
	
	return 0;
}
//------------------------------------------------------------------------------
void vtkMAFProfilingActor::FPSCreate()
//------------------------------------------------------------------------------
{
  m_Timer = vtkTimerLog::New();
  if (m_TextFPS == NULL) 
    m_TextFPS = vtkTextActor::New();
}
//----------------------------------------------------------------------------
void vtkMAFProfilingActor::FPSUpdate(vtkRenderer *ren)
//----------------------------------------------------------------------------
{
  m_Timer->StopTimer();
  double render_time = ren->GetLastRenderTimeInSeconds();
  m_Timer->StartTimer();

  m_UpdateTime += render_time;

  double fps = 1.0/render_time;

  int *size = ren->GetSize();
  m_TextFPS->SetPosition(10,size[1]-40);
  m_TextFPS->Modified();

  if(m_UpdateTime < m_UpdateFrequency || fps > MAXIMUM_FPS) return;

  m_UpdateTime = 0.;
  sprintf(m_TextBuff,"fps: %.1f \nrender time: %.3f s",fps, render_time);
  m_TextFPS->SetInput(this->m_TextBuff);

  //mafLogMessage(m_TextBuff);

  // output to console
  /*static int i=0;
  ++i;
  cout << "Frame " << i << "\t" << "Render-Time = " << render_time << " sec"
        << "\t" << "Frame-Rate  = " << fps << " 1/sec" << endl;*/
}
