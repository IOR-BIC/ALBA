/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoTransformInterface.h,v $
  Language:  C++
  Date:      $Date: 2005-11-23 12:32:30 $
  Version:   $Revision: 1.3 $
  Authors:   Stefano Perticoni        
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mmoTransformInterface_H__
#define __mmoTransformInterface_H__

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
class mafGuiTransformMouse;
class mafInteractor;
class mmiCompositorMouse;
class mmiGenericMouse;
class mafVME;
class mafNode;
class mafTransform;

//----------------------------------------------------------------------------
// mmoTransformInterface :
//----------------------------------------------------------------------------
/** 
  Abstract base class for transform operations

  This class defines the basic structure to assemble components for vme transformations.
  Typical object that can be plugged in mmoTransformInterface concrete descendants are
  gizmos and transform controls. See mmoMAFTransform for a concrete implementation.

  @sa
  mmoMAFTransform, mafGuiTransformInterface, mafGizmoInterface

  @todo
  - improve components plugging: see comments in mmoMAFTransform.cpp

*/
class mmoTransformInterface : public mafOp
{
public:
  virtual ~mmoTransformInterface(); 
 
  /** Return true for the acceptable vme type. */
  bool Accept(mafNode* vme) {return true;};

  /** Override superclass */
  mafOp* Copy();

  /** Override superclass */
  void OpDo();
  
  /** Set/Get the vme used as refsys, the vme is referenced*/
  void SetRefSysVME(mafVME *refSysVme);
  mafVME *GetRefSysVME() {return RefSysVME;};

  /**
  Enable/disable scaling. If scaling is enable scaling part of the transformation matrix is applied to vme 
  data while roto-translation is applied to vme pose matrix. If Disabled only roto-translation is applied 
  while scaling is discarded */
  void EnableScalingOn(int enable) {EnableScaling = enable;};
  int GetEnableScaling() {return EnableScaling;};

protected:
  
  mmoTransformInterface(wxString label);

  /** Create the gui */
  virtual void CreateGui() {};
  
  virtual void OnEventThis(mafEventBase *maf_event) {};
  virtual void OnEventGizmoTranslate(mafEventBase *maf_event) {};
  virtual void OnEventGizmoRotate(mafEventBase *maf_event) {};
  
  /** Internal actions to be performed when RefSys vme has changed */
  virtual void RefSysVmeChanged() {};

  /** Postmultiply event matrix to vme abs matrix; also update Redo ivar NewAbsMatrix */;
  virtual void PostMultiplyEventMatrix(mafEventBase *maf_event);
  
  enum ACTIVE_GIZMO {TR_GIZMO = 0, ROT_GIZMO, SCAL_GIZMO};

  int EnableScaling;

  int UseGizmo;
  int ActiveGizmo;

  mafTimeStamp CurrentTime;
  
  /** 
  vme used as reference system */
  mafVME *RefSysVME;

  mafString RefSysVMEName;

  mafMatrix OldAbsMatrix; // used by Undo()
  mafMatrix NewAbsMatrix; // used by Do()
};
#endif
