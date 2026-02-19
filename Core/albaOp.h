/*=========================================================================
 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOp
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.
=========================================================================*/
#ifndef __albaOp_H__
#define __albaOp_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDefines.h" //important: albaDefines should always be included as first
#include "albaEvent.h"
#include "albaObserver.h"
#include "albaObject.h"
#include "albaServiceClient.h"
#include "albaAbsLogicManager.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVME;
class albaGUI;
class albaGUIHolder;
class albaDeviceButtonsPadMouse;
class albaGUISettings;

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
// albaOp :
class ALBA_EXPORT albaOp : public albaObserver, public albaObject, public albaServiceClient
{
public:
	enum ALBAOP_ID
	{
		ID_HELP = MINID,
		MINID,
	};

	albaTypeMacro(albaOp, albaObject);

	albaOp();
	albaOp(const wxString &label);
	virtual	~albaOp();
	void SetListener(albaObserver *Listener) { m_Listener = Listener; };

	/** Return the type of the operation: OPTYPE_OP, OPTYPE_IMPORTER, OPTYPE_EXPORTER, OPTYPE_EDIT or OPTYPE_STATECHANGER*/
	int GetType();

	virtual void OnEvent(albaEventBase *alba_event);
	virtual albaOp* Copy();

	/** Builds operation's interface. */
	virtual void OpRun();

	/** Initialize operation's variables according to the parameter's list. */
	virtual void SetParameters(void *param) {};

	/** Return parameters used by operation. */
	virtual albaString GetParameters() { albaString parameters; return parameters; };

	/** Execute the operation. */
	virtual void OpDo();

	/** Makes the undo for the operation. */
	virtual void OpUndo();

	/** Return the operation's interface. */
	virtual albaGUI *GetGui() { return m_Gui; };

	/** Set/Get the input vme for the operation. */
	virtual void SetInput(albaVME* vme) { m_Input = vme; };
	virtual albaVME	*GetInput() { return m_Input; };

	/** Return the albaVME result of the operation.*/
	virtual albaVME *GetOutput() { return m_Output; };
	virtual void SetOutput(albaVME *output) { m_Output = output; };

	/** Return true for the acceptable vme type. */
	bool Accept(albaVME* vme);

	void EnableAccept(bool enable = true) { m_EnableAccept = enable; }

	/** Return true if the operation is undoable. */
	virtual bool CanUndo();

	/** Return true if the operation preserve the input vme. */
	bool IsInputPreserving() { return m_InputPreserving; };

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
	albaOp					 *m_Next; ///< Pointer to the next operation in the operation's list.

	//SIL 22/04/04
	long            m_Compatibility;
	bool 						IsCompatible(long state);
	wxMenuItem     *m_MenuItem;
	wxString        m_OpMenuPath;

	//MARCO 7/05/04
	virtual const char **GetActions() { return NULL; };

	/** Initialize the mouse device. */
	void SetMouse(albaDeviceButtonsPadMouse *mouse);
	albaDeviceButtonsPadMouse *GetMouse() { return m_Mouse; };

	/** Turn On m_TestMode flag.
	The m_TestMode flag is used to exclude the execution of GUI components that conflicts with test machine.*/
	void TestModeOn() { m_TestMode = true; };

	/** Used to turn off m_TestMode flag.*/
	void TestModeOff() { m_TestMode = false; };

	/** Get TestMode*/
	bool GetTestMode() { return m_TestMode; };

	/** Set the Canundo flag for the operation when is plugged.*/
	void SetCanundo(bool can_undo = true) { m_Canundo = can_undo; };

	/** Return the Canundo flag for the operation.*/
	bool GetCanundo() { return m_Canundo; };

	/** Set the reference to the operation's setting panel.*/
	void SetSetting(albaGUISettings *setting) { m_SettingPanel = setting; };

	/** Get the reference to the operation's setting panel.*/
	albaGUISettings *GetSetting() { return m_SettingPanel; };

	/** Return an xpm-icon that can be used to represent this operation */
	virtual char ** GetIcon();

protected:

	/** Return true for the acceptable vme type. */
	virtual bool InternalAccept(albaVME* vme);

	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	virtual void OpStop(int result);

	albaVME				 *m_Input; ///< Pointer to the Input VME.
	albaVME         *m_Output; ///< Pointer to the Output VME
	albaGUI      	 *m_Gui; ///< Pointer to the operation's GUI.
	albaGUIHolder	 *m_Guih;
	bool 						m_Canundo; ///< Flag to establish if the operation define the UnDo method or not.
	int 						m_OpType; ///< Store the type of the operation: OPTYPE_OP, OPTYPE_IMPORTER, OPTYPE_EXPORTER
	bool						m_InputPreserving; ///< Flag to say if the operation change the input data (m_InputPreserving = false) or not.
	albaObserver    *m_Listener;
	albaDeviceButtonsPadMouse       *m_Mouse;
	albaGUISettings *m_SettingPanel;
	bool            m_TestMode; ///< Flag used with cppunitTest: put this flag at true when executing tests to avoid busy-info or splash screen to be created, default is false.
	bool m_EnableAccept;
};
#endif
