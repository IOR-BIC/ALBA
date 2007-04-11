/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoMMLParameterView.cpp,v $
  Language:  C++
  Date:      $Date: 2007-04-11 11:53:06 $
  Version:   $Revision: 1.1 $
  Authors:   Mel Krokos
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#include "mafDefines.h"

//#include "MuscleRegistrationProject.h"
#include "mmoMMLParameterView.h"
#include "assert.h"
#include "vtkProperty2D.h"

//----------------------------------------------------------------------------
mmoMMLParameterView::mmoMMLParameterView(vtkRenderWindow *rw, vtkRenderer *ren)
//----------------------------------------------------------------------------
{
	m_pRenderer = ren;
	m_pRenderer->SetBackground(0.2,0.4,0.6);
  
	m_pRenderWindow = rw;
	m_pRenderWindow->PointSmoothingOn();
	m_pRenderWindow->LineSmoothingOn();
	m_pRenderWindow->AddRenderer(m_pRenderer);

	int *size = m_pRenderWindow->GetSize();
	m_Width = size[0];
	m_Height= size[1];

	assert(m_Width > 0);
	assert(m_Height > 0);

	// data points
	m_pPiecewiseFunction = vtkPiecewiseFunction::New();
	m_pDataPointsPolyData = vtkPolyData::New();
	m_pDataPointsPolyDataPoints = vtkPoints::New();
	m_pDataPointsPolyData->SetPoints(m_pDataPointsPolyDataPoints);
	m_pDataPointsGlyphSource2D = vtkGlyphSource2D::New();
	m_pDataPointsGlyph2D = vtkGlyph2D::New();
	m_pDataPointsGlyph2D->SetInput(m_pDataPointsPolyData);
	m_pDataPointsGlyph2D->SetSource(m_pDataPointsGlyphSource2D->GetOutput());
	m_pDataPointsGlyphSource2D->SetGlyphTypeToSquare();
	m_pDataPointsGlyphSource2D->SetScale(8);
	m_pDataPointsGlyphSource2D->FilledOn();
	m_pDataPointsPolyDataMapper2D = vtkPolyDataMapper2D::New();
	m_pDataPointsPolyDataMapper2D->SetInput(m_pDataPointsGlyph2D->GetOutput());
	m_pDataPointsActor2D = vtkActor2D::New();
	m_pDataPointsActor2D->SetMapper(m_pDataPointsPolyDataMapper2D);
	m_pRenderer->AddActor(m_pDataPointsActor2D);
	m_pDataPointsActor2D->VisibilityOn();
	
	// spline
	m_pKochanekSpline = vtkKochanekSpline::New();
	m_pKochanekSpline->ClosedOn();
	m_nNumberOfKochanekSplinePoints = 100;
	m_pSplinePolyData = vtkPolyData::New();
	m_pSplinePolyDataPoints = vtkPoints::New();
	m_pSplinePolyDataLines = vtkCellArray::New();
	m_pSplinePolyData->SetPoints(m_pSplinePolyDataPoints);
	m_pSplinePolyData->SetLines(m_pSplinePolyDataLines);
	m_pSplinePolyDataMapper2D = vtkPolyDataMapper2D::New();
	m_pSplinePolyDataMapper2D->SetInput(m_pSplinePolyData);
	m_pSplineActor2D = vtkActor2D::New();
	m_pSplineActor2D->SetMapper(m_pSplinePolyDataMapper2D);
	m_pRenderer->AddActor(m_pSplineActor2D);

	// vertical line
	m_pLineSource = vtkLineSource::New();
	m_pLinePolyDataMapper2D = vtkPolyDataMapper2D::New();
	m_pLinePolyDataMapper2D->SetInput(m_pLineSource->GetOutput());
	m_pLineActor2D = vtkActor2D::New();
	m_pLineActor2D->SetMapper(m_pLinePolyDataMapper2D);
	m_pRenderer->AddActor(m_pLineActor2D);

	// horizontal line
	m_pHorizontalLineSource = vtkLineSource::New();
	m_pHorizontalLinePolyDataMapper2D = vtkPolyDataMapper2D::New();
	m_pHorizontalLinePolyDataMapper2D->SetInput(m_pHorizontalLineSource->GetOutput());
	m_pHorizontalLineActor2D = vtkActor2D::New();
	m_pHorizontalLineActor2D->SetMapper(m_pHorizontalLinePolyDataMapper2D);
	m_pRenderer->AddActor(m_pHorizontalLineActor2D);
	
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
mmoMMLParameterView::~mmoMMLParameterView()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mmoMMLParameterView::ComputeConvertTommoMMLParameterViewX()
//----------------------------------------------------------------------------
{
	double dRange;

	dRange = m_dXMax - m_dXMin;
	assert(dRange > 0);
 	m_dXPosConvertRatio = m_Width / dRange;
}
//----------------------------------------------------------------------------
double mmoMMLParameterView::ConvertTommoMMLParameterViewX(double X)
//----------------------------------------------------------------------------
{
	//assert(X >= m_dXMin);
	//assert(X <= m_dXMax);
	return m_dXPosConvertRatio * X;
}
//----------------------------------------------------------------------------
void mmoMMLParameterView::ComputeConvertTommoMMLParameterViewY()
//----------------------------------------------------------------------------
{
	double dRange1, dRange2;

	dRange1 = m_dYMax - m_dYMid;
	assert(dRange1 > 0);
	dRange2 = m_dYMid - m_dYMin;
	assert(dRange2 > 0);

	m_dYPosConvertTrans1 = -1.0 * m_dYMid;
 	m_dYPosConvertRatio1 = (0.5 * ((double) (m_Height))) / dRange1;
	
	m_dYPosConvertTrans2 = -1.0 * m_dYMin;
 	m_dYPosConvertRatio2 = (0.5 * ((double) (m_Height))) / dRange2;
}
//----------------------------------------------------------------------------
double mmoMMLParameterView::ConvertTommoMMLParameterViewY(double Y)
//----------------------------------------------------------------------------
{
	double dMiddle;

	//assert(Y >= m_dYMin);
	//assert(Y <= m_dYMax);

 	dMiddle = 0.5 * ((double) (m_Height));

	if (Y > m_dYMid)
	{
		return ((Y + m_dYPosConvertTrans1) * m_dYPosConvertRatio1 + dMiddle);
	}
	else
	if (Y < m_dYMid)
	{
		return ((Y + m_dYPosConvertTrans2) * m_dYPosConvertRatio2);
	}
	else
		return dMiddle;
}
//----------------------------------------------------------------------------
double mmoMMLParameterView::ConvertFrommmoMMLParameterViewX(double X)
//----------------------------------------------------------------------------
{
	return X / m_dXPosConvertRatio;
}
//----------------------------------------------------------------------------
double mmoMMLParameterView::ConverFrommmoMMLParameterViewY(double Y)
//----------------------------------------------------------------------------
{
	double dMiddle;

 	dMiddle = 0.5 * ((double) (m_Height));

	if (Y > dMiddle)
	{
		return ((Y - dMiddle) / m_dYPosConvertRatio1 - m_dYPosConvertTrans1);
	}
	else
	if (Y < dMiddle)
	{
		return (Y / m_dYPosConvertRatio2 - m_dYPosConvertTrans2);
	}
	else
		return m_dYMid;
}
//----------------------------------------------------------------------------
void mmoMMLParameterView::ComputeSpline()
//----------------------------------------------------------------------------
{
	int i;
	double *pflDataPoints;
	int nNumberOfDataPoints;
	
	// how many data points?
	nNumberOfDataPoints = m_pPiecewiseFunction->GetSize();
	
	// spline needs at least two data points
	assert(nNumberOfDataPoints > 1);

	// pointer to data points
	pflDataPoints = m_pPiecewiseFunction->GetDataPointer();  
    
	// display
	m_pDataPointsPolyDataPoints->Reset();
	m_pDataPointsPolyDataPoints->Modified();

	for (i = 0; i < nNumberOfDataPoints; i++)
    {
		m_pDataPointsPolyDataPoints->InsertPoint(i,
			                                     *(pflDataPoints + 2 * i),
												 *(pflDataPoints + 2 * i + 1),
												 0.0);   
    }

	// spline interpolation points
	for (i = 0; i < nNumberOfDataPoints; i++)
    {
		m_pKochanekSpline->AddPoint(*(pflDataPoints + 2 * i), *(pflDataPoints + 2 * i + 1));   
    }

	double PosConvertRatio, t;
 	PosConvertRatio = ((double) (m_Width)) / 
 		              ((double) (m_nNumberOfKochanekSplinePoints));

	// spline display points
	for(i=0; i<m_nNumberOfKochanekSplinePoints; i++)
	{
      t = PosConvertRatio * i;
      m_pSplinePolyDataPoints->InsertPoint(i, t, m_pKochanekSpline->Evaluate(t), 0.0); 
	}
	
	// spline display lines
	m_pSplinePolyDataLines->InsertNextCell(m_nNumberOfKochanekSplinePoints);
	for(i=0; i< m_nNumberOfKochanekSplinePoints; i++)
	{
		m_pSplinePolyDataLines->InsertCellPoint(i);
	}
}
//----------------------------------------------------------------------------
void mmoMMLParameterView::Render()
//----------------------------------------------------------------------------
{
	ComputeSpline();
	m_pRenderWindow->Render();
}
//----------------------------------------------------------------------------
void mmoMMLParameterView::ResetSpline()
//----------------------------------------------------------------------------
{
	int i;
	double *pflDataPoints;
	int nNumberOfDataPoints;

	// how many data points?
	nNumberOfDataPoints = m_pPiecewiseFunction->GetSize();

	if (nNumberOfDataPoints > 0)
	{
		// pointer to data points
		pflDataPoints = m_pPiecewiseFunction->GetDataPointer ();

		for(i = 0; i < nNumberOfDataPoints; i++)
		{
			m_pKochanekSpline->RemovePoint(*(pflDataPoints + 2 * i));
		}
	}
}
//----------------------------------------------------------------------------
void mmoMMLParameterView::SetMinX(double x)
//----------------------------------------------------------------------------
{
	m_dXMin = x;
}
//----------------------------------------------------------------------------
void mmoMMLParameterView::SetMaxX(double x)
//----------------------------------------------------------------------------
{
	m_dXMax = x;
}
//----------------------------------------------------------------------------
void mmoMMLParameterView::SetMinY(double y)
//----------------------------------------------------------------------------
{
	m_dYMin = y;
}
//----------------------------------------------------------------------------
void mmoMMLParameterView::SetMaxY(double y)
//----------------------------------------------------------------------------
{
	m_dYMax = y;
}
//----------------------------------------------------------------------------
void mmoMMLParameterView::SetMidY(double y)
//----------------------------------------------------------------------------
{
	m_dYMid = y;
}
//----------------------------------------------------------------------------
double mmoMMLParameterView::GetMinY()
//----------------------------------------------------------------------------
{
	return m_dYMin;
}
//----------------------------------------------------------------------------
double mmoMMLParameterView::GetMaxY()
//----------------------------------------------------------------------------
{
	return m_dYMax;
}
//----------------------------------------------------------------------------
vtkActor2D* mmoMMLParameterView::GetLineActor()
//----------------------------------------------------------------------------
{
	return m_pLineActor2D;
}
//----------------------------------------------------------------------------
vtkActor2D* mmoMMLParameterView::GetSplineActor()
//----------------------------------------------------------------------------
{
	return m_pSplineActor2D;
}
//----------------------------------------------------------------------------
void mmoMMLParameterView::SetSplineSamples(int s)
//----------------------------------------------------------------------------
{
	m_nNumberOfKochanekSplinePoints = s;
}
//----------------------------------------------------------------------------
void mmoMMLParameterView::SetRangeX(int r)
//----------------------------------------------------------------------------
{
	SetMinX(0);
	SetMaxX(r - 1);
	ComputeConvertTommoMMLParameterViewX();
}
//----------------------------------------------------------------------------
void mmoMMLParameterView::InitialiseLineActor()
//----------------------------------------------------------------------------
{
	m_pLineSource->SetPoint1(0, 0, 0);
	m_pLineSource->SetPoint2(0, ConvertTommoMMLParameterViewY(m_dYMax), 0);
}
//----------------------------------------------------------------------------
void mmoMMLParameterView::AddPoint(double x, double y)
//----------------------------------------------------------------------------
{
	ResetSpline();
	m_pPiecewiseFunction->AddPoint(ConvertTommoMMLParameterViewX(x),ConvertTommoMMLParameterViewY(y));
   /* m_pDataPointsPolyDataPoints->Modified();
	m_pPiecewiseFunction->Update();
	m_pRenderWindow->Modified();*/

}

//----------------------------------------------------------------------------
float mmoMMLParameterView::GetValue(double x)
//----------------------------------------------------------------------------
{
	return m_pPiecewiseFunction->GetValue(x);
}

//----------------------------------------------------------------------------
void mmoMMLParameterView::RemovePoint(double x)
//----------------------------------------------------------------------------
{
	ResetSpline();
	m_pPiecewiseFunction->RemovePoint(ConvertTommoMMLParameterViewX(x));
}
//----------------------------------------------------------------------------
void mmoMMLParameterView::SetRangeY(double left, double middle, double right)
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
vtkActor2D* mmoMMLParameterView::GetPointsActor()
//----------------------------------------------------------------------------
{
	return m_pDataPointsActor2D;
}
//----------------------------------------------------------------------------
void mmoMMLParameterView::SetLineActorX(int x)
//----------------------------------------------------------------------------
{
	double x1, y1;
	double x2, y2;

	x1 = ConvertTommoMMLParameterViewX(x);
	y1 = ConvertTommoMMLParameterViewY(GetMinY());
	m_pLineSource->SetPoint1(x1,
			                 y1,
							 0);

	x2 = ConvertTommoMMLParameterViewX(x);
	y2 = ConvertTommoMMLParameterViewY(GetMaxY());
    m_pLineSource->SetPoint2(x2,
			                 y2,
							 0);
	//Render();
}
//----------------------------------------------------------------------------
vtkActor2D* mmoMMLParameterView::GetHorizontalLineActor()
//----------------------------------------------------------------------------
{
	return m_pHorizontalLineActor2D;
}
//----------------------------------------------------------------------------
void mmoMMLParameterView::InitialiseHorizontalLineActor()
//----------------------------------------------------------------------------
{
	m_pHorizontalLineSource->SetPoint1(0, ConvertTommoMMLParameterViewY(m_dYMid), 0);
	m_pHorizontalLineSource->SetPoint2(ConvertTommoMMLParameterViewX(m_dXMax), ConvertTommoMMLParameterViewY(m_dYMid), 0);
}
//----------------------------------------------------------------------------
vtkRenderer* mmoMMLParameterView::GetRenderer()
//----------------------------------------------------------------------------
{
	return m_pRenderer;
}

//----------------------------------------------------------------------------
int mmoMMLParameterView::GetNumberOfDataPoints()
//----------------------------------------------------------------------------
{
	return m_pDataPointsPolyDataPoints->GetNumberOfPoints();
}
