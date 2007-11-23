/*=========================================================================

  Program:   Multimod Fundation Library
  Module:    $RCSfile: vtkMAFTextOrientator.h,v $
  Language:  C++
  Date:      $Date: 2007-11-23 10:18:37 $
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
  
  int	 RenderOverlay(vtkViewport *viewport);
  int	 RenderOpaqueGeometry(vtkViewport *viewport);      
  int	 RenderTranslucentGeometry(vtkViewport *viewport)  {return 0;}
 
  enum ID_TEXT_ORIENTATOR
  {
    ID_ACTOR_LEFT = 0,
    ID_ACTOR_DOWN,
    ID_ACTOR_RIGHT,
    ID_ACTOR_UP
  };
  
  char*   GetTextLeft(){return TextSourceLeft->GetText();}
  char*   GetTextDown(){return TextSourceDown->GetText();} 
  char*   GetTextRight(){return TextSourceRight->GetText();}
  char*   GetTextUp(){return TextSourceUp->GetText();}

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

protected:
					vtkMAFTextOrientator();
					~vtkMAFTextOrientator();
	//methods
	
	void			OrientatorCreate();	
	void			OrientatorUpdate(vtkRenderer *ren);
   //variables

  int                     m_Dimension;

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
