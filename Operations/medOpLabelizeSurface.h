/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medOpLabelizeSurface.h,v $
Language:  C++
Date:      $Date: 2007-08-15 20:14:29 $
Version:   $Revision: 1.1 $
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

#ifndef __medOpLabelizeSurface_H__
#define __medOpLabelizeSurface_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVMEGizmo;
class mafVMESurface;
class medVMESurfaceEditor;
class mmiCompositorMouse;
class mmiGenericMouse;
class vtkPlane;
class vtkPlaneSource;
class vtkArrowSource;
class vtkAppendPolyData;
class vtkGlyph3D;
class vtkClipSurfaceBoundingBox;
class vtkPolyData;

//----------------------------------------------------------------------------
// medOpLabelizeSurface :
//----------------------------------------------------------------------------
/** */
class medOpLabelizeSurface: public mafOp
{
public:
	medOpLabelizeSurface(const wxString &label = "ClipSurface");
	~medOpLabelizeSurface(); 
	virtual void OnEvent(mafEventBase *maf_event);

	mafTypeMacro(medOpLabelizeSurface, mafOp);

	mafOp* Copy();

	bool Accept(mafNode *node);   
	void OpRun();
	void OpDo();
	void OpUndo();

protected: 
	virtual void OpStop(int result);

	/** Create the GUI */
	void CreateGui();

	void ShowClipPlane(bool show);
	void CreateGizmos();
	void AttachInteraction();
	void UpdateISARefSys();
	void Labelize();
	void Undo();

	void OnEventGizmoPlane(mafEventBase *maf_event);
	void OnEventThis(mafEventBase *maf_event);

	mmiCompositorMouse *m_IsaCompositor;
	mmiGenericMouse    *m_IsaTranslate;
	mmiGenericMouse    *m_IsaRotate;
	mmiGenericMouse		 *m_IsaChangeArrow;
	mmiGenericMouse		 *m_IsaClip;

	int		m_ClipInside;

	bool	PlaneCreated;

	double m_PlaneWidth;
	double m_PlaneHeight;

	double m_LabelValue;

	mafVMESurface				*m_InputSurface;
	medVMESurfaceEditor *m_VmeEditor;

	mafVMEGizmo				*m_ImplicitPlaneGizmo;
	vtkPlane					*m_ClipperPlane;
	vtkPlaneSource		*m_PlaneSource;
	vtkArrowSource		*m_ArrowShape;
	vtkAppendPolyData	*m_Gizmo;
	vtkGlyph3D				*m_Arrow;

	vtkClipSurfaceBoundingBox	*m_ClipperBoundingBox;

	std::vector<vtkPolyData*> m_ResultPolyData;
	vtkPolyData	*m_OriginalPolydata;
};
#endif
