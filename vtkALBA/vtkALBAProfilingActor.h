/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAProfilingActor
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*//*=========================================================================

  Program:   Multimod Fundation Library
  Module:    $RCSfile: vtkALBAProfilingActor.h,v $
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

#ifndef __vtkALBAProfilingActor_h
#define __vtkALBAProfilingActor_h

#include "albaConfigure.h" 


#include "vtkTextActor.h"

class vtkViewport;
class vtkRenderer;
class vtkTimerLog;
class vtkMapper;

/**
class name: vtkALBAProfilingActor
an actor displaying the frame per second value and the time elapsed between
last render and previous render.
*/
class ALBA_EXPORT vtkALBAProfilingActor : public vtkActor2D
{
 public:
  /** RTTI Macro */
  vtkTypeMacro(vtkALBAProfilingActor,vtkActor2D);
  /** Print Object Information */
  void PrintSelf(ostream& os, vtkIndent indent);
  /** create an instance of the object */
  static	vtkALBAProfilingActor *New();
  
  /** Draw the object to the screen */
  int	 RenderOverlay(vtkViewport *viewport);
  /** Draw the object to the screen */
  int	 RenderOpaqueGeometry(vtkViewport *viewport);
  /** Draw the object to the screen */
  int	 RenderTranslucentGeometry(vtkViewport *viewport)  {return 0;}
 
protected:
    /** constructor */
					vtkALBAProfilingActor();
    /** destructor */
					~vtkALBAProfilingActor();
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
  vtkALBAProfilingActor(const vtkALBAProfilingActor&);
  /** operator =, not implemented */
  void operator=(const vtkALBAProfilingActor&);  // Not implemented.
};
#endif
