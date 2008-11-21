/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpManager.cpp,v $
  Language:  C++
  Date:      $Date: 2008-11-21 15:37:41 $
  Version:   $Revision: 1.40.2.6 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "mafOpManager.h"
#include <wx/tokenzr.h>
#include "mmuIdFactory.h"
#include "mafDecl.h"
#include "mafOp.h"
#include "mafUser.h"
#include "mafGUI.h"
#include "mafGUISettings.h"
#include "mafOpStack.h"
#include "mafOpContextStack.h"
#include "mafOpSelect.h"
#include "mafGUISettingsDialog.h"
#include "mafAttributeTraceability.h"
#include "mmdMouse.h"

#include "mafStorage.h"
#include "mafEventIO.h"
#include "mafDataVector.h"
#include "mafTagArray.h"
#include "mafTagItem.h"
#include "mafNode.h"
#include "mafVMEGenericAbstract.h"

//------------------------------------------------------------------------------
// Events
//------------------------------------------------------------------------------
MAF_ID_IMP(mafOpManager::OPERATION_INTERFACE_EVENT)
MAF_ID_IMP(mafOpManager::RUN_OPERATION_EVENT)

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
mafOpManager::mafOpManager()
//----------------------------------------------------------------------------
{
  m_Listener	     = NULL;
  m_RemoteListener = NULL;
  m_RunningOp      = NULL;
	m_Selected	     = NULL;
  m_NaturalNode    = NULL;
  m_User           = NULL;
  m_Warn           = true;
  m_FromRemote     = false;

  m_OpParameters = NULL;
  
	m_Menu[OPTYPE_IMPORTER] = NULL;
  m_Menu[OPTYPE_EXPORTER] = NULL;
  m_Menu[OPTYPE_OP] = NULL;

  m_OpSelect    = new mafOpSelect();
  m_OpCut       = new mafOpCut();
  m_OpCopy      = new mafOpCopy();
  m_OpPaste     = new mafOpPaste();
  //m_optransform = new mafOpTransform();

	m_ToolBar = NULL;
	m_MenuBar = NULL;

  m_CollaborateStatus = false;

  m_NumOfAccelerators = 0;
  m_NumOp = 0;
  m_OpList.clear();
}
//----------------------------------------------------------------------------
mafOpManager::~mafOpManager()
//----------------------------------------------------------------------------
{
  // clear clipboard to avoid crash if some VME is still into the clipboard.
  m_Context.Clear();
  m_OpCut->ClipboardClear();

  for(int i = 0; i < m_NumOp; i++)
  {
    mafGUISettings *s = m_OpList[i]->GetSetting();
    if (s != NULL)
    {
      delete s;
    }
    delete m_OpList[i];
  }
  m_OpList.clear();

  cppDEL(m_OpSelect);
  cppDEL(m_OpCut);
  cppDEL(m_OpCopy);
  cppDEL(m_OpPaste);
  cppDEL(m_User);
}
//----------------------------------------------------------------------------
void mafOpManager::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  mafOp* o = NULL; 

  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
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
        o = (mafOp*) e->GetSender();
        assert(o == m_Context.Caller());
        OpRunOk(o);
      break; 
      case OP_RUN_CANCEL:
        o = (mafOp*) e->GetSender();
        assert(o == m_Context.Caller());
        OpRunCancel(o);
      break;
      case REMOTE_PARAMETER:
        if(m_CollaborateStatus && m_RemoteListener)
        {
          // Send the event to synchronize the remote application in case of collaboration modality
          long w_id = e->GetArg();
          WidgetDataType w_data;
          w_data.dValue  = 0.0;
          w_data.fValue  = 0.0;
          w_data.iValue  = 0;
          w_data.sValue  = "";
          w_data.dType = NULL_DATA;
          m_RunningOp->GetGui()->GetWidgetValue(w_id, w_data);
          mafEvent ev(this,OPERATION_INTERFACE_EVENT,w_data,w_id);
          ev.SetChannel(REMOTE_COMMAND_CHANNEL);
          m_RemoteListener->OnEvent(&ev);
        }
      break;
      default:
        mafEventMacro(*e);
      break;
    }
  }
}
//----------------------------------------------------------------------------
void mafOpManager::OpAdd(mafOp *op, wxString menuPath, bool can_undo, mafGUISettings *setting)
//----------------------------------------------------------------------------
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
/*void mafOpManager::FillMenu (wxMenu* import, wxMenu* mexport, wxMenu* operations)
//----------------------------------------------------------------------------
{
  int submenu_id = 1;
  
  m_Menu[OPTYPE_IMPORTER] = import;
  m_Menu[OPTYPE_EXPORTER] = mexport;
  m_Menu[OPTYPE_OP] = operations;
   
	for(int i=0; i<m_NumOp; i++)
  {
    mafOp *o = m_OpList[i];
    if (o->m_OpMenuPath != "")
    {
      wxMenu *sub_menu = NULL;

      int item = m_Menu[o->GetType()]->FindItem(o->m_OpMenuPath);
      if (item != wxNOT_FOUND)
      {
         wxMenuItem *menu_item = m_Menu[o->GetType()]->FindItem(item);
         if (menu_item)
           sub_menu = menu_item->GetSubMenu();
      }
      else
      {
        sub_menu = new wxMenu;
        m_Menu[o->GetType()]->Append(submenu_id++,o->m_OpMenuPath,sub_menu);
      }
      
      if(sub_menu)
        sub_menu->Append(o->m_Id, o->m_Label, o->m_Label);
      else
        mafLogMessage("error in FillMenu");
    }
    else
    {
      m_Menu[o->GetType()]->Append(o->m_Id, o->m_Label, o->m_Label);
    }
    SetAccelerator(o);
  }
  wxAcceleratorTable accel(MAXOP, m_OpAccelEntries);
  if (accel.Ok())
    mafGetFrame()->SetAcceleratorTable(accel);
}*/
//----------------------------------------------------------------------------
void mafOpManager::FillSettingDialog(mafGUISettingsDialog *settingDialog)
//----------------------------------------------------------------------------
{
  for(int i=0; i<m_NumOp; i++)
  {
    mafOp *o = m_OpList[i];
    mafGUISettings *setting = o->GetSetting();
    if (setting != NULL)
    {
      settingDialog->AddPage(setting->GetGui(), setting->GetLabel());
    }
  }
}
//----------------------------------------------------------------------------
void mafOpManager::FillMenu(wxMenu* import, wxMenu* mexport, wxMenu* operations)
//----------------------------------------------------------------------------
{
  int submenu_id = 1;

  m_Menu[OPTYPE_IMPORTER] = import;
  m_Menu[OPTYPE_EXPORTER] = mexport;
  m_Menu[OPTYPE_OP] = operations;

  for(int i=0; i<m_NumOp; i++)
  {
    mafOp *o = m_OpList[i];
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
        sub_menu->Append(o->m_Id, _(o->m_Label), _(o->m_Label));
      else
        mafLogMessage(_("error in FillMenu"));
    }
    else
    {
      m_Menu[o->GetType()]->Append(o->m_Id, _(o->m_Label), _(o->m_Label));
    }
    SetAccelerator(o);
  }
  wxAcceleratorTable accel(MAXOP, m_OpAccelEntries);
  if (accel.Ok())
    mafGetFrame()->SetAcceleratorTable(accel);
}
//----------------------------------------------------------------------------
void mafOpManager::SetAccelerator(mafOp *op)
//----------------------------------------------------------------------------
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

    m_OpAccelEntries[m_NumOfAccelerators++].Set(flag_num,  (int) *key_code.c_str(), op->m_Id);
  }
}
//----------------------------------------------------------------------------
void mafOpManager::VmeSelected(mafNode* v)   
//----------------------------------------------------------------------------
{
  m_Selected = v;
  EnableOp();
}
//----------------------------------------------------------------------------
mafNode* mafOpManager::GetSelectedVme()
//----------------------------------------------------------------------------
{
	return m_Selected;
}
//----------------------------------------------------------------------------
void mafOpManager::OpSelect(mafNode* vme)
//----------------------------------------------------------------------------
{
	if(vme == m_Selected ) 
    return;
	m_OpSelect->SetNewSel(vme);
	OpExec(m_OpSelect);
  mafLogMessage("node selected: %s", vme->GetName());
}
/*
//----------------------------------------------------------------------------
void mafOpManager::OpTransform(vtkMatrix4x4* new_matrix,vtkMatrix4x4* old_matrix)
//----------------------------------------------------------------------------
{
	m_optransform->SetNewMatrix(new_matrix);
	m_optransform->SetOldMatrix(old_matrix);
	OpExec(m_optransform); 
}
*/
//----------------------------------------------------------------------------
void mafOpManager::ClearUndoStack()
//----------------------------------------------------------------------------
{
   m_Context.Clear();
   m_OpCut->ClipboardClear();
	 EnableOp();
}
//----------------------------------------------------------------------------
void mafOpManager::EnableToolbar(bool CanEnable) 
//----------------------------------------------------------------------------
{
	if(!CanEnable)
	{
/*		m_ToolBar->EnableTool(MENU_FILE_NEW,false);	//Removed by Paolo 15-9-2003: HAL men
		m_ToolBar->EnableTool(MENU_FILE_OPEN,false);
		m_ToolBar->EnableTool(MENU_FILE_SAVE,false);*/
		m_ToolBar->EnableTool(OP_UNDO,false);
		m_ToolBar->EnableTool(OP_REDO,false);
		m_ToolBar->EnableTool(OP_CUT,false);
		m_ToolBar->EnableTool(OP_COPY,false);
		m_ToolBar->EnableTool(OP_PASTE,false);
	}
	else
	{
/*		m_ToolBar->EnableTool(MENU_FILE_NEW,true);
		m_ToolBar->EnableTool(MENU_FILE_OPEN,true);
		m_ToolBar->EnableTool(MENU_FILE_SAVE,true);*/
		m_ToolBar->EnableTool(OP_UNDO,	!m_Context.Undo_IsEmpty());
		m_ToolBar->EnableTool(OP_REDO,	!m_Context.Redo_IsEmpty());
		m_ToolBar->EnableTool(OP_CUT,		m_OpCut->Accept(m_Selected) );
		m_ToolBar->EnableTool(OP_COPY,	m_OpCopy->Accept(m_Selected) );
		m_ToolBar->EnableTool(OP_PASTE,	m_OpPaste->Accept(m_Selected) );
	}
}
//----------------------------------------------------------------------------
void mafOpManager::EnableOp(bool CanEnable) 
//----------------------------------------------------------------------------
{
  mafOp *o = NULL;
	if(m_MenuBar)
  {
    if(!CanEnable)
	  {
		  if(m_MenuBar->FindItem(OP_DELETE))	m_MenuBar->Enable(OP_DELETE,false);
		  if(m_MenuBar->FindItem(OP_CUT))			m_MenuBar->Enable(OP_CUT,false);
		  if(m_MenuBar->FindItem(OP_COPY))		m_MenuBar->Enable(OP_COPY,false);
		  if(m_MenuBar->FindItem(OP_PASTE))		m_MenuBar->Enable(OP_PASTE,false);
		  if(m_MenuBar->FindItem(OP_UNDO))		m_MenuBar->Enable(OP_UNDO,false); 
		  if(m_MenuBar->FindItem(OP_REDO))		m_MenuBar->Enable(OP_REDO,false); 

		  for(int i=0; i<m_NumOp; i++)
		  {
        mafOp *o = m_OpList[i];
        if(m_MenuBar->FindItem(o->m_Id))
          m_MenuBar->Enable(o->m_Id,false); 
      }
	  }
	  else
	  {
		  if(m_MenuBar->FindItem(OP_UNDO))		m_MenuBar->Enable(OP_UNDO,    !m_Context.Undo_IsEmpty());
		  if(m_MenuBar->FindItem(OP_REDO))		m_MenuBar->Enable(OP_REDO,    !m_Context.Redo_IsEmpty());
		  if(m_MenuBar->FindItem(OP_DELETE))	m_MenuBar->Enable(OP_DELETE,  m_OpCut->Accept(m_Selected));
		  if(m_MenuBar->FindItem(OP_CUT))			m_MenuBar->Enable(OP_CUT,     m_OpCut->Accept(m_Selected));
		  if(m_MenuBar->FindItem(OP_COPY))		m_MenuBar->Enable(OP_COPY,    m_OpCopy->Accept(m_Selected));
		  if(m_MenuBar->FindItem(OP_PASTE))		m_MenuBar->Enable(OP_PASTE,   m_OpPaste->Accept(m_Selected)); 

		  for(int i=0; i<m_NumOp; i++)
		  {
        mafOp *o = m_OpList[i]; 
        if(m_MenuBar->FindItem(o->m_Id))
          m_MenuBar->Enable(o->m_Id,o->Accept(m_Selected)); 
      }
	  }
  }
	if(m_ToolBar) EnableToolbar(CanEnable);
}
//----------------------------------------------------------------------------
bool mafOpManager::WarnUser(mafOp *op)
//----------------------------------------------------------------------------
{
	bool go = true;
	if(m_Warn)
	{
    bool show_message = (op == NULL) || !op->CanUndo();
    if (show_message)
    {
      wxMessageDialog dialog(
        mafGetFrame(),
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
void mafOpManager::OpRun(mafString &op_type, void *op_param)
//----------------------------------------------------------------------------
{
  for (int i=0; i< m_NumOp; i++)
  {
    if (op_type.Equals(m_OpList[i]->GetTypeName()))
    {
      OpRun(m_OpList[i], op_param);
      break;
    }
  }
}
//----------------------------------------------------------------------------
void mafOpManager::OpRun(int op_id)
//----------------------------------------------------------------------------
{
  switch(op_id)
  {
	  case OP_UNDO:
			OpUndo();
	  break;
	  case OP_REDO:
			OpRedo();
	  break;
	  case OP_DELETE:
    {
      if (WarnUser(NULL))
      {
        mafNode *node_to_del = m_Selected;
        OpSelect(m_Selected->GetParent());

        // do not remove binary files but fill a list with files to be deleted on save by the storage.
        mafEventIO e(this,NODE_GET_STORAGE);
        node_to_del->ForwardUpEvent(e);
        mafStorage *storage = e.GetStorage();
        mafNodeIterator *iter = node_to_del->NewIterator();
        mafString data_filename;
        for (mafNode *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
        {
          if(mafVMEGenericAbstract::SafeDownCast(node))
          {
            mafVMEGenericAbstract *vme = mafVMEGenericAbstract::SafeDownCast(node);
            mafDataVector *dv = vme->GetDataVector();
            if (dv != NULL)
            {
              if (dv->GetSingleFileMode())
              {
                mafString archive_filename = dv->GetArchiveName();
                if (archive_filename != "")
                {
                  storage->ReleaseURL(archive_filename);
                }
              }
              else
              {
                mafVMEItem *item;
                int i;
                for (i = 0; i < dv->GetNumberOfItems(); i++)
                {
                  item = dv->GetItemByIndex(i);
                  data_filename = item->GetURL();
                  storage->ReleaseURL(data_filename);
                }
              }
            }
          }
        }
        iter->Delete();
        node_to_del->ReparentTo(NULL);
        ClearUndoStack();
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
      }
    }
    break;
	  case OP_CUT:
			OpExec(m_OpCut);
	  break;
	  case OP_COPY:
			OpExec(m_OpCopy);
	  break;
	  case OP_PASTE:
			OpExec(m_OpPaste);
	  break;
	  default:
		{
      int index = op_id - OP_USER;
		  if(index >=0 && index <m_NumOp) 
        OpRun(m_OpList[index]);
		}
	  break;
	}
}
//----------------------------------------------------------------------------
void mafOpManager::OpRun(mafOp *op, void *op_param)
//----------------------------------------------------------------------------
{
  if(!WarnUser(op))
  {
    return;
  }

	//Code to manage operation's Input Preserving
  mafTagItem *ti = NULL;
  mafString tag_nature = "";
  if (m_Selected->GetTagArray()->IsTagPresent("VME_NATURE"))
  {
    ti = m_Selected->GetTagArray()->GetTag("VME_NATURE");
    tag_nature = ti->GetValue();
  }
  
	if(!tag_nature.IsEmpty() && tag_nature.Equals("NATURAL") && !op->IsInputPreserving())
	{
		wxString warning_msg = _("The operation do not preserve input VME integrity, a copy is required! \nThis should require a lot of memory and time depending on data dimension. \nDo you want to continue?");
		wxMessageDialog dialog(mafGetFrame(),warning_msg, _("Warning"), wxYES_NO | wxYES_DEFAULT);
		if(dialog.ShowModal() == wxID_YES)
    {
      wxString synthetic_name = "Copied ";
      mafAutoPointer<mafNode> synthetic_vme = m_Selected->MakeCopy();
      synthetic_vme->ReparentTo(m_Selected->GetParent());
      synthetic_name.Append(m_Selected->GetName());
      synthetic_vme->SetName(synthetic_name);
      ti = synthetic_vme->GetTagArray()->GetTag("VME_NATURE");
      ti->SetValue("SYNTHETIC");
      mafEventMacro(mafEvent(this,VME_SHOW,m_Selected,false));
      m_NaturalNode = m_Selected;
      OpSelect(synthetic_vme);
      mafEventMacro(mafEvent(this,VME_SHOW,synthetic_vme,true));
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
  m_RunningOp->Collaborate(m_CollaborateStatus);
  if (op_param != NULL)
  {
    m_RunningOp->SetParameters(op_param);
  }
  mafGUISettings *settings = m_RunningOp->GetSetting();
  if (settings != NULL)
  {
    settings->SetListener(m_RunningOp);
  }

  Notify(OP_RUN_STARTING);  //SIL. 17-9-2004: - moved here in order to notify which op is started

  m_Context.Push(m_RunningOp);
  m_RunningOp->OpRun();
}
//----------------------------------------------------------------------------
void mafOpManager::OpRunOk(mafOp *op)
//----------------------------------------------------------------------------
{	
  m_Context.Pop();

  m_RunningOp = NULL;
  m_NaturalNode = NULL;
	OpDo(op);
  Notify(OP_RUN_TERMINATED);
	if(m_Context.Caller() == NULL) 	
    EnableOp();
}
//----------------------------------------------------------------------------
void mafOpManager::OpRunCancel(mafOp *op)
//----------------------------------------------------------------------------
{
  m_Context.Pop();

	m_RunningOp = NULL;
	delete op;

  if (m_NaturalNode != NULL)
  {
    m_Selected->ReparentTo(NULL);
    OpSelect(m_NaturalNode);
    mafEventMacro(mafEvent(this,VME_SHOW,m_NaturalNode,true));
    m_NaturalNode = NULL;
  }

  Notify(OP_RUN_TERMINATED);

	if(m_Context.Caller() == NULL) 	
    EnableOp();
}
//----------------------------------------------------------------------------
void mafOpManager::OpExec(mafOp *op)
/** call this to exec an operation without user interaction but with undo/redo services */
//----------------------------------------------------------------------------
{
	EnableOp(false);

	assert(op);
	if(op->Accept(m_Selected))
  {
		mafOp *o = op->Copy();
		o->SetListener(this);
		o->SetInput(m_Selected);
		OpDo(o);
  }
	EnableOp();
}
//----------------------------------------------------------------------------
void mafOpManager::OpDo(mafOp *op)
//----------------------------------------------------------------------------
{
  m_Context.Redo_Clear();
  op->OpDo();
  mafNode *in_node = op->GetInput();
  mafNode *out_node = op->GetOutput();

  if (in_node != NULL)
  {
    mafLogMessage("executed operation '%s' on input data: %s",op->m_Label.c_str(), in_node->GetName());
  }
  else
  {
    mafLogMessage("executed operation '%s'",op->m_Label.c_str());
  }
  if (out_node != NULL)
  {
    mafLogMessage("operation '%s' generate %s as output",op->m_Label.c_str(), out_node->GetName());
  }

  if (op->GetType() != OPTYPE_EDIT)
      FillTraceabilityAttribute(op, in_node, out_node);

  if(op->CanUndo()) 
  {
	  m_Context.Undo_Push(op);
  }
  else
  {
	  m_Context.Undo_Clear();
    delete op;  
  }
}

//----------------------------------------------------------------------------
void mafOpManager::SetMafUser(mafUser *user)
//----------------------------------------------------------------------------
{
  m_User = user;
}

//----------------------------------------------------------------------------
void mafOpManager::FillTraceabilityAttribute(mafOp *op, mafNode *in_node, mafNode *out_node)
//----------------------------------------------------------------------------
{
  mafString trialEvent = "Modify";
  mafString operationName;
  mafString parameters;
  mafString appStamp;
  mafString userID;
  mafString isNatural;
  wxString revision;
  wxString dateAndTime;

  operationName = op->GetTypeName();
  parameters = op->GetParameters();

  wxDateTime time = wxDateTime::UNow();
  dateAndTime  = wxString::Format("%02d/%02d/%02d %02d:%02d:%02d",time.GetDay(), time.GetMonth(), time.GetYear(), time.GetHour(), time.GetMinute(),time.GetSecond());

  if (m_User != NULL && m_User->IsAuthenticated())
      userID = m_User->GetName();

  if (in_node != NULL)
  {
    mafAttributeTraceability *traceability = (mafAttributeTraceability *)in_node->GetAttribute("TrialAttribute");
    if (traceability == NULL)
    {
      traceability = mafAttributeTraceability::New();
      traceability->SetName("TrialAttribute");
      in_node->SetAttribute("TrialAttribute", traceability);
    }

    if(in_node->GetRoot()->GetTagArray()->IsTagPresent("APP_STAMP"))
      appStamp = in_node->GetRoot()->GetTagArray()->GetTag("APP_STAMP")->GetValue();


#ifdef _WIN32
    mafString regKeyPath = "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\";
    regKeyPath.Append(appStamp.GetCStr());

    wxRegKey RegKey(wxString(regKeyPath.GetCStr()));
    if(RegKey.Exists())
    {
      RegKey.Create();
      RegKey.QueryValue(wxString("DisplayVersion"), revision);
    }
    appStamp.Append(" ");
    appStamp.Append(revision.c_str());
#endif
   
    if(in_node->GetTagArray()->IsTagPresent("VME_NATURE"))
      isNatural = in_node->GetTagArray()->GetTag("VME_NATURE")->GetValue();

    traceability->AddTraceabilityEvent(trialEvent, operationName, parameters, dateAndTime, appStamp, userID, isNatural);
  }

  if (out_node != NULL)
  {
    int c = 0; //counter not to write single parameter on first VME which is a group
    wxString singleParameter = parameters;
    mafNodeIterator *iter = out_node->NewIterator();
    for (mafNode *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
    {
      if (node != NULL)
      {
        c++;
        mafAttributeTraceability *traceability = (mafAttributeTraceability *)node->GetAttribute("TrialAttribute");
        if (traceability == NULL)
        {
          trialEvent = "Create";
          traceability = mafAttributeTraceability::New();
          traceability->SetName("TrialAttribute");
          node->SetAttribute("TrialAttribute", traceability);
        }
        else
        {
          mafString trial = traceability->GetLastTrialEvent();
          if (trial.IsEmpty())
            trialEvent = "Create";
        }

        if(in_node->GetRoot()->GetTagArray()->IsTagPresent("APP_STAMP"))
          appStamp = in_node->GetRoot()->GetTagArray()->GetTag("APP_STAMP")->GetValue();


        mafString regKeyPath = "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\";
        regKeyPath.Append(appStamp.GetCStr());

        wxRegKey RegKey(wxString(regKeyPath.GetCStr()));
        if(RegKey.Exists())
        {
          RegKey.Create();
          RegKey.QueryValue(wxString("DisplayName"), revision);
        }

        if(node->GetTagArray()->IsTagPresent("VME_NATURE"))
          isNatural = node->GetTagArray()->GetTag("VME_NATURE")->GetValue();

        if (out_node->GetNumberOfChildren() == 0 || c == 1)
        {
           traceability->AddTraceabilityEvent(trialEvent, operationName, parameters, dateAndTime, appStamp, userID, isNatural);
        }
        else
        {
          int count = singleParameter.find_first_of('=');
          wxString par = singleParameter.Mid(0, count);
          singleParameter = singleParameter.AfterFirst('=');
          count = singleParameter.Find(par.c_str());
          par.Append("=");
          par.Append(singleParameter.substr(0, count-2));
          singleParameter = singleParameter.Mid(count);
          traceability->AddTraceabilityEvent(trialEvent, operationName, par, dateAndTime, appStamp, userID, isNatural);
        }
      }
    }
    iter->Delete();
  }
}
//----------------------------------------------------------------------------
void mafOpManager::OpUndo()
//----------------------------------------------------------------------------
{
  if( m_Context.Undo_IsEmpty()) 
  {
    mafLogMessage(_("empty undo stack"));
    return;
  }
	EnableOp(false);

	mafOp* op = m_Context.Undo_Pop();
  mafNode *in_node = op->GetInput();
  mafNode *out_node = op->GetOutput();
  if (in_node != NULL)
  {
    mafLogMessage("undo = %s on input data: %s",op->m_Label.c_str(), in_node->GetName());
    mafAttributeTraceability *traceability = (mafAttributeTraceability *)in_node->GetAttribute("TrialAttribute");
    if (traceability != NULL)
    {
      traceability->RemoveTraceabilityEvent();
      mafString trial = traceability->GetLastTrialEvent();
      if (trial.IsEmpty())
        in_node->RemoveAttribute("TrialAttribute");
    }
  }
  else
  {
    mafLogMessage("undo = %s",op->m_Label.c_str());
  }

  if (out_node != NULL)
  {
    mafNodeIterator *iter = out_node->NewIterator();
    for (mafNode *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
    {
      if (node != NULL)
      {
        mafAttributeTraceability *traceability = (mafAttributeTraceability *)node->GetAttribute("TrialAttribute");
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
void mafOpManager::OpRedo()   
//----------------------------------------------------------------------------
{
  if( m_Context.Redo_IsEmpty())
  {
    mafLogMessage("empty redo stack");
    return;
  }
	EnableOp(false);

	mafOp* op = m_Context.Redo_Pop();
  mafNode *in_node = op->GetInput();
  mafNode *out_node = op->GetOutput();
  mafString parameters = op->GetParameters();
  if (in_node != NULL)
  {
    mafLogMessage("redo = %s on input data: %s",op->m_Label.c_str(), in_node->GetName());
  }
  else
  {
    mafLogMessage("redo = %s",op->m_Label.c_str());
  }
	op->OpDo();
  
	m_Context.Undo_Push(op);

  FillTraceabilityAttribute(op, in_node, out_node);

	EnableOp();
}
//----------------------------------------------------------------------------
void mafOpManager::Notify(int msg)   
//----------------------------------------------------------------------------
{
	if(m_Context.Caller() == NULL)
	// not a nested operation - notify logic
		mafEventMacro(mafEvent(this,msg,m_RunningOp));   //SIL. 17-9-2004: added the m_RunningOp at the event (may be NULL)
	//else
	// nested operation - notify caller
		    // m_Context.Caller()->OnEvent(mafEvent(this,msg));   
        // NO - it is dangerous - if the caller don't handle the msg,
				// msg will be bounced and then forwarded to Logic
				// better wait until this feature is really needed 
}
//----------------------------------------------------------------------------
bool mafOpManager::StopCurrentOperation()
//----------------------------------------------------------------------------
{
//  assert(false); //SIL. 2-7-2004: -- Seems that no-one is using this -- but tell me if the program stop here -- thanks
  mafOp *prev_running_op = m_RunningOp;
	
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
mafOp *mafOpManager::GetRunningOperation()
//----------------------------------------------------------------------------
{
  return m_RunningOp;
}
//----------------------------------------------------------------------------
bool mafOpManager::ForceStopWithOk()
//----------------------------------------------------------------------------
{
  if(!m_RunningOp) 
    return false;
  mafOp *to_be_killed = m_RunningOp;

  if(m_RunningOp->OkEnabled())
    m_RunningOp->ForceStopWithOk();
  
  return m_RunningOp != to_be_killed;
}
//----------------------------------------------------------------------------
bool mafOpManager::ForceStopWithCancel()
//----------------------------------------------------------------------------
{
  if(!m_RunningOp) 
    return false;
  mafOp *to_be_killed = m_RunningOp;

  m_RunningOp->ForceStopWithCancel();

  return m_RunningOp != to_be_killed;
}
//----------------------------------------------------------------------------
void mafOpManager::SetMouse(mmdMouse *mouse)
//----------------------------------------------------------------------------
{
  m_Mouse = mouse;
}
//----------------------------------------------------------------------------
void mafOpManager::Collaborate(bool status)
//----------------------------------------------------------------------------
{
  m_CollaborateStatus = status;

  for(int index = 0; index < m_NumOp; index++)
    m_OpList[index]->Collaborate(status);
}
//----------------------------------------------------------------------------
void mafOpManager::RefreshMenu()
//----------------------------------------------------------------------------
{
  EnableOp(!Running());
}
//----------------------------------------------------------------------------
mafOp *mafOpManager::GetOperationById(int id)
//----------------------------------------------------------------------------
{
  std::vector<mafOp *>::iterator it = m_OpList.begin();
  for(int i=0;i<m_OpList.size(); i++)
  {
    if(m_OpList[i]->m_Id == id)
      return m_OpList[i];
  }

  return NULL;
}
