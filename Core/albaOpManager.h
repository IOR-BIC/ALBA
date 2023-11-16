/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpManager
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpManager_H__
#define __albaOpManager_H__

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include <wx/accel.h>
#include <wx/menu.h>
#include "albaEvent.h"
#include "albaObserver.h"
#include "albaOpContextStack.h"
#include "albaServiceClient.h"
#include "albaAbsLogicManager.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVME;
class albaVME;
class albaOp;
class albaOpSelect;
class albaOpCut;
class albaOpCopy;
class albaOpPaste;
class albaOpRename;
class albaOpReparentTo;
class albaOpCreateGroup;
class albaOpShowHistory;
class vtkMatrix4x4;
class albaDeviceButtonsPadMouse;
class albaGUISettings;
class albaGUISettingsDialog;
class albaUser;

#ifdef ALBA_EXPORTS
#include "albaDllMacros.h"
EXPORT_STL_VECTOR(ALBA_EXPORT,albaOp*);
#endif
#include "albaGUITreeContextualMenu.h"

//----------------------------------------------------------------------------
// albaOpManager :
//----------------------------------------------------------------------------
/**  */
class ALBA_EXPORT albaOpManager: public albaObserver, public albaServiceClient
{
public:
	         albaOpManager();
	virtual ~albaOpManager(); 
	virtual void SetListener(albaObserver *Listener) {m_Listener = Listener;};
	virtual void OnEvent(albaEventBase *alba_event);

  /** Event IDs used in collaborative modality.*/
  ALBA_ID_DEC(OPERATION_INTERFACE_EVENT)
  ALBA_ID_DEC(RUN_OPERATION_EVENT)

	/** Add the operation 'op' to the list of available operations. */
	virtual void OpAdd(albaOp *op, wxString menuPath = "", bool can_undo = true, albaGUISettings *setting = NULL);

	/** Fill the application men with the operations name.	*/
	virtual void FillMenu(wxMenu* import, wxMenu* mexport, wxMenu* operations);

  /** Fill the setting dialog with the settings associated to the plugged operations.*/
  void FillSettingDialog(albaGUISettingsDialog *settingDialog);

	/** Record the selected vme and enable the menu_entries relative to the compatible operations. */
	virtual void VmeSelected(albaVME* v);

	/** Record the modified vme and enable the menu_entries relative to the compatible operations. */
	virtual void VmeModified(albaVME* v);

	/** Vme removed notification */
	virtual void VmeRemove(albaVME* v);
	
	/** Return the selected vme. */
	virtual albaVME* GetSelectedVme();

	/** Run the operation by id. */
  virtual void OpRun(int op_id);
	
	/** Call this to exec an operation with user interaction and undo/redo services. */
  virtual void OpRun(albaOp *op, void *op_param = NULL);

  /** Run the operation by searching it from its type name.*/
  virtual void OpRun(albaString &op_type, void *op_param = NULL);

	void RunOpAddGroup();

	void RunOpCut();
	void RunOpCopy();
	void RunOpPaste();
	void RunOpDelete();
	void RunOpRename();
	void RunOpReparentTo();
	void RunOpShowHistory();

	/** Execute the operation 'op' and warn the user if the operation is undoable. */
  virtual void OpExec		(albaOp *op);

	/** Execute the select operation. */
  virtual void OpSelect(albaVME* v);

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

	void EnableContextualMenu(albaGUITreeContextualMenu *contextualMenu, albaVME *node, bool CanEnable = true);

  /** Return the current running operation. Return NULL if no operation is running.*/
  albaOp *GetRunningOperation();

  /** Return an instance of operation from id.*/
  albaOp *GetOperationById(int id);

  /** 
  Initialize the action for the mouse device. */
  void SetMouse(albaDeviceButtonsPadMouse *mouse);
	
  /** Called by logic to refresh the operation's menù items.*/
  void RefreshMenu();

  /** Set MafUser */
  void SetMafUser(albaUser *user);

  /** Return the current alba user */
  albaUser* GetMafUser(){return m_User;};

	/** Enable/Disable the men items operation. */
	virtual void EnableOp(bool CanEnable = true);


	/**Set Build Num*/
	albaString GetBuildNum() const { return m_BuildNum; }

	/** Get Build Num*/
	void SetBuildNum(albaString val) { m_BuildNum = val; }

protected:
	/** Execute the current operation. */
  virtual void OpDo(albaOp *op);

	/** This method is called if the operation must be executed. */
  virtual void OpRunOk(albaOp *op);

	/** The operation is not executed and is deleted. */
  virtual void OpRunCancel(albaOp *op);

	/** Execute the 'UnDo' method of the operation. */
  virtual void OpUndo();

	/** Execute the 'Do' method of the operation. */
	virtual void OpRedo();

	/** Warn the user if the operation is undoable. */
  virtual bool WarnUser(albaOp *op);

	/** Sent a string message to the listener. */
  virtual void Notify(int msg, long arg=0L);

	/** Enable/Disable the toolbar's buttons. */
	virtual void EnableToolbar(bool CanEnable = true);

  /** Fill the attribute for traceability events*/
  void FillTraceabilityAttribute(albaOp *op, albaVME *in_node, albaVME *out_node);

  void SetAccelerator(albaOp *op);

  albaDeviceButtonsPadMouse          *m_Mouse; ///< Pointer to the mouse devices.
  bool               m_Warn; ///< Flag to warn the user when an operation that can not undo is starting.
	albaOpContextStack  m_Context;
  albaOp             *m_RunningOp; ///< Pointer to the current running operation.
	wxMenu            *m_Menu[3]; ///< Array of pointers to the menu 'Operations', 'Importer' and 'Exporter'
  albaVME						*m_Selected; ///< Pointer to the current selected node.
  albaVME           	*m_NaturalNode; ///< Pointer to the NATURAL node on which is running a non-input preserving operation.

  albaUser           *m_User; ///<User credentials

  std::vector<albaOp *> m_OpList; ///< List of pointer of plugged operations.
  int                m_NumOp; ///< Number of plugged operations.
  wxAcceleratorEntry m_OpAccelEntries[MAXOP]; ///< List of Accelerators for menu items.
  int                m_NumOfAccelerators;

  void *m_OpParameters; ///< Pointer to the operation's parameter list.

  albaOpSelect       *m_OpSelect; ///< Pointer to the (always available) operation for selecting VMEs
  albaOpCut          *m_OpCut; ///< Pointer to the (always available) operation for cutting VMEs
  albaOpCopy         *m_OpCopy; ///< Pointer to the (always available) operation for copying VMEs
  albaOpPaste        *m_OpPaste; ///< Pointer to the (always available) operation for pasting VMEs
  albaOpRename		*m_OpRename; ///< Pointer to the (always available) operation for reparenting VMEs
	albaOpReparentTo		*m_OpReparent; ///< Pointer to the (always available) operation for reparenting VMEs
	albaOpCreateGroup	*m_OpAddGroup; ///< Pointer to the (always available) operation for Adding Group VMEs
	albaOpShowHistory *m_OpShowHistory;
	wxMenuBar         *m_MenuBar; ///< Pointer to the Application's main menù
	wxToolBar         *m_ToolBar; ///< Pointer to the application's Toolbal
	albaString				 m_BuildNum;

  albaObserver       *m_Listener;

  /** test friend */
  friend class albaOpManagerTest;
};
#endif
