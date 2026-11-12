/*=========================================================================
Program:   ALBA (Agile Library for Biomedical Applications)
Module:    alba2DMeasureSegmentTool.h
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

#ifndef __alba2DMeasureSegmentTool_h
#define __alba2DMeasureSegmentTool_h

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "alba2DMeasureTool.h"
#include "alba2DMeasureDistanceTool.h"

//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------
class vtkLineSource;
class vtkPolyDataMapper2D;
class vtkActor2D;

// Class Name: alba2DMeasureSegmentTool
class ALBA_EXPORT alba2DMeasureSegmentTool : public alba2DMeasureDistanceTool
{
public:

	albaTypeMacro(alba2DMeasureSegmentTool, alba2DMeasureDistanceTool);

	// MEASURE

	/** Add Measure*/
	//virtual void AddMeasure(double *point1, double *point2);
	/** Edit Measure*/
	virtual void EditMeasure(int index, double *point);

	bool Load(albaVME *input, wxString tag);
	bool Save(albaVME *input, wxString tag);

protected:

	alba2DMeasureSegmentTool();
	virtual ~alba2DMeasureSegmentTool();
	
private:
	alba2DMeasureSegmentTool(const alba2DMeasureSegmentTool&);   // Not implemented.
	void operator=(const alba2DMeasureSegmentTool&);  // Not implemented.
	friend class alba2DMeasureSegmentToolTest;
};
#endif
