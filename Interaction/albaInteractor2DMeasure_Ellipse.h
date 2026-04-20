/*=========================================================================
Program:   ALBA (Agile Library for Biomedical Applications)
Module:    albaInteractor2DMeasure_Ellipse.h
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

#ifndef __albaInteractor2DMeasure_Ellipse_h
#define __albaInteractor2DMeasure_Ellipse_h

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

// Class Name: albaInteractor2DMeasure_Ellipse
class ALBA_EXPORT albaInteractor2DMeasure_Ellipse : public albaInteractor2DMeasure
{
public:

	albaTypeMacro(albaInteractor2DMeasure_Ellipse, albaInteractor2DMeasure);

	// MEASURE
	/** Enable/Disable Measure Interactor*/
	virtual void Enable(bool enable = true);

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

	/** Get measure extremity points*/
	void GetMeasurePoints(int index, double *point1, double *point2);

	void GetCenter(int index, double *center);
	
	bool Load(albaVME *input, wxString tag);
	bool Save(albaVME *input, wxString tag);

	virtual void OnEvent(albaEventBase *event);

protected:

	albaInteractor2DMeasure_Ellipse();
	virtual ~albaInteractor2DMeasure_Ellipse();

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
	albaInteractor2DMeasure_Ellipse(const albaInteractor2DMeasure_Ellipse&);   // Not implemented.
	void operator=(const albaInteractor2DMeasure_Ellipse&);  // Not implemented.
	friend class albaInteractor2DMeasure_EllipseTest;
};
#endif
