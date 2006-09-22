/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpManager.cpp,v $
  Language:  C++
  Date:      $Date: 2006-09-22 10:07:11 $
  Version:   $Revision: 1.20 $
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
#include "mmgGui.h"
#include "mafOpStack.h"
#include "mafOpContextStack.h"
#include "mafOpSelect.h"

#include "mmdMouse.h"

#include "mafNode.h"

#include "vtkMatrix4x4.h"

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
  m_Listener	= NULL;
  m_RemoteListener = NULL;
  m_RunningOp = NULL;
	m_Selected	= NULL;
	m_Warn			= false;
  m_FromRemote= false;
  
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
	for(int i=0; i<MAXOP; i++ ) 
    m_OpList[i] = NULL;
}
//----------------------------------------------------------------------------
mafOpManager::~mafOpManager()
//----------------------------------------------------------------------------
{
  // clear clipboard to avoid crash if some VME is still into the clipboard.
  m_Context.Clear();
  m_OpCut->ClipboardClear();

  for(int i=0; i<m_NumOp; i++) delete m_OpList[i];

  cppDEL(m_OpSelect);
  cppDEL(m_OpCut);
  cppDEL(m_OpCopy);
  cppDEL(m_OpPaste);
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
          m_RunningOp->OpGui()->GetWidgetValue(w_id, w_data);
          mafEvent ev(this,OPERATION_INTERFACE_EVENT,w_data,w_id);
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
void mafOpManager::OpAdd(mafOp *op, wxString menuPath)
//----------------------------------------------------------------------------
{
  assert(m_NumOp < MAXOP);
  m_OpList[m_NumOp] = op;
  op->m_OpMenuPath = menuPath;
	op->m_Id = m_NumOp + OP_USER;
  op->SetListener(this);
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
void mafOpManager::FillMenu (wxMenu* import, wxMenu* mexport, wxMenu* operations)
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
	if(m_Warn && !op->CanUndo())
	{
		wxMessageDialog dialog(
							mafGetFrame(),
							"This operation can not UnDo. Continue?",
							"UnDo Warning",
							wxYES_NO|wxYES_DEFAULT
							);
		if(dialog.ShowModal() == wxID_NO) go = false;
	}
  return go;
}
//----------------------------------------------------------------------------
void mafOpManager::OpRun(mafString &op_type)
//----------------------------------------------------------------------------
{
  for (int i=0; i< m_NumOp; i++)
  {
    if (op_type.Equals(m_OpList[i]->GetTypeName()))
    {
      OpRun(m_OpList[i]);
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
      if(m_CollaborateStatus && m_RemoteListener && !m_FromRemote && m_RunningOp->GetType() == OPTYPE_OP)
      {
        mafEvent e(this,RUN_OPERATION_EVENT,m_RunningOp);
        m_RemoteListener->OnEvent(&e);
      }
		}
	  break;
	}
}
//----------------------------------------------------------------------------
void mafOpManager::OpRun(mafOp *op)   
//----------------------------------------------------------------------------
{
	//Code to manage operation's Input Preserving
	if(!op->IsInputPreserving())
	{
		wxString warning_msg = "The operation do not preserve input VME integrity, a copy is required! \nThis should take a lot of memory and time depending on data dimension. \nDo you want to make a copy?";
		wxMessageDialog dialog(mafGetFrame(),warning_msg, "VME Preserving Warning",wxYES_NO|wxYES_DEFAULT);
		if(dialog.ShowModal() == wxID_YES)
    {
      wxString synthetic_name = "Copyed ";
      mafAutoPointer<mafNode> synthetic_vme = m_Selected->MakeCopy();
      synthetic_vme->ReparentTo(m_Selected->GetParent());
      synthetic_name.Append(m_Selected->GetName());
      synthetic_vme->SetName(synthetic_name);
      OpSelect(synthetic_vme);
      mafEventMacro(mafEvent(this,VME_SHOW,synthetic_vme,true));
    }
	}

	EnableOp(false);
  //Notify(OP_RUN_STARTING); //SIL. 17-9-2004: --- moved after m_RunningOp has been set

	mafOp *o = op->Copy();
  o->m_Id = op->m_Id;    //Paolo 15/09/2004 The operation ID is not copied from the Copy() method.
	o->SetListener(this);
	o->SetInput(m_Selected);
  o->SetMouse(m_Mouse);
  m_RunningOp = o;

  Notify(OP_RUN_STARTING);  //SIL. 17-9-2004: - moved here in order to notify which op is started

  m_Context.Push(o);
  o->OpRun();
}
//----------------------------------------------------------------------------
void mafOpManager::OpRunOk(mafOp *op)
//----------------------------------------------------------------------------
{	
  m_Context.Pop();

  m_RunningOp = NULL;
	if(WarnUser(op))
	{
		OpDo(op);
	}
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
    if(WarnUser(op))
	  {
		  mafOp *o = op->Copy();
		  o->SetListener(this);
		  o->SetInput(m_Selected);
		  OpDo(o);
	  }
  }
	EnableOp();
}
//----------------------------------------------------------------------------
void mafOpManager::OpDo(mafOp *op)
//----------------------------------------------------------------------------
{
 	if(!WarnUser(op)) 
  {
    delete op;
		return;
	}
  
  m_Context.Redo_Clear();
  op->OpDo();
  
  mafLogMessage("do=%s",op->m_Label.c_str());

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
void mafOpManager::OpUndo()
//----------------------------------------------------------------------------
{
  if( m_Context.Undo_IsEmpty()) 
  {
    mafLogMessage("empty undo stack");
    return;
  }
	EnableOp(false);

	mafOp* op = m_Context.Undo_Pop();
	mafLogMessage("undo=%s",op->m_Label.c_str());
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
	mafLogMessage("redo=%s",op->m_Label.c_str());
	op->OpDo();
	m_Context.Undo_Push(op);

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
