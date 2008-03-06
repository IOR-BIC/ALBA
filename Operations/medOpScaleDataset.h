/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medOpScaleDataset.h,v $
  Language:  C++
  Date:      $Date: 2008-03-06 12:03:55 $
  Version:   $Revision: 1.2 $
  Authors:   Daniele Giunchi , Stefano Perticoni  
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mmoMAFTransformScale_H__
#define __mmoMAFTransformScale_H__

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafOpTransformInterface.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafGizmoScale;
class mafGuiTransformMouse;
class mafGuiSaveRestorePose;
class mafGuiTransformTextEntries;
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
  medOpScaleDataset(const wxString &label = "MAFTransform");
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
  mafGuiSaveRestorePose       *m_GuiSaveRestorePose;

  /** 
  Override superclass*/
  void RefSysVmeChanged();
};
#endif
  


