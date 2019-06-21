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
#include "vtkALBAXYPlotActor.h"

#include "vtkAppendPolyData.h"
#include "vtkAxisActor2D.h"
#include "vtkCellArray.h"
#include "vtkDataObjectCollection.h"
#include "vtkDataSetCollection.h"
#include "vtkFieldData.h"
#include "vtkDoubleArray.h"
#include "vtkGlyph2D.h"
#include "vtkGlyphSource2D.h"
#include "vtkIntArray.h"
#include "vtkLegendBoxActor.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkPlane.h"
#include "vtkPlanes.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper2D.h"
#include "vtkProperty2D.h"
#include "vtkTextMapper.h"
#include "vtkTextProperty.h"
#include "vtkViewport.h"
#include "vtkSetGet.h"

#define VTK_MAX_PLOTS 50

vtkCxxRevisionMacro(vtkALBAXYPlotActor, "$Revision: 1.1.2.1 $");
vtkStandardNewMacro(vtkALBAXYPlotActor);

vtkCxxSetObjectMacro(vtkALBAXYPlotActor,TitleTextProperty,vtkTextProperty);
vtkCxxSetObjectMacro(vtkALBAXYPlotActor,AxisLabelTextProperty,vtkTextProperty);
vtkCxxSetObjectMacro(vtkALBAXYPlotActor,AxisTitleTextProperty,vtkTextProperty);

//----------------------------------------------------------------------------
// Instantiate object
vtkALBAXYPlotActor::vtkALBAXYPlotActor()
{
  this->PositionCoordinate->SetCoordinateSystemToNormalizedViewport();
  this->PositionCoordinate->SetValue(0.25,0.25);
  this->Position2Coordinate->SetValue(0.5, 0.5);

  this->InputList = vtkDataSetCollection::New();
  this->SelectedInputScalars = NULL;
  this->SelectedInputScalarsComponent = vtkIntArray::New();
  this->DataObjectInputList = vtkDataObjectCollection::New();

  this->Title = NULL;
  this->XTitle = new char[7];
  sprintf(this->XTitle,"%s","X Axis");
  this->YTitle = new char[7];
  sprintf(this->YTitle,"%s","Y Axis");

  this->XValues = VTK_XYPLOT_INDEX;

  this->NumberOfXLabels = 5;
  this->NumberOfYLabels = 5;

  this->TitleTextProperty = vtkTextProperty::New();
  this->TitleTextProperty->SetBold(1);
  this->TitleTextProperty->SetItalic(1);
  this->TitleTextProperty->SetShadow(1);
  this->TitleTextProperty->SetFontFamilyToArial();

  this->AxisLabelTextProperty = vtkTextProperty::New();
  this->AxisLabelTextProperty->ShallowCopy(this->TitleTextProperty);

  this->AxisTitleTextProperty = vtkTextProperty::New();
  this->AxisTitleTextProperty->ShallowCopy(this->AxisLabelTextProperty);

  this->LabelFormat = new char[8]; 
  sprintf(this->LabelFormat,"%s","%-#6.3g");

  this->Logx = 0;
  
  this->XRange[0] = 0.0;
  this->XRange[1] = 0.0;
  this->YRange[0] = 0.0;
  this->YRange[1] = 0.0;

  this->Border = 5;
  this->PlotLines = 1;
  this->PlotPoints = 0;
  this->PlotCurveLines = 0;
  this->PlotCurvePoints = 0;
  this->ExchangeAxes = 0;
  this->ReverseXAxis = 0;
  this->ReverseYAxis = 0;

  this->TitleMapper = vtkTextMapper::New();
  this->TitleActor = vtkActor2D::New();
  this->TitleActor->SetMapper(this->TitleMapper);
  this->TitleActor->GetPositionCoordinate()->SetCoordinateSystemToViewport();

  this->XAxis = vtkAxisActor2D::New();
  this->XAxis->GetPositionCoordinate()->SetCoordinateSystemToViewport();
  this->XAxis->GetPosition2Coordinate()->SetCoordinateSystemToViewport();
  this->XAxis->SetProperty(this->GetProperty());

  this->YAxis = vtkAxisActor2D::New();
  this->YAxis->GetPositionCoordinate()->SetCoordinateSystemToViewport();
  this->YAxis->GetPosition2Coordinate()->SetCoordinateSystemToViewport();
  this->YAxis->SetProperty(this->GetProperty());
  
  this->NumberOfInputs = 0;
  this->PlotData = NULL;
  this->PlotGlyph = NULL;
  this->PlotAppend = NULL;
  this->PlotMapper = NULL;
  this->PlotActor = NULL;

  this->ViewportCoordinate[0] = 0.0;
  this->ViewportCoordinate[1] = 0.0;
  this->PlotCoordinate[0] = 0.0;
  this->PlotCoordinate[1] = 0.0;

  this->DataObjectPlotMode = VTK_XYPLOT_COLUMN;
  this->XComponent = vtkIntArray::New();
  this->XComponent->SetNumberOfValues(VTK_MAX_PLOTS);
  this->YComponent = vtkIntArray::New();
  this->YComponent->SetNumberOfValues(VTK_MAX_PLOTS);

  this->LinesOn = vtkIntArray::New();
  this->LinesOn->SetNumberOfValues(VTK_MAX_PLOTS);
  this->PointsOn = vtkIntArray::New();
  this->PointsOn->SetNumberOfValues(VTK_MAX_PLOTS);
  for (int i=0; i<VTK_MAX_PLOTS; i++)
    {
    this->XComponent->SetValue(i,0);
    this->YComponent->SetValue(i,0);
    this->LinesOn->SetValue(i,this->PlotLines);
    this->PointsOn->SetValue(i,this->PlotPoints);
    }

  this->Legend = 0;
  this->LegendPosition[0] = 0.85;
  this->LegendPosition[1] = 0.75;
  this->LegendPosition2[0] = 0.15;
  this->LegendPosition2[1] = 0.20;
  this->LegendActor = vtkLegendBoxActor::New();
  this->LegendActor->GetPositionCoordinate()->SetCoordinateSystemToViewport();
  this->LegendActor->GetPosition2Coordinate()->SetCoordinateSystemToViewport();
  this->LegendActor->GetPosition2Coordinate()->SetReferenceCoordinate(NULL);
  this->LegendActor->BorderOff();
  this->LegendActor->SetNumberOfEntries(VTK_MAX_PLOTS); //initial allocation
  this->GlyphSource = vtkGlyphSource2D::New();
  this->GlyphSource->SetGlyphTypeToNone();
  this->GlyphSource->DashOn();
  this->GlyphSource->FilledOff();
  this->GlyphSize = 0.020;

  this->ClipPlanes = vtkPlanes::New();
  vtkPoints *pts = vtkPoints::New();
  pts->SetNumberOfPoints(4);
  this->ClipPlanes->SetPoints(pts);
  pts->Delete();
  vtkDoubleArray *n = vtkDoubleArray::New();
  n->SetNumberOfComponents(3);
  n->SetNumberOfTuples(4);
  this->ClipPlanes->SetNormals(n);
  n->Delete();

  this->CachedSize[0] = 0;
  this->CachedSize[1] = 0;
}

//----------------------------------------------------------------------------
vtkALBAXYPlotActor::~vtkALBAXYPlotActor()
{
  // Get rid of the list of array names.
  int num = this->InputList->GetNumberOfItems();
  if (this->SelectedInputScalars)
    {
    for (int i = 0; i < num; ++i)
      {
      if (this->SelectedInputScalars[i])
        {
        delete [] this->SelectedInputScalars[i];
        this->SelectedInputScalars[i] = NULL;
        }
      }
    delete [] this->SelectedInputScalars;
    this->SelectedInputScalars = NULL;  
    }
  this->SelectedInputScalarsComponent->Delete();
  this->SelectedInputScalarsComponent = NULL;

  //  Now we can get rid of the inputs. 
  this->InputList->Delete();
  this->InputList = NULL;

  this->DataObjectInputList->Delete();
  this->DataObjectInputList = NULL;

  this->TitleMapper->Delete();
  this->TitleMapper = NULL;
  this->TitleActor->Delete();
  this->TitleActor = NULL;

  if (this->Title)
    {
    delete [] this->Title;
    this->Title = NULL;
    }
  
  if (this->XTitle)
    {
    delete [] this->XTitle;
    this->XTitle = NULL;
    }
  
  if (this->YTitle)
    {
    delete [] this->YTitle;
    this->YTitle = NULL;
    }
  
  if (this->LabelFormat) 
    {
    delete [] this->LabelFormat;
    this->LabelFormat = NULL;
    }

  this->XAxis->Delete();
  this->YAxis->Delete();
  
  this->InitializeEntries();

  this->LegendActor->Delete();
  this->GlyphSource->Delete();
  this->ClipPlanes->Delete();
  
  this->XComponent->Delete();
  this->YComponent->Delete();

  this->LinesOn->Delete();
  this->PointsOn->Delete();

  this->SetTitleTextProperty(NULL);
  this->SetAxisLabelTextProperty(NULL);
  this->SetAxisTitleTextProperty(NULL);
}

//----------------------------------------------------------------------------
void vtkALBAXYPlotActor::InitializeEntries()
{
  if ( this->NumberOfInputs > 0 )
    {
    for (int i=0; i<this->NumberOfInputs; i++)
      {
      this->PlotData[i]->Delete();
      this->PlotGlyph[i]->Delete();
      this->PlotAppend[i]->Delete();
      this->PlotMapper[i]->Delete();
      this->PlotActor[i]->Delete();
      }//for all entries
    delete [] this->PlotData; this->PlotData = NULL;
    delete [] this->PlotGlyph; this->PlotGlyph = NULL;
    delete [] this->PlotAppend; this->PlotAppend = NULL;
    delete [] this->PlotMapper; this->PlotMapper = NULL;
    delete [] this->PlotActor; this->PlotActor = NULL;
    this->NumberOfInputs = 0;
    }//if entries have been defined
}
  
//----------------------------------------------------------------------------
// Add a dataset and array to the list of data to plot.
void vtkALBAXYPlotActor::AddInput(vtkDataSet *ds, const char *arrayName, int component)
{
  int idx, num;
  char** newNames;

  // I cannot change the input list, because the user has direct 
  // access to the collection.  I cannot store the index of the array, 
  // because the index might change from render to render ...
  // I have to store the list of string array names.

  // I believe idx starts at 1 and goes to "NumberOfItems".
  idx = this->InputList->IsItemPresent(ds);
  if (idx > 0)
    { // Return if arrays are the same.
    if (arrayName == NULL && this->SelectedInputScalars[idx-1] == NULL &&
        component == this->SelectedInputScalarsComponent->GetValue(idx-1))
      {
      return;
      }
    if (arrayName != NULL && this->SelectedInputScalars[idx-1] != NULL &&
        strcmp(arrayName, this->SelectedInputScalars[idx-1]) == 0 &&
        component == this->SelectedInputScalarsComponent->GetValue(idx-1))
      {
      return;
      }
    }

  // The input/array/component must be a unique combination.  Add it to our input list.

  // Now reallocate the list of strings and add the new value.
  num = this->InputList->GetNumberOfItems();
  newNames = new char*[num+1];
  for (idx = 0; idx < num; ++idx)
    {
    newNames[idx] = this->SelectedInputScalars[idx];
    }
  if (arrayName == NULL)
    {
    newNames[num] = NULL;
    }
  else
    {
    newNames[num] = new char[strlen(arrayName)+1];
    strcpy(newNames[num],arrayName);
    }
  delete [] this->SelectedInputScalars;
  this->SelectedInputScalars = newNames;

  // Save the component it the int array.
  this->SelectedInputScalarsComponent->InsertValue(num, component);

  // Add the data set to the collection
  this->InputList->AddItem(ds);

  this->Modified();
}

//----------------------------------------------------------------------------
void vtkALBAXYPlotActor::RemoveAllInputs()
{
  int idx, num;

  num = this->InputList->GetNumberOfItems();
  this->InputList->RemoveAllItems();

  for (idx = 0; idx < num; ++idx)
    {
    if (this->SelectedInputScalars[idx])
      {
      delete [] this->SelectedInputScalars[idx];
      this->SelectedInputScalars[idx] = NULL;
      }
    }
  this->SelectedInputScalarsComponent->Reset();
}

//----------------------------------------------------------------------------
// Remove a dataset from the list of data to plot.
void vtkALBAXYPlotActor::RemoveInput(vtkDataSet *ds, const char *arrayName, int component)
{
  int idx, num;
  vtkDataSet *input;
  int found = -1;

  // This is my own find routine, because the array names have to match also.
  num = this->InputList->GetNumberOfItems();
  this->InputList->InitTraversal();
  for (idx = 0; idx < num && found == -1; ++idx)
    {
    input = this->InputList->GetNextItem();
    if (input == ds)
      {
      if (arrayName == NULL && this->SelectedInputScalars[idx] == NULL &&
          component == this->SelectedInputScalarsComponent->GetValue(idx-1))
        {
        found = idx;
        }
      if (arrayName != NULL && this->SelectedInputScalars[idx] != NULL &&
          strcmp(arrayName, this->SelectedInputScalars[idx]) == 0 &&
          component == this->SelectedInputScalarsComponent->GetValue(idx-1))
        {
        found = idx;
        }
      }
    }

  if (found == -1)
    {
    return;
    }
  
  this->Modified();
  // Collections index their items starting at 1.
  this->InputList->RemoveItem(found+1);

  // Do not bother reallocating the SelectedInputScalars 
  // string array to make it smaller.
  if (this->SelectedInputScalars[found])
    {
    delete [] this->SelectedInputScalars[found];
    this->SelectedInputScalars[found] = NULL;
    }
  for (idx = found+1; idx < num; ++idx)
    {
    this->SelectedInputScalars[idx-1] = this->SelectedInputScalars[idx];
    this->SelectedInputScalarsComponent->SetValue(idx-1, 
                          this->SelectedInputScalarsComponent->GetValue(idx));
    }
  // Reseting the last item is not really necessary, 
  // but to be clean we do it anyway.
  this->SelectedInputScalarsComponent->SetValue(num-1, -1); 
  this->SelectedInputScalars[num-1] = NULL;
}

//----------------------------------------------------------------------------
// Add a data object to the list of data to plot.
void vtkALBAXYPlotActor::AddDataObjectInput(vtkDataObject *in)
{
  if ( ! this->DataObjectInputList->IsItemPresent(in) )
    {
    this->Modified();
    this->DataObjectInputList->AddItem(in);
    }
}

//----------------------------------------------------------------------------
// Remove a data object from the list of data to plot.
void vtkALBAXYPlotActor::RemoveDataObjectInput(vtkDataObject *in)
{
  if ( this->DataObjectInputList->IsItemPresent(in) )
    {
    this->Modified();
    this->DataObjectInputList->RemoveItem(in);
    }
}

//----------------------------------------------------------------------------
// Plot scalar data for each input dataset.
int vtkALBAXYPlotActor::RenderOverlay(vtkViewport *viewport)
{
  int renderedSomething=0;

  // Make sure input is up to date.
  if ( this->InputList->GetNumberOfItems() < 1 && 
       this->DataObjectInputList->GetNumberOfItems() < 1 )
    {
    vtkErrorMacro(<< "Nothing to plot!");
    return 0;
    }

  renderedSomething += this->XAxis->RenderOverlay(viewport);
  renderedSomething += this->YAxis->RenderOverlay(viewport);
  if ( this->Title != NULL )
    {
    renderedSomething += this->TitleActor->RenderOverlay(viewport);
    }
  for (int i=0; i < this->NumberOfInputs; i++)
    {
    renderedSomething += this->PlotActor[i]->RenderOverlay(viewport);
    }
  if ( this->Legend )
    {
    renderedSomething += this->LegendActor->RenderOverlay(viewport);
    }

  return renderedSomething;
}

//----------------------------------------------------------------------------
// Plot scalar data for each input dataset.
int vtkALBAXYPlotActor::RenderOpaqueGeometry(vtkViewport *viewport)
{
  unsigned long mtime, dsMtime;
  vtkDataSet *ds;
  vtkDataObject *dobj;
  int numDS, numDO, renderedSomething=0;

  // Initialize
  // Make sure input is up to date.
  numDS = this->InputList->GetNumberOfItems();
  numDO = this->DataObjectInputList->GetNumberOfItems();
  if ( numDS > 0 )
    {
    vtkDebugMacro(<<"Plotting input data sets");
    for (mtime=0, this->InputList->InitTraversal(); 
         (ds = this->InputList->GetNextItem()); )
      {
      ds->Update();
      dsMtime = ds->GetMTime();
      if ( dsMtime > mtime )
        {
        mtime = dsMtime;
        }
      }
    }
  else if ( numDO > 0 )
    {
    vtkDebugMacro(<<"Plotting input data objects");
    for (mtime=0, this->DataObjectInputList->InitTraversal(); 
         (dobj = this->DataObjectInputList->GetNextItem()); )
      {
      dobj->Update();
      dsMtime = dobj->GetMTime();
      if ( dsMtime > mtime )
        {
        mtime = dsMtime;
        }
      }
    }
  else
    {
    vtkErrorMacro(<< "Nothing to plot!");
    return 0;
    }

  if (this->Title && this->Title[0] && !this->TitleTextProperty)
    {
    vtkErrorMacro(<< "Need a title text property to render plot title");
    return 0;
    }

  // Check modified time to see whether we have to rebuild.
  // Pay attention that GetMTime() has been redefined (see below)

  int *size=viewport->GetSize();
  if (mtime > this->BuildTime || 
      size[0] != this->CachedSize[0] || size[1] != this->CachedSize[1] ||
      this->GetMTime() > this->BuildTime ||
      (this->Title && this->Title[0] && 
       this->TitleTextProperty->GetMTime() > this->BuildTime) ||
      (this->AxisLabelTextProperty &&
       this->AxisLabelTextProperty->GetMTime() > this->BuildTime) ||
      (this->AxisTitleTextProperty &&
       this->AxisTitleTextProperty->GetMTime() > this->BuildTime))
    {
    double range[2], yrange[2], xRange[2], yRange[2], interval, *lengths=NULL;
    int pos[2], pos2[2], numTicks;
    int stringSize[2];
    int num = ( numDS > 0 ? numDS : numDO );

    vtkDebugMacro(<<"Rebuilding plot");
    this->CachedSize[0] = size[0];
    this->CachedSize[1] = size[1];

    // manage legend
    vtkDebugMacro(<<"Rebuilding legend");
    if ( this->Legend )
      {
      int legPos[2], legPos2[2];
      int *p1 = this->PositionCoordinate->GetComputedViewportValue(viewport);
      int *p2 = this->Position2Coordinate->GetComputedViewportValue(viewport);
      legPos[0] = (int)(p1[0] + this->LegendPosition[0]*(p2[0]-p1[0]));
      legPos2[0] = (int)(legPos[0] + this->LegendPosition2[0]*(p2[0]-p1[0]));
      legPos[1] = (int)(p1[1] + this->LegendPosition[1]*(p2[1]-p1[1]));
      legPos2[1] = (int)(legPos[1] + this->LegendPosition2[1]*(p2[1]-p1[1]));
      
      this->LegendActor->GetPositionCoordinate()->SetValue(
        (double)legPos[0], (double)legPos[1]);
      this->LegendActor->GetPosition2Coordinate()->SetValue(
        (double)legPos2[0], (double)legPos2[1]);
      this->LegendActor->SetNumberOfEntries(num);
      for (int i=0; i<num; i++)
        {
        if ( ! this->LegendActor->GetEntrySymbol(i) )
          {
          this->LegendActor->SetEntrySymbol(i,this->GlyphSource->GetOutput());
          }
        if ( ! this->LegendActor->GetEntryString(i) )
          {
          static char legendString[12];
          sprintf(legendString, "%s%d", "Curve ", i);
          this->LegendActor->SetEntryString(i,legendString);
          }
        }

      this->LegendActor->SetPadding(2);
      this->LegendActor->GetProperty()->DeepCopy(this->GetProperty());
      this->LegendActor->ScalarVisibilityOff();
      }

    // Rebuid text props
    // Perform shallow copy here since each individual axis can be
    // accessed through the class API (i.e. each individual axis text prop
    // can be changed). Therefore, we can not just assign pointers otherwise
    // each individual axis text prop would point to the same text prop.

    if (this->AxisLabelTextProperty &&
        this->AxisLabelTextProperty->GetMTime() > this->BuildTime)
      {
      if (this->XAxis->GetTitleTextProperty())
        {
        this->XAxis->GetLabelTextProperty()->ShallowCopy(
          this->AxisLabelTextProperty);
        }
      if (this->YAxis->GetTitleTextProperty())
        {
        this->YAxis->GetLabelTextProperty()->ShallowCopy(
          this->AxisLabelTextProperty);
        }
      }
    
    if (this->AxisTitleTextProperty &&
        this->AxisTitleTextProperty->GetMTime() > this->BuildTime)
      {
      if (this->XAxis->GetTitleTextProperty())
        {
        this->XAxis->GetTitleTextProperty()->ShallowCopy(
          this->AxisTitleTextProperty);
        }
      if (this->YAxis->GetTitleTextProperty())
        {
        this->YAxis->GetTitleTextProperty()->ShallowCopy(
          this->AxisTitleTextProperty);
        }
      }
    
    // setup x-axis
    vtkDebugMacro(<<"Rebuilding x-axis");
    
    this->XAxis->SetTitle(this->XTitle);
    this->XAxis->SetNumberOfLabels(this->NumberOfXLabels);
    this->XAxis->SetProperty(this->GetProperty());

    lengths = new double[num];
    if ( numDS > 0 ) //plotting data sets
      {
      this->ComputeXRange(range, lengths);
      }
    else
      {
      this->ComputeDORange(range, yrange, lengths);
      }
    if ( this->XRange[0] < this->XRange[1] )
      {
      range[0] = this->XRange[0];
      range[1] = this->XRange[1];
      }

    vtkAxisActor2D::ComputeRange(range, xRange, this->NumberOfXLabels,
                                 numTicks, interval);
    if ( !this->ExchangeAxes )
      {
      this->XComputedRange[0] = xRange[0];
      this->XComputedRange[1] = xRange[1];
      if ( this->ReverseXAxis )
        {
        this->XAxis->SetRange(range[1],range[0]);
        }
      else
        {
        this->XAxis->SetRange(range[0],range[1]);
        }
      }
    else
      {
      this->XComputedRange[1] = xRange[0];
      this->XComputedRange[0] = xRange[1];
      if ( this->ReverseYAxis )
        {
        this->XAxis->SetRange(range[0],range[1]);
        }
      else
        {
        this->XAxis->SetRange(range[1],range[0]);
        }
      }
    
    // setup y-axis
    vtkDebugMacro(<<"Rebuilding y-axis");
    this->YAxis->SetTitle(this->YTitle);
    this->YAxis->SetNumberOfLabels(this->NumberOfYLabels);

    if ( this->YRange[0] >= this->YRange[1] )
      {
      if ( numDS > 0 ) //plotting data sets
        {
        this->ComputeYRange(yrange);
        }
      }
    else
      {
      yrange[0] = this->YRange[0];
      yrange[1] = this->YRange[1];
      }
    vtkAxisActor2D::ComputeRange(yrange, yRange, this->NumberOfYLabels,
                                 numTicks, interval);

    if ( !this->ExchangeAxes )
      {
      this->YComputedRange[0] = yRange[0];
      this->YComputedRange[1] = yRange[1];
      if ( this->ReverseYAxis )
        {
        this->YAxis->SetRange(yrange[0],yrange[1]);
        }
      else
        {
        this->YAxis->SetRange(yrange[1],yrange[0]);
        }
      }
    else
      {
      this->YComputedRange[1] = yRange[0];
      this->YComputedRange[0] = yRange[1];
      if ( this->ReverseXAxis )
        {
        this->YAxis->SetRange(yrange[1],yrange[0]);
        }
      else
        {
        this->YAxis->SetRange(yrange[0],yrange[1]);
        }
      }

    this->PlaceAxes(viewport, size, pos, pos2);
    
    // manage title
    if (this->Title != NULL && this->Title[0])
      {
      this->TitleMapper->SetInput(this->Title);
      if (this->TitleTextProperty->GetMTime() > this->BuildTime)
        {
        this->TitleMapper->GetTextProperty()->ShallowCopy(
          this->TitleTextProperty);
        }

      vtkAxisActor2D::SetFontSize(viewport, 
                                  this->TitleMapper, 
                                  size, 
                                  1.0,
                                  stringSize);

      this->TitleActor->GetPositionCoordinate()->SetValue(
        pos[0] + 0.5 * (pos2[0] - pos[0]) - stringSize[0] / 2.0, 
        pos2[1] - stringSize[1] / 2.0);

      this->TitleActor->SetProperty(this->GetProperty());
      }

    vtkDebugMacro(<<"Creating Plot Data");
    // Okay, now create the plot data and set up the pipeline
    this->CreatePlotData(pos, pos2, xRange, yRange, lengths, numDS, numDO);
    delete [] lengths;
    
    this->BuildTime.Modified();

    }//if need to rebuild the plot

  vtkDebugMacro(<<"Rendering Axes");
  renderedSomething += this->XAxis->RenderOpaqueGeometry(viewport);
  renderedSomething += this->YAxis->RenderOpaqueGeometry(viewport);
  for (int i=0; i < this->NumberOfInputs; i++)
    {
    vtkDebugMacro(<<"Rendering plotactors");
    renderedSomething += this->PlotActor[i]->RenderOpaqueGeometry(viewport);
    }
  if ( this->Title != NULL )
    {
    vtkDebugMacro(<<"Rendering titleactors");
    renderedSomething += this->TitleActor->RenderOpaqueGeometry(viewport);
    }
  if ( this->Legend )
    {
    vtkDebugMacro(<<"Rendering legendeactors");
    renderedSomething += this->LegendActor->RenderOpaqueGeometry(viewport);
    }

  return renderedSomething;
}

//----------------------------------------------------------------------------
const char *vtkALBAXYPlotActor::GetXValuesAsString()
{
  switch (this->XValues)
    {
    case VTK_XYPLOT_INDEX:
      return "Index";
    case VTK_XYPLOT_ARC_LENGTH:
      return "ArcLength";
    case VTK_XYPLOT_NORMALIZED_ARC_LENGTH:
      return "NormalizedArcLength";
    default:
      return "Value";
    }
}

//----------------------------------------------------------------------------
const char *vtkALBAXYPlotActor::GetDataObjectPlotModeAsString()
{
  if ( this->XValues == VTK_XYPLOT_ROW ) 
    {
    return "Plot Rows";
    }
  else 
    {
    return "Plot Columns";
    }
}

//----------------------------------------------------------------------------
// Release any graphics resources that are being consumed by this actor.
// The parameter window could be used to determine which graphic
// resources to release.
void vtkALBAXYPlotActor::ReleaseGraphicsResources(vtkWindow *win)
{
  this->TitleActor->ReleaseGraphicsResources(win);
  this->XAxis->ReleaseGraphicsResources(win);
  this->YAxis->ReleaseGraphicsResources(win);
  for (int i=0; i < this->NumberOfInputs; i++)
    {
    this->PlotActor[i]->ReleaseGraphicsResources(win);
    }
  this->LegendActor->ReleaseGraphicsResources(win);
}

//----------------------------------------------------------------------------
unsigned long vtkALBAXYPlotActor::GetMTime()
{
  unsigned long mtime, mtime2;
  mtime = this->vtkActor2D::GetMTime();

  if (this->Legend)
    {
    mtime2 = this->LegendActor->GetMTime();
    if (mtime2 > mtime)
      {
      mtime = mtime2;
      }
    }

  return mtime;
}

//----------------------------------------------------------------------------
void vtkALBAXYPlotActor::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkIndent i2 = indent.GetNextIndent();
  vtkDataSet *input;
  char *array;
  int component;
  int idx, num;

  this->Superclass::PrintSelf(os,indent);

  this->InputList->InitTraversal();
  num = this->InputList->GetNumberOfItems();
  os << indent << "DataSetInputs: " << endl;
  for (idx = 0; idx < num; ++idx)
    {
    input = this->InputList->GetNextItem();
    array = this->SelectedInputScalars[idx];
    component = this->SelectedInputScalarsComponent->GetValue((vtkIdType)idx);
    if (array == NULL)
      {
      os << i2 << "(" << input << ") Default Scalars,  Component = " << component << endl;
      }
    else
      {
      os << i2 << "(" << input << ") " << array << ",  Component = " << component << endl;
      }
    }

  os << indent << "Input DataObjects:\n";
  this->DataObjectInputList->PrintSelf(os,indent.GetNextIndent());
  
  if (this->TitleTextProperty)
    {
    os << indent << "Title Text Property:\n";
    this->TitleTextProperty->PrintSelf(os,indent.GetNextIndent());
    }
  else
    {
    os << indent << "Title Text Property: (none)\n";
    }

  if (this->AxisTitleTextProperty)
    {
    os << indent << "Axis Title Text Property:\n";
    this->AxisTitleTextProperty->PrintSelf(os,indent.GetNextIndent());
    }
  else
    {
    os << indent << "Axis Title Text Property: (none)\n";
    }

  if (this->AxisLabelTextProperty)
    {
    os << indent << "Axis Label Text Property:\n";
    this->AxisLabelTextProperty->PrintSelf(os,indent.GetNextIndent());
    }
  else
    {
    os << indent << "Axis Label Text Property: (none)\n";
    }

  os << indent << "Data Object Plot Mode: " << this->GetDataObjectPlotModeAsString() << endl;

  os << indent << "Title: " << (this->Title ? this->Title : "(none)") << "\n";
  os << indent << "X Title: " 
     << (this->XTitle ? this->XTitle : "(none)") << "\n";
  os << indent << "Y Title: " 
     << (this->YTitle ? this->YTitle : "(none)") << "\n";
 
  os << indent << "X Values: " << this->GetXValuesAsString() << endl;
  os << indent << "Log X Values: " << (this->Logx ? "On\n" : "Off\n");

  os << indent << "Plot global-points: " << (this->PlotPoints ? "On\n" : "Off\n");
  os << indent << "Plot global-lines: " << (this->PlotLines ? "On\n" : "Off\n");
  os << indent << "Plot per-curve points: " << (this->PlotCurvePoints ? "On\n" : "Off\n");
  os << indent << "Plot per-curve lines: " << (this->PlotCurveLines ? "On\n" : "Off\n");
  os << indent << "Exchange Axes: " << (this->ExchangeAxes ? "On\n" : "Off\n");
  os << indent << "Reverse X Axis: " << (this->ReverseXAxis ? "On\n" : "Off\n");
  os << indent << "Reverse Y Axis: " << (this->ReverseYAxis ? "On\n" : "Off\n");

  os << indent << "Number Of X Labels: " << this->NumberOfXLabels << "\n";
  os << indent << "Number Of Y Labels: " << this->NumberOfYLabels << "\n";

  os << indent << "Label Format: " << this->LabelFormat << "\n";
  os << indent << "Border: " << this->Border << "\n";
  
  os << indent << "X Range: ";
  if ( this->XRange[0] >= this->XRange[1] )
    {
    os << indent << "(Automatically Computed)\n";
    }
  else
    {
    os << "(" << this->XRange[0] << ", " << this->XRange[1] << ")\n";
    }

  os << indent << "Y Range: ";
  if ( this->XRange[0] >= this->YRange[1] )
    {
    os << indent << "(Automatically Computed)\n";
    }
  else
    {
    os << "(" << this->YRange[0] << ", " << this->YRange[1] << ")\n";
    }

  os << indent << "Viewport Coordinate: ("
     << this->ViewportCoordinate[0] << ", " 
     << this->ViewportCoordinate[1] << ")\n";

  os << indent << "Plot Coordinate: ("
     << this->PlotCoordinate[0] << ", " 
     << this->PlotCoordinate[1] << ")\n";

  os << indent << "Legend: " << (this->Legend ? "On\n" : "Off\n");
  os << indent << "Legend Position: ("
     << this->LegendPosition[0] << ", " 
     << this->LegendPosition[1] << ")\n";
  os << indent << "Legend Position2: ("
     << this->LegendPosition2[0] << ", " 
     << this->LegendPosition2[1] << ")\n";

  os << indent << "Glyph Size: " << this->GlyphSize << endl;
}

//----------------------------------------------------------------------------
void vtkALBAXYPlotActor::ComputeXRange(double range[2], double *lengths)
{
  int dsNum;
  vtkIdType numPts, ptId, maxNum;
  double maxLength=0.0, xPrev[3], x[3];
  vtkDataSet *ds;

  range[0] = VTK_DOUBLE_MAX;
  range[1] = -VTK_DOUBLE_MAX;
  for ( dsNum=0, maxNum=0, this->InputList->InitTraversal(); 
        (ds = this->InputList->GetNextItem()); dsNum++)
    {
    numPts = ds->GetNumberOfPoints();

    if ( this->XValues != VTK_XYPLOT_INDEX )
      {
      ds->GetPoint(0, xPrev);
      for ( lengths[dsNum]=0.0, ptId=0; ptId < numPts; ptId++ )
        {
        ds->GetPoint(ptId, x);
        switch (this->XValues)
          {
          case VTK_XYPLOT_VALUE:
            if (this->GetLogx() == 0)
              {
              if ( x[this->XComponent->GetValue(dsNum)] < range[0] )
                {
                range[0] = x[this->XComponent->GetValue(dsNum)];
                }
              if ( x[this->XComponent->GetValue(dsNum)] > range[1] )
                {
                range[1] = x[this->XComponent->GetValue(dsNum)];
                }
              }
            else
              {
              //ensure range strictly > 0 for log
              if ( (x[this->XComponent->GetValue(dsNum)]) < range[0] && 
                   (x[this->XComponent->GetValue(dsNum)] > 0))
                {
                range[0] = x[this->XComponent->GetValue(dsNum)];
                }
              if ( (x[this->XComponent->GetValue(dsNum)] > range[1]) && 
                   (x[this->XComponent->GetValue(dsNum)] > 0))
                {
                range[1] = x[this->XComponent->GetValue(dsNum)];
                }
              }
            break;
          default:
            lengths[dsNum] += sqrt(vtkMath::Distance2BetweenPoints(x,xPrev));
            xPrev[0] = x[0]; xPrev[1] = x[1]; xPrev[2] = x[2];
          }
        }//for all points
      if ( lengths[dsNum] > maxLength )
        {
        maxLength = lengths[dsNum];
        }
      }//if need to visit all points
    
    else //if ( this->XValues == VTK_XYPLOT_INDEX )
      {
      if ( numPts > maxNum )
        {
        maxNum = numPts;
        }
      }
    }//over all datasets

  // determine the range
  switch (this->XValues)
    {
    case VTK_XYPLOT_ARC_LENGTH:
      range[0] = 0.0;
      range[1] = maxLength;
      break;
    case VTK_XYPLOT_NORMALIZED_ARC_LENGTH:
      range[0] = 0.0;
      range[1] = 1.0;
      break;
    case VTK_XYPLOT_INDEX:
      range[0] = 0.0;
      range[1] = (double)(maxNum - 1);
      break;
    case VTK_XYPLOT_VALUE:
      if (this->GetLogx() == 1)
        {
        if (range[0] > range[1]) 
          {
          range[0] = 0;
          range[1] = 0;
          }
        else
          {
          range[0] = log10(range[0]);
          range[1] = log10(range[1]);
          }
        }
      break; //range computed in for loop above
    default:
      vtkErrorMacro(<< "Unkown X-Value option.");
      return;
    }
}

//----------------------------------------------------------------------------
void vtkALBAXYPlotActor::ComputeYRange(double range[2])
{
  vtkDataSet *ds;
  vtkDataArray *scalars;
  double sRange[2];
  int count;
  int component;

  range[0]=VTK_DOUBLE_MAX, range[1]=(-VTK_DOUBLE_MAX);

  for ( this->InputList->InitTraversal(), count = 0; 
        (ds = this->InputList->GetNextItem()); ++count)
    {
    scalars = ds->GetPointData()->GetScalars(this->SelectedInputScalars[count]);
    component = this->SelectedInputScalarsComponent->GetValue(count);
    if ( !scalars)
      {
      vtkErrorMacro(<<"No scalar data to plot!");
      continue;
      }
    if ( component < 0 || component >= scalars->GetNumberOfComponents())
      {
      vtkErrorMacro(<<"Bad component!");
      continue;
      }
    
    scalars->GetRange(sRange, component);
    if ( sRange[0] < range[0] )
      {
      range[0] = sRange[0];
      }

    if ( sRange[1] > range[1] )
      {
      range[1] = sRange[1];
      }
    }//over all datasets
}

//----------------------------------------------------------------------------
void vtkALBAXYPlotActor::ComputeDORange(double xrange[2], double yrange[2], 
                                    double *lengths)
{
  int i;
  vtkDataObject *dobj;
  vtkFieldData *field;
  int doNum, numColumns;
  vtkIdType numTuples, numRows, num, ptId, maxNum;
  double maxLength=0.0, x, y, xPrev = 0.0;
  vtkDataArray *array;

  xrange[0] = yrange[0] = VTK_DOUBLE_MAX;
  xrange[1] = yrange[1] = -VTK_DOUBLE_MAX;
  for ( doNum=0, maxNum=0, this->DataObjectInputList->InitTraversal(); 
        (dobj = this->DataObjectInputList->GetNextItem()); doNum++)
    {
    lengths[doNum] = 0.0;
    field = dobj->GetFieldData();
    numColumns = field->GetNumberOfComponents(); //number of "columns"
    for (numRows = VTK_LARGE_ID, i=0; i<field->GetNumberOfArrays(); i++)
      {
      array = field->GetArray(i);
      numTuples = array->GetNumberOfTuples();
      if ( numTuples < numRows )
        {
        numRows = numTuples;
        }
      }

    num = (this->DataObjectPlotMode == VTK_XYPLOT_ROW ? 
           numColumns : numRows);

    if ( this->XValues != VTK_XYPLOT_INDEX )
      {
      // gather the information to form a plot
      for ( ptId=0; ptId < num; ptId++ )
        {
        if ( this->DataObjectPlotMode == VTK_XYPLOT_ROW )
          {
          x = field->GetComponent(this->XComponent->GetValue(doNum), ptId);
          }
        else //if ( this->DataObjectPlotMode == VTK_XYPLOT_COLUMN )
          {
          x = field->GetComponent(ptId, this->XComponent->GetValue(doNum));
          }
        if ( ptId == 0 )
          {
          xPrev = x;
          }
              
        switch (this->XValues)
          {
          case VTK_XYPLOT_VALUE:
            if (this->GetLogx() == 0)
              {
              if ( x < xrange[0] )
                {
                xrange[0] = x;
                }
              if ( x > xrange[1] )
                {
                xrange[1] = x;
                }
              }
            else //ensure positive values
              {
              if ( (x < xrange[0]) && (x > 0) )
                {
                xrange[0] = x;
                }
              if ( x > xrange[1]  && (x > 0) )
                {
                xrange[1] = x;
                }
              }
            break;
          default:
            lengths[doNum] += fabs(x-xPrev);
            xPrev = x;
          }
        }//for all points
      if ( lengths[doNum] > maxLength )
        {
        maxLength = lengths[doNum];
        }
      }//if all data has to be visited
    
    else //if (this->XValues == VTK_XYPLOT_INDEX)
      {
      if ( num > maxNum )
        {
        maxNum = num;
        }
      }

    // Get the y-values
    for ( ptId=0; ptId < num; ptId++ )
      {
      if ( this->DataObjectPlotMode == VTK_XYPLOT_ROW )
        {
        y = field->GetComponent(this->YComponent->GetValue(doNum), ptId);
        }
      else //if ( this->DataObjectPlotMode == VTK_XYPLOT_COLUMN )
        {
        y = field->GetComponent(ptId, this->YComponent->GetValue(doNum));
        }
      if ( y < yrange[0] )
        {
        yrange[0] = y;
        }
      if ( y > yrange[1] )
        {
        yrange[1] = y;
        }
      }//over all y values
    }//over all dataobjects

  // determine the range
  switch (this->XValues)
    {
    case VTK_XYPLOT_ARC_LENGTH:
      xrange[0] = 0.0;
      xrange[1] = maxLength;
      break;
    case VTK_XYPLOT_NORMALIZED_ARC_LENGTH:
      xrange[0] = 0.0;
      xrange[1] = 1.0;
      break;
    case VTK_XYPLOT_INDEX:
      xrange[0] = 0.0;
      xrange[1] = (double)(maxNum - 1);
      break;
    case VTK_XYPLOT_VALUE:
      if (this->GetLogx() == 1)
        {
        xrange[0] = log10(xrange[0]);
        xrange[1] = log10(xrange[1]);
        }
      break;
    default:
      vtkErrorMacro(<< "Unknown X-Value option");
      return;
    }
}

//----------------------------------------------------------------------------
void vtkALBAXYPlotActor::CreatePlotData(int *pos, int *pos2, double xRange[2], 
                                    double yRange[2], double *lengths,
                                    int numDS, int numDO)
{
  double xyz[3]; xyz[2] = 0.0;
  int i, numLinePts, dsNum, doNum, num;
  vtkIdType numPts, ptId, id;
  double length, x[3], xPrev[3];
  vtkDataArray *scalars;
  int component;
  vtkDataSet *ds;
  vtkCellArray *lines;
  vtkPoints *pts;
  int clippingRequired = 0;

  // Allocate resources for the polygonal plots
  //
  num = (numDS > numDO ? numDS : numDO);
  this->InitializeEntries();
  this->NumberOfInputs = num;
  this->PlotData = new vtkPolyData* [num];
  this->PlotGlyph = new vtkGlyph2D* [num];
  this->PlotAppend = new vtkAppendPolyData* [num];
  this->PlotMapper = new vtkPolyDataMapper2D* [num];
  this->PlotActor = new vtkActor2D* [num];
  for (i=0; i<num; i++)
    {
    this->PlotData[i] = vtkPolyData::New();
    this->PlotGlyph[i] = vtkGlyph2D::New();
    this->PlotGlyph[i]->SetInput(this->PlotData[i]);
    this->PlotGlyph[i]->SetScaleModeToDataScalingOff();
    this->PlotAppend[i] = vtkAppendPolyData::New();
    this->PlotAppend[i]->AddInput(this->PlotData[i]);
    if ( this->LegendActor->GetEntrySymbol(i) != NULL &&
         this->LegendActor->GetEntrySymbol(i) != this->GlyphSource->GetOutput() )
      {
      this->PlotGlyph[i]->SetSource(this->LegendActor->GetEntrySymbol(i));
      this->PlotGlyph[i]->SetScaleFactor(this->ComputeGlyphScale(i,pos,pos2));
      this->PlotAppend[i]->AddInput(this->PlotGlyph[i]->GetOutput());
      }
    this->PlotMapper[i] = vtkPolyDataMapper2D::New();
    this->PlotMapper[i]->SetInput(this->PlotAppend[i]->GetOutput());
    this->PlotMapper[i]->ScalarVisibilityOff();
    this->PlotActor[i] = vtkActor2D::New();
    this->PlotActor[i]->SetMapper(this->PlotMapper[i]);
    this->PlotActor[i]->GetProperty()->DeepCopy(this->GetProperty());
    if ( this->LegendActor->GetEntryColor(i)[0] < 0.0 )
      {
      this->PlotActor[i]->GetProperty()->SetColor(
        this->GetProperty()->GetColor());
      }
    else
      {
      this->PlotActor[i]->GetProperty()->SetColor(
        this->LegendActor->GetEntryColor(i));
      }
    }

  // Prepare to receive data
  this->GenerateClipPlanes(pos,pos2);
  for (i=0; i<this->NumberOfInputs; i++)
    {
    lines = vtkCellArray::New();
    pts = vtkPoints::New();

    lines->Allocate(10,10);
    pts->Allocate(10,10);
    this->PlotData[i]->SetPoints(pts);
    this->PlotData[i]->SetVerts(lines);
    this->PlotData[i]->SetLines(lines);

    pts->Delete();
    lines->Delete();
    }
   
  // Okay, for each input generate plot data. Depending on the input
  // we use either dataset or data object.
  //
  if ( numDS > 0 )
    {
    for ( dsNum=0, this->InputList->InitTraversal(); 
          (ds = this->InputList->GetNextItem()); dsNum++ )
      {
      clippingRequired = 0;
      numPts = ds->GetNumberOfPoints();
      scalars = ds->GetPointData()->GetScalars(this->SelectedInputScalars[dsNum]);
      if ( !scalars)
        {
        continue;
        }
      component = this->SelectedInputScalarsComponent->GetValue(dsNum);
      if ( component < 0 || component >= scalars->GetNumberOfComponents())
        {
        continue;
        }

      pts = this->PlotData[dsNum]->GetPoints();
      lines = this->PlotData[dsNum]->GetLines();
      lines->InsertNextCell(0); //update the count later

      ds->GetPoint(0, xPrev);
      for ( numLinePts=0, length=0.0, ptId=0; ptId < numPts; ptId++ )
        {
        xyz[1] = scalars->GetComponent(ptId, component);
        ds->GetPoint(ptId, x);
        switch (this->XValues)
          {
          case VTK_XYPLOT_NORMALIZED_ARC_LENGTH:
            length += sqrt(vtkMath::Distance2BetweenPoints(x,xPrev));
            xyz[0] = length / lengths[dsNum];
            xPrev[0] = x[0]; xPrev[1] = x[1]; xPrev[2] = x[2];
            break;
          case VTK_XYPLOT_INDEX:
            xyz[0] = (double)ptId;
            break;
          case VTK_XYPLOT_ARC_LENGTH:
            length += sqrt(vtkMath::Distance2BetweenPoints(x,xPrev));
            xyz[0] = length;
            xPrev[0] = x[0]; xPrev[1] = x[1]; xPrev[2] = x[2];
            break;
          case VTK_XYPLOT_VALUE:
            xyz[0] = x[this->XComponent->GetValue(dsNum)];
            break;
          default:
            vtkErrorMacro(<< "Unknown X-Component option");
          }
        
        if ( this->GetLogx() == 1 )
          {
          if (xyz[0] > 0)
            {
            xyz[0] = log10(xyz[0]);
            // normalize and position
            if ( xyz[0] < xRange[0] || xyz[0] > xRange[1] ||
                 xyz[1] < yRange[0] || xyz[1] > yRange[1] )
              {
              clippingRequired = 1;
              }

            numLinePts++;
            xyz[0] = pos[0] + 
              (xyz[0]-xRange[0])/(xRange[1]-xRange[0])*(pos2[0]-pos[0]);
            xyz[1] = pos[1] + 
              (xyz[1]-yRange[0])/(yRange[1]-yRange[0])*(pos2[1]-pos[1]);
            id = pts->InsertNextPoint(xyz);
            lines->InsertCellPoint(id);
            }
          } 
        else
          {
          // normalize and position
          if ( xyz[0] < xRange[0] || xyz[0] > xRange[1] ||
               xyz[1] < yRange[0] || xyz[1] > yRange[1] )
            {
            clippingRequired = 1;
            }

          numLinePts++;
          xyz[0] = pos[0] + 
            (xyz[0]-xRange[0])/(xRange[1]-xRange[0])*(pos2[0]-pos[0]);
          xyz[1] = pos[1] + 
            (xyz[1]-yRange[0])/(yRange[1]-yRange[0])*(pos2[1]-pos[1]);
          id = pts->InsertNextPoint(xyz);
          lines->InsertCellPoint(id);
          }
        }//for all input points

      lines->UpdateCellCount(numLinePts);
      if ( clippingRequired )
        {
        this->ClipPlotData(pos,pos2,this->PlotData[dsNum]);
        }
      }//loop over all input data sets
    }//if plotting datasets

  else //plot data from data objects
    {
    vtkDataObject *dobj;
    int numColumns;
    vtkIdType numRows, numTuples;
    vtkDataArray *array;
    vtkFieldData *field;
    for ( doNum=0, this->DataObjectInputList->InitTraversal(); 
          (dobj = this->DataObjectInputList->GetNextItem()); doNum++ )
      {
      // determine the shape of the field
      field = dobj->GetFieldData();
      numColumns = field->GetNumberOfComponents(); //number of "columns"
      for (numRows = VTK_LARGE_ID, i=0; i<field->GetNumberOfArrays(); i++)
        {
        array = field->GetArray(i);
        numTuples = array->GetNumberOfTuples();
        if ( numTuples < numRows )
          {
          numRows = numTuples;
          }
        }

      pts = this->PlotData[doNum]->GetPoints();
      lines = this->PlotData[doNum]->GetLines();
      lines->InsertNextCell(0); //update the count later

      numPts = (this->DataObjectPlotMode == VTK_XYPLOT_ROW ? 
                numColumns : numRows);

      // gather the information to form a plot
      for ( numLinePts=0, length=0.0, ptId=0; ptId < numPts; ptId++ )
        {
        if ( this->DataObjectPlotMode == VTK_XYPLOT_ROW )
          {
          x[0] = field->GetComponent(this->XComponent->GetValue(doNum),ptId);
          xyz[1] = field->GetComponent(this->YComponent->GetValue(doNum),ptId);
          }
        else //if ( this->DataObjectPlotMode == VTK_XYPLOT_COLUMN )
          {
          x[0] = field->GetComponent(ptId, this->XComponent->GetValue(doNum));
          xyz[1] = field->GetComponent(ptId, this->YComponent->GetValue(doNum));
          }

        switch (this->XValues)
          {
          case VTK_XYPLOT_NORMALIZED_ARC_LENGTH:
            length += fabs(x[0]-xPrev[0]);
            xyz[0] = length / lengths[doNum];
            xPrev[0] = x[0];
            break;
          case VTK_XYPLOT_INDEX:
            xyz[0] = (double)ptId;
            break;
          case VTK_XYPLOT_ARC_LENGTH:
            length += fabs(x[0]-xPrev[0]);
            xyz[0] = length;
            xPrev[0] = x[0];
            break;
          case VTK_XYPLOT_VALUE:
            xyz[0] = x[0];
            break;
          default:
            vtkErrorMacro(<< "Unknown X-Value option");
          }
        


        if ( this->GetLogx() == 1 )
          {
          if (xyz[0] > 0)
            {
            xyz[0] = log10(xyz[0]);
            // normalize and position
            if ( xyz[0] < xRange[0] || xyz[0] > xRange[1] ||
                 xyz[1] < yRange[0] || xyz[1] > yRange[1] )
              {
              clippingRequired = 1;
              }
            numLinePts++;
            xyz[0] = pos[0] + 
              (xyz[0]-xRange[0])/(xRange[1]-xRange[0])*(pos2[0]-pos[0]);
            xyz[1] = pos[1] + 
              (xyz[1]-yRange[0])/(yRange[1]-yRange[0])*(pos2[1]-pos[1]);
            id = pts->InsertNextPoint(xyz);
            lines->InsertCellPoint(id);
            }
          } 
        else
          {
          // normalize and position
          if ( xyz[0] < xRange[0] || xyz[0] > xRange[1] ||
               xyz[1] < yRange[0] || xyz[1] > yRange[1] )
            {
            clippingRequired = 1;
            }    
          numLinePts++;
          xyz[0] = pos[0] + 
            (xyz[0]-xRange[0])/(xRange[1]-xRange[0])*(pos2[0]-pos[0]);
          xyz[1] = pos[1] + 
            (xyz[1]-yRange[0])/(yRange[1]-yRange[0])*(pos2[1]-pos[1]);
          id = pts->InsertNextPoint(xyz);
          lines->InsertCellPoint(id);
          }
        }//for all input points

      lines->UpdateCellCount(numLinePts);
      if ( clippingRequired )
        {
        this->ClipPlotData(pos,pos2,this->PlotData[doNum]);
        }
      }//loop over all input data sets
    }
  
  // Remove points/lines as directed by the user
  for ( i = 0; i < num; i++)
    {
    if (!this->PlotCurveLines) 
      {
      if ( !this->PlotLines ) 
        {
        this->PlotData[i]->SetLines(NULL);
        }
      }
    else
      {
      if ( this->GetPlotLines(i) == 0)
        {
        this->PlotData[i]->SetLines(NULL);
        }
      }

    if (!this->PlotCurvePoints) 
      {
      if ( !this->PlotPoints || (this->LegendActor->GetEntrySymbol(i) &&
                                 this->LegendActor->GetEntrySymbol(i) != 
                                 this->GlyphSource->GetOutput()))
        {
        this->PlotData[i]->SetVerts(NULL);
        }
      }
    else
      {
      if ( this->GetPlotPoints(i) == 0 || 
           (this->LegendActor->GetEntrySymbol(i) &&
            this->LegendActor->GetEntrySymbol(i) != 
            this->GlyphSource->GetOutput()))
        {
        this->PlotData[i]->SetVerts(NULL);
        }
      }
    }
}

//----------------------------------------------------------------------------
// Position the axes taking into account the expected padding due to labels
// and titles. We want the result to fit in the box specified. This method
// knows something about how the vtkAxisActor2D functions, so it may have 
// to change if that class changes dramatically.
//
void vtkALBAXYPlotActor::PlaceAxes(vtkViewport *viewport, int *size,
                               int pos[2], int pos2[2])
{
  int titleSizeX[2], titleSizeY[2], labelSizeX[2], labelSizeY[2];
  double labelFactorX, labelFactorY;
  double fontFactorX, fontFactorY;
  double tickOffsetX, tickOffsetY;
  double tickLengthX, tickLengthY;

  vtkAxisActor2D *axisX;
  vtkAxisActor2D *axisY;

  char str1[512], str2[512];

  if (this->ExchangeAxes)
    {
    axisX = this->YAxis;
    axisY = this->XAxis;
    }
  else
    {
    axisX = this->XAxis;
    axisY = this->YAxis;
    }

  fontFactorY = axisY->GetFontFactor();
  fontFactorX = axisX->GetFontFactor();

  labelFactorY = axisY->GetLabelFactor();
  labelFactorX = axisX->GetLabelFactor();

  // Create a dummy text mapper for getting font sizes
  vtkTextMapper *textMapper = vtkTextMapper::New();
  vtkTextProperty *tprop = textMapper->GetTextProperty();

  // Get the location of the corners of the box
  int *p1 = this->PositionCoordinate->GetComputedViewportValue(viewport);
  int *p2 = this->Position2Coordinate->GetComputedViewportValue(viewport);

  // Estimate the padding around the X and Y axes
  tprop->ShallowCopy(axisX->GetTitleTextProperty());
  textMapper->SetInput(axisX->GetTitle());
  vtkAxisActor2D::SetFontSize(
    viewport, textMapper, size, fontFactorX, titleSizeX);

  tprop->ShallowCopy(axisY->GetTitleTextProperty());
  textMapper->SetInput(axisY->GetTitle());
  vtkAxisActor2D::SetFontSize(
    viewport, textMapper, size, fontFactorY, titleSizeY);

  // At this point the thing to do would be to actually ask the Y axis
  // actor to return the largest label.
  // In the meantime, let's try with the min and max
  sprintf(str1, axisY->GetLabelFormat(), axisY->GetAdjustedRange()[0]);
  sprintf(str2, axisY->GetLabelFormat(), axisY->GetAdjustedRange()[1]);
  tprop->ShallowCopy(axisY->GetLabelTextProperty());
  textMapper->SetInput(strlen(str1) > strlen(str2) ? str1 : str2);
  vtkAxisActor2D::SetFontSize(
    viewport, textMapper, size, labelFactorY * fontFactorY, labelSizeY);

  // We do only care of the height of the label in the X axis, so let's
  // use the min for example
  sprintf(str1, axisX->GetLabelFormat(), axisX->GetAdjustedRange()[0]);
  tprop->ShallowCopy(axisX->GetLabelTextProperty());
  textMapper->SetInput(str1);
  vtkAxisActor2D::SetFontSize(
    viewport, textMapper, size, labelFactorX * fontFactorX, labelSizeX);

  tickOffsetX = axisX->GetTickOffset();
  tickOffsetY = axisY->GetTickOffset();
  tickLengthX = axisX->GetTickLength();
  tickLengthY = axisY->GetTickLength();

  // Okay, estimate the size
  pos[0] = (int)(p1[0] + titleSizeY[0] + 2.0 * tickOffsetY + tickLengthY + 
                 labelSizeY[0] + this->Border);

  pos[1] = (int)(p1[1] + titleSizeX[1] + 2.0 * tickOffsetX + tickLengthX + 
                 labelSizeX[1] + this->Border);

  pos2[0] = (int)(p2[0] - labelSizeY[0] / 2 - tickOffsetY - this->Border);

  pos2[1] = (int)(p2[1] - labelSizeX[1] / 2 - tickOffsetX - this->Border);

  // Now specify the location of the axes
  axisX->GetPositionCoordinate()->SetValue(
    (double)pos[0], (double)pos[1]);
  axisX->GetPosition2Coordinate()->SetValue(
    (double)pos2[0], (double)pos[1]);
  axisY->GetPositionCoordinate()->SetValue(
    (double)pos[0], (double)pos2[1]);
  axisY->GetPosition2Coordinate()->SetValue(
    (double)pos[0], (double)pos[1]);

  textMapper->Delete();
}

//----------------------------------------------------------------------------
void vtkALBAXYPlotActor::ViewportToPlotCoordinate(vtkViewport *viewport, double &u, double &v)
{
  int *p0, *p1, *p2;

  // XAxis, YAxis are in viewport coordinates already
  p0 = this->XAxis->GetPositionCoordinate()->GetComputedViewportValue(viewport);
  p1 = this->XAxis->GetPosition2Coordinate()->GetComputedViewportValue(viewport);
  p2 = this->YAxis->GetPositionCoordinate()->GetComputedViewportValue(viewport);

  u = ((u - p0[0]) / (double)(p1[0] - p0[0]))
    *(this->XComputedRange[1] - this->XComputedRange[0])
    + this->XComputedRange[0];
  v = ((v - p0[1]) / (double)(p2[1] - p0[1]))
    *(this->YComputedRange[1] - this->YComputedRange[0])
    + this->YComputedRange[0];
}

//----------------------------------------------------------------------------
void vtkALBAXYPlotActor::PlotToViewportCoordinate(vtkViewport *viewport,
                                              double &u, double &v)
{
  int *p0, *p1, *p2;

  // XAxis, YAxis are in viewport coordinates already
  p0 = this->XAxis->GetPositionCoordinate()->GetComputedViewportValue(viewport);
  p1 = this->XAxis->GetPosition2Coordinate()->GetComputedViewportValue(viewport);
  p2 = this->YAxis->GetPositionCoordinate()->GetComputedViewportValue(viewport);

  u = (((u - this->XComputedRange[0])
        / (this->XComputedRange[1] - this->XComputedRange[0]))
       * (double)(p1[0] - p0[0])) + p0[0];
  v = (((v - this->YComputedRange[0])
        / (this->YComputedRange[1] - this->YComputedRange[0]))
       * (double)(p2[1] - p0[1])) + p0[1];
}

//----------------------------------------------------------------------------
void vtkALBAXYPlotActor::ViewportToPlotCoordinate(vtkViewport *viewport)
{
  this->ViewportToPlotCoordinate(viewport, 
                                 this->ViewportCoordinate[0],
                                 this->ViewportCoordinate[1]);
}

//----------------------------------------------------------------------------
void vtkALBAXYPlotActor::PlotToViewportCoordinate(vtkViewport *viewport)
{
  this->PlotToViewportCoordinate(viewport, 
                                 this->PlotCoordinate[0],
                                 this->PlotCoordinate[1]);
}

//----------------------------------------------------------------------------
int vtkALBAXYPlotActor::IsInPlot(vtkViewport *viewport, double u, double v)
{
  int *p0, *p1, *p2;

  // Bounds of the plot are based on the axes...
  p0 = this->XAxis->GetPositionCoordinate()->GetComputedViewportValue(viewport);
  p1 = this->XAxis->GetPosition2Coordinate()->GetComputedViewportValue(viewport);
  p2 = this->YAxis->GetPositionCoordinate()->GetComputedViewportValue(viewport);
  
  if (u >= p0[0] && u <= p1[0] && v >= p0[1] && v <= p2[1])
    {
    return 1;
    }

  return 0;
}

//----------------------------------------------------------------------------
void vtkALBAXYPlotActor::SetPlotLines(int i, int isOn)
{
  i = ( i < 0 ? 0 : (i >=VTK_MAX_PLOTS ? VTK_MAX_PLOTS-1 : i));
  int val = this->LinesOn->GetValue(i);
  if ( val != isOn )
    {
    this->Modified();
    this->LinesOn->SetValue(i, isOn);
    }
}

//----------------------------------------------------------------------------
int vtkALBAXYPlotActor::GetPlotLines(int i)
{
  i = ( i < 0 ? 0 : (i >=VTK_MAX_PLOTS ? VTK_MAX_PLOTS-1 : i));
  return this->LinesOn->GetValue(i);
}

//----------------------------------------------------------------------------
void vtkALBAXYPlotActor::SetPlotPoints(int i, int isOn)
{
  i = ( i < 0 ? 0 : (i >=VTK_MAX_PLOTS ? VTK_MAX_PLOTS-1 : i));
  int val = this->PointsOn->GetValue(i);
  if ( val != isOn )
    {
    this->Modified();
    this->PointsOn->SetValue(i, isOn);
    }
}

//----------------------------------------------------------------------------
int vtkALBAXYPlotActor::GetPlotPoints(int i)
{
  i = ( i < 0 ? 0 : (i >=VTK_MAX_PLOTS ? VTK_MAX_PLOTS-1 : i));
  return this->PointsOn->GetValue(i);
}

//----------------------------------------------------------------------------
void vtkALBAXYPlotActor::SetPlotColor(int i, double r, double g, double b)
{
  this->LegendActor->SetEntryColor(i, r, g, b);
}

//----------------------------------------------------------------------------
double *vtkALBAXYPlotActor::GetPlotColor(int i)
{
  return this->LegendActor->GetEntryColor(i);
}

//----------------------------------------------------------------------------
void vtkALBAXYPlotActor::SetPlotSymbol(int i,vtkPolyData *input)
{
  this->LegendActor->SetEntrySymbol(i, input);
}

//----------------------------------------------------------------------------
vtkPolyData *vtkALBAXYPlotActor::GetPlotSymbol(int i)
{
  return this->LegendActor->GetEntrySymbol(i);
}

//----------------------------------------------------------------------------
void vtkALBAXYPlotActor::SetPlotLabel(int i, const char *label)
{
  this->LegendActor->SetEntryString(i, label);
}

//----------------------------------------------------------------------------
const char *vtkALBAXYPlotActor::GetPlotLabel(int i)
{
  return this->LegendActor->GetEntryString(i);
}

//----------------------------------------------------------------------------
void vtkALBAXYPlotActor::GenerateClipPlanes(int *pos, int *pos2)
{
  double n[3], x[3];
  vtkPoints *pts=this->ClipPlanes->GetPoints();
  vtkDataArray *normals=this->ClipPlanes->GetNormals();
  
  n[2] = x[2] = 0.0;
  
  //first
  n[0] = 0.0;
  n[1] = -1.0;
  normals->SetTuple(0,n);
  x[0] = (double)0.5*(pos[0]+pos2[0]);
  x[1] = (double)pos[1];
  pts->SetPoint(0,x);
  
  //second
  n[0] = 1.0;
  n[1] = 0.0;
  normals->SetTuple(1,n);
  x[0] = (double)pos2[0];
  x[1] = (double)0.5*(pos[1]+pos2[1]);
  pts->SetPoint(1,x);
  
  //third
  n[0] = 0.0;
  n[1] = 1.0;
  normals->SetTuple(2,n);
  x[0] = (double)0.5*(pos[0]+pos2[0]);
  x[1] = (double)pos2[1];
  pts->SetPoint(2,x);
  
  //fourth
  n[0] = -1.0;
  n[1] = 0.0;
  normals->SetTuple(3,n);
  x[0] = (double)pos[0];
  x[1] = (double)0.5*(pos[1]+pos2[1]);
  pts->SetPoint(3,x);
}

//----------------------------------------------------------------------------
double vtkALBAXYPlotActor::ComputeGlyphScale(int i, int *pos, int *pos2)
{
  vtkPolyData *pd=this->LegendActor->GetEntrySymbol(i);
  pd->Update();
  double length=pd->GetLength();
  double sf = this->GlyphSize * sqrt((double)(pos[0]-pos2[0])*(pos[0]-pos2[0]) + 
                                    (pos[1]-pos2[1])*(pos[1]-pos2[1])) / length;

  return sf;
}

//----------------------------------------------------------------------------
//This assumes that there are multiple polylines
void vtkALBAXYPlotActor::ClipPlotData(int *pos, int *pos2, vtkPolyData *pd)
{
  vtkPoints *points=pd->GetPoints();
  vtkPoints *newPoints;
  vtkCellArray *lines=pd->GetLines();
  vtkCellArray *newLines, *newVerts;
  vtkIdType numPts=pd->GetNumberOfPoints();
  vtkIdType npts = 0;
  vtkIdType newPts[2];
  vtkIdType *pts=0;
  vtkIdType i, id;
  int j;
  int added;
  double x1[3], x2[3], px[3], n[3], xint[3], t;
  double p1[2], p2[2];

  p1[0] = (double)pos[0]; p1[1] = (double)pos[1];
  p2[0] = (double)pos2[0]; p2[1] = (double)pos2[1];
  
  newPoints = vtkPoints::New();
  newPoints->Allocate(numPts);
  newVerts = vtkCellArray::New();
  newVerts->Allocate(lines->GetSize());
  newLines = vtkCellArray::New();
  newLines->Allocate(2*lines->GetSize());
  int *pointMap = new int [numPts];
  for (i=0; i<numPts; i++)
    {
    pointMap[i] = -1;
    }
  
  //Loop over polyverts eliminating those that are outside
  for ( lines->InitTraversal(); lines->GetNextCell(npts,pts); )
    {
    //loop over verts keeping only those that are not clipped
    for (i=0; i<npts; i++)
      {
      points->GetPoint(pts[i], x1);

      if (x1[0] >= p1[0] && x1[0] <= p2[0] && x1[1] >= p1[1] && x1[1] <= p2[1] )
        {
        id = newPoints->InsertNextPoint(x1);
        pointMap[i] = id;
        newPts[0] = id;
        newVerts->InsertNextCell(1,newPts);
        }
      }
    }

  //Loop over polylines clipping each line segment
  for ( lines->InitTraversal(); lines->GetNextCell(npts,pts); )
    {
    //loop over line segment making up the polyline
    for (i=0; i<(npts-1); i++)
      {
      points->GetPoint(pts[i], x1);
      points->GetPoint(pts[i+1], x2);

      //intersect each segment with the four planes
      if ( (x1[0] < p1[0] && x2[0] < p1[0]) || (x1[0] > p2[0] && x2[0] > p2[0]) ||
           (x1[1] < p1[1] && x2[1] < p1[1]) || (x1[1] > p2[1] && x2[1] > p2[1]) )
        {
        ;//trivial rejection
        }
      else if (x1[0] >= p1[0] && x2[0] >= p1[0] && x1[0] <= p2[0] && x2[0] <= p2[0] &&
               x1[1] >= p1[1] && x2[1] >= p1[1] && x1[1] <= p2[1] && x2[1] <= p2[1] )
        {//trivial acceptance
        newPts[0] = pointMap[pts[i]];
        newPts[1] = pointMap[pts[i+1]];
        newLines->InsertNextCell(2,newPts);
        }
      else
        {
        added = 0;
        if (x1[0] >= p1[0] && x1[0] <= p2[0] && x1[1] >= p1[1] && x1[1] <= p2[1] )
        {//first point in
          newPts[0] = pointMap[pts[i]];
          added=1;
        }
        else if (x2[0] >= p1[0] && x2[0] <= p2[0] && x2[1] >= p1[1] && x2[1] <= p2[1] )
        {//second point in
          newPts[0] = pointMap[pts[i+1]];
          added=1;
        }

        //only create cell if either x1 or x2 is inside the range
        for (j=0; j<4; j++)
          {
            this->ClipPlanes->GetPoints()->GetPoint(j, px);
            this->ClipPlanes->GetNormals()->GetTuple(j, n);
            if ( vtkPlane::IntersectWithLine(x1,x2,n,px,t,xint) && t >= 0 && t <= 1.0 )
            {
              newPts[added] = newPoints->InsertNextPoint(xint);
              added++;
              if(added==2) break;
            }
          }
        
        newLines->InsertNextCell(2,newPts);
        }
      }
    }
  delete [] pointMap;
  
  //Update the lines
  pd->SetPoints(newPoints);
  pd->SetVerts(newVerts);
  pd->SetLines(newLines);
  
  newPoints->Delete();
  newVerts->Delete();
  newLines->Delete();
  
}

//----------------------------------------------------------------------------
void vtkALBAXYPlotActor::SetDataObjectXComponent(int i, int comp)
{
  i = ( i < 0 ? 0 : (i >=VTK_MAX_PLOTS ? VTK_MAX_PLOTS-1 : i));
  int val=this->XComponent->GetValue(i);
  if ( val != comp )
    {
    this->Modified();
    this->XComponent->SetValue(i,comp);
    }
}

//----------------------------------------------------------------------------
int vtkALBAXYPlotActor::GetDataObjectXComponent(int i)
{
  i = ( i < 0 ? 0 : (i >=VTK_MAX_PLOTS ? VTK_MAX_PLOTS-1 : i));
  return this->XComponent->GetValue(i);
}

//----------------------------------------------------------------------------
void vtkALBAXYPlotActor::SetDataObjectYComponent(int i, int comp)
{
  i = ( i < 0 ? 0 : (i >=VTK_MAX_PLOTS ? VTK_MAX_PLOTS-1 : i));
  int val=this->YComponent->GetValue(i);
  if ( val != comp )
    {
    this->Modified();
    this->YComponent->SetValue(i,comp);
    }
}

//----------------------------------------------------------------------------
int vtkALBAXYPlotActor::GetDataObjectYComponent(int i)
{
  i = ( i < 0 ? 0 : (i >=VTK_MAX_PLOTS ? VTK_MAX_PLOTS-1 : i));
  return this->YComponent->GetValue(i);
}

void vtkALBAXYPlotActor::SetPointComponent(int i, int comp)
{
  i = ( i < 0 ? 0 : (i >=VTK_MAX_PLOTS ? VTK_MAX_PLOTS-1 : i));
  int val=this->XComponent->GetValue(i);
  if ( val != comp )
    {
    this->Modified();
    this->XComponent->SetValue(i,comp);
    }
}

//----------------------------------------------------------------------------
int vtkALBAXYPlotActor::GetPointComponent(int i)
{
  i = ( i < 0 ? 0 : (i >=VTK_MAX_PLOTS ? VTK_MAX_PLOTS-1 : i));
  return this->XComponent->GetValue(i);
}

//----------------------------------------------------------------------------
double *vtkALBAXYPlotActor::TransformPoint(int pos[2], int pos2[2], double x[3], double xNew[3])
{
  // First worry about exchanging axes
  if ( this->ExchangeAxes )
    {
    double sx = (x[0]-pos[0]) / (pos2[0]-pos[0]);
    double sy = (x[1]-pos[1]) / (pos2[1]-pos[1]);
    xNew[0] = sy*(pos2[0]-pos[0]) + pos[0];
    xNew[1] = sx*(pos2[1]-pos[1]) + pos[1];
    xNew[2] = x[2];
    }
  else
    {
    xNew[0] = x[0];
    xNew[1] = x[1];
    xNew[2] = x[2];
    }

  // Okay, now swap the axes around if reverse is on
  if ( this->ReverseXAxis )
    {
    xNew[0] = pos[0] + (pos2[0]-xNew[0]);
    }
  if ( this->ReverseYAxis )
    {
    xNew[1] = pos[1] + (pos2[1]-xNew[1]);
    }

  return xNew;
}
    
//----------------------------------------------------------------------------
void vtkALBAXYPlotActor::SetLabelFormat(const char* _arg)
{
  if (this->LabelFormat == NULL && _arg == NULL) 
    { 
    return;
    }

  if (this->LabelFormat && _arg && (!strcmp(this->LabelFormat,_arg))) 
    { 
    return;
    }

  if (this->LabelFormat) 
    { 
    delete [] this->LabelFormat; 
    }

  if (_arg)
    {
    this->LabelFormat = new char[strlen(_arg)+1];
    strcpy(this->LabelFormat,_arg);
    }
  else
    {
    this->LabelFormat = NULL;
    }

  this->XAxis->SetLabelFormat(this->LabelFormat);
  this->YAxis->SetLabelFormat(this->LabelFormat);

  this->Modified();
}
