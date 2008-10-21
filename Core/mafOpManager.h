/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpManager.h,v $
  Language:  C++
  Date:      $Date: 2008-10-21 15:53:01 $
  Version:   $Revision: 1.16.2.1 $
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
class mafGUISettings;
class mafGUISettingsDialog;
class mafUser;

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
	virtual void OpAdd(mafOp *op, wxString menuPath = "", bool can_undo = true, mafGUISettings *setting = NULL);

	/** Fill the application men with the operations name.	*/
	virtual void FillMenu(wxMenu* import, wxMenu* mexport, wxMenu* operations);

  /** Fill the setting dialog with the settings associated to the plugged operations.*/
  void FillSettingDialog(mafGUISettingsDialog *settingDialog);

	/** Record the selected vme and enable the menu_entries relative to the compatible operations. */
	virtual void VmeSelected(mafNode* v);
	
	/** Return the selected vme. */
	virtual mafNode* GetSelectedVme();

	/** Run the operation by id. */
  virtual void OpRun(int op_id);
	
	/** Call this to exec an operation with user interaction and undo/redo services. */
  virtual void OpRun(mafOp *op, void *op_param = NULL);

  /** Run the operation by searching it from its type name.*/
  virtual void OpRun(mafString &op_type, void *op_param = NULL);

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

  /** Called by logic to refresh the operation's menù items.*/
  void RefreshMenu();

  /** Set MafUser */
  void SetMafUser(mafUser *user);

protected:
	/** Execute the current operation. */
  virtual void OpDo(mafOp *op);

	/** This method is called if the operation must be executed. */
  virtual void OpRunOk(mafOp *op);

	/** The operation is not executed and is deleted. */
  virtual void OpRunCancel(mafOp *op);

	/** Execute the 'UnDo' method of the operation. */
  virtual void OpUndo();

	/** Execute the 'Do' method of the operation. */
	virtual void OpRedo();

	/** Warn the user if the operation is undoable. */
  virtual bool WarnUser(mafOp *op);

	/** Sent a string message to the listener. */
  virtual void Notify(int msg);

	/** Enable/Disable the men items operation. */
  virtual void EnableOp(bool CanEnable = true);

	/** Enable/Disable the toolbar's buttons. */
	virtual void EnableToolbar(bool CanEnable = true);

  /** Fill the attribute for traceability events*/
  void FillTraceabilityAttribute(mafOp *op, mafNode *in_node, mafNode *out_node);

  void SetAccelerator(mafOp *op);

  mmdMouse          *m_Mouse; ///< Pointer to the mouse devices.
  bool               m_Warn; ///< Flag to warn the user when an operation that can not undo is starting.
	mafOpContextStack  m_Context;
  mafOp             *m_RunningOp; ///< Pointer to the current running operation.
	wxMenu            *m_Menu[3]; ///< Array of pointers to the menu 'Operations', 'Importer' and 'Exporter'
  mafNode						*m_Selected; ///< Pointer to the current selected node.
  mafNode           *m_NaturalNode; ///< Pointer to the NATURAL node on which is running a non-input preserving operation.

  mafUser           *m_User; ///<User credentials

  std::vector<mafOp *> m_OpList; ///< List of pointer of plugged operations.
  int                m_NumOp; ///< Number of plugged operations.
  wxAcceleratorEntry m_OpAccelEntries[MAXOP]; ///< List of Accelerators for menu items.
  int                m_NumOfAccelerators;

  void *m_OpParameters; ///< Pointer to the operation's parameter list.

  mafOpSelect       *m_OpSelect; ///< Pointer to the (always available) operation for selecting VMEs
  mafOpCut          *m_OpCut; ///< Pointer to the (always available) operation for cutting VMEs
  mafOpCopy         *m_OpCopy; ///< Pointer to the (always available) operation for copying VMEs
  mafOpPaste        *m_OpPaste; ///< Pointer to the (always available) operation for pasting VMEs
  //mafOpTransform    *m_optransform;

  wxMenuBar         *m_MenuBar; ///< Pointer to the Application's main menù
	wxToolBar         *m_ToolBar; ///< Pointer to the application's Toolbal

  bool m_CollaborateStatus;  ///< Flag set to know if the application is in collaborative mode or no.

  mafObserver       *m_Listener;
  mafObserver       *m_RemoteListener; ///< Listener used to send messages to remote applications
};
#endif
