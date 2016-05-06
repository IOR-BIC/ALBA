/*=========================================================================

 Program: MAF2
 Module: mafOpTransformInterface
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpTransformInterface_H__
#define __mafOpTransformInterface_H__

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafOp.h"
#include "mafMatrix.h"
#include "mafString.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafGizmoTranslate;
class mafGizmoRotate;
class mafGUITransformMouse;
class mafInteractor;
class mafInteractorCompositorMouse;
class mafInteractorGenericMouse;
class mafVME;
class mafVME;

//----------------------------------------------------------------------------
// mafOpTransformInterface :
//----------------------------------------------------------------------------
/** 
  Abstract base class for transform operations

  This class defines the basic structure to assemble components for vme transformations.
  Typical object that can be plugged in mafOpTransformInterface concrete descendants are
  gizmos (mafGizmoXXX)and gui transform components (mafGUITransformXXX). See mafOpMAFTransform
  for a concrete implementation.

  @sa
  mafOpMAFTransform, mafGUITransformInterface, mafGizmoInterface

  @todo
  - improve components plugging: see comments in mafOpMAFTransform.cpp

*/
class MAF_EXPORT mafOpTransformInterface : public mafOp
{
public:
  mafOpTransformInterface(const wxString &label = "TransformInterface");
  virtual ~mafOpTransformInterface(); 
 
  /** Return true for the acceptable vme type. */
  bool Accept(mafVME* vme) {return true;};

  mafTypeMacro(mafOpTransformInterface, mafOp);

  /** Override superclass */
  mafOp* Copy();

  /** Override superclass */
  void OpDo();
  
  /** Set/Get the vme used as refsys, the vme is referenced*/
  void SetRefSysVME(mafVME *refSysVme);
  mafVME *GetRefSysVME() {return m_RefSysVME;};

  /**
  Enable/disable scaling. If scaling is enable scaling part of the transformation matrix is applied to vme 
  data while roto-translation is applied to vme pose matrix. If Disabled only roto-translation is applied 
  while scaling is discarded (default is enable)*/
  void EnableScalingOn(int enable) {m_EnableScaling = enable;};
  int GetEnableScaling() {return m_EnableScaling;};

  virtual void SetNewAbsMatrix(mafMatrix matrix){m_NewAbsMatrix = matrix;};
  virtual void SetOldAbsMatrix(mafMatrix matrix){m_OldAbsMatrix = matrix;};

protected:
  /** Create the gui */
  virtual void CreateGui() {};
  
  virtual void OnEventThis(mafEventBase *maf_event) {};
  virtual void OnEventGizmoTranslate(mafEventBase *maf_event) {};
  virtual void OnEventGizmoRotate(mafEventBase *maf_event) {};
  
  /** Internal actions to be performed when RefSys vme has changed */
  virtual void RefSysVmeChanged() {};

  /** Postmultiply event matrix to vme abs matrix; also update Redo ivar m_NewAbsMatrix */;
  virtual void PostMultiplyEventMatrix(mafEventBase *maf_event);
  
  enum ACTIVE_GIZMO {TR_GIZMO = 0, ROT_GIZMO, SCAL_GIZMO};

  int m_EnableScaling;

  int m_UseGizmo;
  int m_ActiveGizmo;

  mafTimeStamp m_CurrentTime;
  
  /** 
  vme used as reference system */
  mafVME *m_RefSysVME;

  mafString m_RefSysVMEName;

  mafMatrix m_OldAbsMatrix; // used by Undo()
  mafMatrix m_NewAbsMatrix; // used by Do()

  friend class mafOpTransformInterfaceTest;
};
#endif


