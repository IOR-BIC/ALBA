/*=========================================================================

  Program:   Multimod Fundation Library
  Module:    $RCSfile: vtkHistogram.h,v $
  Language:  C++
  Date:      $Date: 2006-05-03 09:45:18 $
  Version:   $Revision: 1.2 $
  Authors:   Paolo Quadrani
  Project:   MultiMod Project

========================================================================= */

#ifndef __vtkHistogram_h
#define __vtkHistogram_h

#define DEFAULT_HISTOGRAM_COLOR 0.5

#include "vtkMAFConfigure.h"
#include "vtkActor2D.h"
#include "vtkImageData.h"

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

//-----------------------------------------------------------------------------
class VTK_vtkMAF_EXPORT vtkHistogram : public vtkActor2D
//-----------------------------------------------------------------------------
{
 public:
  vtkTypeRevisionMacro(vtkHistogram, vtkActor2D);
  void PrintSelf(ostream& os, vtkIndent indent);
  static	vtkHistogram *New();

  enum HISTOGRAM_VISUAL_REPRESENTATION
  {
    POINT_REPRESENTATION,
    LINE_REPRESENTATION,
    BAR_REPRESENTATION
  };

	/** Set/Get the vtkImageData on which make the histogram. */
  vtkSetObjectMacro(ImageData, vtkImageData);
  vtkGetObjectMacro(ImageData, vtkImageData);
  
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
  vtkSetMacro(AutoscaleHistogram,int);
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

  long int GetHistogramValue(int x, int y);

  void SetLabel(const char *lab);

  int	 RenderOverlay(vtkViewport *viewport);
  int	 RenderOpaqueGeometry(vtkViewport *viewport);      
  int	 RenderTranslucentGeometry(vtkViewport *viewport)  {return 0;};
  void AdjustClippingRange(vtkViewport *viewport)        {};

protected:
	vtkHistogram();
	~vtkHistogram();

  /** Create the Histogram pipeline.*/
  void HistogramCreate();

  /** Update the histogram during rendering. */
  void HistogramUpdate(vtkRenderer *ren);

  double ScaleFactor;
  double LogScaleConstant;
  double Color[3];
  int    NumberOfBins;
  int    AutoscaleHistogram;
  int    LogHistogram;
  int    LabelVisibility;
  int    HisctogramRepresentation;
  int    RenderWidth;

  vtkTextMapper	*TextMapper;
  vtkActor2D    *TextActor;
  vtkActor2D    *HistActor;

  vtkPolyData               *PointsRepresentation;
  vtkLineSource             *LineRepresentation;
  vtkImageAccumulate        *Accumulate;
  vtkImageChangeInformation *ChangeInfo;
  vtkImageLogarithmicScale  *LogScale;
  vtkGlyph3D                *Glyph;
  vtkImageData              *ImageData;

private:
  // hide the two parameter Render() method from the user and the compiler.
  virtual void Render(vtkRenderer *, vtkMapper *) {};
private:
  vtkHistogram(const vtkHistogram&);  	// Not implemented.
  void operator=(const vtkHistogram&);  // Not implemented.
};
#endif
