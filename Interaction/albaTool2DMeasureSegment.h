/*=========================================================================
Program:   ALBA (Agile Library for Biomedical Applications)
Module:    albaTool2DMeasureSegment.h
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

#ifndef __albaTool2DMeasureSegment_h
#define __albaTool2DMeasureSegment_h

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaTool2DMeasure.h"
#include "albaTool2DMeasureDistance.h"

//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------
class vtkLineSource;
class vtkPolyDataMapper2D;
class vtkActor2D;

// Class Name: albaTool2DMeasureSegment
class ALBA_EXPORT albaTool2DMeasureSegment : public albaTool2DMeasureDistance
{
public:

	albaTypeMacro(albaTool2DMeasureSegment, albaTool2DMeasureDistance);

	// MEASURE

	/** Add Measure*/
	//virtual void AddMeasure(double *point1, double *point2);
	/** Edit Measure*/
	virtual void EditMeasure(int index, double *point);

	bool Load(albaVME *input, wxString tag);
	bool Save(albaVME *input, wxString tag);

protected:

	albaTool2DMeasureSegment();
	virtual ~albaTool2DMeasureSegment();
	
private:
	albaTool2DMeasureSegment(const albaTool2DMeasureSegment&);   // Not implemented.
	void operator=(const albaTool2DMeasureSegment&);  // Not implemented.
	friend class albaTool2DMeasureSegmentTest;
};
#endif
