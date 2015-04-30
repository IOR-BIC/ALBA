/*=========================================================================

 Program: MAF2
 Module: vtkMAFRulerActor2D
 Authors: Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*//*=========================================================================

  Program:   Multimod Fundation Library
  Module:    $RCSfile: vtkMAFRulerActor2D.h,v $
  Language:  C++
  Date:      $Date: 2011-05-25 09:56:41 $
  Version:   $Revision: 1.1.2.1 $
  Authors:   Silvano Imboden 
  Project:   MultiMod Project (www.ior.it/multimod)

==========================================================================
  Copyright (c) 2002/2003 
  CINECA - Interuniversity Consortium (www.cineca.it)
  v. Magnanelli 6/3
  40033 Casalecchio di Reno (BO)
  Italy
  ph. +39-051-6171411 (90 lines) - Fax +39-051-6132198
========================================================================= */

#ifndef __vtkMAFRulerActor2D_h
#define __vtkMAFRulerActor2D_h

#define DEFAULT_GRID_COLOR 0.5

#include "mafConfigure.h"

#include "vtkActor.h"
#include "vtkActor2D.h"
#include "vtkTextActor.h"

class vtkViewport;
class vtkCamera;
class vtkPoints;

/**
-- an actor displaying X and Y axes as Actor2D
-- work only with ParallelCamera, and with a camera looking down-to Z -- should be generalized
*/

# define NUM_LAB 30

//-----------------------------------------------------------------------------
class __declspec( dllexport ) vtkMAFRulerActor2D : public vtkActor2D
//-----------------------------------------------------------------------------
{
 public:
  vtkTypeRevisionMacro(vtkMAFRulerActor2D,vtkActor2D);
  void PrintSelf(ostream& os, vtkIndent indent);
  static	vtkMAFRulerActor2D *New();

	void SetColor(double r,double g,double b);
  void SetLabelScaleVisibility(bool visibility = true) {ScaleLabelVisibility = visibility;};
  void SetLabelAxesVisibility(bool visibility = true) {AxesLabelVisibility = visibility;};
  void SetAxesVisibility(bool visibility = true) {AxesVisibility = visibility;};
  void SetTickVisibility(bool visibility = true) {TickVisibility = visibility;};
  void SetLegend(const char *legend);
  void SetScaleFactor(double factor);
  double  GetScaleFactor() {return ScaleFactor;};

  int	 RenderOverlay(vtkViewport *viewport);
  int	 RenderOpaqueGeometry(vtkViewport *viewport);      
  int	 RenderTranslucentGeometry(vtkViewport *viewport)  {return 0;};
  void AdjustClippingRange(vtkViewport *viewport)        {};

  void UseGlobalAxes(bool globalAxes) {GlobalAxes = globalAxes; Modified();};
  void UseGlobalAxesOff() {GlobalAxes = false; Modified();};
  void UseGlobalAxesOn() {GlobalAxes = true; Modified();};

protected:
										vtkMAFRulerActor2D();
									 ~vtkMAFRulerActor2D();

					 void			RulerCreate();
           void			RulerUpdate(vtkCamera *camera, vtkRenderer *ren);
             int		round(double val);
          double		GetTickSpacing(double val);
          double    GetMidTickSpacing(double val);
          double    GetLongTickSpacing(double val);
           bool     CheckProjectionPlane(vtkCamera *cam);
           bool     IsMultiple(double val, double multiplier);

  vtkPoints        *Points;
  vtkActor2D			 *Axis;  
  vtkActor2D			 *Tick;  
  vtkTextActor     *ScaleLabel; 
  vtkTextActor     *HorizontalAxesLabel;
  vtkTextActor     *VerticalAxesLabel;

  vtkTextActor     *Labx[NUM_LAB];
  vtkTextActor     *Laby[NUM_LAB];

  int rwWidth;
  int rwHeight;
  int shortTickLen;
  int midTickLen;
  int longTickLen;
  double DesiredTickSpacing;

  int margin;
  int ntick;
  int x_index;
  int y_index;
  
  bool   GlobalAxes;
  bool   ScaleLabelVisibility;
  bool   AxesLabelVisibility;
  bool   AxesVisibility;
  bool   TickVisibility;
  double ScaleFactor;
  char  *Legend;

  inline void   DecomposeValue(double val, int *sign, double *mantissa, int *exponent);
  inline double RicomposeValue(int sign, double mantissa, int exponent);
  inline double NearestTick(double val, double TickSpacing);

private:
  // hide the two parameter Render() method from the user and the compiler.
  virtual void Render(vtkRenderer *, vtkMapper *) {};
private:
  vtkMAFRulerActor2D(const vtkMAFRulerActor2D&);  	// Not implemented.
  void operator=(const vtkMAFRulerActor2D&);  // Not implemented.
};
#endif
