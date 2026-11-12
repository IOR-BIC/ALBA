/*=========================================================================
Program:   ALBA (Agile Library for Biomedical Applications)
Module:    alba2DMeasureLineTool.h
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

#ifndef __alba2DMeasureLineTool_h
#define __alba2DMeasureLineTool_h

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

// Class Name: alba2DMeasureLineTool
class ALBA_EXPORT alba2DMeasureLineTool : public alba2DMeasureTool
{
public:

	albaTypeMacro(alba2DMeasureLineTool, alba2DMeasureTool);

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

	virtual void Show(bool show);

	void SetLineExtensionLength(double length) { m_LineExtensionLength = length; };
	void SetLineStipplePattern(int lp) { m_LineStipplePattern = lp; };

	/** Get measure line extremity points*/
	void GetMeasureLinePoints(int index, double *point1, double *point2);

	bool Load(albaVME *input, wxString tag);
	bool Save(albaVME *input, wxString tag);

protected:

	alba2DMeasureLineTool();
	virtual ~alba2DMeasureLineTool();

	// Draw Measure
	void MoveMeasure(int index, double * point);
	void DrawNewMeasure(double * wp);
	
	// RENDERING
	virtual void UpdateLineActors(double * point1, double * point2);
	virtual void UpdatePointsActor(double * point1, double * point2);
	void UpdateLR(vtkLineSource *line, vtkLineSource *lineL, vtkLineSource *lineR);
	void UpdateTextActor(double * point1, double * point2);
	
	//UTILS
	virtual void FindAndHighlight(double * point);

	// Persistent line
	std::vector<albaActor2dStackHelper *> m_LineStackVector;
	std::vector<albaActor2dStackHelper *> m_LineStackVectorL;	// Persistent line
	std::vector<albaActor2dStackHelper *> m_LineStackVectorR;

	// Persistent Points
	std::vector<albaActor2dStackHelper *> m_PointsStackVectorL;
	std::vector<albaActor2dStackHelper *> m_PointsStackVectorR;

	double m_LineExtensionLength;
	int m_LineStipplePattern;

private:
	alba2DMeasureLineTool(const alba2DMeasureLineTool&);   // Not implemented.
	void operator=(const alba2DMeasureLineTool&);  // Not implemented.
	friend class alba2DMeasureLineToolTest;
};
#endif
