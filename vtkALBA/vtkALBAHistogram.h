/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAHistogram
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*//*=========================================================================

  Program:   Multimod Fundation Library
  Module:    $RCSfile: vtkALBAHistogram.h,v $
  Language:  C++
  Date:      $Date: 2011-12-15 16:30:14 $
  Version:   $Revision: 1.1.2.5 $
  Authors:   Paolo Quadrani
  Project:   MultiMod Project

========================================================================= */

#ifndef __vtkALBAHistogram_h
#define __vtkALBAHistogram_h

#define DEFAULT_HISTOGRAM_COLOR 0.5

#include "albaConfigure.h"
#include "vtkActor2D.h"
#include "vtkImageData.h"
#include "vtkDataArray.h"

//---------------------------------------------------------
// Class forwarding
//---------------------------------------------------------
class vtkViewport;
class vtkTextMapper;
class vtkImageAccumulate;
class vtkImageChangeInformation;
class vtkImageLogarithmicScale;
class vtkGlyph3D;
class vtkRenderer;
class vtkMapper;
class vtkPolyData;
class vtkLineSource;

/**
class Name: vtkALBAHistogram
This class handles a group of actors in order to generate an histogram.
There are three representation: POINT, LINE, BAR.
*/
class ALBA_EXPORT vtkALBAHistogram : public vtkActor2D

{
 public:
  /** RTTI macro */
  vtkTypeMacro(vtkALBAHistogram, vtkActor2D);
  /** Print Object Information*/
  void PrintSelf(ostream& os, vtkIndent indent);
  /** Function that create an instance of the object*/
  static	vtkALBAHistogram *New();

  /** visual representations of the histogram*/
  enum HISTOGRAM_VISUAL_REPRESENTATION
  {
    POINT_REPRESENTATION,
    LINE_REPRESENTATION,
    BAR_REPRESENTATION
  };

	/** Set the vtkImageData on which make the histogram. */
  void SetInputData(vtkDataArray* inputData, int component);
  //vtkSetObjectMacro(InputData, vtkDataArray);
  /** Get the vtkImageData on which make the histogram. */
  vtkGetObjectMacro(InputData, vtkDataArray);
  
  /** Set/Get the scale factor for scaling the histogram */
  vtkSetMacro(ScaleFactor, double);
  /** Set/Get the scale factor for scaling the histogram */
  vtkGetMacro(ScaleFactor, double);
  
  /** Set/Get the histogram representation.*/
  vtkSetMacro(HisctogramRepresentation, int);
  /** Set/Get the histogram representation.*/
  vtkGetMacro(HisctogramRepresentation, int);

  /** Set/Get the logarithmic scale constant for scaling the histogram */
  vtkSetMacro(LogScaleConstant, double);
  /** Set/Get the logarithmic scale constant for scaling the histogram */
  vtkGetMacro(LogScaleConstant, double);
  
  /** Set/Get the histogram color.*/
  vtkSetVector3Macro(Color,double);
  /** Set/Get the histogram color.*/
  vtkGetVectorMacro(Color,double,3);

  /** Turn on/off label visibility.*/
  vtkSetMacro(LabelVisibility,int);
  /** Turn on/off label visibility.*/
  vtkBooleanMacro(LabelVisibility,int);
  /** Get the state of the label visibility.*/
  vtkGetMacro(LabelVisibility,int);

  /** Turn on/off auto-scale.*/
  void SetAutoscaleHistogram(int autoscale);
  
  /** Turn on/off auto-scale.*/
  vtkBooleanMacro(AutoscaleHistogram,int);
  /** Get the state of the auto-scale.*/
  vtkGetMacro(AutoscaleHistogram,int);

  /** Turn on/off logarithmic scale.*/
  vtkSetMacro(LogHistogram,int);
  /** Turn on/off logarithmic scale.*/
  vtkBooleanMacro(LogHistogram,int);
  /** Get the state of the logarithmic scale flag.*/
  vtkGetMacro(LogHistogram,int);

  /** Turn on/off show lines.*/
  vtkSetMacro(ShowLines,int);
  /** Turn on/off show lines.*/
  vtkBooleanMacro(ShowLines,int);

	/** Set/Get the number of bins.*/
	vtkSetMacro(NumberOfBins, int);
	/** Set/Get the number of bins.*/
 	vtkGetMacro(NumberOfBins, int);

	/** Set/Get the number of bins.*/
	vtkGetMacro(Component, int);

	/** Update the position of the gizmos */
	void UpdateLines(double range[2]);

	/** Update the position of the gizmos */
	void UpdateLines(double low, double hi);

  /** retrieve histogram value (count) from x*/
  long int GetHistogramValue(int x, int y);

  /** retrieve scalar value from x*/
  double GetScalarValue(int x, int y);

  /** Change text label actor*/
  void SetLabel(const char *lab);

  /** Draw the object to the screen */
  int	 RenderOverlay(vtkViewport *viewport);
  /** Method is intended for rendering Opaque Geometry.*/
  int	 RenderOpaqueGeometry(vtkViewport *viewport);
  /** Method is intended for rendering Translucent Geometry. */
  int	 RenderTranslucentGeometry(vtkViewport *viewport)  {return 0;};
  /** Adjust the clipping range (this method is empty).*/
  void AdjustClippingRange(vtkViewport *viewport)        {};

protected:
  /** constructor */
	vtkALBAHistogram();
  /** destructor */
	~vtkALBAHistogram();

  /** Create the Histogram pipeline.*/
  void HistogramCreate();

  /** Update the histogram during rendering. */
  void HistogramUpdate(vtkRenderer *ren);

  /** Extract the selected Component */
  void ExtractComponent();

  double ScaleFactor;
  double LogScaleConstant;
  double Color[3];
  int    NumberOfBins;
  int    AutoscaleHistogram;
  int    LogHistogram;
  int    LabelVisibility;
  int    HisctogramRepresentation;
  int    RenderWidth;
  int    RenderH;
  int    OriginY;
  int    OriginX;
	double CurrRange[2];
  int    ShowLines;
  int    Component;
  int    LastComponent;
  bool   AutoscaleCalculated;

  vtkTextMapper	*TextMapper;
  vtkActor2D    *TextActor;
  vtkActor2D    *HistActor;
  vtkActor2D    *Line1Actor;
  vtkActor2D    *Line2Actor;

  vtkLineSource             *Line1;
  vtkLineSource             *Line2;
  vtkPolyData               *PointsRepresentation;
  vtkLineSource             *LineRepresentation;
  vtkImageAccumulate        *Accumulate;
  vtkImageChangeInformation *ChangeInfo;
  vtkImageLogarithmicScale  *LogScale;
  vtkGlyph3D                *Glyph;
  vtkDataArray              *InputData;
  vtkDataArray              *LastInputData;
  vtkDataArray              *ExtractedCompArray;
  vtkImageData              *ImageData;

private:
  /** hide the two parameter Render() method from the user and the compiler. */
  virtual void Render(vtkRenderer *, vtkMapper *) {};
private:
  /** copy constructor, not implemented */
  vtkALBAHistogram(const vtkALBAHistogram&);
  /** operator =, not implemented */
  void operator=(const vtkALBAHistogram&);
};
#endif
