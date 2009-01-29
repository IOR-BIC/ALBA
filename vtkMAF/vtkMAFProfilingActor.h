/*=========================================================================

  Program:   Multimod Fundation Library
  Module:    $RCSfile: vtkMAFProfilingActor.h,v $
  Language:  C++
  Date:      $Date: 2009-01-29 11:17:14 $
  Version:   $Revision: 1.1.2.1 $
  Authors:   Daniele Giunchi
  Project:   MultiMod Project (www.ior.it/multimod)

==========================================================================
  Copyright (c) 2002/2003 
  CINECA - Interuniversity Consortium (www.cineca.it)
  v. Magnanelli 6/3
  40033 Casalecchio di Reno (BO)
  Italy
  ph. +39-051-6171411 (90 lines) - Fax +39-051-6132198
========================================================================= */

#ifndef __vtkMAFProfilingActor_h
#define __vtkMAFProfilingActor_h

#include "vtkMAFConfigure.h" //??


#include "vtkTextActor.h"

class vtkViewport;
class vtkRenderer;
class vtkTimerLog;
class vtkMapper;

/**
an actor displaying the frame per second value and the time elapsed between
last render and previous render.
*/

//-----------------------------------------------------------------------------
class VTK_vtkMAF_EXPORT vtkMAFProfilingActor : public vtkActor2D
//-----------------------------------------------------------------------------
{
 public:
  vtkTypeRevisionMacro(vtkMAFProfilingActor,vtkActor2D);
  void PrintSelf(ostream& os, vtkIndent indent);
  static	vtkMAFProfilingActor *New();
  
  int	 RenderOverlay(vtkViewport *viewport);
  int	 RenderOpaqueGeometry(vtkViewport *viewport);      
  int	 RenderTranslucentGeometry(vtkViewport *viewport)  {return 0;}
 
protected:
					vtkMAFProfilingActor();
					~vtkMAFProfilingActor();
	//methods
	void			FPSCreate();	
	void			FPSUpdate(vtkRenderer *ren);
   //variables
  vtkTextActor *TextFPS;
  vtkTimerLog  *Timer;
  double        UpdateTime;
  double        UpdateFrequency;

	char TextBuff[128];
	
private:
  // hide the two paraOrientator Render() method from the user and the compiler.
  virtual void Render(vtkRenderer *, vtkMapper *) {};
private:
  vtkMAFProfilingActor(const vtkMAFProfilingActor&);  	// Not implemented.
  void operator=(const vtkMAFProfilingActor&);  // Not implemented.
};
#endif
