/*=========================================================================
Program:   ALBA (Agile Library for Biomedical Applications)
Module:    albaTool2DMeasureCenterPoint.h
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

#ifndef __albaTool2DMeasureCenterPoint_h
#define __albaTool2DMeasureCenterPoint_h

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaTool2DMeasure.h"

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

// Class Name: albaTool2DMeasureCenterPoint
class ALBA_EXPORT albaTool2DMeasureCenterPoint : public albaTool2DMeasure
{
public:

	albaTypeMacro(albaTool2DMeasureCenterPoint, albaTool2DMeasure);

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
	void SetCenterPointSize(double size);
	virtual void SetLineWidth(double width);

	/** Get measure line extremity points*/
	void GetMeasureLinePoints(int index, double *point1, double *point2);

	double GetCircleRadius(int index);

	void GetCenter(int index, double *center);
	
	bool Load(albaVME *input, wxString tag);
	bool Save(albaVME *input, wxString tag);

protected:

	albaTool2DMeasureCenterPoint();
	virtual ~albaTool2DMeasureCenterPoint();

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
	albaTool2DMeasureCenterPoint(const albaTool2DMeasureCenterPoint&);   // Not implemented.
	void operator=(const albaTool2DMeasureCenterPoint&);  // Not implemented.
	friend class albaTool2DMeasureCenterPointTest;
};
#endif
