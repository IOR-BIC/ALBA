/*=========================================================================

  Program:   Multimod Fundation Library
  Module:    $RCSfile: vtkMAFTextOrientator.h,v $
  Language:  C++
  Date:      $Date: 2009-01-29 11:17:14 $
  Version:   $Revision: 1.3.2.2 $
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

#ifndef __vtkMAFTextOrientator_h
#define __vtkMAFTextOrientator_h


#include "vtkMAFConfigure.h" //??


#include "vtkActor2D.h"
#include "vtkTextSource.h"


class vtkViewport;
class vtkCamera;
class vtkRenderer;
class vtkProperty2D;
class vtkPolyDataMapper2D;
class vtkMapper;


/**
an actor displaying world X and Y axes as an Actor2D.
Camera must be in ParallelProjection and aligned with world axis
*/

//-----------------------------------------------------------------------------
class VTK_vtkMAF_EXPORT vtkMAFTextOrientator : public vtkActor2D
//-----------------------------------------------------------------------------
{
 public:
  vtkTypeRevisionMacro(vtkMAFTextOrientator,vtkActor2D);
  void PrintSelf(ostream& os, vtkIndent indent);
  static	vtkMAFTextOrientator *New();
  
  
  enum ID_TEXT_ORIENTATOR
  {
    ID_ACTOR_LEFT = 0,
    ID_ACTOR_DOWN,
    ID_ACTOR_RIGHT,
    ID_ACTOR_UP
  };
  
  const char*   GetTextLeft(){return TextSourceLeft->GetText();}
  const char*   GetTextDown(){return TextSourceDown->GetText();} 
  const char*   GetTextRight(){return TextSourceRight->GetText();}
  const char*   GetTextUp(){return TextSourceUp->GetText();}

  void 	  SetTextLeft(const char * inputString){TextSourceLeft->SetText(inputString);}
  void 	  SetTextDown(const char * inputString){TextSourceDown->SetText(inputString);} 
  void 	  SetTextRight(const char * inputString){TextSourceRight->SetText(inputString);}
  void 	  SetTextUp(const char * inputString){TextSourceUp->SetText(inputString);}
  
  void SetSingleActorVisibility(int actor, bool show);
  
  /** Set Visibility for background*/
  void SetBackgroundVisibility(bool show);

  /** Set text color*/
  void SetTextColor(double red, double green, double blue);

  /** Set background color*/
  void SetBackgroundColor(double red, double green, double blue);

  /** Set scale actor*/
  void SetScale(double multiple);

  void SetAttachPositionFlag(bool enable){AttachPositionFlag = enable;};

  void SetAttachPositions(double up[3], double right[3], double Down[3], double left[3]);
  void SetDisplayOffsetUp(int x, int y){DisplayOffsetUp[0] = x; DisplayOffsetUp[1]=y;};
  void SetDisplayOffsetRight(int x, int y){DisplayOffsetRight[0] = x; DisplayOffsetRight[1]=y;};
  void SetDisplayOffsetDown(int x, int y){DisplayOffsetDown[0] = x; DisplayOffsetDown[1]=y;};
  void SetDisplayOffsetLeft(int x, int y){DisplayOffsetLeft[0] = x; DisplayOffsetLeft[1]=y;};
protected:
					vtkMAFTextOrientator();
					~vtkMAFTextOrientator();
	//methods
	
	void			OrientatorCreate();	
	void			OrientatorUpdate(vtkRenderer *ren);
   
  int	 RenderOverlay(vtkViewport *viewport);
  int	 RenderOpaqueGeometry(vtkViewport *viewport);      
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
  // hide the two paraOrientator Render() method from the user and the compiler.
  virtual void Render(vtkRenderer *, vtkMapper *) {};
private:
  vtkMAFTextOrientator(const vtkMAFTextOrientator&);  	// Not implemented.
  void operator=(const vtkMAFTextOrientator&);  // Not implemented.
};
#endif
