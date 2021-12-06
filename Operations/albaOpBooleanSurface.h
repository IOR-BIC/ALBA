/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpBooleanSurface
 Authors: Daniele Giunchi - Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpBooleanSurface_H__
#define __albaOpBooleanSurface_H__

#include "albaOp.h"
#include "albaVME.h"
#include "albaVMESurface.h"
#include "albaVMESurfaceParametric.h"


//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

class vtkPolyData;
class vtkPlane;
class vtkGlyph3D;
class vtkPlaneSource;
class albaVMEGizmo;
class albaInteractorCompositorMouse;
class albaInteractorGenericMouse;

#ifdef ALBA_EXPORTS
#include "albaDllMacros.h"
EXPORT_STL_VECTOR(ALBA_EXPORT,vtkPolyData*);
#endif

//----------------------------------------------------------------------------
// albaOpBooleanSurface :
//----------------------------------------------------------------------------
/** */
class ALBA_EXPORT albaOpBooleanSurface: public albaOp
{
public:
	albaOpBooleanSurface(const wxString &label = "FilterSurface");
	~albaOpBooleanSurface(); 
	virtual void OnEvent(albaEventBase *alba_event);

	albaTypeMacro(albaOpBooleanSurface, albaOp);

	static bool SurfaceAccept(albaVME*node) {return(node != NULL && (node->IsALBAType(albaVMESurface) || node->IsALBAType(albaVMESurfaceParametric)));};

	enum SURFACE_MODALITY
	{
		MODE_SURFACE = 0,
		MODE_IMPLICIT_FUNCTION
	};

	albaOp* Copy();

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

	void SetFactor1(albaVMESurface *surface){m_FirstOperatorVME=surface;};
	void SetFactor2(albaVMESurface *surface){m_SecondOperatorVME=surface;};

protected:

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	void OpStop(int result);

	void VmeChoose(albaString title,albaEvent *e);
	void ShowClipPlane(bool show);
	void AttachInteraction();
	void UpdateISARefSys();
	void Clip();

	vtkPlane				*m_ClipperPlane;
	vtkGlyph3D			*m_Arrow;
	vtkPlaneSource	*m_PlaneSource;
	albaVMEGizmo			*m_ImplicitPlaneGizmo;
	bool						m_PlaneCreated;

	albaInteractorCompositorMouse *m_IsaCompositor;
	albaInteractorGenericMouse    *m_IsaTranslate;
	albaInteractorGenericMouse    *m_IsaRotate;

	albaVMESurface *m_SecondOperatorVME;
	albaVMESurface *m_FirstOperatorVME;

  albaVMESurface *m_ResultVME; //<the vme result of operation

  albaVMESurface *m_SecondOperatorFromParametric;

	std::vector<vtkPolyData*> m_VTKResult;

	int m_Modality;
	int m_ClipInside;
	int m_Subdivision;

};
#endif
