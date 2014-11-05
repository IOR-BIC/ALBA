/*=========================================================================

 Program: MAF2Medical
 Module: medVMEOutputPolylineEditor
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __medVMEOutputPolylineEditor_h
#define __medVMEOutputPolylineEditor_h

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "medVMEDefines.h"
#include "mafVMEOutputVTK.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkPolyData;

class MED_VME_EXPORT medVMEOutputPolylineEditor : public mafVMEOutputVTK
{
public:
	medVMEOutputPolylineEditor();
	virtual ~medVMEOutputPolylineEditor();

	mafTypeMacro(medVMEOutputPolylineEditor,mafVMEOutputVTK);

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
	mafGUI *CreateGui();

private:
	medVMEOutputPolylineEditor(const medVMEOutputPolylineEditor&); // Not implemented
	void operator=(const medVMEOutputPolylineEditor&); // Not implemented
};

#endif
