/*=========================================================================

 Program: MAF2
 Module: mafOpScaleDataset
 Authors: Daniele Giunchi , Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpScaleDataset_H__
#define __mafOpScaleDataset_H__

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafOpTransformInterface.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafGizmoScale;
class mafGUITransformMouse;
class mafGUISaveRestorePose;
class mafGUITransformTextEntries;
class mafVME;

//----------------------------------------------------------------------------
// mafOpScaleDataset :
//----------------------------------------------------------------------------
/** 
Scale vme data set

Scale vme dataset using the scaling gizmo or gui text entries
*/

class MAF_EXPORT mafOpScaleDataset : public mafOpTransformInterface
{
public:
  mafOpScaleDataset(const wxString &label = "Scale Dataset");
 ~mafOpScaleDataset(); 
  virtual void OnEvent(mafEventBase *maf_event);
  
  mafTypeMacro(mafOpScaleDataset, mafOp);

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

  void SetNewAbsMatrix(mafMatrix matrix){m_NewAbsMatrix = matrix;};
  void SetOldAbsMatrix(mafMatrix matrix){m_OldAbsMatrix = matrix;};

protected:

  /** Create the gui */
  virtual void CreateGui();
  
  void OnEventThis(mafEventBase *maf_event);  
  void OnEventGizmoScale(mafEventBase *maf_event);
  void OnEventGuiSaveRestorePose(mafEventBase *maf_event);
  void OpStop(int result);

  /** 
  plugged objects */
  mafGizmoScale               *m_GizmoScale;
  mafGUISaveRestorePose       *m_GuiSaveRestorePose;

  /** 
  Override superclass*/
  void RefSysVmeChanged();
};
#endif
  


