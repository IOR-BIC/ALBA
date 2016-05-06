/*=========================================================================

 Program: MAF2
 Module: mafOpBooleanSurface
 Authors: Daniele Giunchi - Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpBooleanSurface_H__
#define __mafOpBooleanSurface_H__

#include "mafOp.h"
#include "mafVME.h"
#include "mafVMESurface.h"
#include "mafVMESurfaceParametric.h"


//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

class vtkPolyData;
class vtkPlane;
class vtkGlyph3D;
class vtkPlaneSource;
class mafVMEGizmo;
class mafInteractorCompositorMouse;
class mafInteractorGenericMouse;

#ifdef MAF_EXPORTS
#include "mafDllMacros.h"
EXPORT_STL_VECTOR(MAF_EXPORT,vtkPolyData*);
#endif

//----------------------------------------------------------------------------
// mafOpBooleanSurface :
//----------------------------------------------------------------------------
/** */
class MAF_EXPORT mafOpBooleanSurface: public mafOp
{
public:
	mafOpBooleanSurface(const wxString &label = "FilterSurface");
	~mafOpBooleanSurface(); 
	virtual void OnEvent(mafEventBase *maf_event);

	mafTypeMacro(mafOpBooleanSurface, mafOp);

	static bool SurfaceAccept(mafVME*node) {return(node != NULL && (node->IsMAFType(mafVMESurface) || node->IsMAFType(mafVMESurfaceParametric)));};

	enum SURFACE_MODALITY
	{
		MODE_SURFACE = 0,
		MODE_IMPLICIT_FUNCTION
	};

	mafOp* Copy();

	/** Return true for the acceptable vme type. */
	bool Accept(mafVME*node);

	/** Builds operation's interface. */
	void OpRun();

	/** Execute the operation. */
	void OpDo();

	/** Makes the undo for the operation. */
	void OpUndo();

	void Union();
	void Intersection();
	void Difference();
	void Undo();

	void SetFactor1(mafVMESurface *surface){m_FirstOperatorVME=surface;};
	void SetFactor2(mafVMESurface *surface){m_SecondOperatorVME=surface;};

protected:
	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	void OpStop(int result);

	void VmeChoose(mafString title,mafEvent *e);
	void ShowClipPlane(bool show);
	void AttachInteraction();
	void UpdateISARefSys();
	void Clip();

	vtkPlane				*m_ClipperPlane;
	vtkGlyph3D			*m_Arrow;
	vtkPlaneSource	*m_PlaneSource;
	mafVMEGizmo			*m_ImplicitPlaneGizmo;
	bool						m_PlaneCreated;

	mafInteractorCompositorMouse *m_IsaCompositor;
	mafInteractorGenericMouse    *m_IsaTranslate;
	mafInteractorGenericMouse    *m_IsaRotate;

	mafVMESurface *m_SecondOperatorVME;
	mafVMESurface *m_FirstOperatorVME;

  mafVMESurface *m_ResultVME; //<the vme result of operation

  mafVMESurface *m_SecondOperatorFromParametric;

	std::vector<vtkPolyData*> m_VTKResult;

	int m_Modality;
	int m_ClipInside;
	int m_Subdivision;

};
#endif
