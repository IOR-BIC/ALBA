/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medVMEPolylineEditor..h,v $
Language:  C++
Date:      $Date: 2007-07-04 09:44:00 $
Version:   $Revision: 1.1 $
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
#ifndef __medVMEPolylineEditor_h
#define __medVMEPolylineEditor_h

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafVMEGeneric.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------

class MAF_EXPORT medVMEPolylineEditor : public mafVMEGeneric
{
public:
	mafTypeMacro(medVMEPolylineEditor, mafVMEGeneric);

	/** 
	Set data for the give timestamp. This function automatically creates a
	a VMEItem for the data to be stored. The optional parameter specify if
	data must be copied, referenced, or detached from original source. Default
	is to copy data which creates a new data set copying the original one.
	Referencing means the data is kept connected to its original source, and
	detaching means disconnect is like referencing but also disconnect the data
	from the source.
	Return MAF_OK if succeeded, MAF_ERROR if they kind of data is not accepted by
	this type of VME. */
	virtual int SetData(vtkDataSet *data, mafTimeStamp t, int mode=MAF_VME_COPY_DATA);

	/** Return the suggested pipe-typename for the visualization of this vme */
	virtual mafString GetVisualPipe() {return m_VisualPipeName;};

	/** return the right type of output */  
	virtual mafVMEOutput *GetOutput();

	/** Set the suggested pipe-typename for the visualization of this vme */
	void SetVisualPipe(mafString vpipe){m_VisualPipeName = vpipe;};

protected:
	medVMEPolylineEditor();
	virtual ~medVMEPolylineEditor(); 

	mafString m_VisualPipeName;

private:
	medVMEPolylineEditor(const medVMEPolylineEditor&); // Not implemented
	void operator=(const medVMEPolylineEditor&); // Not implemented
};
#endif
