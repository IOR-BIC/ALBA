/*=========================================================================
Program:   ALBA (Agile Library for Biomedical Applications)
Module:    albaInteractor2DMeasure_CenterPoint.h
Language:  C++
Date:      $Date: 2021-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Nicola Vanella
==========================================================================
Copyright (c) BIC-IOR 2021 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#ifndef __albaInteractor2DMeasure_CenterPoint_h
#define __albaInteractor2DMeasure_CenterPoint_h

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

// Class Name: albaInteractor2DMeasure_CenterPoint
class ALBA_EXPORT albaInteractor2DMeasure_CenterPoint : public albaInteractor2DMeasure
{
public:

	albaTypeMacro(albaInteractor2DMeasure_CenterPoint, albaInteractor2DMeasure);

	// MEASURE
	/** Add Measure*/
	virtual void AddMeasure(double *point1, double *point2);
	/** Edit Measure*/
	virtual void EditMeasure(int index, double *point);
	/** Delete the Measure*/
	virtual void RemoveMeasure(int index);
	/** Select a Measure*/
	virtual void SelectMeasure(int index);

	virtual void SetPointSize(double size);
	void SetCenterPointSize(double size);
	virtual void SetLineWidth(double width);

	/** Get measure line extremity points*/
	void GetMeasureLinePoints(int index, double *point1, double *point2);

	void GetCenter(int index, double *center);
	
	bool Load(albaVME *input, wxString tag);
	bool Save(albaVME *input, wxString tag);

protected:

	albaInteractor2DMeasure_CenterPoint();
	virtual ~albaInteractor2DMeasure_CenterPoint();

	// Draw Measure
	void MoveMeasure(int index, double * point);
	void DrawNewMeasure(double * wp);

	// RENDERING
	virtual void UpdatePointsActor(double * point1, double * point2);
	virtual void UpdateLineActors(double * point1, double * point2);
	void UpdateCircleActor(double * point1, double * point2);
	void UpdateTextActor(double * point1, double * point2);

	//UTILS
	virtual void FindAndHighlight(double * point);

	// Persistent line
	std::vector<albaActor2dStackHelper *> m_LineStackVector;

	// Persistent Points
	std::vector<albaActor2dStackHelper *> m_PointsStackVectorL;
	std::vector<albaActor2dStackHelper *> m_PointsStackVectorR;
	std::vector<albaActor2dStackHelper *> m_PointsStackVectorC;

	// Persistent Circle
	std::vector<albaActor2dStackHelper *> m_CircleStackVector;

	double m_CenterPointSize;

private:
	albaInteractor2DMeasure_CenterPoint(const albaInteractor2DMeasure_CenterPoint&);   // Not implemented.
	void operator=(const albaInteractor2DMeasure_CenterPoint&);  // Not implemented.
	friend class albaInteractor2DMeasure_CenterPointTest;
};
#endif
