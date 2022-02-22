/*=========================================================================
Program:	 ALBA (Agile Library for Biomedical Applications)
Module:    albaInteractor2DMeasure_Distance.h
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

#ifndef __albaInteractor2DMeasure_Distance_h
#define __albaInteractor2DMeasure_Distance_h

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

// Class Name: albaInteractor2DMeasure_Distance
class ALBA_EXPORT albaInteractor2DMeasure_Distance : public albaInteractor2DMeasure
{
public:

	albaTypeMacro(albaInteractor2DMeasure_Distance, albaInteractor2DMeasure);

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

	/** Get measure line extremity points*/
	void GetMeasureLinePoints(int index, double *point1, double *point2);

	bool Load(albaVME *input, wxString tag);
	bool Save(albaVME *input, wxString tag);

protected:

	albaInteractor2DMeasure_Distance();
	virtual ~albaInteractor2DMeasure_Distance();

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
private:
	albaInteractor2DMeasure_Distance(const albaInteractor2DMeasure_Distance&);   // Not implemented.
	void operator=(const albaInteractor2DMeasure_Distance&);  // Not implemented.
	friend class albaInteractor2DMeasure_DistanceTest;
};
#endif
