/*=========================================================================

  Program:   Multimod Fundation Library
  Module:    $RCSfile: vtkRulerActor2D.h,v $
  Language:  C++
  Date:      $Date: 2006-03-02 19:13:11 $
  Version:   $Revision: 1.2 $
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

#ifndef __vtkRulerActor2D_h
#define __vtkRulerActor2D_h

#define DEFAULT_GRID_COLOR 0.5

#include "vtkMAFConfigure.h" //??

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
class VTK_vtkMAF_EXPORT vtkRulerActor2D : public vtkActor2D
//-----------------------------------------------------------------------------
{
 public:
  vtkTypeRevisionMacro(vtkRulerActor2D,vtkActor2D);
  void PrintSelf(ostream& os, vtkIndent indent);
  static	vtkRulerActor2D *New();

				   void			SetColor    (double r,double g,double b);
	//vtkTextActor		 *GetScaleFactorLabelActor() {return Label;};
  //vtkTextActor		 *GetXAxesLabelActor() {return Label1;};
  //vtkTextActor		 *GetYAxesLabelActor() {return Label2;};

  void SetLabelScaleVisibility(bool visibility = true) {ScaleLabelVisibility = visibility;};
  void SetLabelAxesVisibility(bool visibility = true) {AxesLabelVisibility = visibility;};
  void SetAxesVisibility(bool visibility = true) {AxesVisibility = visibility;};
  void SetTickVisibility(bool visibility = true) {TickVisibility = visibility;};

  int	 RenderOverlay(vtkViewport *viewport);
  int	 RenderOpaqueGeometry(vtkViewport *viewport);      
  int	 RenderTranslucentGeometry(vtkViewport *viewport)  {return 0;};
  void AdjustClippingRange(vtkViewport *viewport)        {};
  void SetScaleFactor(double factor);
  double  GetScaleFactor() {return ScaleFactor;};

  void SetLegend(const char *legend);

protected:
										vtkRulerActor2D();
									 ~vtkRulerActor2D();

					 void			RulerCreate();
           void			RulerUpdate(vtkCamera *camera, vtkRenderer *ren);
          // void			RulerUpdate_old(vtkCamera *camera, vtkRenderer *ren);
          // void			RulerUpdate_2(vtkCamera *camera, vtkRenderer *ren);
          //double		Round(double val);
          //double		Round2(double val);
             int		round(double val);
          double		GetTickSpacing(double val);
          double    GetMidTickSpacing(double val);
          double    GetLongTickSpacing(double val);
           bool     CheckProjectionPlane(vtkCamera *cam);
           bool     IsMultiple(double val, double multiplier);

  vtkPoints        *Points;
  vtkActor2D			 *Axis;  
  vtkActor2D			 *Tick;  
  vtkTextActor     *Label; 
  vtkTextActor     *Label1;
  vtkTextActor     *Label2;

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
  
  bool   ScaleLabelVisibility;
  bool   AxesLabelVisibility;
  bool   AxesVisibility;
  bool   TickVisibility;
  double ScaleFactor;

  char *Legend;


  inline void   DecomposeValue(double val, int *sign, double *mantissa, int *exponent);
  inline double RicomposeValue(int sign, double mantissa, int exponent);
  inline double NearestTick(double val, double TickSpacing);


private:
  // hide the two parameter Render() method from the user and the compiler.
  virtual void Render(vtkRenderer *, vtkMapper *) {};
private:
  vtkRulerActor2D(const vtkRulerActor2D&);  	// Not implemented.
  void operator=(const vtkRulerActor2D&);  // Not implemented.
};
#endif

