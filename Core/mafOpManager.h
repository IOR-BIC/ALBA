/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpManager.h,v $
  Language:  C++
  Date:      $Date: 2006-06-03 10:59:22 $
  Version:   $Revision: 1.8 $
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
class mmdMouse;

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
  virtual void SetRemoteListener(mafObserver *Listener) {m_RemoteListener = Listener;};
	virtual void OnEvent(mafEventBase *maf_event);

  /** Event IDs used in collaborative modality.*/
  MAF_ID_DEC(OPERATION_INTERFACE_EVENT)
  MAF_ID_DEC(RUN_OPERATION_EVENT)

	/** Add the operation 'op' to the list of available operations. */
	virtual void OpAdd		(mafOp *op, wxString menuPath = "");

	/** Fill the application men with the operations name.	*/
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
  virtual void WarningIfCantUndo (bool warn) {m_Warn = warn;};

	/** Set a reference to the main toolbar. */
  virtual void SetToolbar(wxToolBar *tb) {m_ToolBar = tb;};

	/** Set a reference to the main men. */
  virtual void SetMenubar(wxMenuBar *mb) {m_MenuBar = mb;};

	/** return true if there is a running operation. */
  virtual bool Running()								 {return m_Context.Caller() != NULL;};

	/** Clear the stack of executed operation. */
  virtual void ClearUndoStack(); 

	/** Try to Stop the Current operation with OK condition. */
	virtual bool ForceStopWithOk();

	/** Try to Stop the Current Stop operation with CANCEL condition. */
	virtual bool ForceStopWithCancel();

	/** Stop the current operation in any case - to be used as last resort. */
  virtual bool StopCurrentOperation();

  /** Return the current running operation. Return NULL if no operation is running.*/
  mafOp *GetRunningOperation();

  /** 
  Initialize the action for the mouse device. */
  void SetMouse(mmdMouse *mouse);

  /** Turn On/Off the collaboration status. */
  void Collaborate(bool status);

  bool m_FromRemote; ///< Flag used to check if a command comes from local or remote application.

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

	/** Enable/Disable the men items operation. */
  virtual void EnableOp(bool CanEnable = true);

	/** Enable/Disable the toolbar's buttons. */
	virtual void EnableToolbar(bool CanEnable = true);

  void SetAccelerator(mafOp *op);

  mmdMouse          *m_Mouse;
  bool							 m_Warn;
	mafOpContextStack  m_Context;
  mafOp             *m_RunningOp;
	wxMenu            *m_Menu[3];
  mafNode						*m_Selected;

	mafOp             *m_OpList[MAXOP];
  int                m_NumOp;
  wxAcceleratorEntry m_OpAccelEntries[MAXOP];
  int                m_NumOfAccelerators;

  mafOpSelect       *m_OpSelect;
  mafOpCut          *m_OpCut;
  mafOpCopy         *m_OpCopy;
  mafOpPaste        *m_OpPaste;
  //mafOpTransform    *m_optransform;

  wxMenuBar         *m_MenuBar;
	wxToolBar         *m_ToolBar;

  bool m_CollaborateStatus;  ///< Flag set to know if the application is in collaborative mode or no.

  mafObserver       *m_Listener;
  mafObserver       *m_RemoteListener; ///< Listener used to send messages to remote applications
};
#endif
