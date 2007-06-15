/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mmoCropDeformableROI.h,v $
Language:  C++
Date:      $Date: 2007-06-15 14:17:50 $
Version:   $Revision: 1.4 $
Authors:   Matteo Giacomoni - Daniele Giunchi
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

#ifndef __mmoCropDeformableROI_H__
#define __mmoCropDeformableROI_H__

#include "mafOp.h"
#include "mafVME.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

class vtkMaskPolyDataFilter;
class mafVMEVolumeGray;
class mafVMESurface;

//----------------------------------------------------------------------------
// mmoCropDeformableROI :
//----------------------------------------------------------------------------
/** */
class mmoCropDeformableROI: public mafOp
{
public:
	mmoCropDeformableROI(const wxString &label = "CropDeformableROI");
	~mmoCropDeformableROI(); 

	virtual void OnEvent(mafEventBase *maf_event);

	mafTypeMacro(mmoCropDeformableROI, mafOp);

	static bool OutputSurfaceAccept(mafNode *node) {return(node != NULL && ((mafVME*)node)->GetOutput()->IsA("mafVMEOutputSurface"));};

	mafOp* Copy();

	/** Return true for the acceptable vme type. */
	bool Accept(mafNode *node);

	/** Builds operation's interface. */
	void OpRun();

	/** Execute the operation. */
	void OpDo();

	/** Makes the undo for the operation. */
	void OpUndo();

protected:
	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	void OpStop(int result);

	void Algorithm(mafVME *vme);

	vtkMaskPolyDataFilter *m_MaskPolydataFilter;

	double m_Distance;
	double m_FillValue;
	int		m_InsideOut;
	double	m_MaxDistance;
	mafNode *m_pNode;

	mafVMEVolumeGray *m_ResultVme;
  mafVMESurface    *m_Surface;

};
#endif
