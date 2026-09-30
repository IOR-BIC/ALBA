/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpMML3ParameterView
 Authors: Mel Krokos
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaDefines.h"

#include "albaOpMML3ParameterView.h"
#include "assert.h"
#include "vtkProperty2D.h"

//----------------------------------------------------------------------------
albaOpMML3ParameterView::albaOpMML3ParameterView(vtkRenderWindow *rw, vtkRenderer *ren)
//----------------------------------------------------------------------------
{
	m_Renderer = ren;
	m_Renderer->SetBackground(0.2,0.4,0.6);
  
	m_RenderWindow = rw;
	m_RenderWindow->PointSmoothingOn();
	m_RenderWindow->LineSmoothingOn();
	m_RenderWindow->AddRenderer(m_Renderer);

	int *size = m_RenderWindow->GetSize();
	m_Width = size[0];
	m_Height= size[1];

	assert(m_Width > 0);
	assert(m_Height > 0);

	// data points
	m_PiecewiseFunction = vtkPiecewiseFunction::New();
	m_DataPointsPolyData = vtkPolyData::New();
	m_DataPointsPolyDataPoints = vtkPoints::New();
	m_DataPointsPolyData->SetPoints(m_DataPointsPolyDataPoints);
	m_DataPointsGlyphSource2D = vtkGlyphSource2D::New();
	m_DataPointsGlyph2D = vtkGlyph2D::New();
	m_DataPointsGlyph2D->SetInputData(m_DataPointsPolyData);
	m_DataPointsGlyph2D->SetSourceConnection(m_DataPointsGlyphSource2D->GetOutputPort());
	m_DataPointsGlyphSource2D->SetGlyphTypeToSquare();
	m_DataPointsGlyphSource2D->SetScale(8);
	m_DataPointsGlyphSource2D->FilledOn();
	m_DataPointsPolyDataMapper2D = vtkPolyDataMapper2D::New();
	m_DataPointsPolyDataMapper2D->SetInputConnection(m_DataPointsGlyph2D->GetOutputPort());
	m_DataPointsActor2D = vtkActor2D::New();
	m_DataPointsActor2D->SetMapper(m_DataPointsPolyDataMapper2D);
	m_DataPointsActor2D->VisibilityOn();
  m_Renderer->AddActor(m_DataPointsActor2D);
	
	// spline
	m_KochanekSpline = vtkKochanekSpline::New();
	m_KochanekSpline->ClosedOn();
	m_NNumberOfKochanekSplinePoints = 100;
	m_SplinePolyData = vtkPolyData::New();
	m_SplinePolyDataPoints = vtkPoints::New();
	m_SplinePolyDataLines = vtkCellArray::New();
	m_SplinePolyData->SetPoints(m_SplinePolyDataPoints);
	m_SplinePolyData->SetLines(m_SplinePolyDataLines);
	m_SplinePolyDataMapper2D = vtkPolyDataMapper2D::New();
	m_SplinePolyDataMapper2D->SetInputData(m_SplinePolyData);
	m_SplineActor2D = vtkActor2D::New();
	m_SplineActor2D->SetMapper(m_SplinePolyDataMapper2D);
	m_Renderer->AddActor(m_SplineActor2D);

	// vertical line
	m_LineSource = vtkLineSource::New();
	m_LinePolyDataMapper2D = vtkPolyDataMapper2D::New();
	m_LinePolyDataMapper2D->SetInputConnection(m_LineSource->GetOutputPort());
	m_LineActor2D = vtkActor2D::New();
	m_LineActor2D->SetMapper(m_LinePolyDataMapper2D);
	m_Renderer->AddActor(m_LineActor2D);

	// horizontal line
	m_HorizontalLineSource = vtkLineSource::New();
	m_HorizontalLinePolyDataMapper2D = vtkPolyDataMapper2D::New();
	m_HorizontalLinePolyDataMapper2D->SetInputConnection(m_HorizontalLineSource->GetOutputPort());
	m_HorizontalLineActor2D = vtkActor2D::New();
	m_HorizontalLineActor2D->SetMapper(m_HorizontalLinePolyDataMapper2D);
	m_Renderer->AddActor(m_HorizontalLineActor2D);
	
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
// Destructor
albaOpMML3ParameterView::~albaOpMML3ParameterView()
//----------------------------------------------------------------------------
{
  m_PiecewiseFunction->Delete() ;
  m_DataPointsPolyData->Delete() ;
  m_DataPointsPolyDataPoints->Delete() ;
  m_DataPointsGlyphSource2D->Delete() ;
  m_DataPointsGlyph2D->Delete() ;
  m_DataPointsPolyDataMapper2D->Delete() ;
  m_DataPointsActor2D->Delete() ;

  m_KochanekSpline->Delete() ;
  m_SplinePolyData->Delete() ;
  m_SplinePolyDataPoints->Delete() ;
  m_SplinePolyDataLines->Delete() ;
  m_SplinePolyDataMapper2D->Delete() ;
  m_SplineActor2D->Delete() ;

  m_LineSource->Delete() ;
  m_LinePolyDataMapper2D->Delete() ;
  m_LineActor2D->Delete() ;

  m_HorizontalLineSource->Delete() ;
  m_HorizontalLinePolyDataMapper2D->Delete() ;
  m_HorizontalLineActor2D->Delete() ;
}



//----------------------------------------------------------------------------
void albaOpMML3ParameterView::ComputeConvertTommoMMLParameterViewX()
//----------------------------------------------------------------------------
{
	double dRange;

	dRange = m_DXMax - m_DXMin;
	assert(dRange > 0);
 	m_DXPosConvertRatio = m_Width / dRange;
}


//----------------------------------------------------------------------------
double albaOpMML3ParameterView::ConvertTommoMMLParameterViewX(double X)
//----------------------------------------------------------------------------
{
	//assert(X >= m_DXMin);
	//assert(X <= m_DXMax);
	return m_DXPosConvertRatio * X;
}


//----------------------------------------------------------------------------
void albaOpMML3ParameterView::ComputeConvertTommoMMLParameterViewY()
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
double albaOpMML3ParameterView::ConvertTommoMMLParameterViewY(double Y)
//----------------------------------------------------------------------------
{
	double dMiddle;

	//assert(Y >= m_DYMin);
	//assert(Y <= m_DYMax);

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
double albaOpMML3ParameterView::ConvertFrommmoMMLParameterViewX(double X)
//----------------------------------------------------------------------------
{
	return X / m_DXPosConvertRatio;
}


//----------------------------------------------------------------------------
double albaOpMML3ParameterView::ConverFrommmoMMLParameterViewY(double Y)
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
void albaOpMML3ParameterView::ComputeSpline()
//----------------------------------------------------------------------------
{
	int i;
	double *pflDataPoints;
	int nNumberOfDataPoints;
	
	// how many data points?
	nNumberOfDataPoints = m_PiecewiseFunction->GetSize();
	
	// spline needs at least two data points
	assert(nNumberOfDataPoints > 1);

	// pointer to data points
	pflDataPoints = m_PiecewiseFunction->GetDataPointer();  
    
	// display
	m_DataPointsPolyDataPoints->Reset();
	m_DataPointsPolyDataPoints->Modified();

	for (i = 0; i < nNumberOfDataPoints; i++)
    {
		m_DataPointsPolyDataPoints->InsertPoint(i,
			                                     *(pflDataPoints + 2 * i),
												 *(pflDataPoints + 2 * i + 1),
												 0.0);   
    }

	// spline interpolation points
	for (i = 0; i < nNumberOfDataPoints; i++)
    {
		m_KochanekSpline->AddPoint(*(pflDataPoints + 2 * i), *(pflDataPoints + 2 * i + 1));   
    }

	double PosConvertRatio, t;
 	PosConvertRatio = ((double) (m_Width)) / 
 		              ((double) (m_NNumberOfKochanekSplinePoints));

	// spline display points
	for(i=0; i<m_NNumberOfKochanekSplinePoints; i++)
	{
      t = PosConvertRatio * i;
      m_SplinePolyDataPoints->InsertPoint(i, t, m_KochanekSpline->Evaluate(t), 0.0); 
	}
	
	// spline display lines
	m_SplinePolyDataLines->InsertNextCell(m_NNumberOfKochanekSplinePoints);
	for(i=0; i< m_NNumberOfKochanekSplinePoints; i++)
	{
		m_SplinePolyDataLines->InsertCellPoint(i);
	}
}


//----------------------------------------------------------------------------
void albaOpMML3ParameterView::Render()
//----------------------------------------------------------------------------
{
	ComputeSpline();
	m_RenderWindow->Render();
}


//----------------------------------------------------------------------------
void albaOpMML3ParameterView::ResetSpline()
//----------------------------------------------------------------------------
{
	int i;
	double *pflDataPoints;
	int nNumberOfDataPoints;

	// how many data points?
	nNumberOfDataPoints = m_PiecewiseFunction->GetSize();

	if (nNumberOfDataPoints > 0)
	{
		// pointer to data points
		pflDataPoints = m_PiecewiseFunction->GetDataPointer ();

		for(i = 0; i < nNumberOfDataPoints; i++)
		{
			m_KochanekSpline->RemovePoint(*(pflDataPoints + 2 * i));
		}
	}
}



//----------------------------------------------------------------------------
void albaOpMML3ParameterView::SetRangeX(int r)
//----------------------------------------------------------------------------
{
	SetMinX(0);
	SetMaxX(r - 1);
	ComputeConvertTommoMMLParameterViewX();
}


//----------------------------------------------------------------------------
void albaOpMML3ParameterView::InitialiseLineActor()
//----------------------------------------------------------------------------
{
	m_LineSource->SetPoint1(0, 0, 0);
	m_LineSource->SetPoint2(0, ConvertTommoMMLParameterViewY(m_DYMax), 0);
}


//----------------------------------------------------------------------------
void albaOpMML3ParameterView::AddPoint(double x, double y)
//----------------------------------------------------------------------------
{
	ResetSpline();
	m_PiecewiseFunction->AddPoint(ConvertTommoMMLParameterViewX(x),ConvertTommoMMLParameterViewY(y));
   /* m_PDataPointsPolyDataPoints->Modified();
	m_PPiecewiseFunction->Update();
	m_PRenderWindow->Modified();*/

}


//----------------------------------------------------------------------------
float albaOpMML3ParameterView::GetValue(double x)
//----------------------------------------------------------------------------
{
	return m_PiecewiseFunction->GetValue(x);
}


//----------------------------------------------------------------------------
void albaOpMML3ParameterView::RemovePoint(double x)
//----------------------------------------------------------------------------
{
	ResetSpline();
	m_PiecewiseFunction->RemovePoint(ConvertTommoMMLParameterViewX(x));
}


//----------------------------------------------------------------------------
void albaOpMML3ParameterView::SetRangeY(double left, double middle, double right)
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
void albaOpMML3ParameterView::SetLineActorX(int x)
//----------------------------------------------------------------------------
{
	double x1, y1;
	double x2, y2;

	x1 = ConvertTommoMMLParameterViewX(x);
	y1 = ConvertTommoMMLParameterViewY(GetMinY());
	m_LineSource->SetPoint1(x1,
			                 y1,
							 0);

	x2 = ConvertTommoMMLParameterViewX(x);
	y2 = ConvertTommoMMLParameterViewY(GetMaxY());
    m_LineSource->SetPoint2(x2,
			                 y2,
							 0);
	//Render();
}




//----------------------------------------------------------------------------
void albaOpMML3ParameterView::InitialiseHorizontalLineActor()
//----------------------------------------------------------------------------
{
	m_HorizontalLineSource->SetPoint1(0, ConvertTommoMMLParameterViewY(m_DYMid), 0);
	m_HorizontalLineSource->SetPoint2(ConvertTommoMMLParameterViewX(m_DXMax), ConvertTommoMMLParameterViewY(m_DYMid), 0);
}



//----------------------------------------------------------------------------
int albaOpMML3ParameterView::GetNumberOfDataPoints()
//----------------------------------------------------------------------------
{
	return m_DataPointsPolyDataPoints->GetNumberOfPoints();
}
