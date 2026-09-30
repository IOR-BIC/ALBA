/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBATextOrientator
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*//*=========================================================================

  Program:   Multimod Fundation Library
  Module:    $RCSfile: vtkALBATextOrientator.h,v $
  Language:  C++
  Date:      $Date: 2009-11-17 09:32:20 $
  Version:   $Revision: 1.3.2.3 $
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

#ifndef __vtkALBATextOrientator_h
#define __vtkALBATextOrientator_h


#include "albaConfigure.h" 


#include "vtkActor2D.h"
#include "vtkTextSource.h"


class vtkViewport;
class vtkCamera;
class vtkRenderer;
class vtkProperty2D;
class vtkPolyDataMapper2D;
class vtkMapper;


/**
class name: vtkALBATextOrientator
an actor displaying world X and Y axes as an Actor2D.
Camera must be in ParallelProjection and aligned with world axis
*/
class ALBA_EXPORT vtkALBATextOrientator : public vtkActor2D
{
 public:
  /** RTTI Macro */
  vtkTypeMacro(vtkALBATextOrientator,vtkActor2D);
  /** Print Object Information */
  void PrintSelf(ostream& os, vtkIndent indent);
  /** create an instance of the object */
  static	vtkALBATextOrientator *New();
  
  
  enum ID_TEXT_ORIENTATOR
  {
    ID_ACTOR_LEFT = 0,
    ID_ACTOR_DOWN,
    ID_ACTOR_RIGHT,
    ID_ACTOR_UP
  };
  
  /** retrieve text left */
  const char*   GetTextLeft(){return TextSourceLeft->GetText();}
  /** retrieve text down */
  const char*   GetTextDown(){return TextSourceDown->GetText();} 
  /** retrieve text right */
  const char*   GetTextRight(){return TextSourceRight->GetText();}
  /** retrieve text up */
  const char*   GetTextUp(){return TextSourceUp->GetText();}

  /** set text left */
  void 	  SetTextLeft(const char * inputString){TextSourceLeft->SetText(inputString);}
  /** set text down */
  void 	  SetTextDown(const char * inputString){TextSourceDown->SetText(inputString);} 
  /** set text right */
  void 	  SetTextRight(const char * inputString){TextSourceRight->SetText(inputString);}
  /** set text up */
  void 	  SetTextUp(const char * inputString){TextSourceUp->SetText(inputString);}
  
  /** Set Visibility for actor*/
  void SetSingleActorVisibility(int actor, bool show);
  
  /** Set Visibility for background*/
  void SetBackgroundVisibility(bool show);

  /** Set text color*/
  void SetTextColor(double red, double green, double blue);

  /** Set background color*/
  void SetBackgroundColor(double red, double green, double blue);

  /** Set scale actor*/
  void SetScale(double multiple);

  /** Flag for attaching the orientator in specific position*/
  void SetAttachPositionFlag(bool enable){AttachPositionFlag = enable;};

  /** set the points in 3d in which orientator must be attached */
  void SetAttachPositions(double up[3], double right[3], double Down[3], double left[3]);
  /** set 2d display offset for up label */
  void SetDisplayOffsetUp(int x, int y){DisplayOffsetUp[0] = x; DisplayOffsetUp[1]=y;};
  /** set 2d display offset for right label */
  void SetDisplayOffsetRight(int x, int y){DisplayOffsetRight[0] = x; DisplayOffsetRight[1]=y;};
  /** set 2d display offset for down label */
  void SetDisplayOffsetDown(int x, int y){DisplayOffsetDown[0] = x; DisplayOffsetDown[1]=y;};
  /** set 2d display offset for left label */
  void SetDisplayOffsetLeft(int x, int y){DisplayOffsetLeft[0] = x; DisplayOffsetLeft[1]=y;};
protected:
    /** constructor */
					vtkALBATextOrientator();
	/** destructor */
					~vtkALBATextOrientator();
	/** create orientator actor */
	void			OrientatorCreate();	
    /** update orientator actor */
	void			OrientatorUpdate(vtkRenderer *ren);
  /** Draw the object to the screen */
  int	 RenderOverlay(vtkViewport *viewport);
  /** Draw the object to the screen */
  int	 RenderOpaqueGeometry(vtkViewport *viewport);      
  /** Draw the object to the screen */
  int	 RenderTranslucentGeometry(vtkViewport *viewport)  {return 0;}

  //variables
  int                     Dimension;

  bool AttachPositionFlag;
  double AttachPositionUp[3];
  double AttachPositionRight[3];
  double AttachPositionDown[3];
  double AttachPositionLeft[3];

  int DisplayOffsetUp[2];  
  int DisplayOffsetRight[2];
  int DisplayOffsetDown[2];
  int DisplayOffsetLeft[2];

  vtkActor2D						 *TextSourceLeftActor;
  vtkPolyDataMapper2D		 *TextSourceLeftMapper;
  vtkTextSource          *TextSourceLeft;

  vtkActor2D						 *TextSourceDownActor;
  vtkPolyDataMapper2D		 *TextSourceDownMapper;
  vtkTextSource          *TextSourceDown;

  vtkActor2D						 *TextSourceRightActor;
  vtkPolyDataMapper2D		 *TextSourceRightMapper;
  vtkTextSource          *TextSourceRight;

  vtkActor2D						 *TextSourceUpActor;
  vtkPolyDataMapper2D		 *TextSourceUpMapper;
  vtkTextSource          *TextSourceUp;
	
private:
  /** hide the two paraOrientator Render() method from the user and the compiler. */
  virtual void Render(vtkRenderer *, vtkMapper *) {};
private:
  /** Copy Constructor , not implemented */
  vtkALBATextOrientator(const vtkALBATextOrientator&);  	// Not implemented.
  /** operator =, not implemented */
  void operator=(const vtkALBATextOrientator&);  // Not implemented.
};
#endif
