/*=========================================================================
Program:	 ALBA (Agile Library for Biomedical Applications)
Module:    alba2DMeasureDistanceTool.h
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

#ifndef __alba2DMeasureDistanceTool_h
#define __alba2DMeasureDistanceTool_h

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "alba2DMeasureTool.h"

//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------
class vtkLineSource;
class vtkPolyDataMapper2D;
class vtkActor2D;

// Class Name: alba2DMeasureDistanceTool
class ALBA_EXPORT alba2DMeasureDistanceTool : public alba2DMeasureTool
{
public:
	enum MEASURE_CONSTRAINTS
	{
		NO_CONSTRAINT = 0,
		HORIZONTAL_CONSTRAINT,
		VERTICAL_CONSTRAINT,
	};

	albaTypeMacro(alba2DMeasureDistanceTool, alba2DMeasureTool);

	// MEASURE
	/** Add Measure*/
	virtual void AddMeasure(double *point1, double *point2);
	/** Edit Measure*/
	virtual void EditMeasure(int index, double *point);
	/** Delete the Measure*/
	virtual void RemoveMeasure(int index);
	/** Select a Measure*/
	virtual void SelectMeasure(int index);

	virtual void SetMinDistance(double minDistance);

	virtual void SetLineWidth(double width);
	virtual void SetLineTickWidth(double width);
	virtual void SetLineTickLenght(double lenght);

	virtual void Show(bool show);

	/** Get measure line extremity points*/
	void GetMeasureLinePoints(int index, double *point1, double *point2);

	/** Get the i-th Distance*/
	double GetDistance(int index);

	bool Load(albaVME *input, wxString tag);
	bool Save(albaVME *input, wxString tag);

	/** Getter for GetConstraint */
	int GetConstraint() const { return m_Constraint; }

	/** Setter for SetConstraint */
	void SetConstraint(int val) { m_Constraint = val; }
protected:

	alba2DMeasureDistanceTool();
	virtual ~alba2DMeasureDistanceTool();

	// Draw Measure
	void MoveMeasure(int index, double * point);
	void DrawNewMeasure(double * wp);
	
	// RENDERING
	virtual void UpdateLineActors(double * point1, double * point2);
	virtual void UpdateLineTickActor(double * point1, double * point2);
	void UpdateTextActor(double * point1, double * point2);

	//UTILS
	virtual void FindAndHighlight(double * point);
	
	// Persistent line
	std::vector<albaActor2dStackHelper *> m_LineStackVector;
	std::vector<albaActor2dStackHelper *> m_TickStackVectorL;
	std::vector<albaActor2dStackHelper *> m_TickStackVectorR;

	double m_MinDistance;
	double m_LineTickWidth;
	double m_TickLenght;

	int m_Constraint;

private:
	alba2DMeasureDistanceTool(const alba2DMeasureDistanceTool&);   // Not implemented.
	void operator=(const alba2DMeasureDistanceTool&);  // Not implemented.
	friend class alba2DMeasureDistanceToolTest;
};
#endif
