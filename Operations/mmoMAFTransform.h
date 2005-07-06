/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoMAFTransform.h,v $
  Language:  C++
  Date:      $Date: 2005-07-06 13:51:39 $
  Version:   $Revision: 1.1 $
  Authors:   Stefano Perticoni        
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mmoMAFTransform_H__
#define __mmoMAFTransform_H__

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafEvent.h"
#include "mmoTransformInterface.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafGizmoTranslate;
class mafGizmoRotate;
class mafGizmoScale;
class mafGuiTransformMouse;
class mafGuiSaveRestorePose;
class mafGuiTransformTextEntries;
class mafNode;

//----------------------------------------------------------------------------
// mmoMAFTransform :
//----------------------------------------------------------------------------
/** 
Transform a vme using constrains 
*/

class mmoMAFTransform : public mmoTransformInterface
{
public:
  mmoMAFTransform(wxString label);
 ~mmoMAFTransform(); 
  virtual void OnEvent(mafEventBase *maf_event);
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
  void OnEventGizmoTranslate(mafEventBase *maf_event);
  void OnEventGizmoRotate(mafEventBase *maf_event);
  void OnEventGizmoScale(mafEventBase *maf_event);
  void OnEventGuiTransform(mafEventBase *maf_event);
  void OnEventGuiSaveRestorePose(mafEventBase *maf_event);
  void OnEventGuiTransformTextEntries(mafEventBase *maf_event);

  void OpStop(int result);

  /** 
  plugged objects */
  mafGizmoTranslate *GizmoTranslate;  
  mafGizmoRotate *GizmoRotate;
  mafGizmoScale *GizmoScale;
  mafGuiTransformMouse *GuiTransform;
  mafGuiSaveRestorePose *GuiSaveRestorePose;
  mafGuiTransformTextEntries *GuiTransformTextEntries;

  /** 
  Override superclass*/
  void RefSysVmeChanged();
};
#endif
  