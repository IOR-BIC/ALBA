/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medVMEOutputSurfaceEditor.h,v $
Language:  C++
Date:      $Date: 2007-08-30 08:46:19 $
Version:   $Revision: 1.2 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)

MafMedical Library use license agreement

The software named MafMedical Library and any accompanying documentation, 
manuals or data (hereafter collectively "SOFTWARE") is property of the SCS s.r.l.
This is an open-source copyright as follows:
Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, 
this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation and/or 
other materials provided with the distribution.
* Modified source versions must be plainly marked as such, and must not be misrepresented 
as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

MafMedical is partially based on OpenMAF.
=========================================================================*/
#ifndef __medVMEOutputSurfaceEditor_h
#define __medVMEOutputSurfaceEditor_h

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafVMEOutputSurface.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkPolyData;

class MAF_EXPORT medVMEOutputSurfaceEditor : public mafVMEOutputSurface
{
public:
	medVMEOutputSurfaceEditor();
	virtual ~medVMEOutputSurfaceEditor();

	mafTypeMacro(medVMEOutputSurfaceEditor,mafVMEOutputSurface);

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
	mmgGui *CreateGui();

private:
	medVMEOutputSurfaceEditor(const medVMEOutputSurfaceEditor&); // Not implemented
	void operator=(const medVMEOutputSurfaceEditor&); // Not implemented
};

#endif
