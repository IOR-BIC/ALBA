/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoClipSurface.h,v $
  Language:  C++
  Date:      $Date: 2005-10-13 13:42:22 $
  Version:   $Revision: 1.1 $
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

class vtkPlane;
class vtkPolyData;
class vtkClipPolyData;
class vtkGlyph3D;

//----------------------------------------------------------------------------
// mmoClipSurface :
//----------------------------------------------------------------------------
/** */
class mmoClipSurface: public mafOp
{
public:
  mmoClipSurface(wxString label);
 ~mmoClipSurface(); 
	virtual void OnEvent(mafEventBase *maf_event);

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

protected: 
	virtual void OpStop(int result);
	
  /** Show/hide the gizmo representing the clipping implicit plane. */
  void ShowClipPlane(bool show);

  /** Attach the interaction to the implicit plane. */
  void AttachInteraction();

  /** Update interactor ref sys*/
  void UpdateISARefSys();

  mafVMESurface   *m_ClipperVME;
  mafVMEGizmo     *m_ImplicitPlaneGizmo;
  vtkPlane        *m_ClipperPlane;
  vtkClipPolyData *m_Clipper;
  vtkGlyph3D      *m_Arrow;

  mmiCompositorMouse *m_IsaCompositor;
  mmiGenericMouse    *m_IsaTranslate;
  mmiGenericMouse    *m_IsaRotate;

  vtkPolyData *m_OldSurface;

  int  m_ClipModality;
  int  ClipInside;
  bool PlaneCreated;
};
#endif
