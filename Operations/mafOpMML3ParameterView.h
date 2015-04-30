/*=========================================================================

 Program: MAF2
 Module: mafOpMML3ParameterView
 Authors: Mel Krokos
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpMML3ParameterView_H__
#define __mafOpMML3ParameterView_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
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
#include "vtkPoints.h"


//----------------------------------------------------------------------------
/// mafOpMML3ParameterView. \n
/// Helper class for mafOpMML3. \n
/// Visual pipe for displaying plot of parameter value against slice number.
//----------------------------------------------------------------------------
class MAF_EXPORT mafOpMML3ParameterView  
{
public:
	mafOpMML3ParameterView( vtkRenderWindow *rw, vtkRenderer *ren );
	virtual ~mafOpMML3ParameterView();

  float GetValue(double x);
  int GetNumberOfDataPoints();

  double GetMaxY() const {return m_DYMax ;}
  double GetMinY() const {return m_DYMin ;}

  void SetLineActorX(int x) ;
  vtkActor2D* GetPointsActor() const {return m_DataPointsActor2D ;}
  vtkActor2D* GetSplineActor() const {return m_SplineActor2D ;}

  void SetRangeX(int r);
	void SetRangeY(double left, double middle, double right);
	void RemovePoint(double x);
	void AddPoint(double x, double y);
	void Render();

protected:
  vtkRenderer* GetRenderer() const {return m_Renderer ;}
  void InitialiseHorizontalLineActor();
  vtkActor2D* GetHorizontalLineActor() const {return m_HorizontalLineActor2D ;}
  vtkActor2D* GetLineActor() const {return m_LineActor2D ;}
  void InitialiseLineActor();
  void SetSplineSamples(int s) {m_NNumberOfKochanekSplinePoints = s ;}
  void SetMidY(double y){m_DYMid = y;}
  void SetMaxY(double y){m_DYMax = y;}
  void SetMinY(double y){m_DYMin = y;}
  void SetMaxX(double x){m_DXMax = x;}
  void SetMinX(double x) {m_DXMin = x;}
  void ResetSpline();
  void ComputeSpline();
  double ConverFrommmoMMLParameterViewY(double Y);
  double ConvertFrommmoMMLParameterViewX(double X);
  void ComputeConvertTommoMMLParameterViewY();
  void ComputeConvertTommoMMLParameterViewX();
  double ConvertTommoMMLParameterViewX(double X);
  double ConvertTommoMMLParameterViewY(double Y);


  int m_NNumberOfKochanekSplinePoints;

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
	

  vtkRenderer 				*m_Renderer;
  vtkRenderWindow 		*m_RenderWindow;

  vtkPiecewiseFunction* m_PiecewiseFunction;

	vtkPolyData 				*m_DataPointsPolyData;
	vtkPoints 					*m_DataPointsPolyDataPoints;
	vtkGlyphSource2D 		*m_DataPointsGlyphSource2D;
	vtkGlyph2D 					*m_DataPointsGlyph2D;
	vtkPolyDataMapper2D *m_DataPointsPolyDataMapper2D;
	vtkActor2D 					*m_DataPointsActor2D;

	vtkKochanekSpline 	*m_KochanekSpline;
	vtkPolyData 				*m_SplinePolyData;
	vtkPoints 					*m_SplinePolyDataPoints;
	vtkCellArray 				*m_SplinePolyDataLines;
	vtkPolyDataMapper2D *m_SplinePolyDataMapper2D;
	vtkActor2D 					*m_SplineActor2D;

	vtkLineSource 			*m_LineSource;
	vtkPolyDataMapper2D *m_LinePolyDataMapper2D;
	vtkActor2D 					*m_LineActor2D;

	vtkLineSource 			*m_HorizontalLineSource;
	vtkPolyDataMapper2D *m_HorizontalLinePolyDataMapper2D;
	vtkActor2D 					*m_HorizontalLineActor2D;
};

#endif

