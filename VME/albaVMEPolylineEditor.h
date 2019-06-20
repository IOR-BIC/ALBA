/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEPolylineEditor
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaVMEPolylineEditor_h
#define __albaVMEPolylineEditor_h

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaVMEGeneric.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------

class ALBA_EXPORT albaVMEPolylineEditor : public albaVMEGeneric
{
public:
	albaTypeMacro(albaVMEPolylineEditor, albaVMEGeneric);

	/** 
	Set data for the give timestamp. This function automatically creates a
	a VMEItem for the data to be stored. The optional parameter specify if
	data must be copied, referenced, or detached from original source. Default
	is to copy data which creates a new data set copying the original one.
	Referencing means the data is kept connected to its original source, and
	detaching means disconnect is like referencing but also disconnect the data
	from the source.
	Return ALBA_OK if succeeded, ALBA_ERROR if they kind of data is not accepted by
	this type of VME. */
	virtual int SetData(vtkDataSet *data, albaTimeStamp t, int mode=ALBA_VME_COPY_DATA);

	/** Return the suggested pipe-typename for the visualization of this vme */
	virtual albaString GetVisualPipe() {return m_VisualPipeName;};

	/** return the right type of output */  
	virtual albaVMEOutput *GetOutput();

	/** Set the suggested pipe-typename for the visualization of this vme */
	void SetVisualPipe(albaString vpipe){m_VisualPipeName = vpipe;};

protected:
	albaVMEPolylineEditor();
	virtual ~albaVMEPolylineEditor(); 

	albaString m_VisualPipeName;

private:
	albaVMEPolylineEditor(const albaVMEPolylineEditor&); // Not implemented
	void operator=(const albaVMEPolylineEditor&); // Not implemented
};
#endif
