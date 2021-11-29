/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpTransformInterface
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpTransformInterface_H__
#define __albaOpTransformInterface_H__

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "albaOp.h"
#include "albaMatrix.h"
#include "albaString.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaGizmoTranslate;
class albaGizmoRotate;
class albaGUITransformMouse;
class albaInteractor;
class albaInteractorCompositorMouse;
class albaInteractorGenericMouse;
class albaVME;
class albaVME;

//----------------------------------------------------------------------------
// albaOpTransformInterface :
//----------------------------------------------------------------------------
/** 
  Abstract base class for transform operations

  This class defines the basic structure to assemble components for vme transformations.
  Typical object that can be plugged in albaOpTransformInterface concrete descendants are
  gizmos (albaGizmoXXX)and gui transform components (albaGUITransformXXX). See albaOpTransformOld
  for a concrete implementation.

  @sa
  albaOpTransformOld, albaGUITransformInterface, albaGizmoInterface

  @todo
  - improve components plugging: see comments in albaOpTransformOld.cpp

*/
class ALBA_EXPORT albaOpTransformInterface : public albaOp
{
public:
  albaOpTransformInterface(const wxString &label = "TransformInterface");
  virtual ~albaOpTransformInterface(); 
 
  /** Return true for the acceptable vme type. */
  bool Accept(albaVME* vme) {return true;};

  albaTypeMacro(albaOpTransformInterface, albaOp);

  /** Override superclass */
  albaOp* Copy();

  /** Override superclass */
  void OpDo();
  
  /** Set/Get the vme used as refsys, the vme is referenced*/
  void SetRefSysVME(albaVME *refSysVme);
  albaVME *GetRefSysVME() {return m_RefSysVME;};

  /**
  Enable/disable scaling. If scaling is enable scaling part of the transformation matrix is applied to vme 
  data while roto-translation is applied to vme pose matrix. If Disabled only roto-translation is applied 
  while scaling is discarded (default is enable)*/
  void EnableScalingOn(int enable) {m_EnableScaling = enable;};
  int GetEnableScaling() {return m_EnableScaling;};

  virtual void SetNewAbsMatrix(albaMatrix matrix){m_NewAbsMatrix = matrix;};
  virtual void SetOldAbsMatrix(albaMatrix matrix){m_OldAbsMatrix = matrix;};

protected:
  /** Create the gui */
  virtual void CreateGui() {};
  
  virtual void OnEventThis(albaEventBase *alba_event) {};
  virtual void OnEventGizmoTranslate(albaEventBase *alba_event) {};
  virtual void OnEventGizmoRotate(albaEventBase *alba_event) {};
  
  /** Internal actions to be performed when RefSys vme has changed */
  virtual void RefSysVmeChanged() {};

  /** Postmultiply event matrix to vme abs matrix; also update Redo ivar m_NewAbsMatrix */;
  virtual void PostMultiplyEventMatrix(albaEventBase *alba_event);
  
  enum ACTIVE_GIZMO {TR_GIZMO = 0, ROT_GIZMO, SCAL_GIZMO};

  int m_EnableScaling;

  int m_UseGizmo;
  int m_ActiveGizmo;

  albaTimeStamp m_CurrentTime;
  
  /** 
  vme used as reference system */
  albaVME *m_RefSysVME;

  albaString m_RefSysVMEName;

  albaMatrix m_OldAbsMatrix; // used by Undo()
  albaMatrix m_NewAbsMatrix; // used by Do()

  friend class albaOpTransformInterfaceTest;
};
#endif


