/*=========================================================================
Program:   ALBA (Agile Library for Biomedical Applications)
Module:    albaInteractor2DMeasureEllipse.h
Language:  C++
Date:      $Date: 2021-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Gianliugi Crimi
==========================================================================
Copyright (c) BIC-IOR 2021 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#ifndef __albaInteractor2DMeasureEllipse_h
#define __albaInteractor2DMeasureEllipse_h

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaInteractor2DMeasure.h"

//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------
class vtkActor;
class vtkActor2D;
class vtkDataSetMapper;
class vtkDiskSource;
class vtkLineSource;
class vtkPolyDataMapper2D;
class vtkTransform;
class vtkTransformPolyDataFilter;

// Class Name: albaInteractor2DMeasureEllipse
class ALBA_EXPORT albaInteractor2DMeasureEllipse : public albaInteractor2DMeasure
{
public:

	albaTypeMacro(albaInteractor2DMeasureEllipse, albaInteractor2DMeasure);

	// MEASURE
	/** Add Measure*/
	virtual void AddMeasure(double *point1, double *point2);
	/** Edit Measure*/
	virtual void EditMeasure(int index, double *point);
	/** Rotate Measure*/
	virtual void RotateMeasure(int index, double *point);;

	/** Delete the Measure*/
	virtual void RemoveMeasure(int index);
	/** Select a Measure*/
	virtual void SelectMeasure(int index);

	virtual void Show(bool show);

	virtual void SetPointSize(double size);
	void SetCenterPointSize(double size);
	virtual void SetLineWidth(double width);

	/** Get measure extremity points*/
	void GetMeasurePoints(int index, double *point1, double *point2);

	void GetCenter(int index, double *center);
	
	bool Load(albaVME *input, wxString tag);
	bool Save(albaVME *input, wxString tag);

	/** CTRL switches from editing to rotating the ellipse, without moving the mouse */
	virtual bool OnKey(int keyCode, bool pressed);

	/** Returns true if the point is inside the ellipse. Works only on current ellipse plane, the other coordinate will be ignored */
	bool IsPointInsideMeasure(double *point, int measureIndex=-1);

	/** Gets mesure bounds*/
	void GetMeasureBounds(double bounds[6], int measureIndex=-1);

	/** Gets measure perimeter*/
	double GetMeasurePerimeter(int measureIndex=-1);


protected:

	albaInteractor2DMeasureEllipse();
	virtual ~albaInteractor2DMeasureEllipse();

	// Draw Measure
	void MoveMeasure(int index, double * point);
	void DrawNewMeasure(double * wp);

	// RENDERING
	virtual void UpdatePointsActor(double * point1, double * point2);
	void UpdateEllipseActor(double * point1, double * point2);
	void UpdateTextActor(double * point1, double * point2);

	//UTILS
	virtual void FindAndHighlight(double * point);

	
	// Persistent Points
	std::vector<albaActor2dStackHelper *> m_PointsStackVectorL;
	std::vector<albaActor2dStackHelper *> m_PointsStackVectorR;
	std::vector<albaActor2dStackHelper *> m_PointsStackVectorC;

	// Persistent Circle
	std::vector<albaActor2dStackHelper *> m_EllipseStackVector;

	double m_CenterPointSize;
	double m_LastPoint[3];
	bool m_CTRLPressed;

private:
	albaInteractor2DMeasureEllipse(const albaInteractor2DMeasureEllipse&);   // Not implemented.
	void operator=(const albaInteractor2DMeasureEllipse&);  // Not implemented.
	friend class albaInteractor2DMeasureEllipseTest;
};
#endif
