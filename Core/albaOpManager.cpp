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


#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "albaOpManager.h"
#include <wx/tokenzr.h>
#include "mmuIdFactory.h"
#include "albaDecl.h"
#include "albaOp.h"
#include "albaUser.h"
#include "albaGUI.h"
#include "albaGUISettings.h"
#include "albaOpStack.h"
#include "albaOpContextStack.h"
#include "albaOpSelect.h"
#include "albaGUISettingsDialog.h"
#include "albaAttributeTraceability.h"
#include "albaDeviceButtonsPadMouse.h"

#include "albaStorage.h"
#include "albaEventIO.h"
#include "albaDataVector.h"
#include "albaTagArray.h"
#include "albaTagItem.h"
#include "albaVME.h"
#include "albaVMEGenericAbstract.h"
#include "albaOpReparentTo.h"
#include "albaOpCreateGroup.h"

//------------------------------------------------------------------------------
// Events
//------------------------------------------------------------------------------
ALBA_ID_IMP(albaOpManager::OPERATION_INTERFACE_EVENT)
ALBA_ID_IMP(albaOpManager::RUN_OPERATION_EVENT)

/**
There are 4 paths in OpProcessor

1) OpRun -> run the op's GUI, ...... in asynchronous way can call OpDo
used into operations op,imp,exp

2) OpExec -> run OpDo in synchronous way
used by nested select,transform,cut,copy,paste, op

3) OpUndo -> do pop e run in synchronous way op->OpUndo
used by undo

4) OpRedo -> do pop e run in synchronous way op->OpRedo
used by redo

During the Run an operation can run nested operations,
the nested operations can be cancelled individually.

the nested operations send OP_RUN_STARTING and OP_RUN_TERMINATED to the caller operation.
instead to Logic - in Logic don't need any more running_op_counter, in theory.
In practice it needs for View-Settings.
No is dangerous - if the operations don't manage OP_RUN_STARTING and OP_RUN_TERMINATED 
these return and reach logic - is better to avoid.

During the Exec, can be run nested operations
but could not be done the undo.
*/

//----------------------------------------------------------------------------
albaOpManager::albaOpManager()
{
	m_Listener = NULL;
	m_RunningOp = NULL;
	m_Selected = NULL;
	m_NaturalNode = NULL;
	m_User = NULL;
	m_Warn = true;

	m_OpParameters = NULL;

	m_Menu[OPTYPE_IMPORTER] = NULL;
	m_Menu[OPTYPE_EXPORTER] = NULL;
	m_Menu[OPTYPE_OP] = NULL;

	m_OpSelect = new albaOpSelect();
	m_OpCut = new albaOpCut();
	m_OpCopy = new albaOpCopy();
	m_OpPaste = new albaOpPaste();
	m_OpRename = new albaOpRename();
	m_OpReparent = new albaOpReparentTo();
	m_OpAddGroup = new albaOpCreateGroup();
	//m_optransform = new albaOpTransform();

	m_ToolBar = NULL;
	m_MenuBar = NULL;

	m_NumOfAccelerators = 0;
	m_NumOp = 0;
	m_OpList.clear();
}
//----------------------------------------------------------------------------
albaOpManager::~albaOpManager()
{
	// clear clipboard to avoid crash if some VME is still into the clipboard.
	m_Context.Clear();
	m_OpCut->ClipboardClear();

	std::set<albaGUISettings *> settingsToDelete;

	for (int i = 0; i < m_NumOp; i++)
	{
		albaGUISettings *s = m_OpList[i]->GetSetting();
		if (s != NULL)
			settingsToDelete.insert(s);

		delete m_OpList[i];
	}
	m_OpList.clear();

	std::set<albaGUISettings *>::iterator it;
	for (it = settingsToDelete.begin(); it != settingsToDelete.end(); ++it)
		delete *it;


	cppDEL(m_OpSelect);
	cppDEL(m_OpCut);
	cppDEL(m_OpCopy);
	cppDEL(m_OpPaste);
	cppDEL(m_OpRename);
	cppDEL(m_OpReparent);
	cppDEL(m_OpAddGroup);
	cppDEL(m_User);
}
//----------------------------------------------------------------------------
void albaOpManager::OnEvent(albaEventBase *alba_event)
{
  albaOp* o = NULL; 

  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch (e->GetId())
    {
      case OP_EXEC:
        o = e->GetOp();
        if(o) 
          OpExec(o);
      break; 
      case OP_RUN:
        o = e->GetOp();
        if(o) 
          OpRun(o);
      break; 
      case OP_RUN_OK:
        o = (albaOp*) e->GetSender();
        assert(o == m_Context.Caller());
        OpRunOk(o);
      break; 
      case OP_RUN_CANCEL:
        o = (albaOp*) e->GetSender();
        assert(o == m_Context.Caller());
        OpRunCancel(o);
      break;
      default:
        albaEventMacro(*e);
      break;
    }
  }
}
//----------------------------------------------------------------------------
void albaOpManager::OpAdd(albaOp *op, wxString menuPath, bool can_undo, albaGUISettings *setting)
{
  m_OpList.push_back(op);
  op->m_OpMenuPath = menuPath;
	op->m_Id = m_NumOp + OP_USER;
  op->SetListener(this);
  op->SetCanundo(can_undo);
  if (setting != NULL)
  {
    op->SetSetting(setting);
  }
  m_NumOp++;
}
//----------------------------------------------------------------------------
void albaOpManager::FillSettingDialog(albaGUISettingsDialog *settingDialog)
{
  for(int i=0; i<m_NumOp; i++)
  {
    albaOp *o = m_OpList[i];
    albaGUISettings *setting = o->GetSetting();
    if (setting != NULL)
    {
      settingDialog->AddPage(setting->GetGui(), setting->GetLabel());
    }
  }
}
//----------------------------------------------------------------------------
void albaOpManager::FillMenu(wxMenu* import, wxMenu* mexport, wxMenu* operations)
{
  int submenu_id = 1;

  m_Menu[OPTYPE_IMPORTER] = import;
  m_Menu[OPTYPE_EXPORTER] = mexport;
  m_Menu[OPTYPE_OP] = operations;

  for(int i=0; i<m_NumOp; i++)
  {
    albaOp *o = m_OpList[i];
    if (o->m_OpMenuPath != "")
    {
      wxMenu *sub_menu = NULL;
      wxMenu *path_menu = m_Menu[o->GetType()];
      wxString op_path = "";
      wxStringTokenizer path_tkz(o->m_OpMenuPath, "/");
      while ( path_tkz.HasMoreTokens() )
      {
        op_path = path_tkz.GetNextToken();
        int item = path_menu->FindItem(_(op_path));
        if (item != wxNOT_FOUND)
        {
          wxMenuItem *menu_item = path_menu->FindItem(item);
          if (menu_item)
            sub_menu = menu_item->GetSubMenu();
        }
        else
        {
          if (sub_menu)
          {
            path_menu = sub_menu;
          }
          sub_menu = new wxMenu;
          path_menu->Append(submenu_id++,_(op_path),sub_menu);
        }
      }

      if(sub_menu)
				albaGUI::AddMenuItem(sub_menu, o->m_Id, _(o->m_Label), o->GetIcon());
      else
        albaLogMessage(_("error in FillMenu"));
    }
    else
    {
			albaGUI::AddMenuItem(m_Menu[o->GetType()], o->m_Id, _(o->m_Label), o->GetIcon());
    }
    SetAccelerator(o);
  }
  wxAcceleratorTable accel(MAXOP, m_OpAccelEntries);
  if (accel.Ok())
    albaGetFrame()->SetAcceleratorTable(accel);
}
//----------------------------------------------------------------------------
void albaOpManager::SetAccelerator(albaOp *op)
{
  wxString accelerator, flag = "", extra_flag = "", key_code = "";
  int flag_num;
  accelerator = op->m_Label;
  wxStringTokenizer tkz(accelerator, "\t");
  int token = tkz.CountTokens();

  if (token > 1)
  {
    accelerator  = tkz.GetNextToken();
    accelerator  = tkz.GetNextToken();
    wxStringTokenizer tkz2(accelerator, "+");
    token = tkz2.CountTokens();
    if (token == 2)
    {
      flag = tkz2.GetNextToken();
      key_code = tkz2.GetNextToken();
    }
    else
    {
      flag = tkz2.GetNextToken();
      extra_flag = tkz2.GetNextToken();
      key_code = tkz2.GetNextToken();
    }
    if (flag == "Ctrl")
      flag_num = wxACCEL_CTRL;
    else if(flag == "Alt")
      flag_num = wxACCEL_ALT;
    else if(flag == "Shift")
      flag_num = wxACCEL_SHIFT;

    if (extra_flag == "Ctrl")
      flag_num |= wxACCEL_CTRL;
    else if(extra_flag == "Alt")
      flag_num |= wxACCEL_ALT;
    else if(extra_flag == "Shift")
      flag_num |= wxACCEL_SHIFT;

    m_OpAccelEntries[m_NumOfAccelerators++].Set(flag_num,  (int) key_code.char_str()[0], op->m_Id);
  }
}
//----------------------------------------------------------------------------
void albaOpManager::VmeSelected(albaVME* v)   
{
  m_Selected = v;
  EnableOp();
}

//----------------------------------------------------------------------------
void albaOpManager::VmeModified(albaVME* v)
{
	if(m_Selected == v)
		EnableOp();
}

//----------------------------------------------------------------------------
void albaOpManager::VmeRemove(albaVME* v)
{
	if (m_Selected == v)
		m_Selected = NULL;
}

//----------------------------------------------------------------------------
albaVME* albaOpManager::GetSelectedVme()
{
	return m_Selected;
}
//----------------------------------------------------------------------------
void albaOpManager::OpSelect(albaVME* vme)
{
	if(vme == m_Selected ) 
    return;
	m_OpSelect->SetNewSel(vme);
	OpRun(m_OpSelect);
  albaLogMessage("node selected: %s", vme->GetName());
}
//----------------------------------------------------------------------------
void albaOpManager::ClearUndoStack()
{
   m_Context.Clear();
   m_OpCut->ClipboardClear();
	 EnableOp();
}
//----------------------------------------------------------------------------
void albaOpManager::EnableToolbar(bool CanEnable) 
{
	if(!CanEnable)
	{
		m_ToolBar->EnableTool(OP_UNDO,false);
		m_ToolBar->EnableTool(OP_REDO,false);
		m_ToolBar->EnableTool(OP_CUT,false);
		m_ToolBar->EnableTool(OP_COPY,false);
		m_ToolBar->EnableTool(OP_PASTE,false);
		m_ToolBar->EnableTool(OP_RENAME, false);
	}
	else
	{
		m_ToolBar->EnableTool(OP_UNDO,	!m_Context.Undo_IsEmpty());
		m_ToolBar->EnableTool(OP_REDO,	!m_Context.Redo_IsEmpty());
		m_ToolBar->EnableTool(OP_CUT,		m_OpCut->Accept(m_Selected) );
		m_ToolBar->EnableTool(OP_COPY,	m_OpCopy->Accept(m_Selected) );
		m_ToolBar->EnableTool(OP_PASTE,	m_OpPaste->Accept(m_Selected) );
		m_ToolBar->EnableTool(OP_RENAME, m_OpRename->Accept(m_Selected));
	}
}
//----------------------------------------------------------------------------
void albaOpManager::EnableOp(bool CanEnable)
{
	albaOp *o = NULL;
	if (m_MenuBar)
	{
		if (!CanEnable)
		{
			if (m_MenuBar->FindItem(OP_DELETE))	m_MenuBar->Enable(OP_DELETE, false);
			if (m_MenuBar->FindItem(OP_CUT))			m_MenuBar->Enable(OP_CUT, false);
			if (m_MenuBar->FindItem(OP_COPY))		m_MenuBar->Enable(OP_COPY, false);
			if (m_MenuBar->FindItem(OP_PASTE))		m_MenuBar->Enable(OP_PASTE, false);
			if (m_MenuBar->FindItem(OP_RENAME))		m_MenuBar->Enable(OP_RENAME, false);
			if (m_MenuBar->FindItem(OP_REPARENT))		m_MenuBar->Enable(OP_REPARENT, false);
			if (m_MenuBar->FindItem(OP_UNDO))		m_MenuBar->Enable(OP_UNDO, false);
			if (m_MenuBar->FindItem(OP_REDO))		m_MenuBar->Enable(OP_REDO, false);

			for (int i = 0; i < m_NumOp; i++)
			{
				albaOp *o = m_OpList[i];
				if (m_MenuBar->FindItem(o->m_Id))
					m_MenuBar->Enable(o->m_Id, false);
			}
		}
		else
		{
			if (m_MenuBar->FindItem(OP_UNDO))		m_MenuBar->Enable(OP_UNDO, !m_Context.Undo_IsEmpty());
			if (m_MenuBar->FindItem(OP_REDO))		m_MenuBar->Enable(OP_REDO, !m_Context.Redo_IsEmpty());
			if (m_MenuBar->FindItem(OP_DELETE))	m_MenuBar->Enable(OP_DELETE, m_Selected && m_OpCut->Accept(m_Selected));
			if (m_MenuBar->FindItem(OP_CUT))			m_MenuBar->Enable(OP_CUT, m_Selected &&  m_OpCut->Accept(m_Selected));
			if (m_MenuBar->FindItem(OP_COPY))		m_MenuBar->Enable(OP_COPY, m_Selected &&  m_OpCopy->Accept(m_Selected));
			if (m_MenuBar->FindItem(OP_PASTE))		m_MenuBar->Enable(OP_PASTE, m_Selected &&   m_OpPaste->Accept(m_Selected));
			if (m_MenuBar->FindItem(OP_RENAME))		m_MenuBar->Enable(OP_RENAME, m_Selected &&  m_OpRename->Accept(m_Selected));
			if (m_MenuBar->FindItem(OP_REPARENT))		m_MenuBar->Enable(OP_REPARENT, m_Selected &&  m_OpReparent->Accept(m_Selected));

			for (int i = 0; i < m_NumOp; i++)
			{
				albaOp *o = m_OpList[i];
				if (m_MenuBar->FindItem(o->m_Id))
					m_MenuBar->Enable(o->m_Id, m_Selected && o->Accept(m_Selected));
			}
		}
	}
	if (m_ToolBar) EnableToolbar(CanEnable);
}
//----------------------------------------------------------------------------
void albaOpManager::EnableContextualMenu(albaGUITreeContextualMenu *contextualMenu, albaVME *node, bool CanEnable)
{
	if(contextualMenu)
	{
		if (!CanEnable)
		{
			contextualMenu->FindItem(RMENU_ADD_GROUP)->Enable(false);

			contextualMenu->FindItem(RMENU_CUT)->Enable(false);
			contextualMenu->FindItem(RMENU_COPY)->Enable(false);
			contextualMenu->FindItem(RMENU_PASTE)->Enable(false);
			contextualMenu->FindItem(RMENU_DELETE)->Enable(false);
			contextualMenu->FindItem(RMENU_RENAME)->Enable(false);
			contextualMenu->FindItem(RMENU_REPARENT)->Enable(false);
		}
		else
		{
			contextualMenu->FindItem(RMENU_ADD_GROUP)->Enable(m_OpAddGroup->Accept(node));

			contextualMenu->FindItem(RMENU_CUT)->Enable(m_OpCut->Accept(node));
			contextualMenu->FindItem(RMENU_COPY)->Enable(m_OpCopy->Accept(node));
			contextualMenu->FindItem(RMENU_PASTE)->Enable(m_OpPaste->Accept(node));
			contextualMenu->FindItem(RMENU_DELETE)->Enable(m_OpCut->Accept(node));
			contextualMenu->FindItem(RMENU_RENAME)->Enable(m_OpRename->Accept(node));
			contextualMenu->FindItem(RMENU_REPARENT)->Enable(m_OpReparent->Accept(node));
		}
	}
}
//----------------------------------------------------------------------------
bool albaOpManager::WarnUser(albaOp *op)
{
	bool go = true;
	if(m_Warn)
	{
    bool show_message = (op == NULL) || !op->CanUndo();
    if (show_message)
    {
      wxMessageDialog dialog(
        albaGetFrame(),
        _("This operation can not UnDo. Continue?"),
        _("Warning"),
        wxYES_NO|wxYES_DEFAULT
        );
      if(dialog.ShowModal() == wxID_NO) go = false;
    }
	}
  return go;
}
//----------------------------------------------------------------------------
void albaOpManager::OpRun(albaString &op_type, void *op_param)
{
  int i;
  for (i=0; i< m_NumOp; i++)
  {
    if (op_type.Equals(m_OpList[i]->GetTypeName()))
    {
      OpRun(m_OpList[i], op_param);
      break;
    }
  }
  if (i==m_NumOp)
    albaLogMessage("Error Op:\"%s\" not found",op_type);
}
//----------------------------------------------------------------------------
void albaOpManager::OpRun(int op_id)
{
	switch (op_id)
	{
	case OP_UNDO:
		OpUndo();
		break;
	case OP_REDO:
		OpRedo();
		break;

	case OP_ADD_GROUP:
		RunOpAddGroup();
		break;
	case OP_DELETE:
		RunOpDelete();
		break;
	case OP_CUT:
		RunOpCut();
		break;
	case OP_COPY:
		RunOpCopy();
		break;
	case OP_PASTE:
		RunOpPaste();
		break;
	case OP_RENAME:
		RunOpRename();
		break;
	case OP_REPARENT:
		RunOpReparentTo();
		break;

	default:
	{
		int index = op_id - OP_USER;
		if (index >= 0 && index < m_NumOp)
			OpRun(m_OpList[index]);
	}
	break;
	}
}
//----------------------------------------------------------------------------
void albaOpManager::OpRun(albaOp *op, void *op_param)
{
  if(!WarnUser(op))
  {
    return;
  }

	//Code to manage operation's Input Preserving
  albaTagItem *ti = NULL;
  albaString tag_nature = "";
  if (m_Selected && m_Selected->GetTagArray()->IsTagPresent("VME_NATURE"))
  {
    ti = m_Selected->GetTagArray()->GetTag("VME_NATURE");
    tag_nature = ti->GetValue();
  }
  
	if(!tag_nature.IsEmpty() && tag_nature.Equals("NATURAL") && !op->IsInputPreserving())
	{
		wxString warning_msg = _("The operation do not preserve input VME integrity, a copy is required! \nThis should require a lot of memory and time depending on data dimension. \nDo you want to continue?");
		wxMessageDialog dialog(albaGetFrame(),warning_msg, _("Warning"), wxYES_NO | wxYES_DEFAULT);
		if(dialog.ShowModal() == wxID_YES)
    {
      wxString synthetic_name = "Copied ";
      albaAutoPointer<albaVME> synthetic_vme = m_Selected->MakeCopy();
      synthetic_vme->ReparentTo(m_Selected->GetParent());
      synthetic_name.Append(m_Selected->GetName());
      synthetic_vme->SetName(synthetic_name);
      ti = synthetic_vme->GetTagArray()->GetTag("VME_NATURE");
      ti->SetValue("SYNTHETIC");

			GetLogicManager()->VmeShow(m_Selected, false);
      m_NaturalNode = m_Selected;
      OpSelect(synthetic_vme);
			GetLogicManager()->VmeShow(synthetic_vme, true);
    }
    else
    {
      m_RunningOp = NULL;
      return;
    }
	}

	EnableOp(false);

	m_RunningOp = op->Copy();
  m_RunningOp->m_Id = op->m_Id;    //Paolo 15/09/2004 The operation ID is not copied from the Copy() method.
	m_RunningOp->SetSetting(op->GetSetting());
  m_RunningOp->SetListener(this);
	m_RunningOp->SetInput(m_Selected);
  m_RunningOp->SetMouse(m_Mouse);

  if (op_param != NULL)
  {
    m_RunningOp->SetParameters(op_param);
  }
  albaGUISettings *settings = m_RunningOp->GetSetting();
  if (settings != NULL)
  {
    settings->SetListener(m_RunningOp);
  }

  Notify(OP_RUN_STARTING);  //SIL. 17-9-2004: - moved here in order to notify which op is started

  m_Context.Push(m_RunningOp);
  m_RunningOp->OpRun();
}

//----------------------------------------------------------------------------
void albaOpManager::RunOpAddGroup()
{
	OpRun(m_OpAddGroup);
}

//----------------------------------------------------------------------------
void albaOpManager::RunOpCut()
{
	OpRun(m_OpCut);
}
//----------------------------------------------------------------------------
void albaOpManager::RunOpCopy()
{
	OpRun(m_OpCopy);
}
//----------------------------------------------------------------------------
void albaOpManager::RunOpPaste()
{
	OpRun(m_OpPaste);
}
//----------------------------------------------------------------------------
void albaOpManager::RunOpDelete()
{
  if (WarnUser(NULL))
  {
		albaVME::albaVMESet dependenciesVMEs = m_Selected->GetDependenciesVMEs();


		//Calculating selection VME, if we delete a ancestor due to dependencies we need to select 
		//the parent of the ancestor because the ancestor and is sub-tree will be deleted
		albaVME *root = m_Selected->GetRoot();
		albaVME *newSelection = m_Selected->GetParent();
		albaVME *vme=newSelection;
		while (vme != root)
		{
			vme = vme->GetParent();
			
			if (dependenciesVMEs.find(vme) != dependenciesVMEs.end())
				newSelection = vme;
		}


		//Ask user for Removing 
		if (!dependenciesVMEs.empty())
		{
			wxString message;
			message << "There are some VME dependent on the current VME. \n";
			message << "The following VME (and relative sub-tree) will be PERMANENTLY removed:\n";

			albaVME::albaVMESet::iterator it;
			for (it = dependenciesVMEs.begin(); it != dependenciesVMEs.end(); ++it)
			{
				message << " " << (*it)->GetName() << "\n";
			}

			message << "\nDo you want to continue?";

			int answer = wxMessageBox(message, "alert", wxYES_NO | wxICON_WARNING| wxCENTRE | wxSTAY_ON_TOP);

			if (answer == wxNO)
				return;
			else 
				m_Selected->RemoveDependenciesVMEs();
		}
		
    albaVME *node_to_del = m_Selected;
    OpSelect(newSelection);

		node_to_del->ReparentTo(NULL);
		
		Notify(OP_RUN_TERMINATED, true);

		ClearUndoStack();
    albaEventMacro(albaEvent(this, CAMERA_UPDATE));
  }
	else
		Notify(OP_RUN_TERMINATED, false);
}
//----------------------------------------------------------------------------
void albaOpManager::RunOpRename()
{
	OpRun(m_OpRename);
}
//----------------------------------------------------------------------------
void albaOpManager::RunOpReparentTo()
{
	OpRun(m_OpReparent);
}

//----------------------------------------------------------------------------
void albaOpManager::OpRunOk(albaOp *op)
{	
  m_Context.Pop();
	
  m_NaturalNode = NULL;
	bool opCanUndo = op->CanUndo();

	OpDo(op);
  //Notify success with "arg" paramerter
  Notify(OP_RUN_TERMINATED,true);

	if (!opCanUndo)
	{
		delete op;
	}

	m_RunningOp = NULL;

	if(m_Context.Caller() == NULL) 	
    EnableOp();
}
//----------------------------------------------------------------------------
void albaOpManager::OpRunCancel(albaOp *op)
{
  m_Context.Pop();
		
  if (m_NaturalNode != NULL)
  {
    m_Selected->ReparentTo(NULL);
    OpSelect(m_NaturalNode);
		GetLogicManager()->VmeShow(m_NaturalNode, true);
    m_NaturalNode = NULL;
  }

  //Notify cancel with "arg" paramerter
  Notify(OP_RUN_TERMINATED,false);

	m_RunningOp = NULL;
	delete op; 

	if(m_Context.Caller() == NULL) 	
    EnableOp();
}
//----------------------------------------------------------------------------
void albaOpManager::OpExec(albaOp *op)
/** call this to exec an operation without user interaction but with undo/redo services */
{
	EnableOp(false);

	assert(op);
	if(op->Accept(m_Selected))
  {
		albaOp *o = op->Copy();
		o->SetListener(this);
		o->SetInput(m_Selected);
		OpDo(o);

		if (!o->CanUndo())
		{
			delete o;
		}
  }
	EnableOp();
}
//----------------------------------------------------------------------------
void albaOpManager::OpDo(albaOp *op)
{
  m_Context.Redo_Clear();
  albaVME *in_node = op->GetInput();
 
  if (in_node != NULL)
  {
    albaLogMessage("executing operation '%s' on input data: %s",op->m_Label.char_str(), in_node->GetName());
  }
  else
  {
    albaLogMessage("executing operation '%s'",op->m_Label.char_str());
  }
 
	if (op->GetType() != OPTYPE_EDIT)
		FillTraceabilityAttribute(op, in_node, NULL);

	op->OpDo();

	
	albaVME *out_node = op->GetOutput();
	if (out_node != NULL)
	{
		if (op->GetType() != OPTYPE_EDIT)
			FillTraceabilityAttribute(op, NULL, out_node);

		albaLogMessage("operation '%s' generate %s as output", op->m_Label.char_str(), out_node->GetName());
	}
	
  if(op->CanUndo()) 
  {
	  m_Context.Undo_Push(op);
  }
  else
  {
	  m_Context.Undo_Clear();
  }
}

//----------------------------------------------------------------------------
void albaOpManager::SetMafUser(albaUser *user)
{
  m_User = user;
}

//----------------------------------------------------------------------------
void albaOpManager::FillTraceabilityAttribute(albaOp *op, albaVME *in_node, albaVME *out_node)
{
  albaString trialEvent = "Modify";
  albaString operationName;
  albaString parameters;
  albaString appStamp;
  albaString userID;
  albaString isNatural = "false";
  wxString revision;
  wxString dateAndTime;

  operationName = op->GetTypeName();
  parameters = op->GetParameters();

  wxDateTime time = wxDateTime::UNow();
  dateAndTime  = albaString::Format("%02d/%02d/%02d %02d:%02d:%02d",time.GetDay(), time.GetMonth()+1, time.GetYear(), time.GetHour(), time.GetMinute(),time.GetSecond());

  if (m_User != NULL && m_User->IsAuthenticated())
      userID = m_User->GetName();

  if (in_node != NULL)
  {
    albaAttributeTraceability *traceability = (albaAttributeTraceability *)in_node->GetAttribute("TrialAttribute");
    if (traceability == NULL)
    {
      traceability = albaAttributeTraceability::New();
      traceability->SetName("TrialAttribute");
      in_node->SetAttribute("TrialAttribute", traceability);
    }

    if(in_node->GetRoot()->GetTagArray()->IsTagPresent("APP_STAMP"))
      appStamp = in_node->GetRoot()->GetTagArray()->GetTag("APP_STAMP")->GetValue();

    if(in_node->GetTagArray()->IsTagPresent("VME_NATURE"))
    {
      isNatural = in_node->GetTagArray()->GetTag("VME_NATURE")->GetValue();
      if (isNatural.Compare("NATURAL") == 0)
        isNatural = "true";
      else
        isNatural = "false";
    }
    traceability->AddTraceabilityEvent(trialEvent, operationName, parameters, dateAndTime, appStamp, m_BuildNum, userID, isNatural);
  }

  if (out_node != NULL)
  {
    int c = 0; //counter not to write single parameter on first VME which is a group
    wxString singleParameter = parameters.GetCStr();
    albaVMEIterator *iter = out_node->NewIterator();
    for (albaVME *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
    {
      if (node != NULL)
      {
        c++;
        albaAttributeTraceability *traceability = (albaAttributeTraceability *)node->GetAttribute("TrialAttribute");
        if (traceability == NULL)
        {
          trialEvent = "Create";
          traceability = albaAttributeTraceability::New();
          traceability->SetName("TrialAttribute");
          node->SetAttribute("TrialAttribute", traceability);
        }
        else
        {
          albaString trial = traceability->GetLastTrialEvent();
          if (trial.IsEmpty())
            trialEvent = "Create";
        }

        if(out_node->GetRoot()->GetTagArray()->IsTagPresent("APP_STAMP"))
          appStamp = out_node->GetRoot()->GetTagArray()->GetTag("APP_STAMP")->GetValue();

        if(out_node->GetTagArray()->IsTagPresent("VME_NATURE"))
        {
          isNatural = out_node->GetTagArray()->GetTag("VME_NATURE")->GetValue();
          if (isNatural.Compare("NATURAL") == 0 )
            isNatural = "true";
          else
            isNatural = "false";
        }

        if (out_node->GetNumberOfChildren() == 0 || c == 1)
        {
           traceability->AddTraceabilityEvent(trialEvent, operationName, parameters, dateAndTime, appStamp,m_BuildNum, userID, isNatural);
        }
        else
        {
          int count = singleParameter.find_first_of('=');
          wxString par = singleParameter.Mid(0, count);
          singleParameter = singleParameter.AfterFirst('=');
          count = singleParameter.Find(par.char_str());
          par.Append("=");
          par.Append(singleParameter.substr(0, count-2));
          singleParameter = singleParameter.Mid(count);
          traceability->AddTraceabilityEvent(trialEvent, operationName, par, dateAndTime, appStamp,m_BuildNum, userID, isNatural);
        }
      }
    }
    iter->Delete();
  }
}
//----------------------------------------------------------------------------
void albaOpManager::OpUndo()
{
  if( m_Context.Undo_IsEmpty()) 
  {
    albaLogMessage(_("empty undo stack"));
    return;
  }
	EnableOp(false);

	albaOp* op = m_Context.Undo_Pop();
  albaVME *in_node = op->GetInput();
  albaVME *out_node = op->GetOutput();
  if (in_node != NULL)
  {
    albaLogMessage("undo = %s on input data: %s",op->m_Label.char_str(), in_node->GetName());
    albaAttributeTraceability *traceability = (albaAttributeTraceability *)in_node->GetAttribute("TrialAttribute");
    if (traceability != NULL)
    {
      traceability->RemoveTraceabilityEvent();
      albaString trial = traceability->GetLastTrialEvent();
      if (trial.IsEmpty())
        in_node->RemoveAttribute("TrialAttribute");
    }
  }
  else
  {
    albaLogMessage("undo = %s",op->m_Label.char_str());
  }

  if (out_node != NULL)
  {
    albaVMEIterator *iter = out_node->NewIterator();
    for (albaVME *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
    {
      if (node != NULL)
      {
        albaAttributeTraceability *traceability = (albaAttributeTraceability *)node->GetAttribute("TrialAttribute");
        if (traceability != NULL)
        {
          traceability->RemoveTraceabilityEvent();
        }
      }
    }
    iter->Delete();
  }

	op->OpUndo();
	m_Context.Redo_Push(op);

	EnableOp();
}
//----------------------------------------------------------------------------
void albaOpManager::OpRedo()   
{
  if( m_Context.Redo_IsEmpty())
  {
    albaLogMessage("empty redo stack");
    return;
  }
	EnableOp(false);

	albaOp* op = m_Context.Redo_Pop();
  albaVME *in_node = op->GetInput();
  albaVME *out_node = op->GetOutput();
  albaString parameters = op->GetParameters();
  if (in_node != NULL)
  {
    albaLogMessage("redo = %s on input data: %s",op->m_Label.char_str(), in_node->GetName());
  }
  else
  {
    albaLogMessage("redo = %s",op->m_Label.char_str());
  }
	op->OpDo();
  
	m_Context.Undo_Push(op);

  FillTraceabilityAttribute(op, in_node, out_node);

	EnableOp();
}
//----------------------------------------------------------------------------
void albaOpManager::Notify(int msg, long arg)   
{
	if(m_Context.Caller() == NULL)
		albaEventMacro(albaEvent(this,msg,m_RunningOp,arg));  
	
}
//----------------------------------------------------------------------------
bool albaOpManager::StopCurrentOperation()
{
//  assert(false); //SIL. 2-7-2004: -- Seems that no-one is using this -- but tell me if the program stop here -- thanks
  albaOp *prev_running_op = m_RunningOp;
	
	while(m_RunningOp) //loop danger !!!
	{
    if(m_RunningOp->OkEnabled())
		  m_RunningOp->ForceStopWithOk();
    else
		  m_RunningOp->ForceStopWithCancel();

    //if(prev_running_op == m_RunningOp)
		//{
      //previous try failed, be more bad
			//OpRunCancel(m_RunningOp); - wrong: op-gui will be still plugged
		//}

    if(prev_running_op == m_RunningOp)
		{
			break; //second try failed, give it up and break to prevent loops
		}
	}
  return (m_RunningOp == NULL);
}
//----------------------------------------------------------------------------
albaOp *albaOpManager::GetRunningOperation()
{
  return m_RunningOp;
}
//----------------------------------------------------------------------------
bool albaOpManager::ForceStopWithOk()
{
  if(!m_RunningOp) 
    return false;
  albaOp *to_be_killed = m_RunningOp;

  if(m_RunningOp->OkEnabled())
    m_RunningOp->ForceStopWithOk();
  
  return m_RunningOp != to_be_killed;
}
//----------------------------------------------------------------------------
bool albaOpManager::ForceStopWithCancel()
{
  if(!m_RunningOp) 
    return false;
  albaOp *to_be_killed = m_RunningOp;

  m_RunningOp->ForceStopWithCancel();

  return m_RunningOp != to_be_killed;
}
//----------------------------------------------------------------------------
void albaOpManager::SetMouse(albaDeviceButtonsPadMouse *mouse)
{
  m_Mouse = mouse;
}

//----------------------------------------------------------------------------
void albaOpManager::RefreshMenu()
{
  EnableOp(!Running());
}
//----------------------------------------------------------------------------
albaOp *albaOpManager::GetOperationById(int id)
{
  std::vector<albaOp *>::iterator it = m_OpList.begin();
  for(int i=0;i<m_OpList.size(); i++)
  {
    if(m_OpList[i]->m_Id == id)
      return m_OpList[i];
  }

  return NULL;
}
