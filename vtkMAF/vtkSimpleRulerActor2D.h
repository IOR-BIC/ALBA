/*=========================================================================

  Program:   Multimod Fundation Library
  Module:    $RCSfile: vtkSimpleRulerActor2D.h,v $
  Language:  C++
  Date:      $Date: 2006-03-03 15:53:44 $
  Version:   $Revision: 1.1 $
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

#ifndef __vtkSimpleRulerActor2D_h
#define __vtkSimpleRulerActor2D_h

#define DEFAULT_GRID_COLOR 0.5

#include "vtkMAFConfigure.h" //??

#include "vtkActor.h"
#include "vtkActor2D.h"
#include "vtkTextActor.h"

class vtkViewport;
class vtkCamera;
class vtkPoints;

/**
an actor displaying world X and Y axes as an Actor2D.
Camera must be in ParallelProjection and aligned with world axis
*/

//-----------------------------------------------------------------------------
class VTK_vtkMAF_EXPORT vtkSimpleRulerActor2D : public vtkActor2D
//-----------------------------------------------------------------------------
{
 public:
  vtkTypeRevisionMacro(vtkSimpleRulerActor2D,vtkActor2D);
  void PrintSelf(ostream& os, vtkIndent indent);
  static	vtkSimpleRulerActor2D *New();

	void SetColor(double r,double g,double b);
  void SetLabelScaleVisibility(bool visibility = true) {ScaleLabelVisibility = visibility;};
  void SetLabelAxesVisibility(bool visibility = true) {AxesLabelVisibility = visibility;};
  void SetAxesVisibility(bool visibility = true) {AxesVisibility = visibility;};
  void SetTickVisibility(bool visibility = true) {TickVisibility = visibility;};
  void SetScaleFactor(double factor);
  double GetScaleFactor() {return ScaleFactor;};
  void SetLegend(const char *legend);

  int	 RenderOverlay(vtkViewport *viewport);
  int	 RenderOpaqueGeometry(vtkViewport *viewport);      
  int	 RenderTranslucentGeometry(vtkViewport *viewport)  {return 0;};
  void AdjustClippingRange(vtkViewport *viewport)        {};

protected:
										vtkSimpleRulerActor2D();
									 ~vtkSimpleRulerActor2D();

					 void			RulerCreate();
           void			RulerUpdate(vtkCamera *camera, vtkRenderer *ren);
             int		round(double val);
          double		GetTickSpacing(double val);
          double    GetLongTickSpacing(double val);
            bool    CheckProjectionPlane(vtkCamera *cam);
            bool    IsMultiple(double val, double multiplier);

  vtkPoints        *Points;
  vtkActor2D			 *Axis;  
  vtkActor2D			 *Tick;  
  vtkTextActor     *ScaleLabel; 
  vtkTextActor     *HorizontalAxeLabel;
  vtkTextActor     *VerticalAxeLabel;

  int rwWidth;
  int rwHeight;
  int shortTickLen;
  int longTickLen;
  double DesiredTickSpacing;

  int margin;
  int ntick;
  int x_index;
  int y_index;
  
  bool   CenterAxes;
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
  vtkSimpleRulerActor2D(const vtkSimpleRulerActor2D&);  	// Not implemented.
  void operator=(const vtkSimpleRulerActor2D&);  // Not implemented.
};
#endif



