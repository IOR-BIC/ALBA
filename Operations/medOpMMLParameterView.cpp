/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medOpMMLParameterView.cpp,v $
  Language:  C++
  Date:      $Date: 2008-07-24 07:55:01 $
  Version:   $Revision: 1.2 $
  Authors:   Mel Krokos
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#include "mafDefines.h"

//#include "MuscleRegistrationProject.h"
#include "medOpMMLParameterView.h"
#include "assert.h"
#include "vtkProperty2D.h"

//----------------------------------------------------------------------------
medOpMMLParameterView::medOpMMLParameterView(vtkRenderWindow *rw, vtkRenderer *ren)
//----------------------------------------------------------------------------
{
	m_PRenderer = ren;
	m_PRenderer->SetBackground(0.2,0.4,0.6);
  
	m_PRenderWindow = rw;
	m_PRenderWindow->PointSmoothingOn();
	m_PRenderWindow->LineSmoothingOn();
	m_PRenderWindow->AddRenderer(m_PRenderer);

	int *size = m_PRenderWindow->GetSize();
	m_Width = size[0];
	m_Height= size[1];

	assert(m_Width > 0);
	assert(m_Height > 0);

	// data points
	m_PPiecewiseFunction = vtkPiecewiseFunction::New();
	m_PDataPointsPolyData = vtkPolyData::New();
	m_PDataPointsPolyDataPoints = vtkPoints::New();
	m_PDataPointsPolyData->SetPoints(m_PDataPointsPolyDataPoints);
	m_PDataPointsGlyphSource2D = vtkGlyphSource2D::New();
	m_PDataPointsGlyph2D = vtkGlyph2D::New();
	m_PDataPointsGlyph2D->SetInput(m_PDataPointsPolyData);
	m_PDataPointsGlyph2D->SetSource(m_PDataPointsGlyphSource2D->GetOutput());
	m_PDataPointsGlyphSource2D->SetGlyphTypeToSquare();
	m_PDataPointsGlyphSource2D->SetScale(8);
	m_PDataPointsGlyphSource2D->FilledOn();
	m_PDataPointsPolyDataMapper2D = vtkPolyDataMapper2D::New();
	m_PDataPointsPolyDataMapper2D->SetInput(m_PDataPointsGlyph2D->GetOutput());
	m_PDataPointsActor2D = vtkActor2D::New();
	m_PDataPointsActor2D->SetMapper(m_PDataPointsPolyDataMapper2D);
	m_PRenderer->AddActor(m_PDataPointsActor2D);
	m_PDataPointsActor2D->VisibilityOn();
	
	// spline
	m_PKochanekSpline = vtkKochanekSpline::New();
	m_PKochanekSpline->ClosedOn();
	m_NNumberOfKochanekSplinePoints = 100;
	m_PSplinePolyData = vtkPolyData::New();
	m_PSplinePolyDataPoints = vtkPoints::New();
	m_PSplinePolyDataLines = vtkCellArray::New();
	m_PSplinePolyData->SetPoints(m_PSplinePolyDataPoints);
	m_PSplinePolyData->SetLines(m_PSplinePolyDataLines);
	m_PSplinePolyDataMapper2D = vtkPolyDataMapper2D::New();
	m_PSplinePolyDataMapper2D->SetInput(m_PSplinePolyData);
	m_PSplineActor2D = vtkActor2D::New();
	m_PSplineActor2D->SetMapper(m_PSplinePolyDataMapper2D);
	m_PRenderer->AddActor(m_PSplineActor2D);

	// vertical line
	m_PLineSource = vtkLineSource::New();
	m_PLinePolyDataMapper2D = vtkPolyDataMapper2D::New();
	m_PLinePolyDataMapper2D->SetInput(m_PLineSource->GetOutput());
	m_PLineActor2D = vtkActor2D::New();
	m_PLineActor2D->SetMapper(m_PLinePolyDataMapper2D);
	m_PRenderer->AddActor(m_PLineActor2D);

	// horizontal line
	m_PHorizontalLineSource = vtkLineSource::New();
	m_PHorizontalLinePolyDataMapper2D = vtkPolyDataMapper2D::New();
	m_PHorizontalLinePolyDataMapper2D->SetInput(m_PHorizontalLineSource->GetOutput());
	m_PHorizontalLineActor2D = vtkActor2D::New();
	m_PHorizontalLineActor2D->SetMapper(m_PHorizontalLinePolyDataMapper2D);
	m_PRenderer->AddActor(m_PHorizontalLineActor2D);
	
  GetLineActor()->GetProperty()->SetLineWidth(1.0);
	GetLineActor()->GetProperty()->SetLineStipplePattern(0x0FFF);
	GetLineActor()->GetProperty()->SetColor(1.0, 1.0, 1.0); 
	GetHorizontalLineActor()->GetProperty()->SetLineWidth(1.0);
	GetHorizontalLineActor()->GetProperty()->SetLineStipplePattern(0xAAAA);
	GetHorizontalLineActor()->GetProperty()->SetColor(1.0, 1.0, 1.0); 
	GetSplineActor()->GetProperty()->SetColor(1.0, 1.0, 1.0);
	GetSplineActor()->GetProperty()->SetLineWidth(1.5);
	GetPointsActor()->GetProperty()->SetColor(1.0, 1.0, 1.0); 
	GetRenderer()->SetBackground(0.2, 0.4, 0.6); 


	
}
//----------------------------------------------------------------------------
medOpMMLParameterView::~medOpMMLParameterView()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void medOpMMLParameterView::ComputeConvertTommoMMLParameterViewX()
//----------------------------------------------------------------------------
{
	double dRange;

	dRange = m_DXMax - m_DXMin;
	assert(dRange > 0);
 	m_DXPosConvertRatio = m_Width / dRange;
}
//----------------------------------------------------------------------------
double medOpMMLParameterView::ConvertTommoMMLParameterViewX(double X)
//----------------------------------------------------------------------------
{
	//assert(X >= m_dXMin);
	//assert(X <= m_dXMax);
	return m_DXPosConvertRatio * X;
}
//----------------------------------------------------------------------------
void medOpMMLParameterView::ComputeConvertTommoMMLParameterViewY()
//----------------------------------------------------------------------------
{
	double dRange1, dRange2;

	dRange1 = m_DYMax - m_DYMid;
	assert(dRange1 > 0);
	dRange2 = m_DYMid - m_DYMin;
	assert(dRange2 > 0);

	m_DYPosConvertTrans1 = -1.0 * m_DYMid;
 	m_DYPosConvertRatio1 = (0.5 * ((double) (m_Height))) / dRange1;
	
	m_DYPosConvertTrans2 = -1.0 * m_DYMin;
 	m_DYPosConvertRatio2 = (0.5 * ((double) (m_Height))) / dRange2;
}
//----------------------------------------------------------------------------
double medOpMMLParameterView::ConvertTommoMMLParameterViewY(double Y)
//----------------------------------------------------------------------------
{
	double dMiddle;

	//assert(Y >= m_dYMin);
	//assert(Y <= m_dYMax);

 	dMiddle = 0.5 * ((double) (m_Height));

	if (Y > m_DYMid)
	{
		return ((Y + m_DYPosConvertTrans1) * m_DYPosConvertRatio1 + dMiddle);
	}
	else
	if (Y < m_DYMid)
	{
		return ((Y + m_DYPosConvertTrans2) * m_DYPosConvertRatio2);
	}
	else
		return dMiddle;
}
//----------------------------------------------------------------------------
double medOpMMLParameterView::ConvertFrommmoMMLParameterViewX(double X)
//----------------------------------------------------------------------------
{
	return X / m_DXPosConvertRatio;
}
//----------------------------------------------------------------------------
double medOpMMLParameterView::ConverFrommmoMMLParameterViewY(double Y)
//----------------------------------------------------------------------------
{
	double dMiddle;

 	dMiddle = 0.5 * ((double) (m_Height));

	if (Y > dMiddle)
	{
		return ((Y - dMiddle) / m_DYPosConvertRatio1 - m_DYPosConvertTrans1);
	}
	else
	if (Y < dMiddle)
	{
		return (Y / m_DYPosConvertRatio2 - m_DYPosConvertTrans2);
	}
	else
		return m_DYMid;
}
//----------------------------------------------------------------------------
void medOpMMLParameterView::ComputeSpline()
//----------------------------------------------------------------------------
{
	int i;
	double *pflDataPoints;
	int nNumberOfDataPoints;
	
	// how many data points?
	nNumberOfDataPoints = m_PPiecewiseFunction->GetSize();
	
	// spline needs at least two data points
	assert(nNumberOfDataPoints > 1);

	// pointer to data points
	pflDataPoints = m_PPiecewiseFunction->GetDataPointer();  
    
	// display
	m_PDataPointsPolyDataPoints->Reset();
	m_PDataPointsPolyDataPoints->Modified();

	for (i = 0; i < nNumberOfDataPoints; i++)
    {
		m_PDataPointsPolyDataPoints->InsertPoint(i,
			                                     *(pflDataPoints + 2 * i),
												 *(pflDataPoints + 2 * i + 1),
												 0.0);   
    }

	// spline interpolation points
	for (i = 0; i < nNumberOfDataPoints; i++)
    {
		m_PKochanekSpline->AddPoint(*(pflDataPoints + 2 * i), *(pflDataPoints + 2 * i + 1));   
    }

	double PosConvertRatio, t;
 	PosConvertRatio = ((double) (m_Width)) / 
 		              ((double) (m_NNumberOfKochanekSplinePoints));

	// spline display points
	for(i=0; i<m_NNumberOfKochanekSplinePoints; i++)
	{
      t = PosConvertRatio * i;
      m_PSplinePolyDataPoints->InsertPoint(i, t, m_PKochanekSpline->Evaluate(t), 0.0); 
	}
	
	// spline display lines
	m_PSplinePolyDataLines->InsertNextCell(m_NNumberOfKochanekSplinePoints);
	for(i=0; i< m_NNumberOfKochanekSplinePoints; i++)
	{
		m_PSplinePolyDataLines->InsertCellPoint(i);
	}
}
//----------------------------------------------------------------------------
void medOpMMLParameterView::Render()
//----------------------------------------------------------------------------
{
	ComputeSpline();
	m_PRenderWindow->Render();
}
//----------------------------------------------------------------------------
void medOpMMLParameterView::ResetSpline()
//----------------------------------------------------------------------------
{
	int i;
	double *pflDataPoints;
	int nNumberOfDataPoints;

	// how many data points?
	nNumberOfDataPoints = m_PPiecewiseFunction->GetSize();

	if (nNumberOfDataPoints > 0)
	{
		// pointer to data points
		pflDataPoints = m_PPiecewiseFunction->GetDataPointer ();

		for(i = 0; i < nNumberOfDataPoints; i++)
		{
			m_PKochanekSpline->RemovePoint(*(pflDataPoints + 2 * i));
		}
	}
}
//----------------------------------------------------------------------------
void medOpMMLParameterView::SetMinX(double x)
//----------------------------------------------------------------------------
{
	m_DXMin = x;
}
//----------------------------------------------------------------------------
void medOpMMLParameterView::SetMaxX(double x)
//----------------------------------------------------------------------------
{
	m_DXMax = x;
}
//----------------------------------------------------------------------------
void medOpMMLParameterView::SetMinY(double y)
//----------------------------------------------------------------------------
{
	m_DYMin = y;
}
//----------------------------------------------------------------------------
void medOpMMLParameterView::SetMaxY(double y)
//----------------------------------------------------------------------------
{
	m_DYMax = y;
}
//----------------------------------------------------------------------------
void medOpMMLParameterView::SetMidY(double y)
//----------------------------------------------------------------------------
{
	m_DYMid = y;
}
//----------------------------------------------------------------------------
double medOpMMLParameterView::GetMinY()
//----------------------------------------------------------------------------
{
	return m_DYMin;
}
//----------------------------------------------------------------------------
double medOpMMLParameterView::GetMaxY()
//----------------------------------------------------------------------------
{
	return m_DYMax;
}
//----------------------------------------------------------------------------
vtkActor2D* medOpMMLParameterView::GetLineActor()
//----------------------------------------------------------------------------
{
	return m_PLineActor2D;
}
//----------------------------------------------------------------------------
vtkActor2D* medOpMMLParameterView::GetSplineActor()
//----------------------------------------------------------------------------
{
	return m_PSplineActor2D;
}
//----------------------------------------------------------------------------
void medOpMMLParameterView::SetSplineSamples(int s)
//----------------------------------------------------------------------------
{
	m_NNumberOfKochanekSplinePoints = s;
}
//----------------------------------------------------------------------------
void medOpMMLParameterView::SetRangeX(int r)
//----------------------------------------------------------------------------
{
	SetMinX(0);
	SetMaxX(r - 1);
	ComputeConvertTommoMMLParameterViewX();
}
//----------------------------------------------------------------------------
void medOpMMLParameterView::InitialiseLineActor()
//----------------------------------------------------------------------------
{
	m_PLineSource->SetPoint1(0, 0, 0);
	m_PLineSource->SetPoint2(0, ConvertTommoMMLParameterViewY(m_DYMax), 0);
}
//----------------------------------------------------------------------------
void medOpMMLParameterView::AddPoint(double x, double y)
//----------------------------------------------------------------------------
{
	ResetSpline();
	m_PPiecewiseFunction->AddPoint(ConvertTommoMMLParameterViewX(x),ConvertTommoMMLParameterViewY(y));
   /* m_pDataPointsPolyDataPoints->Modified();
	m_pPiecewiseFunction->Update();
	m_pRenderWindow->Modified();*/

}

//----------------------------------------------------------------------------
float medOpMMLParameterView::GetValue(double x)
//----------------------------------------------------------------------------
{
	return m_PPiecewiseFunction->GetValue(x);
}

//----------------------------------------------------------------------------
void medOpMMLParameterView::RemovePoint(double x)
//----------------------------------------------------------------------------
{
	ResetSpline();
	m_PPiecewiseFunction->RemovePoint(ConvertTommoMMLParameterViewX(x));
}
//----------------------------------------------------------------------------
void medOpMMLParameterView::SetRangeY(double left, double middle, double right)
//----------------------------------------------------------------------------
{
	SetMinY(left);
	SetMaxY(right);
	SetMidY(middle);
	ComputeConvertTommoMMLParameterViewY();
	InitialiseLineActor();
	InitialiseHorizontalLineActor();
}
//----------------------------------------------------------------------------
vtkActor2D* medOpMMLParameterView::GetPointsActor()
//----------------------------------------------------------------------------
{
	return m_PDataPointsActor2D;
}
//----------------------------------------------------------------------------
void medOpMMLParameterView::SetLineActorX(int x)
//----------------------------------------------------------------------------
{
	double x1, y1;
	double x2, y2;

	x1 = ConvertTommoMMLParameterViewX(x);
	y1 = ConvertTommoMMLParameterViewY(GetMinY());
	m_PLineSource->SetPoint1(x1,
			                 y1,
							 0);

	x2 = ConvertTommoMMLParameterViewX(x);
	y2 = ConvertTommoMMLParameterViewY(GetMaxY());
    m_PLineSource->SetPoint2(x2,
			                 y2,
							 0);
	//Render();
}
//----------------------------------------------------------------------------
vtkActor2D* medOpMMLParameterView::GetHorizontalLineActor()
//----------------------------------------------------------------------------
{
	return m_PHorizontalLineActor2D;
}
//----------------------------------------------------------------------------
void medOpMMLParameterView::InitialiseHorizontalLineActor()
//----------------------------------------------------------------------------
{
	m_PHorizontalLineSource->SetPoint1(0, ConvertTommoMMLParameterViewY(m_DYMid), 0);
	m_PHorizontalLineSource->SetPoint2(ConvertTommoMMLParameterViewX(m_DXMax), ConvertTommoMMLParameterViewY(m_DYMid), 0);
}
//----------------------------------------------------------------------------
vtkRenderer* medOpMMLParameterView::GetRenderer()
//----------------------------------------------------------------------------
{
	return m_PRenderer;
}

//----------------------------------------------------------------------------
int medOpMMLParameterView::GetNumberOfDataPoints()
//----------------------------------------------------------------------------
{
	return m_PDataPointsPolyDataPoints->GetNumberOfPoints();
}
