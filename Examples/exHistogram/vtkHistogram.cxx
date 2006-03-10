/*=========================================================================

  Program:   Multimod Fundation Library
  Module:    $RCSfile: vtkHistogram.cxx,v $
  Language:  C++
  Date:      $Date: 2006-03-10 09:37:03 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
  Project:   MultiMod Project

==========================================================================*/

#include "vtkHistogram.h"

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkObjectFactory.h"
#include "vtkImageAccumulate.h"
#include "vtkImageLogarithmicScale.h"
#include "vtkImageChangeInformation.h"
#include "vtkGlyph3D.h"
#include "vtkLineSource.h"
#include "vtkCoordinate.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper2D.h"
#include "vtkProperty2D.h"
#include "vtkTextMapper.h"
#include "vtkTextProperty.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkPointData.h"
#include "vtkDataArray.h"

vtkCxxRevisionMacro(vtkHistogram, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkHistogram);
//------------------------------------------------------------------------------
vtkHistogram::vtkHistogram()
//------------------------------------------------------------------------------
{
  TextMapper  = NULL;
  TextActor   = NULL;

  Accumulate  = NULL;
  ChangeInfo  = NULL;
  LogScale    = NULL;
  Glyph       = NULL;
  ImageData   = NULL;
  
  PointsRepresentation  = NULL;
  LineRepresentation    = NULL;

  HisctogramRepresentation = LINE_REPRESENTATION;

  Color[0] = Color[1] = Color[2] = 1.0;

  ScaleFactor       = 1.0;
  NumberOfBins      = 1;
  LogScaleConstant  = 10.0;
  RenderWidth       = 1;

  AutoscaleHistogram  = 1;
  LogHistogram        = 0;
  LabelVisibility     = 1;

  HistogramCreate();
}
//------------------------------------------------------------------------------
vtkHistogram::~vtkHistogram()
//------------------------------------------------------------------------------
{
  if(TextMapper)  TextMapper->Delete();
  if(TextActor)   TextActor->Delete();
  if(Accumulate)  Accumulate->Delete();
  if(ChangeInfo)  ChangeInfo->Delete();
  if(LogScale)    LogScale->Delete();
  if(Glyph)       Glyph->Delete();
  if(HistActor)   HistActor->Delete();
  if(PointsRepresentation) PointsRepresentation->Delete();
  if(LineRepresentation)   LineRepresentation->Delete();
}
//------------------------------------------------------------------------------
void vtkHistogram::PrintSelf(ostream& os, vtkIndent indent)
//------------------------------------------------------------------------------
{
  this->Superclass::PrintSelf(os,indent);
}
//------------------------------------------------------------------------------
int vtkHistogram::RenderOverlay(vtkViewport *viewport)
//------------------------------------------------------------------------------
{
  if (ImageData == NULL) {return 0;};

  vtkRenderer *ren = static_cast<vtkRenderer *>(viewport);
  HistogramUpdate(ren);
  this->Modified();

  HistActor->RenderOverlay(viewport);  
  if (LabelVisibility) TextActor->RenderOverlay(viewport);
  return 1;
}
//----------------------------------------------------------------------------
int vtkHistogram::RenderOpaqueGeometry(vtkViewport *viewport)
//----------------------------------------------------------------------------
{
  if (ImageData == NULL) {return 0;};

  if (LabelVisibility) TextActor->RenderOpaqueGeometry(viewport);
  return 0;
}
//----------------------------------------------------------------------------
void vtkHistogram::HistogramCreate()
//----------------------------------------------------------------------------
{
  TextMapper = vtkTextMapper::New();
  TextMapper->SetInput("");
  TextMapper->GetTextProperty()->AntiAliasingOff();
  TextMapper->GetTextProperty()->SetFontFamily(VTK_TIMES);
  TextMapper->GetTextProperty()->SetColor(1,1,1);
  TextMapper->GetTextProperty()->SetLineOffset(0.5);
  TextMapper->GetTextProperty()->SetLineSpacing(1.5);
  TextMapper->GetTextProperty()->SetJustificationToRight();
  TextMapper->GetTextProperty()->SetVerticalJustificationToTop();

  TextActor = vtkActor2D::New();
  TextActor->SetMapper(TextMapper);
  TextActor->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
  TextActor->SetPosition(1, 1);

  Accumulate = vtkImageAccumulate::New();

  LogScale = vtkImageLogarithmicScale::New();
  LogScale->SetConstant(LogScaleConstant);

  ChangeInfo = vtkImageChangeInformation::New();
  ChangeInfo->SetOutputOrigin(0,0,0);

  int cp[2] = {0,1};
  vtkPoints *p = vtkPoints::New();
  p->SetNumberOfPoints(2);
  p->SetPoint(0,0,0,0);
  p->SetPoint(1,0,1,0);
  vtkCellArray *ca = vtkCellArray::New();
  ca->Allocate(ca->EstimateSize(1,2));
  ca->InsertNextCell(2,cp);
  PointsRepresentation = vtkPolyData::New();
  PointsRepresentation->SetPoints(p);
  PointsRepresentation->SetVerts(ca);
  p->Delete();
  ca->Delete();

  LineRepresentation = vtkLineSource::New();
  LineRepresentation->SetPoint1(0,0,0);
  LineRepresentation->SetPoint2(0,1,0);
  LineRepresentation->Update();
  
  Glyph = vtkGlyph3D::New();
  Glyph->SetSource(LineRepresentation->GetOutput());
  Glyph->SetScaleModeToScaleByScalar();
  Glyph->OrientOn();
  
  vtkCoordinate *coordinate = vtkCoordinate::New();
  coordinate->SetCoordinateSystemToNormalizedDisplay();

  vtkPolyDataMapper2D *mapper2d = vtkPolyDataMapper2D::New();
  mapper2d->SetInput(Glyph->GetOutput());
  mapper2d->SetTransformCoordinate(coordinate);
  mapper2d->ScalarVisibilityOff();
  
  SetMapper(mapper2d); // "this" Actor is unhappy without a mapper -- anyway -- this mapper is never used
  PickableOff();
  
  coordinate->Delete();
  mapper2d->Delete();

  HistActor = vtkActor2D::New();
  HistActor->SetMapper(mapper2d);
}
//----------------------------------------------------------------------------
void vtkHistogram::HistogramUpdate(vtkRenderer *ren)
//----------------------------------------------------------------------------
{
  if (ImageData == NULL) {return;}

  double sr[2];
  ImageData->Update();
  ImageData->GetScalarRange(sr);
  double srw = sr[1]-sr[0];

  if (ImageData->GetScalarType() == VTK_CHAR || ImageData->GetScalarType() == VTK_UNSIGNED_CHAR )
  {
    // NumberOfBins  is 256 at most --- HistogramBins coincide with Scalars Values 
    NumberOfBins = srw; 
  }
  else if ( ImageData->GetScalarType() >= VTK_SHORT && ImageData->GetScalarType() <= VTK_UNSIGNED_INT )
  {
    // NumberOfBins  can be >> 256 --- HistogramBins << Scalars Values 
    NumberOfBins = srw;
    int i=1;
    while(NumberOfBins > 500 )
    {
      NumberOfBins = srw / i++;
    }
  }
  else 
  {
    NumberOfBins = ( srw > 500 ) ? 500 : srw ; 
  }

  Accumulate->SetInput(ImageData);
  Accumulate->SetComponentOrigin(sr[0],0,0);  
  Accumulate->SetComponentExtent(0,NumberOfBins,0,0,0,0);
  Accumulate->SetComponentSpacing(srw/NumberOfBins,0,0); // bins maps all the Scalars Range
  Accumulate->Update();

  double acc_min[3], acc_max[3];
  Accumulate->GetMin(acc_min);
  Accumulate->GetMax(acc_max);

  if (AutoscaleHistogram)
  {
    ScaleFactor = .05 / Accumulate->GetMean()[0];
  }

  ChangeInfo->SetInput(Accumulate->GetOutput());
  ChangeInfo->SetOutputSpacing(1.0/NumberOfBins,1,1);  // Histogram width = 1
  ChangeInfo->Update();

  LogScale->SetConstant(LogScaleConstant);
  LogScale->SetInput(ChangeInfo->GetOutput());
  LogScale->Update();

  if (LogHistogram)
  {
    if (AutoscaleHistogram)
    {
      ScaleFactor = .5 / log(1 + Accumulate->GetMean()[0]);
    }
    Glyph->SetInput(LogScale->GetOutput());
  }
  else
  {
    Glyph->SetInput(ChangeInfo->GetOutput());
  }
  Glyph->SetScaleFactor(ScaleFactor);

  GetProperty()->SetColor(Color);
  TextActor->GetProperty()->SetColor(Color);
  RenderWidth = ren->GetSize()[0];

  if (HisctogramRepresentation == BAR_REPRESENTATION) 
  {
    Glyph->SetSource(LineRepresentation->GetOutput());
    float line_width = RenderWidth / (float)(NumberOfBins - 1);
    HistActor->GetProperty()->SetLineWidth(line_width+1);
  }
  else if (HisctogramRepresentation == POINT_REPRESENTATION) 
  {
    Glyph->SetSource(PointsRepresentation);
    HistActor->GetProperty()->SetPointSize(1);
  }
  else
  {
    Glyph->SetSource(LineRepresentation->GetOutput());
  }

  Glyph->Modified();
  Glyph->Update();
}
//----------------------------------------------------------------------------
long int vtkHistogram::GetHistogramValue(int x, int y)
//----------------------------------------------------------------------------
{
  long int hisctogramValue = 0;
  int idx = (x /(RenderWidth *1.0)) * NumberOfBins;
  hisctogramValue = Accumulate->GetOutput()->GetPointData()->GetScalars()->GetTuple1(idx);
  return hisctogramValue;
}
//----------------------------------------------------------------------------
void vtkHistogram::SetLabel(const char *lab)
//----------------------------------------------------------------------------
{
  TextMapper->SetInput(lab);
  Modified();
}
