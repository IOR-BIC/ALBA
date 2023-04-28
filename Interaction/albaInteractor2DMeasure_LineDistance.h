/*=========================================================================
Program:   ALBA (Agile Library for Biomedical Applications)
Module:    albaInteractor2DMeasure_LineDistance.h
Language:  C++
Date:      $Date: 2021-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Gianluigi Crimi
==========================================================================
Copyright (c) BIC-IOR 2021 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#ifndef __albaInteractor2DMeasure_LineDistance_h
#define __albaInteractor2DMeasure_LineDistance_h

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaInteractor2DMeasure.h"

//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------
class vtkLineSource;
class vtkPolyDataMapper2D;
class vtkActor2D;

// Class Name: albaInteractor2DMeasure_LineDistance
class ALBA_EXPORT albaInteractor2DMeasure_LineDistance : public albaInteractor2DMeasure
{
public:

	albaTypeMacro(albaInteractor2DMeasure_LineDistance, albaInteractor2DMeasure);

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
	virtual void SetLineWidth(double width);

	void SetLineExtensionLength(double length) { m_LineExtensionLength = length; };
	void SetLineStipplePattern(int lp) { m_LineStipplePattern = lp; };

	/** Get measure line extremity points*/
	void GetMeasureLinePoints(int index, double *point1, double *point2);

	bool Load(albaVME *input, wxString tag);
	bool Save(albaVME *input, wxString tag);

	double GetDistance(int idx);
protected:

	albaInteractor2DMeasure_LineDistance();
	virtual ~albaInteractor2DMeasure_LineDistance();

	// Draw Measure
	void MoveMeasure(int index, double * point);
	void DrawNewMeasure(double * wp);

	void UpdateMeasure(int index, double measure);

	// RENDERING
	virtual void UpdateLineActors(double * point1, double * point2);
	virtual void UpdatePointsActor(double * point1, double * point2);
	void UpdateTextActor(double * point1, double * point2);

	//UTILS
	virtual void FindAndHighlight(double * point);

	// Persistent line
	std::vector<albaActor2dStackHelper *> m_LineStackVector;	// Main Line 
	std::vector<albaActor2dStackHelper *> m_LineStackVectorB; // Second Line
	std::vector<albaActor2dStackHelper *> m_LineStackVectorPerp; // Perpendicular Line
	
	// Persistent Points
	std::vector<albaActor2dStackHelper *> m_PointsStackVectorL;
	std::vector<albaActor2dStackHelper *> m_PointsStackVectorR;

	std::vector<bool> m_SecondLineAdded;
	std::vector<double> m_Distances;

	double m_LineExtensionLength;
	int m_LineStipplePattern;

	bool m_MoveLineB;
	bool m_AddModeCompleted;

private:
	albaInteractor2DMeasure_LineDistance(const albaInteractor2DMeasure_LineDistance&);   // Not implemented.
	void operator=(const albaInteractor2DMeasure_LineDistance&);  // Not implemented.
	friend class albaInteractor2DMeasure_LineDistanceTest;
};
#endif
