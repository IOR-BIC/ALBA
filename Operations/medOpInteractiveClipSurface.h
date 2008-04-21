/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medOpInteractiveClipSurface.h,v $
  Language:  C++
  Date:      $Date: 2008-04-21 16:52:36 $
  Version:   $Revision: 1.4 $
  Authors:   Paolo Quadrani , Stefano Perticoni 
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __medOpInteractiveClipSurface_H__
#define __medOpInteractiveClipSurface_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"
#include "mafNode.h"
#include "mafVMESurface.h"

#include "medVMEPolylineGraph.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafEvent;
class mmiCompositorMouse;
class mmiGenericMouse;
class mafVMESurface;
class mafVMEGizmo;
class mafVME;
class mafGizmoTranslate;
class mafGizmoRotate;
class mafGizmoScale;

class medCurvilinearAbscissaOnSkeletonHelper;

class vtkPlane;
class vtkPolyData;
class vtkClipPolyData;
class vtkGlyph3D;
class vtkSphereSource;
class vtkPlaneSource;
class vtkArrowSource;
class vtkAppendPolyData;
class vtkClipSurfaceBoundingBox;

//----------------------------------------------------------------------------
// medOpInteractiveClipSurface :
//----------------------------------------------------------------------------
/** */
class medOpInteractiveClipSurface: public mafOp
{
public:
  medOpInteractiveClipSurface(const wxString &label = "ClipSurface");
  ~medOpInteractiveClipSurface(); 
	virtual void OnEvent(mafEventBase *maf_event);

  mafTypeMacro(medOpInteractiveClipSurface, mafOp);

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
  
 	static bool SurfaceAccept(mafNode *node) {return(node != NULL && node->IsMAFType(mafVMESurface));};

  /** Set the modality to clip input surface. Available modality are MODE_SURFACE or MODE_IMPLICIT_FUNCTION*/
  void SetClippingModality(int mode = medOpInteractiveClipSurface::MODE_IMPLICIT_FUNCTION);

  /** Set the surface VME to use to clip the input surface. It is used only in MODE_SURFACE modality clip*/
  void SetClippingSurface(mafVMESurface *surface);

  /** Set the absolute position for the implicit plane used to clip the input surface*/
  void SetImplicitPlanePosition(mafMatrix &matrix);

  /** Function called to clip the input surface. m_ClipModality member variable, says if the surface will be clipped by 
  another surface or by an implicit function. */
  int Clip();

	/** Function that returns polydata results of clipping operation*/
	vtkPolyData *GetResultPolyData(){return m_ResultPolyData[m_ResultPolyData.size()-1];};

  static bool ConstrainAccept(mafNode* node) {return (node != NULL && \
    (node->IsMAFType(medVMEPolylineGraph))) ;};

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

  void GuiEnable();
	/** Change Gizmo visualization */
	void BuildTransformGizmos();

	/** Change type of gizmo in the view */
	void ChangeGizmo();

	/** Menage the undo inside the operation */
	void Undo();

	void OnEventThis(mafEventBase *maf_event);
	void OnEventGizmoTranslate(mafEventBase *maf_event);
	void OnEventGizmoRotate(mafEventBase *maf_event);
	void OnEventGizmoScale(mafEventBase *maf_event);
	void OnEventGizmoPlane(mafEventBase *maf_event);

  void OnChooseConstrainVme( mafNode *vme );

	void PostMultiplyEventMatrix(mafEventBase *maf_event);

	/** Clip Using vtkClipSurfaceBoundingBox */
	void ClipBoundingBox();
  
  mafVMESurface   *m_ClipperVME;
	mafVMESurface   *m_ClippedVME;

  mafVMEGizmo     *m_ImplicitPlaneVMEGizmo;
  vtkPlane        *m_ClipperPlane;
  vtkClipPolyData *m_Clipper;
	vtkClipSurfaceBoundingBox	*m_ClipperBoundingBox;
  vtkGlyph3D      *m_Arrow;

  mmiCompositorMouse *m_IsaCompositor;
  mmiGenericMouse    *m_IsaTranslate;
	mmiGenericMouse		 *m_IsaChangeArrowWithoutGizmo;
	mmiGenericMouse		 *m_IsaClipWithoutGizmo;
	mmiCompositorMouse *m_IsaCompositorWithArrowGizmo;
	mmiGenericMouse		 *m_IsaChangeArrowWithGizmo;
	mmiGenericMouse		 *m_IsaClipWithGizmo;

  vtkPolyData *m_OldSurface;
	std::vector<vtkPolyData*> m_ResultPolyData;

  int		m_ClipModality;
	int		m_GizmoType;
  int		ClipInside;
	int   m_UseGizmo;
	int		m_ClipBoundBox;
  bool	PlaneCreated;

	double m_PlaneWidth;
	double m_PlaneHeight;

	vtkPlaneSource	*m_PlaneSource;
	vtkArrowSource	*m_ArrowSource;
	vtkAppendPolyData	*m_AppendPolydata;
  vtkSphereSource *m_SphereSource;

  mafGizmoTranslate		*m_GizmoTranslate;
	mafGizmoRotate			*m_GizmoRotate;
	mafGizmoScale				*m_GizmoScale;

  medCurvilinearAbscissaOnSkeletonHelper *m_CASH;
  
  mafVME *m_ConstrainMedVMEPolylineGraph;

  int m_ActiveBranchId;

};
#endif
