/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoMAFTransformScale.h,v $
  Language:  C++
  Date:      $Date: 2007-03-15 14:22:25 $
  Version:   $Revision: 1.3 $
  Authors:   Daniele Giunchi  
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mmoMAFTransformScale_H__
#define __mmoMAFTransformScale_H__

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mmoTransformInterface.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafGizmoScale;
class mafGuiTransformMouse;
class mafGuiSaveRestorePose;
class mafGuiTransformTextEntries;
class mafNode;

//----------------------------------------------------------------------------
// mmoMAFTransformScale :
//----------------------------------------------------------------------------
/** 
Transform a vme using constrains 
*/

class mmoMAFTransformScale : public mmoTransformInterface
{
public:
  mmoMAFTransformScale(const wxString &label = "MAFTransform");
 ~mmoMAFTransformScale(); 
  virtual void OnEvent(mafEventBase *maf_event);
  
  mafTypeMacro(mmoMAFTransformScale, mafOp);

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
  void OnEventGuiTransform(mafEventBase *maf_event);
  void OnEventGuiSaveRestorePose(mafEventBase *maf_event);
  void OnEventGuiTransformTextEntries(mafEventBase *maf_event);

  void OpStop(int result);

  /** 
  plugged objects */
  mafGizmoScale               *m_GizmoScale;
  //mafGuiTransformMouse        *m_GuiTransform;
  mafGuiSaveRestorePose       *m_GuiSaveRestorePose;
  //mafGuiTransformTextEntries  *m_GuiTransformTextEntries;

  /** 
  Override superclass*/
  void RefSysVmeChanged();
};
#endif
  


