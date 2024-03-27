/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBASimpleRulerActor2D
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*//*=========================================================================

  Program:   Multimod Fundation Library
  Module:    $RCSfile: vtkALBASimpleRulerActor2D.h,v $
  Language:  C++
  Date:      $Date: 2010-07-14 09:02:29 $
  Version:   $Revision: 1.3.2.2 $
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

#ifndef __vtkALBASimpleRulerActor2D_h
#define __vtkALBASimpleRulerActor2D_h

#define DEFAULT_GRID_COLOR 0.5

#include "albaConfigure.h" 

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
class ALBA_EXPORT vtkALBASimpleRulerActor2D : public vtkActor2D
{
public:
	vtkTypeMacro(vtkALBASimpleRulerActor2D,vtkActor2D);
	void PrintSelf(ostream& os, vtkIndent indent);
	static	vtkALBASimpleRulerActor2D *New();

	void SetColor(double r, double g, double b);
	void SetLabelScaleVisibility(bool visibility = true) { ScaleLabelVisibility = visibility; };
	void SetLabelAxesVisibility(bool visibility = true) { AxesLabelVisibility = visibility; };
	void SetAxesVisibility(bool visibility = true) { AxesVisibility = visibility; };
	void SetTickVisibility(bool visibility = true) { TickVisibility = visibility; };
	void SetScaleFactor(double factor);
	double GetScaleFactor() { return ScaleFactor; };
	void SetLegend(const char *legend);

	/** Draw the ruler at the center of the screen. */
	void CenterAxesOnScreen(bool center = true);

	int	 RenderOverlay(vtkViewport *viewport);
	int	 RenderOpaqueGeometry(vtkViewport *viewport);
	int	 RenderTranslucentGeometry(vtkViewport *viewport) { return 0; };
	void AdjustClippingRange(vtkViewport *viewport) {};

	void UseGlobalAxes(bool globalAxes) { GlobalAxes = globalAxes; Modified(); };
	void UseGlobalAxesOff() { GlobalAxes = false; Modified(); };
	void UseGlobalAxesOn() { GlobalAxes = true; Modified(); };

	void SetInverseTicks(bool inverseTicks) { InverseTicks = inverseTicks; };

	void SetAttachPositionFlag(bool value);
	void SetAttachPosition(double position[3]);

	void ShowFixedTick(bool show) { FixedTickVisibility = show; };

	void ChangeRulerMarginsAndLengths(int marginArg, int shortTickLenArg, int midTickLenArg, int longTickLenArg, int xOffSetArg, int yOffSetArg);

protected:
	vtkALBASimpleRulerActor2D();
	~vtkALBASimpleRulerActor2D();

	void			RulerCreate();
	void			RulerUpdate(vtkCamera *camera, vtkRenderer *ren);
	void CreateFixedTick();
	int		Round(double val);
	double		GetTickSpacing(double val);
	double    GetLongTickSpacing(double val);
	bool    CheckProjectionPlane(vtkCamera *cam);
	bool    IsMultiple(double val, double multiplier);

	vtkPoints									*Points;
	vtkPoints									*FixedTickPoints;
	vtkActor2D								*Axis;
	vtkActor2D								*Tick;
	vtkActor2D								*FixedTickActor;
	vtkTextActor						*ScaleLabel;
	vtkTextActor						*HorizontalAxesLabel;
	vtkTextActor						*VerticalAxesLabel;
	vtkTextActor						*FixedTickLabel;

	int RwWidth;
	int RwHeight;
	int ShortTickLen;
	int LongTickLen;
	double DesiredTickSpacing;

	int XOffSet;
	int YOffSet;

	int Margin;
	int Ntick;
	int XIndex;
	int YIndex;

	bool   GlobalAxes;
	bool   CenterAxes;
	bool   ScaleLabelVisibility;
	bool   AxesLabelVisibility;
	bool   AxesVisibility;
	bool   TickVisibility;
	bool   FixedTickVisibility;
	bool   InverseTicks;
	double ScaleFactor;
	char  *Legend;

	int    Position[3];
	double PositionDisplay[3];
	double PositionWorld[3];
	bool   AttachPositionFlag;

	inline void   DecomposeValue(double val, int *sign, double *mantissa, int *exponent);
	inline double RicomposeValue(int sign, double mantissa, int exponent);
	inline double NearestTick(double val, double TickSpacing);

private:
	// hide the two parameter Render() method from the user and the compiler.
	virtual void Render(vtkRenderer *, vtkMapper *) {};
private:
	vtkALBASimpleRulerActor2D(const vtkALBASimpleRulerActor2D&);  	// Not implemented.
	void operator=(const vtkALBASimpleRulerActor2D&);  // Not implemented.
};
#endif
