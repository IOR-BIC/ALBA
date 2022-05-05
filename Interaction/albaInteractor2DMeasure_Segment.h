/*=========================================================================
Program:   ALBA (Agile Library for Biomedical Applications)
Module:    albaInteractor2DMeasure_Segment.h
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

#ifndef __albaInteractor2DMeasure_Segment_h
#define __albaInteractor2DMeasure_Segment_h

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaInteractor2DMeasure.h"
#include "albaInteractor2DMeasure_Distance.h"

//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------
class vtkLineSource;
class vtkPolyDataMapper2D;
class vtkActor2D;

// Class Name: albaInteractor2DMeasure_Segment
class ALBA_EXPORT albaInteractor2DMeasure_Segment : public albaInteractor2DMeasure_Distance
{
public:

	albaTypeMacro(albaInteractor2DMeasure_Segment, albaInteractor2DMeasure_Distance);

	// MEASURE

	/** Add Measure*/
	//virtual void AddMeasure(double *point1, double *point2);
	/** Edit Measure*/
	virtual void EditMeasure(int index, double *point);

	bool Load(albaVME *input, wxString tag);
	bool Save(albaVME *input, wxString tag);

protected:

	albaInteractor2DMeasure_Segment();
	virtual ~albaInteractor2DMeasure_Segment();
	
private:
	albaInteractor2DMeasure_Segment(const albaInteractor2DMeasure_Segment&);   // Not implemented.
	void operator=(const albaInteractor2DMeasure_Segment&);  // Not implemented.
	friend class albaInteractor2DMeasure_SegmentTest;
};
#endif
