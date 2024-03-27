/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAXYPlotActor
 Authors: Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __vtkALBAXYPlotActor_h
#define __vtkALBAXYPlotActor_h

#include "albaConfigure.h"

#define VTK_XYPLOT_INDEX                 0
#define VTK_XYPLOT_ARC_LENGTH            1
#define VTK_XYPLOT_NORMALIZED_ARC_LENGTH 2
#define VTK_XYPLOT_VALUE                 3

#define VTK_XYPLOT_ROW 0
#define VTK_XYPLOT_COLUMN 1

#include "vtkActor2D.h"

class vtkAppendPolyData;
class vtkAxisActor2D;
class vtkDataObject;
class vtkDataObjectCollection;
class vtkDataSet;
class vtkDataSetCollection;
class vtkGlyph2D;
class vtkGlyphSource2D;
class vtkIntArray;
class vtkLegendBoxActor;
class vtkPlanes;
class vtkPolyData;
class vtkPolyDataMapper2D;
class vtkTextMapper;
class vtkTextProperty;

//----------------------------------------------------------------------------
// vtkALBAXYPlotActor :
//----------------------------------------------------------------------------
/** This class is a modification of vtkXYPlotActor whit some bug correction about plot rendering.
    The original vtk class have some problems when a line intersect the render area, but both
    ending points are outside that area. In this case the original class will render bad lines.
*/
class ALBA_EXPORT vtkALBAXYPlotActor : public vtkActor2D
{
public:
  vtkTypeMacro(vtkALBAXYPlotActor,vtkActor2D);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Instantiate object with autorange computation; bold, italic, and shadows
  // on; arial font family; the number of labels set to 5 for the x and y
  // axes; a label format of "%-#6.3g"; and x coordinates computed from point
  // ids.
  static vtkALBAXYPlotActor *New();

  //---Data Set Input----------------------------------------------------------
  // The following methods are used to plot input datasets. Datasets
  // will be plotted if set as input; otherwise the input data objects
  // will be plotted (if defined).
  
  // Description:
  // Add a dataset to the list of data to append. The array name specifies
  // which point array to plot.  If the array name is NULL, then the default
  // scalars are used.  The array can have multiple components, but only the
  // first component is ploted.
  void AddInput(vtkDataSet *in, const char* arrayName, int component);
  void AddInput(vtkDataSet *in) {this->AddInput(in, NULL, 0);}

  // Description:
  // Remove a dataset from the list of data to append.
  void RemoveInput(vtkDataSet *in, const char* arrayName, int component);
  void RemoveInput(vtkDataSet *in) {this->RemoveInput(in, NULL, 0);}

  // Description:
  // This removes all of the data set inputs, 
  // but does not change the data object inputs.
  void RemoveAllInputs();

  // Description:
  // Return the list of inputs to this filter.
  vtkDataSetCollection *GetInputList() {return this->InputList;}

  // Description:
  // If plotting points by value, which component to use to determine the
  // value. This sets a value per each input dataset (i.e., the ith dataset).
  void SetPointComponent(int i, int comp);
  int GetPointComponent(int i);
  //---end Data Set Input-----------------------------------------------------

  // Description:
  // Specify how the independent (x) variable is computed from the points.
  // The independent variable can be the scalar/point index (i.e., point id),
  // the accumulated arc length along the points, the normalized arc length,
  // or by component value. If plotting datasets (e.g., points), the value
  // that is used is specified by the PointComponent ivar.  (Note: these
  // methods also control how field data is plotted. Field data is usually
  // plotted by value or index, if plotting length 1-dimensional length
  // measures are used.)
  vtkSetClampMacro(XValues,int,VTK_XYPLOT_INDEX,VTK_XYPLOT_VALUE);
  vtkGetMacro(XValues,int);
  void SetXValuesToIndex(){this->SetXValues(VTK_XYPLOT_INDEX);};
  void SetXValuesToArcLength() {this->SetXValues(VTK_XYPLOT_ARC_LENGTH);};
  void SetXValuesToNormalizedArcLength()
    {this->SetXValues(VTK_XYPLOT_NORMALIZED_ARC_LENGTH);};
  void SetXValuesToValue() {this->SetXValues(VTK_XYPLOT_VALUE);};
  const char *GetXValuesAsString();

  //---Data Object Input------------------------------------------------------
  // The following methods are used to plot input data objects. Datasets will
  // be plotted in preference to data objects if set as input; otherwise the
  // input data objects will be plotted (if defined).
  
  // Description:
  // Add a dataset to the list of data to append.
  void AddDataObjectInput(vtkDataObject *in);

  // Description:
  // Remove a dataset from the list of data to append.
  void RemoveDataObjectInput(vtkDataObject *in);

  // Description:
  // Return the list of inputs to this filter.
  vtkDataObjectCollection *GetDataObjectInputList() 
    {return this->DataObjectInputList;}

  // Description:
  // Indicate whether to plot rows or columns. If plotting rows, then
  // the dependent variables is taken from a specified row,
  // versus rows (y). 
  vtkSetClampMacro(DataObjectPlotMode,int,VTK_XYPLOT_COLUMN,VTK_XYPLOT_ROW);
  vtkGetMacro(DataObjectPlotMode,int);
  void SetDataObjectPlotModeToRows()
    {this->SetDataObjectPlotMode(VTK_XYPLOT_ROW);}
  void SetDataObjectPlotModeToColumns()
    {this->SetDataObjectPlotMode(VTK_XYPLOT_COLUMN);}
  const char *GetDataObjectPlotModeAsString();

  // Description:
  // Specify which component of the input data object to use as the
  // independent variable for the ith input data object. (This ivar is
  // ignored if plotting the index.) Note that the value is interpreted
  // differently depending on DataObjectPlotMode. If the mode is Rows, then
  // the value of DataObjectXComponent is the row number; otherwise it's the
  // column number.
  void SetDataObjectXComponent(int i, int comp);
  int GetDataObjectXComponent(int i);

  // Description:
  // Specify which component of the input data object to use as the
  // dependent variable for the ith input data object. (This ivar is
  // ignored if plotting the index.) Note that the value is interpreted
  // differently depending on DataObjectPlotMode. If the mode is Rows, then
  // the value of DataObjectYComponent is the row number; otherwise it's the
  // column number.
  void SetDataObjectYComponent(int i, int comp);
  int GetDataObjectYComponent(int i);
  //---end Data Object Input--------------------------------------------------

  //---Per Curve Properties---------------------------------------------------
  // The following methods are used to set properties on each curve that is
  // plotted. Each input dataset (or data object) results in one curve. The
  // methods that follow have an index i that corresponds to the input dataset
  // or data object. 
  void SetPlotColor(int i, double r, double g, double b);
  void SetPlotColor(int i, const double color[3]) {
    this->SetPlotColor(i, color[0], color[1], color[2]); };
  double *GetPlotColor(int i);
  void SetPlotSymbol(int i,vtkPolyData *input);
  vtkPolyData *GetPlotSymbol(int i);
  void SetPlotLabel(int i, const char *label);
  const char *GetPlotLabel(int i);

  // Allow per-curve specification of line and point rendering.  These override
  // global settings PlotPoints and PlotLines.  If not on, the default behavior
  // is governed by PlotPoints and PlotLines ivars.
  vtkGetMacro(PlotCurvePoints, int);
  vtkSetMacro(PlotCurvePoints, int);
  vtkBooleanMacro(PlotCurvePoints, int);

  vtkGetMacro(PlotCurveLines, int);
  vtkSetMacro(PlotCurveLines, int);
  vtkBooleanMacro(PlotCurveLines, int);

  void SetPlotLines(int i, int);
  int GetPlotLines(int i);

  void SetPlotPoints(int i, int);
  int GetPlotPoints(int i);
  //---end Per Curve Properties-----------------------------------------------

  // Description:
  // Enable/Disable exchange of the x-y axes (i.e., what was x becomes y, and
  // vice-versa). Exchanging axes affects the labeling as well.
  vtkSetMacro(ExchangeAxes, int);
  vtkGetMacro(ExchangeAxes, int);
  vtkBooleanMacro(ExchangeAxes, int);

  // Description:
  // Normally the x-axis is plotted from minimum to maximum. Setting this instance
  // variable causes the x-axis to be plotted from maximum to minimum. Note that
  // boolean always applies to the x-axis even if ExchangeAxes is set.
  vtkSetMacro(ReverseXAxis, int);
  vtkGetMacro(ReverseXAxis, int);
  vtkBooleanMacro(ReverseXAxis, int);

  // Description:
  // Normally the y-axis is plotted from minimum to maximum. Setting this instance
  // variable causes the y-axis to be plotted from maximum to minimum. Note that
  // boolean always applies to the y-axis even if ExchangeAxes is set.
  vtkSetMacro(ReverseYAxis, int);
  vtkGetMacro(ReverseYAxis, int);
  vtkBooleanMacro(ReverseYAxis, int);

  // Description:
  // Retrieve handles to the legend box and glyph source. This is useful
  // if you would like to change the default behavior of the legend box
  // or glyph source. For example, the default glyph can be changed from
  // a line to a vertex plus line, etc.)
  vtkLegendBoxActor *GetLegendBoxActor()
    {return this->LegendActor;}
  vtkGlyphSource2D *GetGlyphSource()
    {return this->GlyphSource;}

  // Description:
  // Set/Get the title of the x-y plot, and the title along the 
  // x and y axes.
  vtkSetStringMacro(Title);
  vtkGetStringMacro(Title);
  vtkSetStringMacro(XTitle);
  vtkGetStringMacro(XTitle);
  vtkSetStringMacro(YTitle);
  vtkGetStringMacro(YTitle);

  // Description:
  // Retrieve handles to the X and Y axis (so that you can set their text
  // properties for example)
  vtkAxisActor2D *GetXAxisActor2D()
    {return this->XAxis;}
  vtkAxisActor2D *GetYAxisActor2D()
    {return this->YAxis;}

  // Description:
  // Set the plot range (range of independent and dependent variables)
  // to plot. Data outside of the range will be clipped. If the plot
  // range of either the x or y variables is set to (v1,v2), where
  // v1 == v2, then the range will be computed automatically. Note that
  // the x-range values should be consistent with the way the independent
  // variable is created (via INDEX, DISTANCE, or ARC_LENGTH).
  vtkSetVector2Macro(XRange,double);
  vtkGetVectorMacro(XRange,double,2);
  vtkSetVector2Macro(YRange,double);
  vtkGetVectorMacro(YRange,double,2);
  void SetPlotRange(double xmin, double ymin, double xmax, double ymax)
    {this->SetXRange(xmin,xmax); this->SetYRange(ymin,ymax);}
  
  // Description:
  // Set/Get the number of annotation labels to show along the x and y axes.
  // This values is a suggestion: the number of labels may vary depending
  // on the particulars of the data. The convenience method 
  // SetNumberOfLables() sets the number of x and y labels to the same value.
  vtkSetClampMacro(NumberOfXLabels, int, 0, 50);
  vtkGetMacro(NumberOfXLabels, int);
  vtkSetClampMacro(NumberOfYLabels, int, 0, 50);
  vtkGetMacro(NumberOfYLabels, int);
  void SetNumberOfLabels(int num)
    {this->SetNumberOfXLabels(num); this->SetNumberOfYLabels(num);}
  
  // Description:
  // Enable/Disable the creation of a legend. If on, the legend labels will
  // be created automatically unless the per plot legend symbol has been
  // set.
  vtkSetMacro(Legend, int);
  vtkGetMacro(Legend, int);
  vtkBooleanMacro(Legend, int);

  // Description: 
  // Use these methods to control the position of the legend. The variables
  // LegendPosition and LegendPosition2 define the lower-left and upper-right
  // position of the legend. The coordinates are expressed as normalized
  // values with respect to the rectangle defined by PositionCoordinate and
  // Position2Coordinate. Note that LegendPosition2 is relative to
  // LegendPosition.
  vtkSetVector2Macro(LegendPosition,double);
  vtkGetVector2Macro(LegendPosition,double);
  vtkSetVector2Macro(LegendPosition2,double);
  vtkGetVector2Macro(LegendPosition2,double);
  
  // Description:
  // Set/Get the title text property.
  virtual void SetTitleTextProperty(vtkTextProperty *p);
  vtkGetObjectMacro(TitleTextProperty,vtkTextProperty);
  
  // Description:
  // Set/Get the title text property of all axes. Note that each axis can
  // be controlled individually through the GetX/YAxisActor2D() methods.
  virtual void SetAxisTitleTextProperty(vtkTextProperty *p);
  vtkGetObjectMacro(AxisTitleTextProperty,vtkTextProperty);
  
  // Description:
  // Set/Get the labels text property of all axes. Note that each axis can
  // be controlled individually through the GetX/YAxisActor2D() methods.
  virtual void SetAxisLabelTextProperty(vtkTextProperty *p);
  vtkGetObjectMacro(AxisLabelTextProperty,vtkTextProperty);
      
  // Description:
  // Enable/Disable plotting of Log of x-values.
  vtkSetMacro(Logx, int);
  vtkGetMacro(Logx, int);
  vtkBooleanMacro(Logx, int);

  // Description:
  // Set/Get the format with which to print the labels on the scalar
  // bar.
  virtual void SetLabelFormat (const char* _arg);
  vtkGetStringMacro(LabelFormat);

  // Description:
  // Set/Get the spacing between the plot window and the plot. The value
  // is specified in pixels.
  vtkSetClampMacro(Border, int, 0, 50);
  vtkGetMacro(Border, int);

  // Description:
  // Set/Get whether the points are rendered.  The point size can be set in
  // the property object. This is a global flag which affects the plot only 
  // if per curve symbols are not defined.
  vtkGetMacro(PlotPoints, int);
  vtkSetMacro(PlotPoints, int);
  vtkBooleanMacro(PlotPoints, int);

  // Description:
  // Set/Get whether the lines are rendered.  The line width can be set in
  // the property object. 
  vtkGetMacro(PlotLines, int);
  vtkSetMacro(PlotLines, int);
  vtkBooleanMacro(PlotLines, int);
  
  // Description:
  // Set/Get the factor that controls how big glyphs are in the plot.
  // The number is expressed as a fraction of the length of the diagonal
  // of the plot bounding box.
  vtkSetClampMacro(GlyphSize, double, 0.0, 0.2);
  vtkGetMacro(GlyphSize, double);

  // Description:
  // Given a position within the viewport used by the plot, return the
  // the plot coordinates (XAxis value, YAxis value)
  void ViewportToPlotCoordinate(vtkViewport *viewport, double &u, double &v);

  // Description:
  // An alternate form of ViewportToPlotCoordinate() above. This method
  // inputs the viewport coordinate pair (defined by the ivar 
  // ViewportCoordinate)and then stores them in the ivar PlotCoordinate. 
  void ViewportToPlotCoordinate(vtkViewport *viewport);
  vtkSetVector2Macro(PlotCoordinate,double);
  vtkGetVector2Macro(PlotCoordinate,double);

  // Description:
  // Given a plot coordinate, return the viewpoint position
  void PlotToViewportCoordinate(vtkViewport *viewport, double &u, double &v);

  // Description:
  // An alternate form of PlotToViewportCoordinate() above. This method
  // inputs the plot coordinate pair (defined in the ivar PlotCoordinate)
  // and then stores them in the ivar ViewportCoordinate. (This method 
  // can be wrapped.)
  void PlotToViewportCoordinate(vtkViewport *viewport);
  vtkSetVector2Macro(ViewportCoordinate,double);
  vtkGetVector2Macro(ViewportCoordinate,double);

  // Description:
  // Is the specified viewport position within the plot area (as opposed to the
  // region used by the plot plus the labels)?
  int IsInPlot(vtkViewport *viewport, double u, double v);
  
  // Description:
  // Take into account the modified time of internal helper classes.
	vtkMTimeType GetMTime();
  
//BTX  
  // Description:
  // WARNING: INTERNAL METHOD - NOT INTENDED FOR GENERAL USE
  // DO NOT USE THIS METHOD OUTSIDE OF THE RENDERING PROCESS.
  // Draw the x-y plot.
  int RenderOpaqueGeometry(vtkViewport*);
  int RenderOverlay(vtkViewport*);
  int RenderTranslucentGeometry(vtkViewport *) {return 0;}

  // Description:
  // Release any graphics resources that are being consumed by this actor.
  // The parameter window could be used to determine which graphic
  // resources to release.
  void ReleaseGraphicsResources(vtkWindow *);
//ETX  

protected:
  vtkALBAXYPlotActor();
  ~vtkALBAXYPlotActor();

  vtkDataSetCollection *InputList; //list of data sets to plot
  char** SelectedInputScalars; // list of data set arrays to plot
  vtkIntArray* SelectedInputScalarsComponent; // list of componenents
  vtkDataObjectCollection *DataObjectInputList; //list of data objects to plot
  char  *Title;
  char  *XTitle;
  char  *YTitle;
  int   XValues;
  int   NumberOfXLabels;
  int   NumberOfYLabels;
  int   Logx;
  char  *LabelFormat;
  double XRange[2];
  double YRange[2];
  double XComputedRange[2];  //range actually used by plot
  double YComputedRange[2];  //range actually used by plot
  int Border;
  int PlotLines;
  int PlotPoints;
  int PlotCurveLines;
  int PlotCurvePoints;
  int ExchangeAxes;
  int ReverseXAxis;
  int ReverseYAxis;
  
  vtkTextMapper   *TitleMapper;
  vtkActor2D      *TitleActor;
  vtkTextProperty *TitleTextProperty;

  vtkAxisActor2D      *XAxis;
  vtkAxisActor2D      *YAxis;

  vtkTextProperty *AxisTitleTextProperty;
  vtkTextProperty *AxisLabelTextProperty;

  double ViewportCoordinate[2];
  double PlotCoordinate[2];
  
  //Handle data objects and datasets
  int DataObjectPlotMode;
  vtkIntArray *XComponent;
  vtkIntArray *YComponent;
  vtkIntArray *LinesOn;
  vtkIntArray *PointsOn;

  //The data drawn within the axes. Each curve is one polydata.
  //color is controlled by scalar data. The curves are appended
  //together, possibly glyphed with point symbols.
  int NumberOfInputs;
  vtkPolyData             **PlotData; 
  vtkGlyph2D              **PlotGlyph;
  vtkAppendPolyData       **PlotAppend;
  vtkPolyDataMapper2D     **PlotMapper;
  vtkActor2D              **PlotActor;
  void                    InitializeEntries();
  
  // Legends and plot symbols. The legend also keeps track of
  // the symbols and such.
  int Legend;
  double LegendPosition[2];
  double LegendPosition2[2];
  vtkLegendBoxActor *LegendActor;
  vtkGlyphSource2D *GlyphSource;
  vtkPlanes *ClipPlanes;
  double GlyphSize;

  // Keep track of changes.
  int CachedSize[2];
  vtkTimeStamp  BuildTime;

  void ComputeXRange(double range[2], double *lengths);
  void ComputeYRange(double range[2]);
  void ComputeDORange(double xrange[2], double yrange[2], double *lengths);

  virtual void CreatePlotData(int *pos, int *pos2, double xRange[2], 
                              double yRange[2], double *norms, 
                              int numDS, int numDO);
  void PlaceAxes(vtkViewport *viewport, int *size, int pos[2], int pos2[2]);
  void GenerateClipPlanes(int *pos, int *pos2);
  double ComputeGlyphScale(int i, int *pos, int *pos2);
  void ClipPlotData(int *pos, int *pos2, vtkPolyData *pd);
  double *TransformPoint(int pos[2], int pos2[2], double x[3], double xNew[3]);
  
private:
  vtkALBAXYPlotActor(const vtkALBAXYPlotActor&);  // Not implemented.
  void operator=(const vtkALBAXYPlotActor&);  // Not implemented.
};


#endif

