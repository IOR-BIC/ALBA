/*=========================================================================

 Program: MAF2
 Module: mafOpTransform
 Authors: Nicola Vanella
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpTransform_H__
#define __mafOpTransform_H__

#include "mafDefines.h"
#include "mafOpTransformInterface.h"


//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

class mafVME;
class mafGui;
class mafEvent;
class mafGizmoTranslate;
class mafGizmoRotate;
class mafGizmoScale;
class mafVMEPolyline;
class mafVMESurface;

//----------------------------------------------------------------------------
// mafOpTransform :
//----------------------------------------------------------------------------
/** */

class MAF_EXPORT mafOpTransform: public mafOpTransformInterface
{
public:
	mafOpTransform(const wxString &label = "Transform  \tCtrl+T");
  ~mafOpTransform();

  mafTypeMacro(mafOpTransform, mafOp);

	virtual void OnEvent(mafEventBase *maf_event);

  mafOp* Copy();

	/** Return true for the acceptable vme type. */
	bool Accept(mafVME* vme);

	/** Builds operation's interface. */
	void OpRun();

	/** Execute the operation. */
	void OpDo();

	/** Makes the undo for the operation. */
	void OpUndo();

	void Reset();

protected: 
	/** Create the GUI */
	virtual void CreateGui();

	void OnEventTransformGizmo(mafEventBase *maf_event);
	void OnEventTransformTextEntries(mafEventBase *maf_event);

	void OpStop(int result);

	void SelectRefSys();

	void UpdateAndSetLocalCentroidRefSys();

	void ChooseRelativeRefSys();

	void UpdateTransformTextEntries();

	/** Accept All VME excluding current */
	static bool AcceptRefSys(mafVME *node);

	// Plugged objects
	mafGizmoTranslate           *m_GizmoTranslate;
	mafGizmoRotate              *m_GizmoRotate;
	mafGizmoScale               *m_GizmoScale;

	int m_UpdateAfterRelease;
	int m_RefSystemMode;

	mafVMEPolyline *m_TransformVME;
	mafVMEPolyline *m_LocalRefSysVME;
	mafVMEPolyline *m_LocalCenterRefSysVME;
	mafVMEPolyline *m_RelativeRefSysVME;
	mafVMEPolyline *m_RelativeCenterRefSysVME;
	mafVMEPolyline *m_ArbitraryRefSysVME;

	double m_Position[3];
	double m_Orientation[3];
	double m_Scaling[3];

	double m_OriginRefSysPosition[3];
	double m_OriginRefSysOrientation[3];

	// Override superclass
	void RefSysVmeChanged();

	/** Postmultiply event matrix to vme abs matrix; also update Redo ivar m_NewAbsMatrix */;
	virtual void PostMultiplyEventMatrix(mafEventBase *maf_event);
};
#endif
