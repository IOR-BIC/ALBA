/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mmoBooleanSurface.h,v $
Language:  C++
Date:      $Date: 2007-03-30 08:29:56 $
Version:   $Revision: 1.2 $
Authors:   Daniele Giunchi - Matteo Giacomoni
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mmoBooleanSurface_H__
#define __mmoBooleanSurface_H__

#include "mafOp.h"
#include "mmgVMEChooserAccept.h"
#include "mafNode.h"
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
class mmiCompositorMouse;
class mmiGenericMouse;

//----------------------------------------------------------------------------
// mmoBooleanSurface :
//----------------------------------------------------------------------------
/** */
class mmoBooleanSurface: public mafOp
{
public:
	mmoBooleanSurface(const wxString &label = "FilterSurface");
	~mmoBooleanSurface(); 
	virtual void OnEvent(mafEventBase *maf_event);

	mafTypeMacro(mmoBooleanSurface, mafOp);

	class mafSurfaceAccept : public mmgVMEChooserAccept
	{
	public:

		mafSurfaceAccept() {};
		~mafSurfaceAccept() {};

		bool Validate(mafNode *node) {return(node != NULL && (node->IsMAFType(mafVMESurface) || node->IsMAFType(mafVMESurfaceParametric)));};
	};
	mafSurfaceAccept *m_SurfaceAccept;

	enum SURFACE_MODALITY
	{
		MODE_SURFACE = 0,
		MODE_IMPLICIT_FUNCTION
	};

	mafOp* Copy();

	/** Return true for the acceptable vme type. */
	bool Accept(mafNode *node);

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

	mmiCompositorMouse *m_IsaCompositor;
	mmiGenericMouse    *m_IsaTranslate;
	mmiGenericMouse    *m_IsaRotate;

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
