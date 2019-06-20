/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEOutputSurfaceEditor
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaVMEOutputSurfaceEditor_h
#define __albaVMEOutputSurfaceEditor_h

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaVMEOutputSurface.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkPolyData;

class ALBA_EXPORT albaVMEOutputSurfaceEditor : public albaVMEOutputSurface
{
public:
	albaVMEOutputSurfaceEditor();
	virtual ~albaVMEOutputSurfaceEditor();

	albaTypeMacro(albaVMEOutputSurfaceEditor,albaVMEOutputSurface);

	/**
	Return a VTK dataset corresponding to the current time. This is
	the output of the DataPipe currently attached to the VME.
	Usually the output is a  "smart copy" of one of the dataset in 
	the DataArray. In some cases it can be NULL, e.g. in case the number
	of stored Items is 0. Also special VME could not support VTK dataset output.
	An event is rised when the output data changes to allow attached classes to 
	update their input.*/
	virtual vtkPolyData *GetSurfaceData();

	/** Update all the output data structures (data, bounds, matrix and abs matrix).*/
	virtual void Update();

protected:
	albaGUI *CreateGui();

private:
	albaVMEOutputSurfaceEditor(const albaVMEOutputSurfaceEditor&); // Not implemented
	void operator=(const albaVMEOutputSurfaceEditor&); // Not implemented
};

#endif
