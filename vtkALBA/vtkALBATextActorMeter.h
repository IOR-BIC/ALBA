/*=========================================================================
Program:   ALBA
Module:    vtkALBATextActorMeter.h
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

#ifndef __vtkALBATextActorMeter_h
#define __vtkALBATextActorMeter_h

//------------------------------------------------------------------------------
// Includes:
//------------------------------------------------------------------------------
#include "albaConfigure.h" 
#include "vtkActor.h"
#include "vtkActor2D.h"
#include "vtkTextActor.h"

//------------------------------------------------------------------------------
// Forward declarations
//------------------------------------------------------------------------------
class vtkViewport;
class vtkCamera;
class vtkRenderer;

/**
class vtkALBATextActorMeter:
  An actor displaying world X and Y axes as an Actor2D.
  Camera must be in ParallelProjection and aligned with world axis.
*/
//-----------------------------------------------------------------------------
class ALBA_EXPORT vtkALBATextActorMeter : public vtkActor2D
{
 public:
  /** RTTI macro. */
  vtkTypeMacro(vtkALBATextActorMeter,vtkActor2D);
  /** Print useful information. */
  void PrintSelf(ostream& os, vtkIndent indent);
  /** Create an instance of the object. */
  static	vtkALBATextActorMeter *New();
  
  /** Method is intended for rendering. */
  int	 RenderOverlay(vtkViewport *viewport);
  /** Method is intended for rendering Opaque Geometry */
  int	 RenderOpaqueGeometry(vtkViewport *viewport);
  /** Method is intended for rendering Translucent Geometry */
  int	 RenderTranslucentGeometry(vtkViewport *viewport)  {return 0;}
 
  /** Retrieve text actor input. */
  char*   GetText(){return TextActor->GetInput();}
  /** Modify text actor input. */
  void 	  SetText(const char * inputString){TextActor->SetInput(inputString);}
  /** Modify text actor position. */
  void    SetTextPosition(double pos[3]){TextPosition[0]=pos[0];TextPosition[1]=pos[1];TextPosition[2]=pos[2];TextPosition[3]=1;}
  /** Retrieve text actor position. */
  double* GetTextPosition();

  /** Modify text actor color. */
  void SetColor(double r,double g,double b);
	
protected:
  /** constructor. */
	vtkALBATextActorMeter();
  /** destructor. */
	~vtkALBATextActorMeter();
	
  /** Create Instances of text actor. */
	void			MeterCreate();	
  /** Update 2d Position of the meter */
	void			MeterUpdate(vtkRenderer *ren);
   
  //variables
  vtkTextActor *TextActor;
  double TextPosition[4];
		
private:
  /** Hide the two parameter Render() method from the user and the compiler. */
  virtual void Render(vtkRenderer *, vtkMapper *) {};
  /** Copy Constructor Not implemented. */
  vtkALBATextActorMeter(const vtkALBATextActorMeter&);
  /** Assign Operator Not implemented. */
  void operator=(const vtkALBATextActorMeter&);
};
#endif // __vtkALBATextActorMeter_h
