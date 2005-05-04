/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpManager.h,v $
  Language:  C++
  Date:      $Date: 2005-05-04 11:43:10 $
  Version:   $Revision: 1.3 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafOpManager_H__
#define __mafOpManager_H__

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include <wx/accel.h>
#include <wx/menu.h>
#include "mafEvent.h"
#include "mafObserver.h"
#include "mafOpContextStack.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafNode;
class mafOp;
class mafOpSelect;
class mafOpCut;
class mafOpCopy;
class mafOpPaste;
class mafOpTransform;
class vtkMatrix4x4;
class mafAction;

//----------------------------------------------------------------------------
// mafOpManager :
//----------------------------------------------------------------------------
/**  */
class mafOpManager: public mafObserver
{
public:
	         mafOpManager();
	virtual ~mafOpManager(); 
	virtual void SetListener(mafObserver *Listener) {m_Listener = Listener;};
	virtual void OnEvent(mafEventBase *event);

	/** Add the operation 'op' to the list of available operations. */
	virtual void OpAdd		(mafOp *op, wxString menuPath = "");

	/** Fill the application menù with the operations name.	*/
	virtual void FillMenu (wxMenu* import, wxMenu* mexport, wxMenu* operations);

	/** Record the selected vme and enable the menu_entries relative to the compatible operations. */
	virtual void VmeSelected(mafNode* v);
	
	/** Return the selected vme. */
	virtual mafNode* GetSelectedVme();

	/** Run the operation by id. */
  virtual void OpRun		(int op_id);
	
	/** Call this to exec an operation with user interaction and undo/redo services. */
  virtual void OpRun		(mafOp *op);

	/** Execute the operation 'op' and warn the user if the operation is undoable. */
  virtual void OpExec		(mafOp *op);

	/** Execute the select operation. */
  virtual void OpSelect(mafNode* v);

	/** Execute the transform operation. */
	//virtual void OpTransform(vtkMatrix4x4* new_matrix,vtkMatrix4x4* old_matrix);

	/** Set the flag for warning the user if the operation is undoable. */
  virtual void WarningIfCantUndo (bool warn) {m_warn = warn;};

	/** Set a reference to the main toolbar. */
  virtual void SetToolbar(wxToolBar *tb) {m_toolbar = tb;};

	/** Set a reference to the main menù. */
  virtual void SetMenubar(wxMenuBar *mb) {m_menubar = mb;};

	/** return true if there is a running operation. */
  virtual bool Running()								 {return m_context.Caller() != NULL;};

	/** Clear the stack of executed operation. */
  virtual void ClearUndoStack(); 

	/** Try to Stop the Current operation with OK condition. */
	virtual bool ForceStopWithOk();

	/** Try to Stop the Current Stop operation with CANCEL condition. */
	virtual bool ForceStopWithCancel();

	/** Stop the current operation in any case - to be used as last resort. */
  virtual bool StopCurrentOperation();

  /** 
  Initialize the action for the mouse device. */
  // - temporary commented out until mafAction is reintroduced
  //void SetMouseAction(mafAction *action);

protected:
	/** Execute the current operation. */
  virtual void OpDo					(mafOp *op);

	/** This method is called if the operation must be executed. */
  virtual void OpRunOk			(mafOp *op);

	/** The operation is not executed and is deleted. */
  virtual void OpRunCancel	(mafOp *op);

	/** Execute the 'UnDo' method of the operation. */
  virtual void OpUndo();

	/** Execute the 'Do' method of the operation. */
	virtual void OpRedo();

	/** Warn the user if the operation is undoable. */
  virtual bool WarnUser			(mafOp *op);

	/** Sent a string message to the listener. */
  virtual void Notify	      (int msg);

	/** Enable/Disable the menù items operation. */
  virtual void EnableOp(bool CanEnable = true);

	/** Enable/Disable the toolbar's buttons. */
	virtual void EnableToolbar(bool CanEnable = true);

  void SetAccelerator(mafOp *op);

  mafAction          *m_MouseAction;
  bool							 m_warn;
	mafOpContextStack  m_context;
  mafOp             *m_running_op;
	wxMenu            *m_menu[3];
  mafNode						*m_selected;

	mafOp             *m_opv[MAXOP];
  int                m_numop;
  wxAcceleratorEntry m_OpAccelEntries[MAXOP];
  int                m_numAccel;

  mafOpSelect       *m_opselect;
  mafOpCut          *m_opcut;
  mafOpCopy         *m_opcopy;
  mafOpPaste        *m_oppaste;
  //mafOpTransform    *m_optransform;

  wxMenuBar         *m_menubar;
	wxToolBar         *m_toolbar;

  mafObserver       *m_Listener;
};
#endif