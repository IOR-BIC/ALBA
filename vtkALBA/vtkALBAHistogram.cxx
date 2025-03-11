/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAHistogram
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "albaDefines.h"
#include "vtkALBAHistogram.h"

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
#include "vtkActor.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"

vtkStandardNewMacro(vtkALBAHistogram);
//------------------------------------------------------------------------------
vtkALBAHistogram::vtkALBAHistogram()
//------------------------------------------------------------------------------
{
  TextMapper  = NULL;
  TextActor   = NULL;
  HistActor   = NULL;
  Line1Actor  = NULL;
  Line2Actor  = NULL;

  Line1       = NULL;
  Line2       = NULL;
  Accumulate  = NULL;
  ChangeInfo  = NULL;
  LogScale    = NULL;
  Glyph       = NULL;
  InputData   = NULL;
  ImageData   = NULL;
  
  PointsRepresentation  = NULL;
  LineRepresentation    = NULL;

  HisctogramRepresentation = BAR_REPRESENTATION;

  Color[0] = Color[1] = Color[2] = 1.0;

  ScaleFactor       = 1.0;
  NumberOfBins      = 100;
  LogScaleConstant  = 10.0;
  RenderWidth       = 1;

  AutoscaleHistogram  = 1;
  LogHistogram        = 0;
  LabelVisibility     = 1;

  AutoscaleCalculated = false;
  ShowLines = false;

  HistogramCreate();
}
//------------------------------------------------------------------------------
vtkALBAHistogram::~vtkALBAHistogram()
//------------------------------------------------------------------------------
{
  if(TextMapper)  TextMapper->Delete();
  if(TextActor)   TextActor->Delete();
  if(Line1Actor)  Line1Actor->Delete();
  if(Line2Actor)  Line2Actor->Delete();
  if(Line1)       Line1->Delete();
  if(Line2)       Line2->Delete();
  if(ImageData)   ImageData->Delete();
  if(Accumulate)  Accumulate->Delete();
  if(ChangeInfo)  ChangeInfo->Delete();
  if(LogScale)    LogScale->Delete();
  if(Glyph)       Glyph->Delete();
  if(HistActor)   HistActor->Delete();
  if(PointsRepresentation) PointsRepresentation->Delete();
  if(LineRepresentation)   LineRepresentation->Delete();
  if (InputData) InputData->UnRegister(this);
}
//------------------------------------------------------------------------------
void vtkALBAHistogram::SetInputData(vtkDataArray* inputData)
//------------------------------------------------------------------------------
{
  if (InputData != inputData)
  {
    if (this->InputData != NULL) { this->InputData->UnRegister(this); }
    this->InputData = inputData;
    if (this->InputData != NULL) { this->InputData->Register(this); }
    this->Modified();

    double sr[2];
		InputData->GetRange(sr);
		UpdateLines(sr);
		double srw = sr[1] - sr[0] + 1;
		NumberOfBins = MIN(srw, 100);

    AutoscaleCalculated = false;
  }
}
//------------------------------------------------------------------------------
void vtkALBAHistogram::SetAutoscaleHistogram(int autoscale)
//------------------------------------------------------------------------------
{
  if (autoscale != 0 && AutoscaleHistogram == 0)
  {
    AutoscaleCalculated = false;
  }
  if (AutoscaleHistogram != autoscale)
  {
    AutoscaleHistogram = autoscale;
    Modified();
  }
}
//------------------------------------------------------------------------------
void vtkALBAHistogram::PrintSelf(ostream& os, vtkIndent indent)
//------------------------------------------------------------------------------
{
  this->Superclass::PrintSelf(os,indent);
}
//------------------------------------------------------------------------------
int vtkALBAHistogram::RenderOverlay(vtkViewport *viewport)
//------------------------------------------------------------------------------
{
  if (InputData == NULL) {return 0;};

  vtkRenderer *ren = static_cast<vtkRenderer *>(viewport);
  HistogramUpdate(ren);
  this->Modified();

  HistActor->RenderOverlay(viewport);
  if (LabelVisibility) TextActor->RenderOverlay(viewport);

  if (ShowLines == true)
  {
	  Line1Actor->RenderOverlay(viewport);
	  Line2Actor->RenderOverlay(viewport);
  }
  return 1;
}
//----------------------------------------------------------------------------
int vtkALBAHistogram::RenderOpaqueGeometry(vtkViewport *viewport)
//----------------------------------------------------------------------------
{
  if (InputData == NULL) {return 0;};

  if (LabelVisibility) TextActor->RenderOpaqueGeometry(viewport);
  return 0;
}
//----------------------------------------------------------------------------
void vtkALBAHistogram::HistogramCreate()
//----------------------------------------------------------------------------
{
  TextMapper = vtkTextMapper::New();
  TextMapper->SetInput("");
	vtkTextProperty * textProperty = TextMapper->GetTextProperty();
  textProperty->SetFontFamily(VTK_TIMES);
  textProperty->SetColor(1,1,1);
  textProperty->SetLineOffset(0.5);
  textProperty->SetLineSpacing(1.5);
  textProperty->SetJustificationToRight();
  textProperty->SetVerticalJustificationToTop();
	textProperty->SetFontSize(14);

  TextActor = vtkActor2D::New();
  TextActor->SetMapper(TextMapper);
  TextActor->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
  TextActor->SetPosition(1, 1);

  ImageData  = vtkImageData::New();
  ImageData->Initialize();
  ImageData->SetSpacing(1.0,0.0,0.0);

  Accumulate = vtkImageAccumulate::New();

  LogScale = vtkImageLogarithmicScale::New();
  LogScale->SetConstant(LogScaleConstant);

  ChangeInfo = vtkImageChangeInformation::New();
  ChangeInfo->SetOutputOrigin(0,0,0);

	vtkIdType cp[2] = {0,1};
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
  Glyph->SetSourceData(LineRepresentation->GetOutput());
  Glyph->SetScaleModeToScaleByScalar();
  Glyph->OrientOn();
	
  vtkCoordinate *coordinate = vtkCoordinate::New();
  coordinate->SetCoordinateSystemToNormalizedDisplay();

  vtkPolyDataMapper2D *mapper2d = vtkPolyDataMapper2D::New();
  mapper2d->SetInputConnection(Glyph->GetOutputPort());
  mapper2d->SetTransformCoordinate(coordinate);
  mapper2d->ScalarVisibilityOff();
  
  SetMapper(mapper2d); // "this" Actor is unhappy without a mapper -- anyway -- this mapper is never used
  PickableOff();
  
  coordinate->Delete();
  mapper2d->Delete();

  HistActor = vtkActor2D::New();
  HistActor->SetMapper(mapper2d);

  Line1 = vtkLineSource::New();
  Line1->SetPoint1(0,0,0);
  Line1->SetPoint2(0,1,0);
  Line1->Update();

  vtkPolyDataMapper2D *mapperLine1 = vtkPolyDataMapper2D::New();
  mapperLine1->SetInputConnection(Line1->GetOutputPort());

  Line1Actor = vtkActor2D::New();
  Line1Actor->SetMapper(mapperLine1);
  Line1Actor->GetProperty()->SetColor(1,0,0);

  Line2 = vtkLineSource::New();
  Line2->SetPoint1(0,0,0);
  Line2->SetPoint2(0,1,0);
  Line2->Update();

  vtkPolyDataMapper2D *mapperLine2 = vtkPolyDataMapper2D::New();
  mapperLine2->SetInputConnection(Line2->GetOutputPort());

  Line2Actor = vtkActor2D::New();
  Line2Actor->SetMapper(mapperLine2);
  Line2Actor->GetProperty()->SetColor(1,0,0);

  mapperLine1->Delete();
  mapperLine2->Delete();

	if (InputData)
	{
		double sr[2];
		ImageData->GetScalarRange(sr);
		double srw = sr[1] - sr[0] + 1;
		NumberOfBins = MIN(srw, 100);
	}

}
//----------------------------------------------------------------------------
void vtkALBAHistogram::HistogramUpdate(vtkRenderer *ren)
//----------------------------------------------------------------------------
{
  if (InputData == NULL) {return;}

  double sr[2];
  ImageData->SetDimensions(InputData->GetNumberOfTuples(),1,1);
  ImageData->AllocateScalars(InputData->GetDataType(),1);
  ImageData->GetPointData()->SetScalars(InputData);
  ImageData->GetScalarRange(sr);
  double srw = sr[1]-sr[0]+1;
	
  Accumulate->SetInputData(ImageData);
  Accumulate->SetComponentOrigin(sr[0],0,0);  
  Accumulate->SetComponentExtent(0,NumberOfBins,0,0,0,0);
  Accumulate->SetComponentSpacing(srw/NumberOfBins,0,0); // bins maps all the Scalars Range
  Accumulate->Update();

  if (AutoscaleHistogram /*&& !AutoscaleCalculated recalculate always fix bug #2462 */)
  {
    AutoscaleCalculated = true;
    double m = VTK_DOUBLE_MIN;
    double as;
    vtkDataArray *arr = Accumulate->GetOutput()->GetPointData()->GetScalars();
		printf("\n");
    for (int s = 1; s < arr->GetNumberOfTuples(); s++)
    {
      arr->GetTuple(s,&as);
			printf("%.4f, ", as);
      m = as>m ? as : m;
    }
		printf("\n");

    double mean = m;
    if (mean < 0) mean = -mean;
    
    if (!LogHistogram)
    {
      ScaleFactor = 0.99 / mean;
    }
    else
    {
      if(mean > 0)
      {
        ScaleFactor = 0.99 / (LogScaleConstant * log(1 + mean));
      }
      else
      {
        ScaleFactor = 0.99 / (- LogScaleConstant * log(1 - mean));
      }
    }
  }

  ChangeInfo->SetInputConnection(Accumulate->GetOutputPort());
  ChangeInfo->SetOutputSpacing(1.0/NumberOfBins,1,1);  // Histogram width = 1
  ChangeInfo->Update();

  LogScale->SetConstant(LogScaleConstant);
  LogScale->SetInputConnection(ChangeInfo->GetOutputPort());
  LogScale->Update();

  if (LogHistogram)
  {
//     if (AutoscaleHistogram)
//     {
//       double mean = Accumulate->GetMean()[0];
//       if (mean < 0) mean = -mean;
//       ScaleFactor = .5 / log(1 + mean);
//     }
    Glyph->SetInputConnection(LogScale->GetOutputPort());
  }
  else
  {
    Glyph->SetInputConnection(ChangeInfo->GetOutputPort());
  }
  Glyph->SetScaleFactor(ScaleFactor);

  GetProperty()->SetColor(Color);
  HistActor->GetProperty()->SetColor(Color);
  //TextActor->GetProperty()->SetColor(Color);
  //TextMapper->GetTextProperty()->SetColor(Color);
  RenderWidth = ren->GetSize()[0];

  if (HisctogramRepresentation == BAR_REPRESENTATION) 
  {
    Glyph->SetSourceData(LineRepresentation->GetOutput());
    float line_width = RenderWidth / (float)(NumberOfBins - 1);
    HistActor->GetProperty()->SetLineWidth(line_width+1);
  }
  else if (HisctogramRepresentation == POINT_REPRESENTATION) 
  {
    Glyph->SetSourceData(PointsRepresentation);
    HistActor->GetProperty()->SetPointSize(1);
  }
  else
  {
    Glyph->SetSourceData(LineRepresentation->GetOutput());
    HistActor->GetProperty()->SetLineWidth(1);
  }

  Glyph->Modified();
  Glyph->Update();

  RenderH = ren->GetSize()[1];
  OriginX = ren->GetOrigin()[0];
  OriginY = ren->GetOrigin()[1];

  InputData->GetRange(sr);

	UpdateLines(CurrRange);
}
//----------------------------------------------------------------------------
void vtkALBAHistogram::UpdateLines(double range[2])
{
	UpdateLines(range[0], range[1]);
}
void vtkALBAHistogram::UpdateLines(double low, double hi)
{
  double sr[2],rangeSize,scaledRange[2];
	int line1X, line2X;

	CurrRange[0] = low;
	CurrRange[1] = hi;

  InputData->GetRange(sr);

	rangeSize = sr[1] - sr[0];
  
  scaledRange[0] = low-sr[0];
  scaledRange[1] = hi-sr[0];

  line1X = ((scaledRange[0]/rangeSize) * (RenderWidth-1))+1;
  line2X = ((scaledRange[1]/rangeSize) * (RenderWidth-1))+1;

  Line1->SetPoint1(line1X,OriginY,0);
  Line1->SetPoint2(line1X,OriginY+RenderH,0);
  Line1->Modified();
  Line1->Update();

  Line2->SetPoint1(line2X,OriginY,0);
  Line2->SetPoint2(line2X,OriginY+RenderH,0);
  Line2->Modified();
  Line2->Update();

}
//----------------------------------------------------------------------------
double vtkALBAHistogram::GetScalarValue(int x, int y)
//----------------------------------------------------------------------------
{
  int idx = (x /(RenderWidth *1.0)) * NumberOfBins;
  double sr[2];
  InputData->GetRange(sr);

  double value = (double)idx*(sr[1]-sr[0])/NumberOfBins;
  double shift = sr[0]>=0 ? 0 : -sr[0];
  value -= shift;

  return value;

}
//----------------------------------------------------------------------------
long int vtkALBAHistogram::GetHistogramValue(int x, int y)
//----------------------------------------------------------------------------
{
  long int hisctogramValue = 0;
  int idx = (x /(RenderWidth *1.0)) * NumberOfBins;
  if (idx>Accumulate->GetOutput()->GetPointData()->GetScalars()->GetNumberOfTuples())
  {
  	return 0;
  }
  hisctogramValue = Accumulate->GetOutput()->GetPointData()->GetScalars()->GetTuple1(idx);
  return hisctogramValue;
}
//----------------------------------------------------------------------------
void vtkALBAHistogram::SetLabel(const char *lab)
//----------------------------------------------------------------------------
{
  TextMapper->SetInput(lab);
  Modified();
}