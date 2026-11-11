/*=========================================================================
Program:   ALBA (Agile Library for Biomedical Applications)
Module:    albaInteractor2DMeasureMidPoint.h
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

#ifndef __albaInteractor2DMeasureMidPoint_h
#define __albaInteractor2DMeasureMidPoint_h

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

// Class Name: albaInteractor2DMeasureMidPoint
class ALBA_EXPORT albaInteractor2DMeasureMidPoint : public albaInteractor2DMeasure
{
public:

	albaTypeMacro(albaInteractor2DMeasureMidPoint, albaInteractor2DMeasure);

	// MEASURE
	/** Add Measure*/
	virtual void AddMeasure(double *point1, double *point2);
	/** Edit Measure*/
	virtual void EditMeasure(int index, double *point);
	/** Delete the Measure*/
	virtual void RemoveMeasure(int index);
	/** Select a Measure*/
	virtual void SelectMeasure(int index);

	virtual void Show(bool show);

	virtual void SetPointSize(double size);
	void SetMidPointSize(double size);
	virtual void SetLineWidth(double width);

	/** Get measure line extremity points*/
	void GetMeasureLinePoints(int index, double *point1, double *point2);

	/** Get measure line extremity points*/
	void GetMeasureMidPoint(int index, double *point);
	
	bool Load(albaVME *input, wxString tag);
	bool Save(albaVME *input, wxString tag);

protected:

	albaInteractor2DMeasureMidPoint();
	virtual ~albaInteractor2DMeasureMidPoint();

	// Draw Measure
	void MoveMeasure(int index, double * point);
	void DrawNewMeasure(double * wp);

	// RENDERING
	virtual void UpdateLineActors(double * point1, double * point2);
	virtual void UpdatePointsActor(double * point1, double * point2);
	void UpdateMidPointActor(double * point1, double * point2);
	void UpdateTextActor(double * point1, double * point2);
	
	//UTILS
	virtual void FindAndHighlight(double * point);

	// Persistent line
	std::vector<albaActor2dStackHelper *> m_LineStackVector;

	// Persistent Points
	std::vector<albaActor2dStackHelper *> m_PointsStackVectorL;
	std::vector<albaActor2dStackHelper *> m_PointsStackVectorR;
	std::vector<albaActor2dStackHelper *> m_MidPointsStackVector;

	double m_MidPointSize;

private:
	albaInteractor2DMeasureMidPoint(const albaInteractor2DMeasureMidPoint&);   // Not implemented.
	void operator=(const albaInteractor2DMeasureMidPoint&);  // Not implemented.
	friend class albaInteractor2DMeasureMidPointTest;
};
#endif
