/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpScaleDataset
 Authors: Daniele Giunchi , Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpScaleDataset_H__
#define __albaOpScaleDataset_H__

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaOpTransformInterface.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaGizmoScale;
class albaGUITransformMouse;
class albaGUISaveRestorePose;
class albaGUITransformTextEntries;
class albaVME;

//----------------------------------------------------------------------------
// albaOpScaleDataset :
//----------------------------------------------------------------------------
/** 
Scale vme data set

Scale vme dataset using the scaling gizmo or gui text entries
*/

class ALBA_EXPORT albaOpScaleDataset : public albaOpTransformInterface
{
public:
  albaOpScaleDataset(const wxString &label = "Scale Dataset");
 ~albaOpScaleDataset(); 
  virtual void OnEvent(albaEventBase *alba_event);
  
  albaTypeMacro(albaOpScaleDataset, albaOp);

  albaOp* Copy();

  /** Builds operation's interface. */
  void OpRun();
  
  /** Execute the operation. */
  void OpDo();
  
  /** Makes the undo for the operation. */
  void OpUndo();
	
  void Reset();

  void SetNewAbsMatrix(albaMatrix matrix){m_NewAbsMatrix = matrix;};
  void SetOldAbsMatrix(albaMatrix matrix){m_OldAbsMatrix = matrix;};

protected:

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

  /** Create the gui */
  virtual void CreateGui();
  
  void OnEventThis(albaEventBase *alba_event);  
  void OnEventGizmoScale(albaEventBase *alba_event);
  void OnEventGuiSaveRestorePose(albaEventBase *alba_event);
  void OpStop(int result);

  /** 
  plugged objects */
  albaGizmoScale               *m_GizmoScale;
  albaGUISaveRestorePose       *m_GuiSaveRestorePose;

  /** 
  Override superclass*/
  void RefSysVmeChanged();
};
#endif
  


