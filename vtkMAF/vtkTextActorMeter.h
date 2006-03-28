/*=========================================================================

  Program:   Multimod Fundation Library
  Module:    $RCSfile: vtkTextActorMeter.h,v $
  Language:  C++
  Date:      $Date: 2006-03-28 08:35:13 $
  Version:   $Revision: 1.1 $
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

#ifndef __vtkTextActorMeter_h
#define __vtkTextActorMeter_h


#include "vtkMAFConfigure.h" //??

#include "vtkActor.h"
#include "vtkActor2D.h"
#include "vtkTextActor.h"

class vtkViewport;
class vtkCamera;
class vtkRenderer;



/**
an actor displaying world X and Y axes as an Actor2D.
Camera must be in ParallelProjection and aligned with world axis
*/

//-----------------------------------------------------------------------------
class VTK_vtkMAF_EXPORT vtkTextActorMeter : public vtkActor2D
//-----------------------------------------------------------------------------
{
 public:
  vtkTypeRevisionMacro(vtkTextActorMeter,vtkActor2D);
  void PrintSelf(ostream& os, vtkIndent indent);
  static	vtkTextActorMeter *New();
  
  int	 RenderOverlay(vtkViewport *viewport);
  int	 RenderOpaqueGeometry(vtkViewport *viewport);      
  int	 RenderTranslucentGeometry(vtkViewport *viewport)  {return 0;}
 
  char*   GetText(){return TextActor->GetInput();}
  void 	  SetText(const char * inputString){TextActor->SetInput(inputString);}
  void    SetTextPosition(double pos[3]){TextPosition[0]=pos[0];TextPosition[1]=pos[1];TextPosition[2]=pos[2];TextPosition[3]=1;}
  double* GetTextPosition();
	
  
protected:
					vtkTextActorMeter();
					~vtkTextActorMeter();
	//methods
	
	void			MeterCreate();	
	void			MeterUpdate(vtkRenderer *ren);
   //variables
    vtkTextActor *TextActor;
    double TextPosition[4];

	
private:
  // hide the two parameter Render() method from the user and the compiler.
  virtual void Render(vtkRenderer *, vtkMapper *) {};
private:
  vtkTextActorMeter(const vtkTextActorMeter&);  	// Not implemented.
  void operator=(const vtkTextActorMeter&);  // Not implemented.
};
#endif
