/*=========================================================================
Program:   ALBA (Agile Library for Biomedical Applications)
Module:    albaInteractor2DMeasure_AngleLine.h
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

#ifndef __albaInteractor2DMeasure_AngleLine_h
#define __albaInteractor2DMeasure_AngleLine_h

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

// Class Name: albaInteractor2DMeasure_AngleLine
class ALBA_EXPORT albaInteractor2DMeasure_AngleLine : public albaInteractor2DMeasure
{
public:

	albaTypeMacro(albaInteractor2DMeasure_AngleLine, albaInteractor2DMeasure);

	// MEASURE
	/** Add Measure*/
	virtual void AddMeasure(double *point1, double *point2, double *point3, double * point4);
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
	void GetMeasureLinePoints(int index, double *point1, double *point2, double * point3, double * point4);
	void GetCenter(int index, double *center);

	bool Load(albaVME *input, wxString tag);
	bool Save(albaVME *input, wxString tag);

	double GetMeasureAngle(int idx);
	double CalculateAngle(double * point1, double * point2, double * point3, double * point4);
protected:

	albaInteractor2DMeasure_AngleLine();
	virtual ~albaInteractor2DMeasure_AngleLine();

	// Draw Measure
	void MoveMeasure(int index, double * point);
	void DrawNewMeasure(double * wp);
	
	void UpdateMeasure(int index, double measure);

	// RENDERING
	virtual void UpdateLineActors(double * point1, double * point2, double * point3, double * point4);
	virtual void UpdatePointsActor(double * point1, double * point2, double * point3, double * point4);
	void UpdateCircleActor(double * point, double angle, double radius);
	void UpdateTextActor(double * point1, double * point2);
	
	// UTILS
	virtual void FindAndHighlight(double * point);

	// Persistent Points
	std::vector<albaActor2dStackHelper *> m_PointsStackVectorA;
	std::vector<albaActor2dStackHelper *> m_PointsStackVectorB;
	std::vector<albaActor2dStackHelper *> m_PointsStackVectorC;
	std::vector<albaActor2dStackHelper *> m_PointsStackVectorD;

	// Persistent line
	std::vector<albaActor2dStackHelper *> m_LineStackVectorAB;	// First Line 
	std::vector<albaActor2dStackHelper *> m_LineStackVectorCD; // Second Line

	// Persistent Circle
	std::vector<albaActor2dStackHelper *> m_CircleStackVector;

	std::vector<bool> m_SecondLineP1Added;
	std::vector<bool> m_SecondLineP2Added;
	std::vector<double> m_Angles;

	double m_LineExtensionLength;
	int m_LineStipplePattern;

	bool m_MoveLineAB;
	bool m_MoveLineCD;
	double m_OldLineP3[3];
	double m_OldLineP4[3];

	Color m_ColorAux;

private:
	albaInteractor2DMeasure_AngleLine(const albaInteractor2DMeasure_AngleLine&);   // Not implemented.
	void operator=(const albaInteractor2DMeasure_AngleLine&);  // Not implemented.
	friend class albaInteractor2DMeasure_AngleLineTest;
};
#endif
