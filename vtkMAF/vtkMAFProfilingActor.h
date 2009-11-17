/*=========================================================================

  Program:   Multimod Fundation Library
  Module:    $RCSfile: vtkMAFProfilingActor.h,v $
  Language:  C++
  Date:      $Date: 2009-11-17 09:32:20 $
  Version:   $Revision: 1.1.2.2 $
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
class name: vtkMAFProfilingActor
an actor displaying the frame per second value and the time elapsed between
last render and previous render.
*/
class VTK_vtkMAF_EXPORT vtkMAFProfilingActor : public vtkActor2D
{
 public:
  /** RTTI Macro */
  vtkTypeRevisionMacro(vtkMAFProfilingActor,vtkActor2D);
  /** Print Object Information */
  void PrintSelf(ostream& os, vtkIndent indent);
  /** create an instance of the object */
  static	vtkMAFProfilingActor *New();
  
  /** Draw the object to the screen */
  int	 RenderOverlay(vtkViewport *viewport);
  /** Draw the object to the screen */
  int	 RenderOpaqueGeometry(vtkViewport *viewport);
  /** Draw the object to the screen */
  int	 RenderTranslucentGeometry(vtkViewport *viewport)  {return 0;}
 
protected:
    /** constructor */
					vtkMAFProfilingActor();
    /** destructor */
					~vtkMAFProfilingActor();
	/** Create FPS Actor */
	void			FPSCreate();
    /** Update FPS Actor */
	void			FPSUpdate(vtkRenderer *ren);
   //variables
  vtkTextActor *TextFPS;
  vtkTimerLog  *Timer;
  double        UpdateTime;
  double        UpdateFrequency;

	char TextBuff[128];
	
private:
  /** hide the two paraOrientator Render() method from the user and the compiler. */
  virtual void Render(vtkRenderer *, vtkMapper *) {};
private:
  /** Copy Constructor , not implemented */
  vtkMAFProfilingActor(const vtkMAFProfilingActor&);
  /** operator =, not implemented */
  void operator=(const vtkMAFProfilingActor&);  // Not implemented.
};
#endif
