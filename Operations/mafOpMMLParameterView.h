/*=========================================================================

 Program: MAF2
 Module: mafOpMMLParameterView
 Authors: Mel Krokos
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpMMLParameterView_H__
#define __mafOpMMLParameterView_H__

#include "mafDefines.h"

// vtk includes
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkGlyphSource2D.h"
#include "vtkGlyph2D.h"
#include "vtkPolyDataMapper2D.h"
#include "vtkPolyData.h"
#include "vtkCellArray.h"
#include "vtkActor2D.h"
#include "vtkLineSource.h"
#include "vtkKochanekSpline.h"
#include "vtkPiecewiseFunction.h"

//----------------------------------------------------------------------------
// mafOpMMLParameterView   :
//----------------------------------------------------------------------------
class mafOpMMLParameterView  
{
public:
	float GetValue(double x);
	int GetNumberOfDataPoints();
	mafOpMMLParameterView( vtkRenderWindow *rw, vtkRenderer *ren );
	virtual ~mafOpMMLParameterView();

	vtkRenderer* GetRenderer();
	void InitialiseHorizontalLineActor();
	vtkActor2D* GetHorizontalLineActor();
	void SetLineActorX(int x);
	double GetMaxY();
	double GetMinY();
	vtkActor2D* GetPointsActor();
	void SetRangeY(double left, double middle, double right);
	void RemovePoint(double x);
	void AddPoint(double x, double y);
	void InitialiseLineActor();
	vtkPiecewiseFunction* m_PPiecewiseFunction;
	void SetRangeX(int r);
	void SetSplineSamples(int s);
	int m_NNumberOfKochanekSplinePoints;
	vtkActor2D* GetSplineActor();
	vtkActor2D* GetLineActor();
	void SetMidY(double y);
	void SetMaxY(double y);
	void SetMinY(double y);
	void SetMaxX(double x);
	void SetMinX(double x);
	void ResetSpline();
	void Render();
	void ComputeSpline();
	double ConverFrommmoMMLParameterViewY(double Y);
	double ConvertFrommmoMMLParameterViewX(double X);
	void ComputeConvertTommoMMLParameterViewY();
	void ComputeConvertTommoMMLParameterViewX();
	double ConvertTommoMMLParameterViewX(double X);
	double ConvertTommoMMLParameterViewY(double Y);

  int m_Width;
  int m_Height;

	double m_DYMin;
	double m_DYMax;
	double m_DYMid;
	double m_DYPosConvertRatio1;
	double m_DYPosConvertTrans1;
	double m_DYPosConvertRatio2;
	double m_DYPosConvertTrans2;

	double m_DXMin;
	double m_DXMax;
	double m_DXPosConvertRatio;
	
	vtkRenderer 				*m_PRenderer;
	vtkRenderWindow 		*m_PRenderWindow;

	vtkPolyData 				*m_PDataPointsPolyData;
	vtkPoints 					*m_PDataPointsPolyDataPoints;
	vtkGlyphSource2D 		*m_PDataPointsGlyphSource2D;
	vtkGlyph2D 					*m_PDataPointsGlyph2D;
	vtkPolyDataMapper2D *m_PDataPointsPolyDataMapper2D;
	vtkActor2D 					*m_PDataPointsActor2D;

	vtkKochanekSpline 	*m_PKochanekSpline;
	vtkPolyData 				*m_PSplinePolyData;
	vtkPoints 					*m_PSplinePolyDataPoints;
	vtkCellArray 				*m_PSplinePolyDataLines;
	vtkPolyDataMapper2D *m_PSplinePolyDataMapper2D;
	vtkActor2D 					*m_PSplineActor2D;

	vtkLineSource 			*m_PLineSource;
	vtkPolyDataMapper2D *m_PLinePolyDataMapper2D;
	vtkActor2D 					*m_PLineActor2D;

	vtkLineSource 			*m_PHorizontalLineSource;
	vtkPolyDataMapper2D *m_PHorizontalLinePolyDataMapper2D;
	vtkActor2D 					*m_PHorizontalLineActor2D;
};

#endif

