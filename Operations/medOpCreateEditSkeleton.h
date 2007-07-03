/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medOpCreateEditSkeleton.h,v $
Language:  C++
Date:      $Date: 2007-07-03 10:59:28 $
Version:   $Revision: 1.2 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2002/2007
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

#ifndef __medOpCreateEditSkeleton_H__
#define __medOpCreateEditSkeleton_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVME;
class medVMEPolylineGraph;
class medGeometryEditorPolylineGraph;
class vtkPolyData;

//----------------------------------------------------------------------------
// medOpCreateEditSkeleton :
//----------------------------------------------------------------------------
class medOpCreateEditSkeleton: public mafOp
{
public:

	medOpCreateEditSkeleton(wxString label = "Create/Edit Skeleton");
	~medOpCreateEditSkeleton(); 

	mafTypeMacro(medOpCreateEditSkeleton, mafOp);

	mafOp* Copy();

	void OnEvent(mafEventBase *maf_event);

	/** Return true for the acceptable vme type. */
	bool Accept(mafNode* vme);

	/** Builds operation's interface by calling CreateOpDialog() method. */
	void OpRun();

	/** Execute the operation. */
	void OpDo();

	/** Makes the undo for the operation. */
	void OpUndo();

protected:
	
	virtual void OpStop(int result);

	void CreateGui();

	medVMEPolylineGraph	*m_Skeleton;
	vtkPolyData					*m_ResultPolydata;

	medGeometryEditorPolylineGraph *m_Editor;

	int m_Modality;

};
#endif
