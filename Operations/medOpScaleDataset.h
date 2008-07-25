/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medOpScaleDataset.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 11:14:48 $
  Version:   $Revision: 1.5 $
  Authors:   Daniele Giunchi , Stefano Perticoni  
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __medOpScaleDataset_H__
#define __medOpScaleDataset_H__

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafOpTransformInterface.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafGizmoScale;
class mafGUITransformMouse;
class mafGUISaveRestorePose;
class mafGUITransformTextEntries;
class mafNode;

//----------------------------------------------------------------------------
// medOpScaleDataset :
//----------------------------------------------------------------------------
/** 
Scale vme data set

Scale vme dataset using the scaling gizmo or gui text entries
*/

class medOpScaleDataset : public mafOpTransformInterface
{
public:
  medOpScaleDataset(const wxString &label = "Scale Dataset");
 ~medOpScaleDataset(); 
  virtual void OnEvent(mafEventBase *maf_event);
  
  mafTypeMacro(medOpScaleDataset, mafOp);

  mafOp* Copy();

  /** Return true for the acceptable vme type. */
  bool Accept(mafNode* vme);

  /** Builds operation's interface. */
  void OpRun();
  
  /** Execute the operation. */
  void OpDo();
  
  /** Makes the undo for the operation. */
  void OpUndo();
	
  void Reset();

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
  


