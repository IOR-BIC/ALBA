/*=========================================================================
Program:	 ALBA (Agile Library for Biomedical Applications)
Module:    albaInteractor2DMeasure_Point.h
Language:  C++
Date:      $Date: 2019-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Nicola Vanella
==========================================================================
Copyright (c) BIC-IOR 2019 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#ifndef __albaInteractor2DMeasure_Point_h
#define __albaInteractor2DMeasure_Point_h

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaInteractor2DMeasure.h"

//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------
class vtkPolyDataMapper2D;

// Class Name: albaInteractor2DMeasure_Point
class ALBA_EXPORT albaInteractor2DMeasure_Point : public albaInteractor2DMeasure
{
public:

	albaTypeMacro(albaInteractor2DMeasure_Point, albaInteractor2DMeasure);
	
	// MEASURE
	/** Add Measure*/
	virtual void AddMeasure(double *point);
	/** Edit Measure*/
	virtual void EditMeasure(int index, double *point);
	/** Delete the Measure*/
	virtual void RemoveMeasure(int index);
	/** Select a Measure*/
	virtual void SelectMeasure(int index);

	virtual void SetPointSize(double size);

	/** Get point coordinate*/
	void GetMeasurePoint(int index, double *point);

	bool Load(albaVME *input, wxString tag);
	bool Save(albaVME *input, wxString tag);

	void FreeEdit() { m_EditConstraint = FREE_EDIT; };
	void LockPointOnLine(double *lineP1, double *lineP2);

protected:

	enum EDIT_POINT_CONSTRAINTS
	{
		FREE_EDIT,
		LOCK_ON_LINE,
// 		LOCK_IN_RECT,
// 		LOCK_IN_CIRCLE,
	};

	albaInteractor2DMeasure_Point();
	virtual ~albaInteractor2DMeasure_Point();

	// Draw Measure
	void MoveMeasure(int index, double * point);
	void DrawNewMeasure(double * wp);

	// RENDERING
	virtual void UpdatePointsActor(double * point1, double * point2 = NULL);
	void UpdateTextActor(int index, double *point);

	// UTILS
	virtual void FindAndHighlight(double * point);

	void GetPointOnLine(double *point);

	// Persistent Points
	std::vector<albaActor2dStackHelper *> m_PointsStackVector;

	// Constraints
	int m_EditConstraint;
	double m_ConstrLineP1[3];
	double m_ConstrLineP2[3];

private:
	albaInteractor2DMeasure_Point(const albaInteractor2DMeasure_Point&);   // Not implemented.
	void operator=(const albaInteractor2DMeasure_Point&);  // Not implemented.
	friend class albaInteractor2DMeasure_PointTest;
};
#endif
