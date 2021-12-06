/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpTransform
 Authors: Nicola Vanella
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpTransform_H__
#define __albaOpTransform_H__

#include "albaDefines.h"
#include "albaOpTransformInterface.h"


//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

class albaVME;
class albaGui;
class albaEvent;
class albaGizmoTranslate;
class albaGizmoRotate;
class albaGizmoScale;
class albaVMEPolyline;
class albaVMESurface;

//----------------------------------------------------------------------------
// albaOpTransform :
//----------------------------------------------------------------------------
/** */

class ALBA_EXPORT albaOpTransform: public albaOpTransformInterface
{
public:
	albaOpTransform(const wxString &label = "Transform  \tCtrl+T");
  ~albaOpTransform();

  albaTypeMacro(albaOpTransform, albaOp);

	virtual void OnEvent(albaEventBase *alba_event);

  albaOp* Copy();

	/** Builds operation's interface. */
	void OpRun();

	/** Execute the operation. */
	void OpDo();

	/** Makes the undo for the operation. */
	void OpUndo();

	/** Undo all changes*/
	void Reset();

	/** Set identity matrix*/
	void Identity();

	/** Load Matrix from another VME */
	void LoadFrom();

	void Translate(double x, double y, double z);
	void Rotate(double x, double y, double z);
	void Scale(double x, double y, double z);

	albaMatrix* GetAbsMatrix() { return &m_NewAbsMatrix; };

	/** Return an xpm-icon that can be used to represent this operation */
	virtual char ** GetIcon();

protected: 

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

	/** Create the GUI */
	virtual void CreateGui();

	void OnEventTransformGizmo(albaEventBase *alba_event);

	void UpdateReferenceSystem();

	void OnEventPoseTextEntries(albaEventBase *alba_event);

	void OpStop(int result);

	void SelectRefSys();

	void ChooseRelativeRefSys();

	void UpdateTransformTextEntries();

	void OnEventTransformText();

	/** Accept All VME excluding current */
	static bool AcceptRefSys(albaVME *node);

	// Plugged objects
	albaGizmoTranslate           *m_GizmoTranslate;
	albaGizmoRotate              *m_GizmoRotate;
	albaGizmoScale               *m_GizmoScale;

	int m_UpdateAfterRelease;
	int m_RefSystemMode;

	albaVMEPolyline *m_TransformVME;
	albaVMEPolyline *m_LocalRefSysVME;
	albaVMEPolyline *m_LocalCenterRefSysVME;
	albaVMEPolyline *m_RelativeRefSysVME;
	albaVMEPolyline *m_RelativeCenterRefSysVME;
	albaVMEPolyline *m_ArbitraryRefSysVME;

	double m_TransformEntries[3];
	double m_Position[3];
	double m_Orientation[3];
	double m_Scaling[3];

	double m_OriginRefSysPosition[3];
	double m_OriginRefSysOrientation[3];

	// Override superclass
	void RefSysVmeChanged();

	/** Postmultiply event matrix to vme abs matrix; also update Redo ivar m_NewAbsMatrix */;
	virtual void PostMultiplyMatrix(albaMatrix *matrix);
	
};
#endif
