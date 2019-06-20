/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEOutputPolylineEditor
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaVMEOutputPolylineEditor_h
#define __albaVMEOutputPolylineEditor_h

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaVMEOutputVTK.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkPolyData;

class ALBA_EXPORT albaVMEOutputPolylineEditor : public albaVMEOutputVTK
{
public:
	albaVMEOutputPolylineEditor();
	virtual ~albaVMEOutputPolylineEditor();

	albaTypeMacro(albaVMEOutputPolylineEditor,albaVMEOutputVTK);

	/**
	Return a VTK dataset corresponding to the current time. This is
	the output of the DataPipe currently attached to the VME.
	Usually the output is a  "smart copy" of one of the dataset in 
	the DataArray. In some cases it can be NULL, e.g. in case the number
	of stored Items is 0. Also special VME could not support VTK dataset output.
	An event is rised when the output data changes to allow attached classes to 
	update their input.*/
	virtual vtkPolyData *GetPolylineData();

	/** Update all the output data structures (data, bounds, matrix and abs matrix).*/
	virtual void Update();

protected:
	albaGUI *CreateGui();

private:
	albaVMEOutputPolylineEditor(const albaVMEOutputPolylineEditor&); // Not implemented
	void operator=(const albaVMEOutputPolylineEditor&); // Not implemented
};

#endif
