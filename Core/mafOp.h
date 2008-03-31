/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOp.h,v $
  Language:  C++
  Date:      $Date: 2008-03-31 12:59:40 $
  Version:   $Revision: 1.23 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __mafOp_H__
#define __mafOp_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafDefines.h" //important: mafDefines should always be included as first
#include "mafEvent.h"
#include "mafObserver.h"
#include "mafObject.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafNode;
class mmgGui;
class mmgGuiHolder;
class mmdMouse;
class mafGUISettings;
//----------------------------------------------------------------------------
// constants :
//----------------------------------------------------------------------------
enum OPERATIONS_TYPE_ID
{
	OPTYPE_OP = 0,
	OPTYPE_IMPORTER,
	OPTYPE_EXPORTER,
	OPTYPE_EDIT,
	OPTYPE_STATECHANGER,
	OPTYPE_MAX,
};
//----------------------------------------------------------------------------
// mafOp :
//----------------------------------------------------------------------------
/**
*/
class mafOp: public mafObserver, public mafObject
{
public:
  mafTypeMacro(mafOp, mafObject);

					mafOp();
					mafOp(const wxString &label);
	virtual	~mafOp(); 
					void SetListener(mafObserver *Listener)	{m_Listener = Listener;};
          
					/** Return the type of the operation: OPTYPE_OP, OPTYPE_IMPORTER, OPTYPE_EXPORTER, OPTYPE_EDIT or OPTYPE_STATECHANGER*/
					int GetType();

	virtual void OnEvent(mafEventBase *maf_event);
	virtual mafOp* Copy();

	/** Builds operation's interface. */
	virtual void OpRun();

  /** Initialize operation's variables according to the parameter's list. */
  virtual void SetParameters(void *param) {};

	/** Execute the operation. */
	virtual void OpDo();

	/** Makes the undo for the operation. */
	virtual void OpUndo();

	/** Return the operation's interface. */
	virtual mmgGui *GetGui()	{return m_Gui;};

	/** Set/Get the input vme for the operation. */
	virtual void SetInput(mafNode* vme)	{m_Input = vme;};
  virtual mafNode	*GetInput()	{return m_Input;};

  /** Return the mafNode result of the operation.*/
  virtual mafNode *GetOutput() {return m_Output;};
  virtual void SetOutput(mafNode *output) {m_Output = output;};

	/** Return true for the acceptable vme type. */
	virtual bool Accept(mafNode* vme);

	/** Return true if the operation is undoable. */
	virtual bool CanUndo();

	/** Return true if the operation is an importer. */
	//bool IsImporter();

	/** Return true if the operation is an exporter. */
	//bool IsExporter();

	/** Return true if the operation is a normal operation. */
	//bool IsOp();

	/** Return true if the operation preserve the input vme. */
	bool IsInputPreserving() {return m_InputPreserving;};

	/** Puts the operation's interface into the gui holder and send the event to plug the interface on the side bar. */
	void ShowGui();

	/** Send the event to remove the interface from the side bar. */
	void HideGui();

	/** Return true if the OK button in operation's interface is enabled. */
	virtual bool OkEnabled();

	/** Stop operation with OK condition. */
	virtual void ForceStopWithOk();

	/** Stop operation with CANCEL condition. */
	virtual void ForceStopWithCancel();

	wxString				m_Label; ///< Label of the operation that will appear on the SideBar tab.
	int							m_Id; ///< Index of the operation referring to the operation list.
	mafOp					 *m_Next; ///< Pointer to the next operation in the operation's list.

  //SIL 22/04/04
	long            m_Compatibility;
	bool 						IsCompatible(long state);
  wxMenuItem     *m_MenuItem;  
  wxString        m_OpMenuPath;

  //MARCO 7/05/04
  virtual const char **GetActions() {return NULL;}; 

  /** Initialize the mouse device. */
  void SetMouse(mmdMouse *mouse);
  mmdMouse *GetMouse(){return m_Mouse;};

  /** Turn On/Off the collaboration status. */
  void Collaborate(bool status);

  /** Turn On m_TestMode flag. 
  The m_TestMode flag is used to exclude the execution of splash screen or wxBusyInfo that conflicts with test machine.*/
  void TestModeOn() {m_TestMode = true;};

  /** Used to turn off m_TestMode flag.*/
  void TestModeOff() {m_TestMode = false;};

  /** Get TestMode*/
  bool GetTestMode(){return m_TestMode;};

  /** Set the Canundo flag for the operation when is plugged.*/
  void SetCanundo(bool can_undo = true) {m_Canundo = can_undo;};

  /** Return the Canundo flag for the operation.*/
  bool GetCanundo() {return m_Canundo;};

  /** Set the reference to the operation's setting panel.*/
  void SetSetting(mafGUISettings *setting) {m_SettingPanel = setting;};

  /** Get the reference to the operation's setting panel.*/
  mafGUISettings *GetSetting() {return m_SettingPanel;};

protected:
  /** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
  virtual void OpStop(int result);

	mafNode				 *m_Input; ///< Pointer to the Input VME.
  mafNode        *m_Output; ///< Pointer to the Output VME
	mmgGui      	 *m_Gui; ///< Pointer to the operation's GUI.
	mmgGuiHolder	 *m_Guih;
	bool 						m_Canundo; ///< Flag to establish if the operation define the UnDo method or not.
	int 						m_OpType; ///< Store the type of the operation: OPTYPE_OP, OPTYPE_IMPORTER, OPTYPE_EXPORTER
	bool						m_InputPreserving; ///< Flag to say if the operation change the input data (m_InputPreserving = false) or not.
	mafObserver    *m_Listener;
  mmdMouse       *m_Mouse;
  bool            m_CollaborateStatus;
  mafGUISettings *m_SettingPanel;
  bool            m_TestMode; ///< Flag used with cppunitTest: put this flag at true when executing tests to avoid busy-info or splash screen to be created, default is false.
};
#endif
