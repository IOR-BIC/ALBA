/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpLabelizeSurface
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpLabelizeSurface_H__
#define __albaOpLabelizeSurface_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVMEGizmo;
class albaGizmoTranslate;
class albaGizmoRotate;
class albaGizmoScale;
class albaVMESurface;
class albaVMESurfaceEditor;
class albaInteractorCompositorMouse;
class albaInteractorGenericMouse;
class vtkPlane;
class vtkPlaneSource;
class vtkArrowSource;
class vtkAppendPolyData;
class vtkGlyph3D;
class vtkALBAClipSurfaceBoundingBox;
class vtkPolyData;
class vtkLookupTable;

//----------------------------------------------------------------------------
// albaOpLabelizeSurface :
//----------------------------------------------------------------------------
/** */
class ALBA_EXPORT albaOpLabelizeSurface: public albaOp
{
public:
	albaOpLabelizeSurface(const wxString &label = "Labelize Surface");
	~albaOpLabelizeSurface(); 
	virtual void OnEvent(albaEventBase *alba_event);

	albaTypeMacro(albaOpLabelizeSurface, albaOp);

	albaOp* Copy();

	void OpRun();
	void OpDo();
	void OpUndo();

	enum GIZMO_TYPE
	{
		GIZMO_TRANSLATE = 0,
		GIZMO_ROTATE,
		GIZMO_SCALE,
	};

	void SetLutEditor(vtkLookupTable *lut);
	void SetLabelValue(double val){m_LabelValue=val;};
	void SetPlaneDimension(double w,double h);
	void Labelize();

	virtual void OpStop(int result);
protected: 

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

	/** Create the GUI */
	void CreateGui();

	void ShowClipPlane(bool show);
	void CreateGizmos();
	void AttachInteraction();
	void UpdateISARefSys();
	void Undo();

	/** Change type of gizmo in the view */
	void ChangeGizmo();

	void OnEventGizmoPlane(albaEventBase *alba_event);
	void OnEventThis(albaEventBase *alba_event);
	void OnEventGizmoTranslate(albaEventBase *alba_event);
	void OnEventGizmoRotate(albaEventBase *alba_event);
	void OnEventGizmoScale(albaEventBase *alba_event);

	void PostMultiplyEventMatrix(albaEventBase *alba_event);

	void SetPlaneDimension();

	albaInteractorCompositorMouse *m_IsaCompositorWithoutGizmo;
	albaInteractorCompositorMouse *m_IsaCompositorWithGizmo;
	albaInteractorGenericMouse    *m_IsaTranslate;
	albaInteractorGenericMouse    *m_IsaRotate;
	albaInteractorGenericMouse		 *m_IsaChangeArrowWithGizmo;
	albaInteractorGenericMouse		 *m_IsaChangeArrowWithoutGizmo;	
	albaInteractorGenericMouse		 *m_IsaLabelizeWithGizmo;
	albaInteractorGenericMouse		 *m_IsaLabelizeWithoutGizmo;

	int		m_LabelInside;

	bool	m_PlaneCreated;

	double m_PlaneWidth;
	double m_PlaneHeight;
	double m_LabelValue;

	int		m_GizmoType;
	int   m_UseGizmo;

	albaVMESurface				*m_InputSurface;
	albaVMESurfaceEditor *m_VmeEditor;

	albaVMEGizmo				*m_ImplicitPlaneGizmo;
	vtkPlane					*m_ClipperPlane;
	vtkPlaneSource		*m_PlaneSource;
	vtkArrowSource		*m_ArrowShape;
	vtkAppendPolyData	*m_Gizmo;
	vtkGlyph3D				*m_Arrow;

	vtkALBAClipSurfaceBoundingBox	*m_ClipperBoundingBox;

	std::vector<vtkPolyData*> m_ResultPolyData;
	vtkPolyData	*m_OriginalPolydata;

	albaGizmoTranslate		*m_GizmoTranslate;
	albaGizmoRotate			*m_GizmoRotate;
	albaGizmoScale				*m_GizmoScale;
};
#endif
