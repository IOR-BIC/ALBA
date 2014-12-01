/*=========================================================================

 Program: MAF2
 Module: medOpLabelizeSurface
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __medOpLabelizeSurface_H__
#define __medOpLabelizeSurface_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVMEGizmo;
class mafGizmoTranslate;
class mafGizmoRotate;
class mafGizmoScale;
class mafVMESurface;
class medVMESurfaceEditor;
class mafInteractorCompositorMouse;
class mafInteractorGenericMouse;
class vtkPlane;
class vtkPlaneSource;
class vtkArrowSource;
class vtkAppendPolyData;
class vtkGlyph3D;
class vtkMAFClipSurfaceBoundingBox;
class vtkPolyData;
class vtkLookupTable;

//----------------------------------------------------------------------------
// medOpLabelizeSurface :
//----------------------------------------------------------------------------
/** */
class MAF_EXPORT medOpLabelizeSurface: public mafOp
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

	/** Create the GUI */
	void CreateGui();

	void ShowClipPlane(bool show);
	void CreateGizmos();
	void AttachInteraction();
	void UpdateISARefSys();
	void Undo();

	/** Change type of gizmo in the view */
	void ChangeGizmo();

	void OnEventGizmoPlane(mafEventBase *maf_event);
	void OnEventThis(mafEventBase *maf_event);
	void OnEventGizmoTranslate(mafEventBase *maf_event);
	void OnEventGizmoRotate(mafEventBase *maf_event);
	void OnEventGizmoScale(mafEventBase *maf_event);

	void PostMultiplyEventMatrix(mafEventBase *maf_event);

	void SetPlaneDimension();

	mafInteractorCompositorMouse *m_IsaCompositorWithoutGizmo;
	mafInteractorCompositorMouse *m_IsaCompositorWithGizmo;
	mafInteractorGenericMouse    *m_IsaTranslate;
	mafInteractorGenericMouse    *m_IsaRotate;
	mafInteractorGenericMouse		 *m_IsaChangeArrowWithGizmo;
	mafInteractorGenericMouse		 *m_IsaChangeArrowWithoutGizmo;	
	mafInteractorGenericMouse		 *m_IsaLabelizeWithGizmo;
	mafInteractorGenericMouse		 *m_IsaLabelizeWithoutGizmo;

	int		m_LabelInside;

	bool	m_PlaneCreated;

	double m_PlaneWidth;
	double m_PlaneHeight;
	double m_LabelValue;

	int		m_GizmoType;
	int   m_UseGizmo;

	mafVMESurface				*m_InputSurface;
	medVMESurfaceEditor *m_VmeEditor;

	mafVMEGizmo				*m_ImplicitPlaneGizmo;
	vtkPlane					*m_ClipperPlane;
	vtkPlaneSource		*m_PlaneSource;
	vtkArrowSource		*m_ArrowShape;
	vtkAppendPolyData	*m_Gizmo;
	vtkGlyph3D				*m_Arrow;

	vtkMAFClipSurfaceBoundingBox	*m_ClipperBoundingBox;

	std::vector<vtkPolyData*> m_ResultPolyData;
	vtkPolyData	*m_OriginalPolydata;

	mafGizmoTranslate		*m_GizmoTranslate;
	mafGizmoRotate			*m_GizmoRotate;
	mafGizmoScale				*m_GizmoScale;
};
#endif
