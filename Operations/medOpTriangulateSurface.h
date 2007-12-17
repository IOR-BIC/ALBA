/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medOpTriangulateSurface.h,v $
Language:  C++
Date:      $Date: 2007-12-17 19:07:40 $
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

#ifndef __medOpTriangulateSurface_H__
#define __medOpTriangulateSurface_H__

#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class vtkPolyData;
class mafEvent;

//----------------------------------------------------------------------------
// medOpSmoothSurface :
//----------------------------------------------------------------------------
/** */
class medOpTriangulateSurface: public mafOp
{
public:
	medOpTriangulateSurface(const wxString &label = "FilterSurface");
	~medOpTriangulateSurface(); 
	virtual void OnEvent(mafEventBase *maf_event);

	mafTypeMacro(medOpTriangulateSurface, mafOp);

	mafOp* Copy();

	/** Return true for the acceptable vme type. */
	bool Accept(mafNode *node);

	/** Builds operation's interface. */
	void OpRun();

	/** Execute the operation. */
	void OpDo();

	/** Makes the undo for the operation. */
	void OpUndo();

  /** Triangulate the surface. */
  void OnTriangle();

protected:
	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	void OpStop(int result);

  void CreateGui();

	bool m_ClearInterfaceFlag;
	bool m_PreviewResultFlag;

	vtkPolyData											*m_ResultPolydata;
	vtkPolyData											*m_OriginalPolydata;


	/** Make the preview of the surface filtering. */
	void OnPreview();  

	/** Clear all the surface applied filtering. */
	void OnClear();
};
#endif
