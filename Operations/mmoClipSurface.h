/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoClipSurface.h,v $
  Language:  C++
  Date:      $Date: 2007-05-17 15:58:55 $
  Version:   $Revision: 1.6 $
  Authors:   Paolo Quadrani    
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mmoClipSurface_H__
#define __mmoClipSurface_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"
#include "mmgVMEChooserAccept.h"
#include "mafNode.h"
#include "mafVMESurface.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafEvent;

class mmiCompositorMouse;
class mmiGenericMouse;
class mafVMESurface;
class mafVMEGizmo;
class mafGizmoTranslate;
class mafGizmoRotate;
class mafGizmoScale;

class vtkPlane;
class vtkPolyData;
class vtkClipPolyData;
class vtkGlyph3D;
class vtkPlaneSource;
class vtkArrowSource;
class vtkAppendPolyData;

//----------------------------------------------------------------------------
// mmoClipSurface :
//----------------------------------------------------------------------------
/** */
class mmoClipSurface: public mafOp
{
public:
  mmoClipSurface(const wxString &label = "ClipSurface");
  ~mmoClipSurface(); 
	virtual void OnEvent(mafEventBase *maf_event);

  mafTypeMacro(mmoClipSurface, mafOp);

  mafOp* Copy();

  bool Accept(mafNode *node);   
  void OpRun();
  void OpDo();
  void OpUndo();

  enum CLIP_SURFACE_MODALITY
  {
    MODE_SURFACE = 0,
    MODE_IMPLICIT_FUNCTION
  };

	enum GIZMO_TYPE
	{
		GIZMO_TRANSLATE = 0,
		GIZMO_ROTATE,
		GIZMO_SCALE,
	};
  
 	class mafSurfaceAccept : public mmgVMEChooserAccept
	{
		public:
			
			mafSurfaceAccept() {};
		 ~mafSurfaceAccept() {};

		bool Validate(mafNode *node) {return(node != NULL && node->IsMAFType(mafVMESurface));};
	};
  mafSurfaceAccept *m_SurfaceAccept;

  /** Set the modality to clip input surface. Available modality are MODE_SURFACE or MODE_IMPLICIT_FUNCTION*/
  void SetClippingModality(int mode = mmoClipSurface::MODE_IMPLICIT_FUNCTION);

  /** Set the surface VME to use to clip the input surface. It is used only in MODE_SURFACE modality clip*/
  void SetClippingSurface(mafVMESurface *surface);

  /** Set the absolute position for the implicit plane used to clip the input surface*/
  void SetImplicitPlanePosition(mafMatrix &matrix);

  /** Function called to clip the input surface. m_ClipModality member variable, says if the surface will be clipped by 
  another surface or by an implicit function. */
  int Clip();

	/** Function that returns polydata results of clipping operation*/
	vtkPolyData *GetResultPolyData(){return m_ResultPolyData;};
	vtkPolyData *GetClippedPolyData(){return m_ClippedPolyData;};

protected: 
	virtual void OpStop(int result);
	
  /** Show/hide the gizmo representing the clipping implicit plane. */
  void ShowClipPlane(bool show);

	/** Show/hide the gizmo for scaling implicit plane. */
	void ShowGizmoScale(bool show);

  /** Attach the interaction to the implicit plane. */
  void AttachInteraction();

  /** Update interactor ref sys*/
  void UpdateISARefSys();

	/** Create the GUI */
	void CreateGui();

	void CreateGizmos();

	/** Change type of gizmo in the view */
	void ChangeGizmo();

	void OnEventThis(mafEventBase *maf_event);
	void OnEventGizmoTranslate(mafEventBase *maf_event);
	void OnEventGizmoRotate(mafEventBase *maf_event);
	void OnEventGizmoScale(mafEventBase *maf_event);
	void OnEventGizmoPlane(mafEventBase *maf_event);

	void PostMultiplyEventMatrix(mafEventBase *maf_event);

  mafVMESurface   *m_ClipperVME;
	mafVMESurface   *m_ClippedVME;

  mafVMEGizmo     *m_ImplicitPlaneGizmo;
  vtkPlane        *m_ClipperPlane;
  vtkClipPolyData *m_Clipper;
  vtkGlyph3D      *m_Arrow;

  mmiCompositorMouse *m_IsaCompositor;
  mmiGenericMouse    *m_IsaTranslate;
  mmiGenericMouse    *m_IsaRotate;

  vtkPolyData *m_OldSurface;
	vtkPolyData *m_ResultPolyData;
	vtkPolyData *m_ClippedPolyData;

  int		m_ClipModality;
	int		m_GizmoType;
  int		ClipInside;
	int   m_UseGizmo;
	int		m_GenerateClippedOutput;
  bool	PlaneCreated;

	double m_PlaneWidth;
	double m_PlaneHeight;

	vtkPlaneSource	*m_PlaneSource;
	vtkArrowSource	*m_ArrowShape;
	vtkAppendPolyData	*m_Gizmo;

	mafGizmoTranslate		*m_GizmoTranslate;
	mafGizmoRotate			*m_GizmoRotate;
	mafGizmoScale				*m_GizmoScale;
};
#endif
