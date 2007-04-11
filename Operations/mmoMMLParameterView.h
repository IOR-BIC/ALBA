/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoMMLParameterView.h,v $
  Language:  C++
  Date:      $Date: 2007-04-11 11:53:06 $
  Version:   $Revision: 1.1 $
  Authors:   Mel Krokos
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mmoMMLParameterView_H__
#define __mmoMMLParameterView_H__

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
// mmoMMLParameterView   :
//----------------------------------------------------------------------------
class mmoMMLParameterView  
{
public:
	float GetValue(double x);
	int GetNumberOfDataPoints();
	mmoMMLParameterView( vtkRenderWindow *rw, vtkRenderer *ren );
	virtual ~mmoMMLParameterView();

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
	vtkPiecewiseFunction* m_pPiecewiseFunction;
	void SetRangeX(int r);
	void SetSplineSamples(int s);
	int m_nNumberOfKochanekSplinePoints;
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

	double m_dYMin;
	double m_dYMax;
	double m_dYMid;
	double m_dYPosConvertRatio1;
	double m_dYPosConvertTrans1;
	double m_dYPosConvertRatio2;
	double m_dYPosConvertTrans2;

	double m_dXMin;
	double m_dXMax;
	double m_dXPosConvertRatio;
	
	vtkRenderer 				*m_pRenderer;
	vtkRenderWindow 		*m_pRenderWindow;

	vtkPolyData 				*m_pDataPointsPolyData;
	vtkPoints 					*m_pDataPointsPolyDataPoints;
	vtkGlyphSource2D 		*m_pDataPointsGlyphSource2D;
	vtkGlyph2D 					*m_pDataPointsGlyph2D;
	vtkPolyDataMapper2D *m_pDataPointsPolyDataMapper2D;
	vtkActor2D 					*m_pDataPointsActor2D;

	vtkKochanekSpline 	*m_pKochanekSpline;
	vtkPolyData 				*m_pSplinePolyData;
	vtkPoints 					*m_pSplinePolyDataPoints;
	vtkCellArray 				*m_pSplinePolyDataLines;
	vtkPolyDataMapper2D *m_pSplinePolyDataMapper2D;
	vtkActor2D 					*m_pSplineActor2D;

	vtkLineSource 			*m_pLineSource;
	vtkPolyDataMapper2D *m_pLinePolyDataMapper2D;
	vtkActor2D 					*m_pLineActor2D;

	vtkLineSource 			*m_pHorizontalLineSource;
	vtkPolyDataMapper2D *m_pHorizontalLinePolyDataMapper2D;
	vtkActor2D 					*m_pHorizontalLineActor2D;
};

#endif

