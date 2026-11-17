/*=========================================================================
Program:	 ALBA (Agile Library for Biomedical Applications)
Module:    alba2DMeasurePointTool.h
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

#ifndef __alba2DMeasurePointTool_h
#define __alba2DMeasurePointTool_h

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "alba2DMeasureTool.h"

//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------
class vtkPolyDataMapper2D;

// Class Name: alba2DMeasurePointTool
class ALBA_EXPORT alba2DMeasurePointTool : public alba2DMeasureTool
{
public:

	albaTypeMacro(alba2DMeasurePointTool, alba2DMeasureTool);
	
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

	virtual void Show(bool show);

	/** Get point coordinate*/
	void GetMeasurePoint(int index, double *point);

	bool Load(albaVME *input, wxString tag);
	bool Save(albaVME *input, wxString tag);

	void FreeEdit() { m_EditConstraint = FREE_EDIT; };
	void LockPointOnLine(double *lineP1, double *lineP2);

	virtual void ReDrawAll();

protected:

	enum EDIT_POINT_CONSTRAINTS
	{
		FREE_EDIT,
		LOCK_ON_LINE,
// 		LOCK_IN_RECT,
// 		LOCK_IN_CIRCLE,
	};

	alba2DMeasurePointTool();
	virtual ~alba2DMeasurePointTool();

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
	alba2DMeasurePointTool(const alba2DMeasurePointTool&);   // Not implemented.
	void operator=(const alba2DMeasurePointTool&);  // Not implemented.
	friend class alba2DMeasurePointToolTest;
};
#endif
